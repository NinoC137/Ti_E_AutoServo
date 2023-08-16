/*****************************************
文件名: PID.c
作者: 朱玉施
版本:   2.0
日期:   2023.5.12
描述:
    该文件中实现了位置式PID的计算(基于面向对象的思想)
    !建议将PID控制函数放于定时器中断回调函数中执行,以保证采样周期(推荐2~20ms,根据实际情况更改)
    使用方法:  ①进行初始化构造对象
              ②将PID参数进行设置    (通常这两个步骤放于只调用一次的初始化函数部分)
              ③根据需求选取计算函数
*****************************************/

#include "PID.h"
#include "Filter.h"

#define PWM_OUT_MAX	100	//PID输出限幅
#define ERR_SUM_MAX 4000

// 定义一个结构体类型变量于全局变量区    此处命名为pid_Motor1_Speed(仅作为示例使用, 暂无实际意义)
tPID pid_Motor1_Speed;
tPID pid_Motor2_Speed;
tPID pid_Turn;

// 初始化PID结构体
tPID PID_Create()
{
    tPID pid;
    pid.target_val = 0.00f;
    pid.actual_val = 0.00f;
    pid.out = 0.00f;

    pid.PID_Error.err = 0.0f;
    pid.PID_Error.err_last = 0.0f;
    pid.PID_Error.err_sum = 0.0f;

    pid.Kp = 0.0f;
    pid.Ki = 0.0f;
    pid.Kd = 0.0f;

    pid.P_set = P_Set;
    pid.I_set = I_Set;
    pid.D_set = D_Set;
    pid.Target_Set = Target_Set;

    return pid;
}

void PID_Init()
{
    pid_Motor1_Speed = PID_Create();
    pid_Motor2_Speed = PID_Create();
    pid_Turn = PID_Create();

    pid_Motor1_Speed.P_set(&pid_Motor1_Speed, 8.6f);
    pid_Motor1_Speed.I_set(&pid_Motor1_Speed, 0.09f);
    pid_Motor1_Speed.D_set(&pid_Motor1_Speed, 0.0f);

    pid_Motor2_Speed.P_set(&pid_Motor2_Speed, 8.6f);
    pid_Motor2_Speed.I_set(&pid_Motor2_Speed, 0.09f);
    pid_Motor2_Speed.D_set(&pid_Motor2_Speed, 0.0f);

    pid_Turn.P_set(&pid_Turn, 0.0f);
    pid_Turn.I_set(&pid_Turn, 0.0f);
    pid_Turn.D_set(&pid_Turn, 0.0f);

}


/**********************************************************************
    函数功能:   PID面向对象式函数实现
    输入参数:
                1.PID结构体
                2.目标值
                3.PID各项系数
    返回值:     无
    作者:   朱玉施
    修改时间:   2023.2.14
**********************************************************************/
void Target_Set(tPID *pid, float target)    { pid->target_val = target; }
void P_Set(tPID *pid, float p)  { pid->Kp = p; }
void I_Set(tPID *pid, float i)  { pid->Ki = i; }
void D_Set(tPID *pid, float d)  { pid->Kd = d; }
//引入一阶滞后滤波的误差计算(slope滤波器)
void Err_Calculate(tPID *pid){
    float Error_temp;   //引入一个局部误差变量,用于引入滤波算法

    Error_temp = pid->actual_val - pid->target_val;

    pid->PID_Error.err = FirstOrderLagFilter_Slope(Error_temp, pid);

}


/**********************************************************************
    函数功能:   使用PID系数进行PID调节(位置式Position)
    输入参数:
                1.PID结构体
                2.当前真实值
    返回值:
                Kp*当前误差 + Ki*误差积累值 + Kd*本次误差与上次误差之差
    注释:
        ①进行了积分制(Ki)的限幅,避免误差值持续积累,导致持续性超调
        ②进行了PID返回值(这里假定为PWM)的限幅
    作者:   朱玉施
    修改时间:   2023.5.13
**********************************************************************/
float PID_realize(tPID *pid, float actual_Val)
{
    pid->actual_val = actual_Val;                 // 传递当前真实值
    Err_Calculate(pid); // 计算得出当前误差(内部进行了slope滤波模型的一阶滞后滤波算法)
    pid->PID_Error.err_sum += pid->PID_Error.err;                     // 累计误差值获取

    //积分项限幅
    if(pid->PID_Error.err_sum >=  ERR_SUM_MAX)  { pid->PID_Error.err_sum =  ERR_SUM_MAX; }
    if(pid->PID_Error.err_sum <= -ERR_SUM_MAX)  { pid->PID_Error.err_sum = -ERR_SUM_MAX; }

    // PID控制实现部分
    pid->out = pid->Kp * pid->PID_Error.err
               + pid->Ki * pid->PID_Error.err_sum
               + pid->Kd * (pid->PID_Error.err - pid->PID_Error.err_last);

    pid->PID_Error.err_last = pid->PID_Error.err; // 将当前误差存至上次误差之中, 实现后续控制

    //PID输出限幅
    if(pid->out >= PWM_OUT_MAX)
    {
        pid->out = PWM_OUT_MAX;
    }
    if(pid->out <= -PWM_OUT_MAX)
    {
        pid->out = -PWM_OUT_MAX;
    }

    return pid->out;
}

/**********************************************************************
    函数功能:   使用PI系数进行PID调节(增量式)
    输入参数:
                1.PID结构体
                2.目标值
    返回值:
        pwm += Kp[e（k）- e(k-1)] + Ki*e(k) + Kd*[e(k)-2e(k-1)+e(k-2)]
        e(k)代表本次偏差   e(k-1)代表上一次的偏差  以此类推
        pwm代表增量输出
    注释:
        ②进行了PID返回值(这里假定为PWM)的限幅
    作者:   朱玉施
    修改时间:   2023.5.13
**********************************************************************/
float PID_realize_Incremental(tPID *pid, float target_Val)
{
    Target_Set(pid,target_Val); //设定目标值
    Err_Calculate(pid);   // 计算得出当前误差(内部进行了slope滤波模型的一阶滞后滤波算法)

    //PI控制实现部分
    pid->out += pid->Kp * (pid->PID_Error.err - pid->PID_Error.err_last)
                + pid->Ki * pid->PID_Error.err
                + pid->Kd * (pid->PID_Error.err - 2*pid->PID_Error.err_last + pid->PID_Error.err_sec_last);

    pid->PID_Error.err_sec_last = pid->PID_Error.err_last;
    pid->PID_Error.err_last = pid->PID_Error.err;   //将当前误差存至上次误差中, 实现后续控制

    //PID输出限幅
    if(pid->out >= PWM_OUT_MAX)
    {
        pid->out = PWM_OUT_MAX;
    }
    if(pid->out <= -PWM_OUT_MAX)
    {
        pid->out = -PWM_OUT_MAX;
    }

    return pid->out;
}



#ifndef __PID_H
#define __PID_H

#include <stdio.h>

typedef struct PIDERROR
{
    //当前偏差值, 上次偏差值, 累计偏差值, 上上次偏差值
    float err;
    float err_last;
    float err_sum;
    float err_sec_last;
}PID_Error;

typedef struct PIDarguments
{
    //目标值与实际值
    float target_val;
    float actual_val;
    float out;

    PID_Error PID_Error;

    //比例, 积分, 微分系数
    float Kp;
    float Ki;
    float Kd;

    //面向对象实现
    void (*P_set)(struct PIDarguments *pid,float aKp);
    void (*I_set)(struct PIDarguments *pid,float aKi);
    void (*D_set)(struct PIDarguments *pid,float aKd);
    void (*Target_Set)(struct PIDarguments *pid,float aTarget);

}tPID;

//功能函数
tPID PID_Create(void);
void Target_Set(tPID *pid, float target);
void P_Set(struct PIDarguments *pid, float p);
void I_Set(struct PIDarguments *pid, float i);
void D_Set(struct PIDarguments *pid, float d);
void Err_Calculate(tPID *pid);

void PID_Init(void);

//位置式PID控制
float PD_realize(tPID *pid, float actual_Val);
float PI_realize(tPID *pid, float actual_Val);
float PID_realize(tPID *pid, float actual_Val);

//增量式PID控制
float PID_realize_Incremental(tPID *pid, float target_Val);

extern tPID pid_Motor1_Speed;
extern tPID pid_Motor2_Speed;
extern tPID pid_Turn;
#endif



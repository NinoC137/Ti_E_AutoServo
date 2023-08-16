#ifndef CODE_FILTER_H_
#define CODE_FILTER_H_

#define FIRST_LAG_P 0.8f        //一阶滞后滤波系数，越小滞后效果越强
#define MVF_BUFFER 8

#include "PID.h"

typedef struct
{
    float Filter_Buffer[MVF_BUFFER];
    int index;
    float sum;
    float average;
}Sliding_Mean_Filter;

typedef struct {
    float x;        // state
    float A;        // x(n)=A*x(n-1)+u(n),u(n)~N(0,q)
    float H;        // z(n)=H*x(n)+w(n),w(n)~N(0,r)
    float q;        // process(predict) noise convariance
    float r;        // measure noise(error) convariance
    float p;        // estimated error convariance
    float gain;     // kalman gain
} kalman1_filter_t;

extern Sliding_Mean_Filter moving_filter;  //定义滑动滤波结构体
extern kalman1_filter_t kalman_filter;       //定义卡尔曼结构体

float FirstOrderLagFilter(float value);         //一阶滞后滤波算法
float FirstOrderLagFilter_Slope(float value, tPID* PID);   //一阶slope滞后滤波算法
float SecondOrderLagFilter(float value);        //二阶滞后滤波算法
float movingAverageFilter(Sliding_Mean_Filter *filter, float value);

float SecondOrderLagFilter_L(float value);
float SecondOrderLagFilter_R(float value);

void kalman1_init(kalman1_filter_t* state, float q, float r);   //初始化卡尔曼滤波器
float kalman1_filter(kalman1_filter_t* state, float z_measure); //卡尔曼滤波算法

float ValueFiltrate(uint8_t chx,float Value);

typedef struct
{
    float parameter;
    float sample_data;
    float output;
    float output_last;
    float filtered_data;
    unsigned char first_filter_flag;
}LowPass_Filter;

extern LowPass_Filter lpf_pitch_error;
extern LowPass_Filter lpf_yaw_error;
extern LowPass_Filter lpf_pitch_out;
extern LowPass_Filter lpf_yaw_out;

float Low_Pass_Filter(LowPass_Filter *filter,float data,float param);

typedef struct
{
    short int slide_count;
    float slide_temp1;
    float slide_temp2;
    float slide_temp3;
    float slide_temp4;
    float slide_temp5;
    float slide_sum;
}Slide_Filter;

extern Slide_Filter slide_pitch_error;
extern Slide_Filter slide_yaw_error;
extern Slide_Filter slide_pitch_out;
extern Slide_Filter slide_yaw_out;

float Window_Slide_Filter(Slide_Filter *filter,float data);
float Low_Pass_Filter(LowPass_Filter *filter,float data,float param);

#endif /* CODE_FILTER_H_ */


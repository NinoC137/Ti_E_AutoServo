#include "Filter.h"

kalman1_filter_t kalman_filter;
Sliding_Mean_Filter moving_filter;

LowPass_Filter lpf_pitch_error;
LowPass_Filter lpf_yaw_error;
LowPass_Filter lpf_pitch_out;
LowPass_Filter lpf_yaw_out;

Slide_Filter slide_pitch_error;
Slide_Filter slide_yaw_error;
Slide_Filter slide_pitch_out;
Slide_Filter slide_yaw_out;
/**
 ** ????????:  ?????????????Bias?????
 ** ??    ??: value???????????????
 ** ?? ?? ?: ???????
 */
float FirstOrderLagFilter(float value) {
    static float last_value;
    value = FIRST_LAG_P * value + (1.0f - FIRST_LAG_P) * last_value; //?????????
    last_value = value;
    return value;
}

/**
 ** ????????:  ?????????????Slope?????
 ** ??    ??: value???????????????
 ** ?? ?? ?: ???????
*/
float FirstOrderLagFilter_Slope(float value, tPID* PID)
{
    //static float last_value;  //上一次滤波结果

    value=FIRST_LAG_P*value+(1.0f-FIRST_LAG_P)*PID->PID_Error.err_last; //一阶滞后滤波
    PID->PID_Error.err_last=value;   //保存此次滤波结果为上一次滤波结果

    return value;
}

/**
 *  ????????:  ????????????
 ** ??    ??: value???????????????
 ** ?? ?? ?: ???????
 */
float SecondOrderLagFilter(float value) {
    static float last_value, last_2_value;
    value = 0.2f * value + 0.4f * last_value + 0.4f * last_2_value;     //??????????
    last_2_value = last_value;
    last_value = value;
    return value;
}

/*
 *******************************************************************************************
 ** 函数功能: 二阶滞后滤波算法，左编码器滤波器
 ** 参    数: value：需要进行滤波的值
 ** 返 回 值: 滤波后的值
 ** 作    者: WBN
 ********************************************************************************************
 */
float SecondOrderLagFilter_L(float value)
{
    static float last_value,last_2_value;

    value=0.7f*value+0.3f*last_value+0.0f*last_2_value;     //二阶滞后滤波

    last_2_value=last_value;
    last_value=value;

    return value;
}

/*
 *******************************************************************************************
 ** 函数功能: 二阶滞后滤波算法，右编码器滤波器
 ** 参    数: value：需要进行滤波的值
 ** 返 回 值: 滤波后的值
 ** 作    者: WBN
 ********************************************************************************************
 */
float SecondOrderLagFilter_R(float value)
{
    static float last_value,last_2_value;

    value=0.7f*value+0.3f*last_value+0.0f*last_2_value;     //二阶滞后滤波

    last_2_value=last_value;
    last_value=value;

    return value;
}

/**
 *  ????????:  ????????????
 ** ??    ??: value???????????????
 ** ?? ?? ?: ???????
 */
float movingAverageFilter(Sliding_Mean_Filter *filter, float value) {
    filter->sum -= filter->Filter_Buffer[filter->index];        //?????????
    filter->sum += value;                                       //??????????
    filter->Filter_Buffer[filter->index] = value;               //???????????????????
    filter->average = filter->sum / MVF_BUFFER;                 //????
    if (++filter->index == MVF_BUFFER)
        filter->index = 0;
    return filter->average;
}


/**
 * ????????:    ????????????????
 * ??    ??:   q,r: ???????????, ????????????
 * ?? ?? ?:   state : ????????????
 */
void kalman1_init(kalman1_filter_t *state, float q, float r) {
    state->x = 0;
    state->p = 0.0f;
    state->A = 1.0f;
    state->H = 1.0f;
    state->q = q;
    state->r = r;
}

/**
 * ????????:    ???????????
 * ??    ??:   z_measure : ??????
 * ?? ?? ?:   state : ????????????
 */
float kalman1_filter(kalman1_filter_t *state, float z_measure) {
    /* Predict */
    // ??????(???): X(k|k-1) = A(k,k-1)*X(k-1|k-1) + B(k)*u(k)
    state->x = state->A * state->x;
    // ????????Э????: P(k|k-1) = A(k,k-1)*A(k,k-1)^T*P(k-1|k-1)+Q(k)
    state->p = state->A * state->A * state->p + state->q;

    /* Measurement */
    // ????????????: K(k) = P(k|k-1)*H(k)^T/(P(k|k-1)*H(k)*H(k)^T + R(k))
    state->gain = state->p * state->H / (state->p * state->H * state->H + state->r);
    // ????????(У??): X(k|k) = X(k|k-1)+K(k)*(Z(k)-H(k)*X(k|k-1))
    state->x = state->x + state->gain * (z_measure - state->H * state->x);
    // ???????Э????: P(k|k) =??I-K(k)*H(k))*P(k|k-1)
    state->p = (1 - state->gain * state->H) * state->p;

    return state->x;
}

#define BUF_DEEP 6
#define CHX_NUM 1
float ValueFiltrate(uint8_t chx,float Value)
{
    static float ValueTempBuf[CHX_NUM][BUF_DEEP];
    static uint8_t point[CHX_NUM] = {0};
    float Max = 0;
    float Min = -200;
    float Sum = 0;
    if(chx >= CHX_NUM)
        return 0;

    ValueTempBuf[chx][point[chx]] = Value;
    point[chx] ++;
    if(point[chx] == BUF_DEEP)
        point[chx] = 0;

    for(uint8_t i = 0;i<BUF_DEEP;i++)
    {
        if(ValueTempBuf[chx][i] >= Max)
            Max = ValueTempBuf[chx][i];
        if(ValueTempBuf[chx][i] <= Min)
            Min = ValueTempBuf[chx][i];

        Sum += ValueTempBuf[chx][i];
    }
    return (Sum - Max - Min)/(BUF_DEEP-2);
}

/*********************************一阶低通滤波***********************************************************/
float Low_Pass_Filter(LowPass_Filter *filter,float data,float param)
{
    filter->parameter=param;
    filter->sample_data=data;
    filter->output=filter->parameter*filter->sample_data+(1.0-filter->parameter)*filter->output_last;
    filter->output_last=filter->output;
    return filter->output;
}

/*********************************窗口滑动滤波***********************************************************/
float Window_Slide_Filter(Slide_Filter *filter,float data)
{
    filter->slide_count++;
    filter->slide_temp1 = data;
    filter->slide_sum = filter->slide_temp1 + filter->slide_temp2 + filter->slide_temp3 + filter->slide_temp4 + filter->slide_temp5;
    if(filter->slide_count > 5)filter->slide_count--;
    filter->slide_temp5 = filter->slide_temp4;
    filter->slide_temp4 = filter->slide_temp3;
    filter->slide_temp3 = filter->slide_temp2;
    filter->slide_temp2 = filter->slide_temp1;
    return filter->slide_sum/filter->slide_count;
}


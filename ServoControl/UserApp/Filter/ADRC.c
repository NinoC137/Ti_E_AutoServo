

#include "ADRC.h"
Fhan_Data adrc_pitch;
Fhan_Data adrc_yaw;
/***********************************************
* @brief : ������
* @param : float number ������
* @return: float
* @date  : 2023.5.25
* @author: �Ƴ���
************************************************/

float my_sqrt(float number)
{
    long i;
    float x, y;
    const float f = 1.5F;
    x = number * 0.5F;
    y = number;
    i = * ( long * ) &y;
    i = 0x5f3759df - ( i >> 1 );

    y = * ( float * ) &i;
    y = y * ( f - ( x * y * y ) );
    y = y * ( f - ( x * y * y ) );
    return number * y;
}
/***********************************************
* @brief : ȡ���ź���
* @param : float Input ������
* @return: ���ź���
* @date  : 2023.5.25
* @author: �Ƴ���
************************************************/

int16_t Sign_ADRC(float Input)
{
    int16_t output=0;
    if(Input>1E-6) output=1;
    else if(Input<-1E-6) output=-1;
    else output=0;
    return output;
}
/***********************************************
* @brief : ADRC������ʼ��
* @param :
* @return: viod
* @date  : 2023.5.25
* @author: �Ƴ���
************************************************/
int16_t Fsg_ADRC(float x,float d)
{
    int16_t output=0;
    output=(Sign_ADRC(x+d)-Sign_ADRC(x-d))/2;
    return output;
}
/***********************************************
* @brief : ADRC������ʼ��
* @param :
* @return: viod
* @date  : 2023.5.25
* @author: �Ƴ���
************************************************/
void ADRC_Init(void)
{
    adrc_pitch.r = 1000000;
    adrc_pitch.h = 0.005f;
    adrc_pitch.N0 = 2;
    adrc_yaw.r = 1000000;
    adrc_yaw.h = 0.005f;
    adrc_yaw.N0 = 2;
}

/***********************************************
* @brief : ADRC���ٸ���΢����TD���Ľ����㷨fhan
* @param : Fhan_Data *fhan_Input:����fhan ����
*          float expect_ADRC : ����
* @return: ΢�ָ���
* @date  : 2023.5.25
* @author: �Ƴ���
* @����    Fhan_ADRC(&ADRC_Controller,Speed.output);//�����������TD ��Ӧ�ٶȼ�������������ʱ���С
*          ADRC_Controller->x1 Ϊ���¸���״̬�� ����Ϊ Speed.output ����������
************************************************/
void Fhan_ADRC(Fhan_Data *fhan_Input,float expect_ADRC)//����ADRC���ȹ���
{
    float d=0,a0=0,y=0,a1=0,a2=0,a=0;
    float x1_delta=0;//ADRC״̬���������
    x1_delta=fhan_Input->x1-expect_ADRC;//��x1-v(k)���x1�õ���ɢ���¹�ʽ
    fhan_Input->h0=fhan_Input->N0*fhan_Input->h;//��h0���h��������ٸ���΢�����ٶȳ�������
    d=fhan_Input->r*fhan_Input->h0*fhan_Input->h0;//d=rh^2;
    a0=fhan_Input->h0*fhan_Input->x2;//a0=h*x2
    y=x1_delta+a0;//y=x1+a0
    a1=my_sqrt(d*(d+8*ABS(y)));//a1=sqrt(d*(d+8*ABS(y))])
    a2=a0+Sign_ADRC(y)*(a1-d)/2;//a2=a0+sign(y)*(a1-d)/2;
    a=(a0+y)*Fsg_ADRC(y,d)+a2*(1-Fsg_ADRC(y,d));
    fhan_Input->fh=-fhan_Input->r*(a/d)*Fsg_ADRC(a,d)
                   -fhan_Input->r*Sign_ADRC(a)*(1-Fsg_ADRC(a,d));//�õ�����΢�ּ��ٶȸ�����
    fhan_Input->x1+=fhan_Input->h*fhan_Input->x2;//�������ٸ���״̬��x1
    fhan_Input->x2+=fhan_Input->h*fhan_Input->fh;//�������ٸ���״̬��΢��x2
}

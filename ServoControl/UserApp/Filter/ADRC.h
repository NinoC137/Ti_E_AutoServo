//
// Created by Mao Junjie on 2022/6/7.
//

#ifndef ADRC_H
#define ADRC_H
#include "main.h"
//ADRC����΢����
typedef struct
{
    /*****���Ź��ȹ���*******/
    float x1;//����΢����״̬��
    float x2;//����΢����״̬��΢����
    float r;//ʱ��߶�
    float h;//ADRCϵͳ����ʱ��
    unsigned short N0;//����΢��������ٶȳ���h0=N*h

    float h0;
    float fh;//����΢�ּ��ٶȸ�����
}Fhan_Data;

extern Fhan_Data adrc_pitch;
extern Fhan_Data adrc_yaw;

void ADRC_Init(void);
void Fhan_ADRC(Fhan_Data *fhan_Input,float expect_ADRC);

//��ѧ����
float my_sqrt(float number);
#define my_pow(a) ((a)*(a))
#define ABS(x) ( (x)>0?(x):-(x) )
#endif //MM32F3277_ADRC_H

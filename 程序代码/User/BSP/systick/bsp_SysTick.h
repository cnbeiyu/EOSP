#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

//�ڵδ�ʱ���жϷ������е���
void TimingDelay_Decrement(void);

// ��ʼ��ϵͳ�δ�ʱ��
void SysTick_Init(void);


//�ṩ��Ӧ�ó������
void Delay_10us(__IO u32 nTime);
#define Delay_ms(x) Delay_10us(100*x)	 //��λms

#endif /* __SYSTICK_H */

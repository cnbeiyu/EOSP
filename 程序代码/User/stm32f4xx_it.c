/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include <includes.h>
#include "bsp_exti.h"
#include "./usart/bsp_debug_usart.h"

/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//void SVC_Handler(void)
//{}

///**
//  * @brief  This function handles PendSV_Handler exception.
//  * @param  None
//  * @retval None
//  */
//void PendSV_Handler(void)
//{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{}

  
  
 
extern OS_TCB	 AppTaskUsartTCB;
extern OS_TCB	 AppTaskKeyTCB; 
  
  
/*********************************************************************************
  * @ ������  �� KEY1_IRQHandler
  * @ ����˵���� �жϷ�����
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
void KEY1_IRQHandler(void)
{

  OS_ERR   err;
	
	OSIntEnter(); 	                                     //�����ж�
	
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
  		/* ���������ź��������� AppTaskKey */
		//OSTaskSemPost((OS_TCB  *)&AppTaskKeyTCB,   //Ŀ������
		//							(OS_OPT   )OS_OPT_POST_NONE, //ûѡ��Ҫ��
		//							(OS_ERR  *)&err);            //���ش�������		
		OS_ERR err;
		OSTaskQPost ((OS_TCB *)&AppTaskKeyTCB, 
		(void *)"1", 
		(OS_MSG_SIZE )sizeof ( "1" ), 
		(OS_OPT )OS_OPT_POST_FIFO, 
		(OS_ERR *)&err); 
		//printf("5a 7 1 1 9 a5\n");
		//����жϱ�־λ
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
  
	OSIntExit();	     
}

/*********************************************************************************
  * @ ������  �� KEY1_IRQHandler
  * @ ����˵���� �жϷ�����
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
void KEY2_IRQHandler(void)
{	
  OS_ERR   err;
	
	OSIntEnter(); 	                                     //�����ж�
	
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
  		/* ���������ź��������� AppTaskKey */
		//OSTaskSemPost((OS_TCB  *)&AppTaskKeyTCB,   //Ŀ������
		//							(OS_OPT   )OS_OPT_POST_NONE, //ûѡ��Ҫ��
		//							(OS_ERR  *)&err);            //���ش�������		
	OS_ERR err;
  OSTaskQPost ((OS_TCB *)&AppTaskKeyTCB, 
  (void *)"2", 
  (OS_MSG_SIZE )sizeof ( "2" ), 
  (OS_OPT )OS_OPT_POST_FIFO, 
  
  (OS_ERR *)&err); 
		//printf("5a 7 1 2 a a5\n");
		//����жϱ�־λ
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
  
	OSIntExit();	                                       //�˳��ж�
 
}

/*********************************************************************************
  * @ ������  �� DEBUG_USART_IRQHandler
  * @ ����˵���� �����жϷ�����
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
void DEBUG_USART_IRQHandler(void)
{
	OS_ERR   err;
	
	OSIntEnter(); 	                                     //�����ж�

	if(USART_GetITStatus(DEBUG_USART,USART_IT_IDLE)!=RESET)
	{		
		  // �ر�DMA ����ֹ����
      DMA_Cmd(DEBUG_USART_DMA_STREAM, DISABLE);      
      // ��DMA��־λ
      DMA_ClearFlag(DEBUG_USART_DMA_STREAM,DMA_FLAG_TCIF2);  
      //  ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ
      DMA_SetCurrDataCounter(DEBUG_USART_DMA_STREAM,USART_RBUFF_SIZE);     
      DMA_Cmd(DEBUG_USART_DMA_STREAM, ENABLE);       
		
    //�����ź��� �����ͽ��յ������ݱ�־����ǰ̨�����ѯ
    		
    /* ���������ź��������� AppTaskKey */
		OSTaskSemPost((OS_TCB  *)&AppTaskUsartTCB,   //Ŀ������
									(OS_OPT   )OS_OPT_POST_NONE, //ûѡ��Ҫ��
									(OS_ERR  *)&err);            //���ش�������		
    
		USART_ReceiveData(DEBUG_USART); /* �����־λ */
	}	 
  
	OSIntExit();	                                       //�˳��ж�
	
}
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

/**
  * @}
  */ 

extern __IO uint16_t ADC_ConvertedValue;

// ADC ת������жϷ������
void ADC_IRQHandler(void)
{
	if(ADC_GetITStatus(RHEOSTAT_ADC,ADC_IT_EOC)==SET)
	{
  // ��ȡADC��ת��ֵ
		ADC_ConvertedValue = ADC_GetConversionValue(RHEOSTAT_ADC);

	}
	ADC_ClearITPendingBit(RHEOSTAT_ADC,ADC_IT_EOC);

}	
/**
  * @}
  */ 


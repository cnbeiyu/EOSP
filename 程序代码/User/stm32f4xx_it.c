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
  * @ 函数名  ： KEY1_IRQHandler
  * @ 功能说明： 中断服务函数
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
void KEY1_IRQHandler(void)
{

  OS_ERR   err;
	
	OSIntEnter(); 	                                     //进入中断
	
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
  		/* 发送任务信号量到任务 AppTaskKey */
		//OSTaskSemPost((OS_TCB  *)&AppTaskKeyTCB,   //目标任务
		//							(OS_OPT   )OS_OPT_POST_NONE, //没选项要求
		//							(OS_ERR  *)&err);            //返回错误类型		
		OS_ERR err;
		OSTaskQPost ((OS_TCB *)&AppTaskKeyTCB, 
		(void *)"1", 
		(OS_MSG_SIZE )sizeof ( "1" ), 
		(OS_OPT )OS_OPT_POST_FIFO, 
		(OS_ERR *)&err); 
		//printf("5a 7 1 1 9 a5\n");
		//清除中断标志位
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
  
	OSIntExit();	     
}

/*********************************************************************************
  * @ 函数名  ： KEY1_IRQHandler
  * @ 功能说明： 中断服务函数
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
void KEY2_IRQHandler(void)
{	
  OS_ERR   err;
	
	OSIntEnter(); 	                                     //进入中断
	
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
  		/* 发送任务信号量到任务 AppTaskKey */
		//OSTaskSemPost((OS_TCB  *)&AppTaskKeyTCB,   //目标任务
		//							(OS_OPT   )OS_OPT_POST_NONE, //没选项要求
		//							(OS_ERR  *)&err);            //返回错误类型		
	OS_ERR err;
  OSTaskQPost ((OS_TCB *)&AppTaskKeyTCB, 
  (void *)"2", 
  (OS_MSG_SIZE )sizeof ( "2" ), 
  (OS_OPT )OS_OPT_POST_FIFO, 
  
  (OS_ERR *)&err); 
		//printf("5a 7 1 2 a a5\n");
		//清除中断标志位
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
  
	OSIntExit();	                                       //退出中断
 
}

/*********************************************************************************
  * @ 函数名  ： DEBUG_USART_IRQHandler
  * @ 功能说明： 串口中断服务函数
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
void DEBUG_USART_IRQHandler(void)
{
	OS_ERR   err;
	
	OSIntEnter(); 	                                     //进入中断

	if(USART_GetITStatus(DEBUG_USART,USART_IT_IDLE)!=RESET)
	{		
		  // 关闭DMA ，防止干扰
      DMA_Cmd(DEBUG_USART_DMA_STREAM, DISABLE);      
      // 清DMA标志位
      DMA_ClearFlag(DEBUG_USART_DMA_STREAM,DMA_FLAG_TCIF2);  
      //  重新赋值计数值，必须大于等于最大可能接收到的数据帧数目
      DMA_SetCurrDataCounter(DEBUG_USART_DMA_STREAM,USART_RBUFF_SIZE);     
      DMA_Cmd(DEBUG_USART_DMA_STREAM, ENABLE);       
		
    //给出信号量 ，发送接收到新数据标志，供前台程序查询
    		
    /* 发送任务信号量到任务 AppTaskKey */
		OSTaskSemPost((OS_TCB  *)&AppTaskUsartTCB,   //目标任务
									(OS_OPT   )OS_OPT_POST_NONE, //没选项要求
									(OS_ERR  *)&err);            //返回错误类型		
    
		USART_ReceiveData(DEBUG_USART); /* 清除标志位 */
	}	 
  
	OSIntExit();	                                       //退出中断
	
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

// ADC 转换完成中断服务程序
void ADC_IRQHandler(void)
{
	if(ADC_GetITStatus(RHEOSTAT_ADC,ADC_IT_EOC)==SET)
	{
  // 读取ADC的转换值
		ADC_ConvertedValue = ADC_GetConversionValue(RHEOSTAT_ADC);

	}
	ADC_ClearITPendingBit(RHEOSTAT_ADC,ADC_IT_EOC);

}	
/**
  * @}
  */ 


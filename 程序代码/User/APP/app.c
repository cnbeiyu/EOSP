#include <includes.h>
#include <string.h>

CPU_TS             ts_start;       //时间戳变量
CPU_TS             ts_end; 
OS_TMR      my_tmr;   //声明软件定时器对象
typedef struct connection{
    u8 head;       
    u8 id;   
    u8 body;    
    u8 tail;         
}con;
static  OS_TCB   AppTaskStartTCB;    //任务控制块

OS_TCB           AppTaskUsartTCB;
OS_TCB           AppTaskKeyTCB;
static OS_TCB   AppTaskckeyTCB;
static  OS_TCB   AppTaskLed1TCB;
static  OS_TCB   AppTaskLed2TCB;
static  OS_TCB   AppTaskLed3TCB;
static  OS_TCB   AppTaskTmrTCB;
static  OS_TCB   AppTaskADCTCB;




static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //任务堆栈

static  CPU_STK  AppTaskUsartStk [ APP_TASK_USART_STK_SIZE ];
static  CPU_STK  AppTaskKeyStk   [ APP_TASK_KEY_STK_SIZE ];
static  CPU_STK  AppTaskLed1Stk [ APP_TASK_LED1_STK_SIZE ];
static  CPU_STK  AppTaskLed2Stk [ APP_TASK_LED2_STK_SIZE ];
static  CPU_STK  AppTaskLed3Stk [ APP_TASK_LED3_STK_SIZE ];
static  CPU_STK  AppTaskTmrStk [ APP_TASK_TMR_STK_SIZE ];
static  CPU_STK  AppTaskADCStk [ APP_TASK_ADC_STK_SIZE ];


//extern char Usart_Rx_Buf[USART_RBUFF_SIZE];
extern u8 Usart_Rx_Buf[USART_RBUFF_SIZE];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //任务函数声明

static  void  AppTaskUsart  ( void * p_arg );
static  void  AppTaskKey    ( void * p_arg );
static  void  AppTaskLed1  ( void * p_arg );
static  void  AppTaskLed2  ( void * p_arg );
static  void  AppTaskLed3  ( void * p_arg );
static  void  AppTaskTmr  ( void * p_arg );
static  void  AppTaskADC  ( void * p_arg );

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
	struct connection con;
	con.head=0xaa;
	con.tail=0xbb;
    OS_ERR  err;


    OSInit(&err);                                                           //初始化 uC/OS-III

	  /* 创建起始任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //任务控制块地址
                 (CPU_CHAR   *)"App Task Start",                            //任务名称
                 (OS_TASK_PTR ) AppTaskStart,                               //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //任务的优先级
                 (CPU_STK    *)&AppTaskStartStk[0],                         //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

    OSStart(&err);                                                          //启动多任务管理（交由uC/OS-III控制）

}



static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


    (void)p_arg;

    BSP_Init();                                                 //板级初始化
    CPU_Init();                                                 //初始化 CPU 组件（时间戳、关中断时间测量和主机名）

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //获取 CPU 内核时钟频率（SysTick 工作时钟）
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
    OS_CPU_SysTickInit(cnts);                                   //调用 SysTick 初始化函数，设置定时器计数值和启动定时器

    Mem_Init();                                                 //初始化内存管理组件（堆内存池和内存池表）

#if OS_CFG_STAT_TASK_EN > 0u                                    //如果使能（默认使能）了统计任务
    OSStatTaskCPUUsageInit(&err);                               //计算没有应用任务（只有空闲任务）运行时 CPU 的（最大）
#endif                                                          //容量（决定 OS_Stat_IdleCtrMax 的值，为后面计算 CPU 
                                                                //使用率使用）。
    CPU_IntDisMeasMaxCurReset();                                //复位（清零）当前最大关中断时间

    
    
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,          //使能时间片轮转调度
		                     (OS_TICK       )0,                    //把 OSCfg_TickRate_Hz / 10 设为默认时间片值
												 (OS_ERR       *)&err );               //返回错误类型

							 

    OSTaskCreate((OS_TCB     *)&AppTaskUsartTCB,                            //任务控制块地址
                 (CPU_CHAR   *)"App Task Usart",                            //任务名称
                 (OS_TASK_PTR ) AppTaskUsart,                               //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_USART_PRIO,                        //任务的优先级
                 (CPU_STK    *)&AppTaskUsartStk[0],                         //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_USART_STK_SIZE / 10,               //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_USART_STK_SIZE,                    //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 50u,                                        //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
								 
								 
	
    OSTaskCreate((OS_TCB     *)&AppTaskKeyTCB,                              //任务控制块地址
                 (CPU_CHAR   *)"App Task Key",                              //任务名称
                 (OS_TASK_PTR ) AppTaskKey,                                 //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_KEY_PRIO,                          //任务的优先级
                 (CPU_STK    *)&AppTaskKeyStk[0],                           //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE / 10,                 //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE,                      //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 50u,                                        //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
		
    OSTaskCreate((OS_TCB     *)&AppTaskLed1TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Led1",                             //任务名称
                 (OS_TASK_PTR ) AppTaskLed1,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_LED1_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskLed1Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
		
	
    OSTaskCreate((OS_TCB     *)&AppTaskLed2TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Led2",                             //任务名称
                 (OS_TASK_PTR ) AppTaskLed2,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_LED2_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskLed2Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型								 

	
    OSTaskCreate((OS_TCB     *)&AppTaskLed3TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Led3",                             //任务名称
                 (OS_TASK_PTR ) AppTaskLed3,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_LED3_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskLed3Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_LED3_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_LED3_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
	
    OSTaskCreate((OS_TCB     *)&AppTaskTmrTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Tmr",                             //任务名称
                 (OS_TASK_PTR ) AppTaskTmr,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_TMR_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskTmrStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_TMR_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_TMR_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
	
    OSTaskCreate((OS_TCB     *)&AppTaskADCTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task ADC",                             //任务名称
                 (OS_TASK_PTR ) AppTaskADC,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_ADC_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskADCStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_ADC_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_ADC_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型	


		OSTaskSuspend ( & AppTaskTmrTCB, & err );
								 
		OSTaskDel ( 0, & err );                     //删除起始任务本身，该任务不再运行

		
}

typedef struct {
	u8 head;
	u8 id;
	u8 len;
	u8 body;
	u8 crc;
	u8 tail;
}m1;
m1 msg;

typedef struct {
u8 head;
u8 id;
u8 len;
u8 body1;
u8 body2;
u8 crc;
u8 tail;
}m2;
m2 msg2;
static  void  AppTaskUsart ( void * p_arg )
{
	OS_ERR         err;
  
	CPU_SR_ALLOC();
	

	(void)p_arg;

					 
	while (DEF_TRUE) {                                           //任务体

		OSTaskSemPend ((OS_TICK   )0,                     //无期限等待
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果信号量不可用就等待
									 (CPU_TS   *)0,                     //获取信号量被发布的时间戳
									 (OS_ERR   *)&err);                 //返回错误类型

		OS_CRITICAL_ENTER();
		printf("recive: ");
		for(int i=0;Usart_Rx_Buf[i]!=NULL;i++){
			printf("%x ",Usart_Rx_Buf[i]);
		}
		printf("\n");
		if(Usart_Rx_Buf[0]==0xaa&&Usart_Rx_Buf[5]==0xbb){
			msg.head=Usart_Rx_Buf[0];
			msg.id=Usart_Rx_Buf[1];
			msg.len=Usart_Rx_Buf[2];
			msg.body=Usart_Rx_Buf[3];
			msg.crc=Usart_Rx_Buf[4];
			msg.tail=Usart_Rx_Buf[5];
		}
		if(msg.id+msg.len+msg.body==msg.crc){
		switch (msg.id){
			case 0x01:
				if(msg.body==0x01){
					LED_RGBOFF;
					OSTaskSemPost((OS_TCB  *)&AppTaskLed1TCB,                              //目标任务
												(OS_OPT   )OS_OPT_POST_NONE,                             //没选项要求
												(OS_ERR  *)&err);                                        //返回错误类型
				}
				if(msg.body==0x02){
					LED_RGBOFF;
				}
				break;
			case 0x02:
				if(msg.body==0x01){
					LED_RGBOFF;
					OSTaskSemPost((OS_TCB  *)&AppTaskLed2TCB,                              //目标任务
												(OS_OPT   )OS_OPT_POST_NONE,                             //没选项要求
												(OS_ERR  *)&err);                                        //返回错误类型
				}
				if(msg.body==0x02){
					LED_RGBOFF;
				}
				break;
			case 0x03:
				if(msg.body==0x01){
					LED_RGBOFF;
					OSTaskSemPost((OS_TCB  *)&AppTaskLed3TCB,                              //目标任务
												(OS_OPT   )OS_OPT_POST_NONE,                             //没选项要求
												(OS_ERR  *)&err);                                        //返回错误类型
				}
				if(msg.body==0x02){
					LED_RGBOFF;
				}
				break;
			case 0x04:
				if(msg.body==0x01){
					BEEP_ON;
				}
				if(msg.body==0x02){
					BEEP_OFF;
				}	
				break;
			case 0x05:
				if(msg.body==0x01){
					OSTmrStart ((OS_TMR   *)&my_tmr, //软件定时器对象
											(OS_ERR   *)err);    //返回错误类型	
					OSTaskResume ( & AppTaskTmrTCB, & err );
				}
				if(msg.body==0x02){
					OS_ERR err;
					OSTmrStop ((OS_TMR *)&my_tmr, 
										 (OS_OPT )OS_OPT_TMR_NONE, 
										 (void *)"Timer Over!", 
										 (OS_ERR *)err); 
					OSTaskSuspend ( & AppTaskTmrTCB, & err );
			  }
				break;
			case 0x06:
				if(msg.body==0x01){
					/* 发布任务信号量 */
					OSTaskSemPost((OS_TCB  *)&AppTaskADCTCB,                              //目标任务
												(OS_OPT   )OS_OPT_POST_NONE,                             //没选项要求
												(OS_ERR  *)&err);                                        //返回错误类型
				}
				break;
			case 0x07:
				{if(msg.body==0x01){
					OSTaskSemPost((OS_TCB  *)&AppTaskckeyTCB,                              //目标任务
												(OS_OPT   )OS_OPT_POST_NONE,                             //没选项要求
												(OS_ERR  *)&err);                                        //返回错误类型
				}}
				break;
			default:break;
		}
	}
    memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);/* 清零 */

		OS_CRITICAL_EXIT();                                        //退出临界段
		
	
		
	}
	
}


static  void  AppTaskKey ( void * p_arg )
{
	OS_ERR         err;
	CPU_TS_TMR     ts_int;
	CPU_INT32U     cpu_clk_freq;
	CPU_SR_ALLOC();
	OS_MSG_SIZE msg_size;
	CPU_TS ts;
	char *pMsg;
	(void)p_arg;

					 
  cpu_clk_freq = BSP_CPU_ClkFreq();                   //获取CPU时钟，时间戳是以该时钟计数

	
	while (DEF_TRUE) {                                  //任务体
		/* 阻塞任务，直到KEY1被按下 */
		//OSTaskSemPend ((OS_TICK   )0,                     //无期限等待
		//							 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果信号量不可用就等待
		//							 (CPU_TS   *)0,                     //获取信号量被发布的时间戳
		//							 (OS_ERR   *)&err);                 //返回错误类型
	pMsg = OSTaskQPend ((OS_TICK )0, 
				(OS_OPT )OS_OPT_PEND_BLOCKING, 
				(OS_MSG_SIZE *)&msg_size, 
				(CPU_TS *)&ts, 
				(OS_ERR *)&err); 
		
		ts_int = CPU_IntDisMeasMaxGet ();                 //获取最大关中断时间

		OS_CRITICAL_ENTER();                              //进入临界段，避免串口打印被打断
		//if(pMsg[1]==1)
		//printf("%d\n",strcmp(pMsg,"1"));
		if(strcmp(pMsg,"1")==0)   //key1
		{printf("aa 07 01 01 09 bb\n");
			printf("KEY1\n");}
		else
			{printf("aa 07 01 02 10 bb\n"); 
			printf("KEY2\n");}        		
		
		OS_CRITICAL_EXIT();                               //退出临界段
		
	}
	
}

static  void  AppTaskLed1 ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;	
	CPU_TS         ts;
	
	(void)p_arg;

	while (DEF_TRUE) {     		//任务体，通常写成一个死循环	
				/* 阻塞任务*/
		OSTaskSemPend ((OS_TICK   )0,                     //无期限等待
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果信号量不可用就等待
									 (CPU_TS   *)&ts,                   //获取信号量被发布的时间戳
									 (OS_ERR   *)&err);                 //返回错误类型
				
	printf("LED1\n");
		LED1_ON;                                 //切换 LED1 的亮灭状态
		
		

	}
		
		
}




static  void  AppTaskLed2 ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;
	CPU_TS         ts;
	(void)p_arg;


	while (DEF_TRUE) {                                 //任务体，通常写成一个死循环
		/* 阻塞任务*/
		OSTaskSemPend ((OS_TICK   )0,                     //无期限等待
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果信号量不可用就等待
									 (CPU_TS   *)&ts,                   //获取信号量被发布的时间戳
									 (OS_ERR   *)&err);                 //返回错误类型
		printf("LED2\n");
		LED2_ON;                               //切换 LED2 的亮灭状态
		
		//OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //相对性延时1000个时钟节拍（1s）
		/* 任务切换，这里是手动切换 */	
		
	}
		
		
}




static  void  AppTaskLed3 ( void * p_arg )
{
	OS_ERR      err;
	CPU_TS         ts;
	OS_REG      value;
	(void)p_arg;


	while (DEF_TRUE) {                                 //任务体，通常写成一个死循环
		/* 阻塞任务*/
		OSTaskSemPend ((OS_TICK   )0,                     //无期限等待
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果信号量不可用就等待
									 (CPU_TS   *)&ts,                   //获取信号量被发布的时间戳
									 (OS_ERR   *)&err);                 //返回错误类型
		printf("LED3\n");
		LED3_ON;                               //切换 LED3 的亮灭状态
		//OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //相对性延时1000个时钟节拍（1s）
		/* 任务切换，这里是手动切换 */		
		
	}

		
}

void TmrCallback (OS_TMR *p_tmr, void *p_arg) //软件定时器MyTmr的回调函数
{	
	CPU_SR_ALLOC();      //使用到临界段（在关/开中断时）时必需该宏，该宏声明和定义一个局部变
											 //量，用于保存关中断前的 CPU 状态寄存器 SR（临界段关中断只需保存SR）
											 //，开中断时将该值还原。  
  printf ( "%s\n", ( char * ) p_arg );
	
	
	//LED1_TOGGLE; 

	
}

void transfer(){
msg2.head = msg.head;
msg2.id = msg.id;
msg2.len = msg.len;
msg2.tail = msg.tail;
}
static  void  AppTaskTmr ( void * p_arg )
{
	OS_ERR      err;


	
	(void)p_arg;


  /* 创建软件定时器 */
  OSTmrCreate ((OS_TMR              *)&my_tmr,             //软件定时器对象
               (CPU_CHAR            *)"MySoftTimer",       //命名软件定时器
               (OS_TICK              )20,                  //定时器初始值，依10Hz时基计算，即为1s
               (OS_TICK              )20,                  //定时器周期重载值，依10Hz时基计算，即为1s
               (OS_OPT               )OS_OPT_TMR_PERIODIC, //周期性定时
               (OS_TMR_CALLBACK_PTR  )TmrCallback,         //回调函数
               (void                *)"aa 07 01 01 09 bb",       //传递实参给回调函数
               (OS_ERR              *)err);                //返回错误类型
	/* 启动软件定时器 */						 
	OSTmrStart ((OS_TMR   *)&my_tmr, //软件定时器对象
							(OS_ERR   *)err);    //返回错误类型										 
							 
	while (DEF_TRUE) {                            //任务体，通常写成一个死循环

		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); //不断阻塞该任务

	}
	
}
//ad task
// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue;

// 局部变量，用于保存转换计算后的电压值 	 
float ADC_Vol; 
u8 adc1;
u8 adc2;
static  void  AppTaskADC ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;
	CPU_TS         ts;
	(void)p_arg;

	while (DEF_TRUE) {                                 //任务体，通常写成一个死循环
		//LED3_ON;                               //切换 LED3 的亮灭状态
		//OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //相对性延时1000个时钟节拍（1s）
		/* 任务切换，这里是手动切换 */		
		/* 阻塞任务*/
		OSTaskSemPend ((OS_TICK   )0,                     //无期限等待
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果信号量不可用就等待
									 (CPU_TS   *)&ts,                   //获取信号量被发布的时间戳
									 (OS_ERR   *)&err);                 //返回错误类型
		ADC_Vol =(float) ADC_ConvertedValue/4096*(float)3.3; // 读取转换的AD值
		msg2.body1= (float) ADC_ConvertedValue/256*(float)3.3;
		msg2.body2= (int) ADC_ConvertedValue%256*(float)3.3;
		transfer();
		msg2.crc = msg.id+msg.len+msg2.body1+(u8)msg2.body2;
		printf("%x %x %x %x %x %x %x\n",msg2.head,msg2.id,msg2.len,msg2.body1,msg2.body2,msg2.crc,msg2.tail); 
		printf("V = %.1f v \n",ADC_Vol);     
	}

		
}

		
		








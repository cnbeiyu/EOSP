#include <includes.h>
#include <string.h>

CPU_TS             ts_start;       //ʱ�������
CPU_TS             ts_end; 
OS_TMR      my_tmr;   //���������ʱ������
typedef struct connection{
    u8 head;       
    u8 id;   
    u8 body;    
    u8 tail;         
}con;
static  OS_TCB   AppTaskStartTCB;    //������ƿ�

OS_TCB           AppTaskUsartTCB;
OS_TCB           AppTaskKeyTCB;
static OS_TCB   AppTaskckeyTCB;
static  OS_TCB   AppTaskLed1TCB;
static  OS_TCB   AppTaskLed2TCB;
static  OS_TCB   AppTaskLed3TCB;
static  OS_TCB   AppTaskTmrTCB;
static  OS_TCB   AppTaskADCTCB;




static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //�����ջ

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

static  void  AppTaskStart  (void *p_arg);               //����������

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


    OSInit(&err);                                                           //��ʼ�� uC/OS-III

	  /* ������ʼ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Start",                            //��������
                 (OS_TASK_PTR ) AppTaskStart,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskStartStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

    OSStart(&err);                                                          //�����������������uC/OS-III���ƣ�

}



static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


    (void)p_arg;

    BSP_Init();                                                 //�弶��ʼ��
    CPU_Init();                                                 //��ʼ�� CPU �����ʱ��������ж�ʱ���������������

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //��ȡ CPU �ں�ʱ��Ƶ�ʣ�SysTick ����ʱ�ӣ�
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //�����û��趨��ʱ�ӽ���Ƶ�ʼ��� SysTick ��ʱ���ļ���ֵ
    OS_CPU_SysTickInit(cnts);                                   //���� SysTick ��ʼ�����������ö�ʱ������ֵ��������ʱ��

    Mem_Init();                                                 //��ʼ���ڴ������������ڴ�غ��ڴ�ر�

#if OS_CFG_STAT_TASK_EN > 0u                                    //���ʹ�ܣ�Ĭ��ʹ�ܣ���ͳ������
    OSStatTaskCPUUsageInit(&err);                               //����û��Ӧ������ֻ�п�����������ʱ CPU �ģ����
#endif                                                          //���������� OS_Stat_IdleCtrMax ��ֵ��Ϊ������� CPU 
                                                                //ʹ����ʹ�ã���
    CPU_IntDisMeasMaxCurReset();                                //��λ�����㣩��ǰ�����ж�ʱ��

    
    
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,          //ʹ��ʱ��Ƭ��ת����
		                     (OS_TICK       )0,                    //�� OSCfg_TickRate_Hz / 10 ��ΪĬ��ʱ��Ƭֵ
												 (OS_ERR       *)&err );               //���ش�������

							 

    OSTaskCreate((OS_TCB     *)&AppTaskUsartTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Usart",                            //��������
                 (OS_TASK_PTR ) AppTaskUsart,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_USART_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskUsartStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_USART_STK_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_USART_STK_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
								 
								 
	
    OSTaskCreate((OS_TCB     *)&AppTaskKeyTCB,                              //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Key",                              //��������
                 (OS_TASK_PTR ) AppTaskKey,                                 //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_KEY_PRIO,                          //��������ȼ�
                 (CPU_STK    *)&AppTaskKeyStk[0],                           //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE / 10,                 //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE,                      //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
		
    OSTaskCreate((OS_TCB     *)&AppTaskLed1TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Led1",                             //��������
                 (OS_TASK_PTR ) AppTaskLed1,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_LED1_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskLed1Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
		
	
    OSTaskCreate((OS_TCB     *)&AppTaskLed2TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Led2",                             //��������
                 (OS_TASK_PTR ) AppTaskLed2,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_LED2_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskLed2Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������								 

	
    OSTaskCreate((OS_TCB     *)&AppTaskLed3TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Led3",                             //��������
                 (OS_TASK_PTR ) AppTaskLed3,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_LED3_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskLed3Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_LED3_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_LED3_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
	
    OSTaskCreate((OS_TCB     *)&AppTaskTmrTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Tmr",                             //��������
                 (OS_TASK_PTR ) AppTaskTmr,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_TMR_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskTmrStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_TMR_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_TMR_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
	
    OSTaskCreate((OS_TCB     *)&AppTaskADCTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task ADC",                             //��������
                 (OS_TASK_PTR ) AppTaskADC,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_ADC_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskADCStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_ADC_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_ADC_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������	


		OSTaskSuspend ( & AppTaskTmrTCB, & err );
								 
		OSTaskDel ( 0, & err );                     //ɾ����ʼ������������������

		
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

					 
	while (DEF_TRUE) {                                           //������

		OSTaskSemPend ((OS_TICK   )0,                     //�����޵ȴ�
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
									 (CPU_TS   *)0,                     //��ȡ�ź�����������ʱ���
									 (OS_ERR   *)&err);                 //���ش�������

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
					OSTaskSemPost((OS_TCB  *)&AppTaskLed1TCB,                              //Ŀ������
												(OS_OPT   )OS_OPT_POST_NONE,                             //ûѡ��Ҫ��
												(OS_ERR  *)&err);                                        //���ش�������
				}
				if(msg.body==0x02){
					LED_RGBOFF;
				}
				break;
			case 0x02:
				if(msg.body==0x01){
					LED_RGBOFF;
					OSTaskSemPost((OS_TCB  *)&AppTaskLed2TCB,                              //Ŀ������
												(OS_OPT   )OS_OPT_POST_NONE,                             //ûѡ��Ҫ��
												(OS_ERR  *)&err);                                        //���ش�������
				}
				if(msg.body==0x02){
					LED_RGBOFF;
				}
				break;
			case 0x03:
				if(msg.body==0x01){
					LED_RGBOFF;
					OSTaskSemPost((OS_TCB  *)&AppTaskLed3TCB,                              //Ŀ������
												(OS_OPT   )OS_OPT_POST_NONE,                             //ûѡ��Ҫ��
												(OS_ERR  *)&err);                                        //���ش�������
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
					OSTmrStart ((OS_TMR   *)&my_tmr, //�����ʱ������
											(OS_ERR   *)err);    //���ش�������	
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
					/* ���������ź��� */
					OSTaskSemPost((OS_TCB  *)&AppTaskADCTCB,                              //Ŀ������
												(OS_OPT   )OS_OPT_POST_NONE,                             //ûѡ��Ҫ��
												(OS_ERR  *)&err);                                        //���ش�������
				}
				break;
			case 0x07:
				{if(msg.body==0x01){
					OSTaskSemPost((OS_TCB  *)&AppTaskckeyTCB,                              //Ŀ������
												(OS_OPT   )OS_OPT_POST_NONE,                             //ûѡ��Ҫ��
												(OS_ERR  *)&err);                                        //���ش�������
				}}
				break;
			default:break;
		}
	}
    memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);/* ���� */

		OS_CRITICAL_EXIT();                                        //�˳��ٽ��
		
	
		
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

					 
  cpu_clk_freq = BSP_CPU_ClkFreq();                   //��ȡCPUʱ�ӣ�ʱ������Ը�ʱ�Ӽ���

	
	while (DEF_TRUE) {                                  //������
		/* ��������ֱ��KEY1������ */
		//OSTaskSemPend ((OS_TICK   )0,                     //�����޵ȴ�
		//							 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
		//							 (CPU_TS   *)0,                     //��ȡ�ź�����������ʱ���
		//							 (OS_ERR   *)&err);                 //���ش�������
	pMsg = OSTaskQPend ((OS_TICK )0, 
				(OS_OPT )OS_OPT_PEND_BLOCKING, 
				(OS_MSG_SIZE *)&msg_size, 
				(CPU_TS *)&ts, 
				(OS_ERR *)&err); 
		
		ts_int = CPU_IntDisMeasMaxGet ();                 //��ȡ�����ж�ʱ��

		OS_CRITICAL_ENTER();                              //�����ٽ�Σ����⴮�ڴ�ӡ�����
		//if(pMsg[1]==1)
		//printf("%d\n",strcmp(pMsg,"1"));
		if(strcmp(pMsg,"1")==0)   //key1
		{printf("aa 07 01 01 09 bb\n");
			printf("KEY1\n");}
		else
			{printf("aa 07 01 02 10 bb\n"); 
			printf("KEY2\n");}        		
		
		OS_CRITICAL_EXIT();                               //�˳��ٽ��
		
	}
	
}

static  void  AppTaskLed1 ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;	
	CPU_TS         ts;
	
	(void)p_arg;

	while (DEF_TRUE) {     		//�����壬ͨ��д��һ����ѭ��	
				/* ��������*/
		OSTaskSemPend ((OS_TICK   )0,                     //�����޵ȴ�
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
									 (CPU_TS   *)&ts,                   //��ȡ�ź�����������ʱ���
									 (OS_ERR   *)&err);                 //���ش�������
				
	printf("LED1\n");
		LED1_ON;                                 //�л� LED1 ������״̬
		
		

	}
		
		
}




static  void  AppTaskLed2 ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;
	CPU_TS         ts;
	(void)p_arg;


	while (DEF_TRUE) {                                 //�����壬ͨ��д��һ����ѭ��
		/* ��������*/
		OSTaskSemPend ((OS_TICK   )0,                     //�����޵ȴ�
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
									 (CPU_TS   *)&ts,                   //��ȡ�ź�����������ʱ���
									 (OS_ERR   *)&err);                 //���ش�������
		printf("LED2\n");
		LED2_ON;                               //�л� LED2 ������״̬
		
		//OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //�������ʱ1000��ʱ�ӽ��ģ�1s��
		/* �����л����������ֶ��л� */	
		
	}
		
		
}




static  void  AppTaskLed3 ( void * p_arg )
{
	OS_ERR      err;
	CPU_TS         ts;
	OS_REG      value;
	(void)p_arg;


	while (DEF_TRUE) {                                 //�����壬ͨ��д��һ����ѭ��
		/* ��������*/
		OSTaskSemPend ((OS_TICK   )0,                     //�����޵ȴ�
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
									 (CPU_TS   *)&ts,                   //��ȡ�ź�����������ʱ���
									 (OS_ERR   *)&err);                 //���ش�������
		printf("LED3\n");
		LED3_ON;                               //�л� LED3 ������״̬
		//OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //�������ʱ1000��ʱ�ӽ��ģ�1s��
		/* �����л����������ֶ��л� */		
		
	}

		
}

void TmrCallback (OS_TMR *p_tmr, void *p_arg) //�����ʱ��MyTmr�Ļص�����
{	
	CPU_SR_ALLOC();      //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
											 //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
											 //�����ж�ʱ����ֵ��ԭ��  
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


  /* ���������ʱ�� */
  OSTmrCreate ((OS_TMR              *)&my_tmr,             //�����ʱ������
               (CPU_CHAR            *)"MySoftTimer",       //���������ʱ��
               (OS_TICK              )20,                  //��ʱ����ʼֵ����10Hzʱ�����㣬��Ϊ1s
               (OS_TICK              )20,                  //��ʱ����������ֵ����10Hzʱ�����㣬��Ϊ1s
               (OS_OPT               )OS_OPT_TMR_PERIODIC, //�����Զ�ʱ
               (OS_TMR_CALLBACK_PTR  )TmrCallback,         //�ص�����
               (void                *)"aa 07 01 01 09 bb",       //����ʵ�θ��ص�����
               (OS_ERR              *)err);                //���ش�������
	/* ���������ʱ�� */						 
	OSTmrStart ((OS_TMR   *)&my_tmr, //�����ʱ������
							(OS_ERR   *)err);    //���ش�������										 
							 
	while (DEF_TRUE) {                            //�����壬ͨ��д��һ����ѭ��

		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); //��������������

	}
	
}
//ad task
// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
extern __IO uint16_t ADC_ConvertedValue;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
float ADC_Vol; 
u8 adc1;
u8 adc2;
static  void  AppTaskADC ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;
	CPU_TS         ts;
	(void)p_arg;

	while (DEF_TRUE) {                                 //�����壬ͨ��д��һ����ѭ��
		//LED3_ON;                               //�л� LED3 ������״̬
		//OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //�������ʱ1000��ʱ�ӽ��ģ�1s��
		/* �����л����������ֶ��л� */		
		/* ��������*/
		OSTaskSemPend ((OS_TICK   )0,                     //�����޵ȴ�
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
									 (CPU_TS   *)&ts,                   //��ȡ�ź�����������ʱ���
									 (OS_ERR   *)&err);                 //���ش�������
		ADC_Vol =(float) ADC_ConvertedValue/4096*(float)3.3; // ��ȡת����ADֵ
		msg2.body1= (float) ADC_ConvertedValue/256*(float)3.3;
		msg2.body2= (int) ADC_ConvertedValue%256*(float)3.3;
		transfer();
		msg2.crc = msg.id+msg.len+msg2.body1+(u8)msg2.body2;
		printf("%x %x %x %x %x %x %x\n",msg2.head,msg2.id,msg2.len,msg2.body1,msg2.body2,msg2.crc,msg2.tail); 
		printf("V = %.1f v \n",ADC_Vol);     
	}

		
}

		
		








/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 2 $
 * $Date: 15/04/13 10:13a $
 * @brief    Implement timer counting in periodic mode.
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
 
 
 
 //TMR page.316
 
#include <stdio.h>
#include "NUC100Series.h"


#define PLL_CLOCK           50000000

volatile uint32_t g_au32TMRINTCount[2] = {0};

uint8_t timer_f = FALSE;
volatile uint16_t key_output[3] = {0x003B, 0x003D, 0X003E};
uint16_t key_value = 0;
uint8_t key_detected = 0;
volatile uint32_t key_counter = 0;



void OpenKeyPad(void)
{
	//GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4), GPIO_PMD_QUASI);	
	PA -> PMD = (PA -> PMD & 0xFFFFF000)|0x0FD5; 	
}


uint8_t ScanKey(void)
{
	
  PA0=1; PA1=1; PA2=0; PA3=1; PA4=1; PA5=1;
	CLK_SysTickDelay(10);
	if (PA3==0) return 1;
	if (PA4==0) return 4;
	if (PA5==0) return 7;
  
	PA0=1; PA1=0; PA2=1; PA3=1; PA4=1; PA5=1;
	CLK_SysTickDelay(10);
	if (PA3==0) return 2;
	if (PA4==0) return 5;
	if (PA5==0) return 8;
	
	PA0=0; PA1=1; PA2=1; PA3=1; PA4=1; PA5=1;
	CLK_SysTickDelay(10);
	if (PA3==0) return 3;
	if (PA4==0) return 6;
	if (PA5==0) return 9;
	return 0;
}

/**
 * @brief       Timer0 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Timer0 default IRQ, declared in startup_NUC100Series.s.
 */
void TMR0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);

        g_au32TMRINTCount[0]++;
    }
}

/**
 * @brief       Timer1 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Timer1 default IRQ, declared in startup_NUC100Series.s.
 */
void TMR1_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER1);

        g_au32TMRINTCount[1]++;
    }
}


void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);
    
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);    
    
    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /* Enable Timer 0~3 module clock */
    CLK_EnableModuleClock(TMR0_MODULE);    
    CLK_EnableModuleClock(TMR1_MODULE);     
   
    /* Select Timer 0~3 module clock source */
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HXT, NULL);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR0_S_HXT, NULL);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD, TXD */
    SYS->GPB_MFP = SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void){

		uint32_t prev_key_value_1 = 0; 
    uint32_t prev_key_value_2 = 0; 
		uint32_t current_key_value = 0; 
		uint8_t timer0_flag = TRUE;
		uint8_t timer1_flag = TRUE;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+------------------------------------------------+\n");
    printf("|    Timer Periodic and Interrupt Sample Code    |\n");
    printf("+------------------------------------------------+\n\n");

    printf("# Key Settings:\n");
    printf("# Key1 press: stop counter1; release: resume counter1.\n");
		printf("# Key2 press: stop counter2; release: resume counter2.\n");
		printf("# Key3 reset all counter.\n");
		printf("# Key4 start/stop counter1.\n");
		printf("# Key5 start/stop counter2.\n");

    /* Open Timer0 in periodic mode, enable interrupt and 1 interrupt tick per second */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 2);
    TIMER_EnableInt(TIMER0);

    /* Open Timer1 in periodic mode, enable interrupt and 2 interrupt ticks per second */
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 3);
    TIMER_EnableInt(TIMER1);

    /* Enable Timer0 ~ Timer3 NVIC */
    NVIC_EnableIRQ(TMR0_IRQn);
		NVIC_EnableIRQ(TMR1_IRQn);

    /* Clear Timer0 ~ Timer3 interrupt counts to 0 */
    g_au32TMRINTCount[0] = g_au32TMRINTCount[1] = 0;

    TIMER_Start(TIMER0);
    TIMER_Start(TIMER1);
		
		
		while(1){
			
			
			
			current_key_value = ScanKey();

			if (prev_key_value_1 == 1 && current_key_value == 0) {
        timer0_flag = !timer0_flag;
        if (timer0_flag) {
            TIMER_Start(TIMER0);
        } else {
            TIMER_Stop(TIMER0);
        } 					 
			}
			prev_key_value_1 = current_key_value;

			if (prev_key_value_2 == 2 && current_key_value == 0) {
        timer1_flag = !timer1_flag; 
        if (timer1_flag) {
            TIMER_Start(TIMER1); 
        } else {
            TIMER_Stop(TIMER1); 
        }  					 
			}
			prev_key_value_2 = current_key_value;

			printf("\rcounter1: %d   counter2: %d"   ,g_au32TMRINTCount[0],g_au32TMRINTCount[1]);
			
		}
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/

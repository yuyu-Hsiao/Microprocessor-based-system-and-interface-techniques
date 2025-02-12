/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 2 $
 * $Date: 15/04/15 7:05p $
 * @brief
 *           Configure I2S as Master mode and demonstrate how I2S works in Master mode.
 *           This sample code needs to work with I2S_Slave sample code.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK           50000000

uint32_t g_u32TxValue;

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    
    /* Enable IRC22M clock */
    CLK->PWRCON |= CLK_PWRCON_IRC22M_EN_Msk;

    /* Waiting for IRC22M clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_IRC22M_STB_Msk));

    /* Switch HCLK clock source to HIRC */
    CLK->CLKSEL0 = CLK_CLKSEL0_HCLK_S_HIRC;

    /* Set PLL to Power-down mode and PLL_STB bit in CLKSTATUS register will be cleared by hardware.*/
    CLK->PLLCON |= CLK_PLLCON_PD_Msk;

    /* Enable external 12 MHz XTAL */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;

    /* Enable PLL and Set PLL frequency */
    CLK->PLLCON = PLLCON_SETTING;

    /* Waiting for clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk));

    /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    CLK->CLKSEL0 = CLK_CLKSEL0_STCLK_S_HCLK_DIV2 | CLK_CLKSEL0_HCLK_S_PLL;

    /* Enable peripheral clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk | CLK_APBCLK_I2S_EN_Msk;

    /* Select HXT as the clock source of UART */
    CLK->CLKSEL1 &= (~CLK_CLKSEL1_UART_S_Msk);
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART_S_HXT;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();


    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP = SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;

    /* Set GPA multi-function pins for I2S MCLK. */
    SYS->GPA_MFP = SYS_GPA_MFP_PA15_I2S_MCLK;
    /* Set multi function pin for I2S: GPC0, GPC1, GPC2, GPC3, GPA15 */
    SYS->GPC_MFP = SYS_GPC_MFP_PC0_I2S_LRCLK | SYS_GPC_MFP_PC1_I2S_BCLK | SYS_GPC_MFP_PC2_I2S_DI | SYS_GPC_MFP_PC3_I2S_DO;
    SYS->ALT_MFP = SYS_ALT_MFP_PA15_I2S_MCLK | SYS_ALT_MFP_PC0_I2S_LRCLK | SYS_ALT_MFP_PC1_I2S_BCLK | SYS_ALT_MFP_PC2_I2S_DI | SYS_ALT_MFP_PC3_I2S_DO;

}

void UART_Init(void)
{
    /* Word length is 8 bits; 1 stop bit; no parity bit. */
    UART0->LCR = UART_LCR_WLS_Msk;
    /* Using mode 2 calculation: UART bit rate = UART peripheral clock rate / (BRD setting + 2) */
    /* UART peripheral clock rate 12MHz; UART bit rate 115200 bps. */
    /* 12000000 / 115200 bps ~= 104 */
    /* 104 - 2 = 0x66. */
    UART0->BAUD = UART_BAUD_DIV_X_EN_Msk | UART_BAUD_DIV_X_ONE_Msk | (0x66);
}

void I2S_IRQHandler()
{
    /* Write 4 Tx values to TX FIFO */
    I2S->TXFIFO = g_u32TxValue;
    I2S->TXFIFO = g_u32TxValue;
    I2S->TXFIFO = g_u32TxValue;
    I2S->TXFIFO = g_u32TxValue;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    uint32_t u32DataCount, u32RxValue1, u32RxValue2;

    /* Unlock protected registers */
    SYS_UnlockReg();
    
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();
    
    /* Init UART for print message */
    UART_Init();

    printf("+-----------------------------------------------------------+\n");
    printf("|            I2S Driver Sample Code (master mode)           |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("  I2S configuration:\n");
    printf("      Sample rate 16 kHz\n");
    printf("      Word width 16 bits\n");
    printf("      Stereo mode\n");
    printf("      I2S format\n");
    printf("      TX value: 0x55005501, 0x55025503, ..., 0x55FE55FF, wraparound\n");
    printf("  The I/O connection for I2S:\n");
    printf("      I2S_LRCLK (PC0)\n      I2S_BCLK(PC1)\n");
    printf("      I2S_DI (PC2)\n      I2S_DO (PC3)\n\n");
    printf("  NOTE: Connect with a I2S slave device.\n");
    printf("        This sample code will transmit a TX value 50000 times, and then change to the next TX value.\n");
    printf("        When TX value or the received value changes, the new TX value or the new TX value and the new received value will be printed.\n");

    /* select source from HXT (12MHz) */
    CLK->CLKSEL2 &= (~CLK_CLKSEL2_I2S_S_Msk);
    CLK->CLKSEL2 |= CLK_CLKSEL2_I2S_S_HXT;

    /* Master mode, 16-bit word width, stereo mode, I2S format. */
    I2S->CON = I2S_MODE_MASTER | I2S_DATABIT_16 | I2S_STEREO | I2S_FORMAT_I2S | I2S_FIFO_TX_LEVEL_WORD_4 | I2S_FIFO_RX_LEVEL_WORD_4;
    /* Sampling rate 16000 Hz */
    I2S->CLKDIV = (I2S->CLKDIV & ~I2S_CLKDIV_BCLK_DIV_Msk) | (11 << I2S_CLKDIV_BCLK_DIV_Pos);
    /* Enable I2S */
    I2S->CON |= I2S_CON_I2SEN_Msk;
    NVIC_EnableIRQ(I2S_IRQn);

    /* Initiate data counter */
    u32DataCount = 0;
    /* Initiate Tx value and Rx value */
    g_u32TxValue = 0x55005501;
    u32RxValue1 = 0;
    u32RxValue2 = 0;
    /* Enable Tx threshold level interrupt */
    I2S->IE |= I2S_IE_TXTHIE_Msk;

    /* Enable I2S Tx function to transmit data */
    I2S->CON |= I2S_CON_TXEN_Msk;
    /* Enable I2S Rx function to receive data */
    I2S->CON |= I2S_CON_RXEN_Msk;

    printf("Start I2S ...\nTx value: 0x%X\n", g_u32TxValue);

    while(1)
    {
        if((I2S->STATUS & I2S_STATUS_RXEMPTY_Msk) == 0)
        {
            u32DataCount++;
            u32RxValue2 = I2S->RXFIFO;
            if(u32RxValue1 != u32RxValue2)
            {
                u32RxValue1 = u32RxValue2;
                printf("Tx value: 0x%X;  Rx value: 0x%X\n", g_u32TxValue, u32RxValue1);
            }
            if(u32DataCount >= 50000)
            {
                g_u32TxValue = 0x55005500 | ((g_u32TxValue + 0x00020002) & 0x00FF00FF); /* g_u32TxValue: 0x55005501, 0x55025503, ..., 0x55FE55FF */
                printf("Tx value: 0x%X\n", g_u32TxValue);
                u32DataCount = 0;
            }
        }
    }
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/

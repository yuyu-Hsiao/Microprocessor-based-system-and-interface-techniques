#include <stdio.h>
#include "NUC100Series.h"
#include "GPIO.h"
#include "SYS.h"
#include "Seven_Segment.h"
#define SEG_N0   0x82 
#define SEG_N1   0xEE 
#define SEG_N2   0x07 
#define SEG_N3   0x46 
#define SEG_N4   0x6A  
#define SEG_N5   0x52 
#define SEG_N6   0x12 
#define SEG_N7   0xE6 
#define SEG_N8   0x02 
#define SEG_N9   0x62
#define SEG_N10	 0x22
#define SEG_N11  0x1A
#define SEG_N12  0x93
#define SEG_N13  0x0E
#define SEG_N14  0x13
#define SEG_N15  0x33

uint8_t SEG_BUF[16]={SEG_N0, SEG_N1, SEG_N2, SEG_N3, SEG_N4, SEG_N5, SEG_N6, SEG_N7, SEG_N8, SEG_N9, SEG_N10, SEG_N11, SEG_N12, SEG_N13, SEG_N14, SEG_N15}; 


void OpenSevenSegment(void)
{
	//GPIO_SetMode(PC, (BIT4 | BIT5 | BIT6 | BIT7), GPIO_PMD_OUTPUT);  
	PC -> PMD = (PC -> PMD & 0xFFFF00FF)|0x5500;	// �]�m port c 4-7��output�Ҧ�
	PC->DOUT &= 0xFF0F;								// �N PC4-PC7 ��DOUT���A�]�� low��L�O�d��Ӫ��A
	
	
	GPIO_SetMode(PE, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7), GPIO_PMD_QUASI); 	// �]�m port E 0-7 �� QUASI �Ҧ�
	PE->DOUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7); 						//�N PE0-PE7 �� DOUT���A�]��low
}



void ShowSevenSegment(uint8_t no, uint8_t number)
{
	uint8_t seg7_scan[4] = {0x10, 0x20, 0x40, 0x80};		//����7�q��ܾ�
	PE->DOUT &= 0xFF00; 									//�O�dPE���줸 �M��PE0-7
	PE->DOUT |= SEG_BUF[number];							//�ھ�number�Ѽ� �qSEG_BUF��Ū�������Ʀr �]�m�DPE0~PE7��
	
	PC->DOUT &= 0xFF0F;										//�O�dPC��L�줸 �M��PC4-7
	PC->DOUT |= seg7_scan[no]; 								//�ھ�no�ѿ�ܭ��@����ܾ���ܼƦr

}

//�����Ҧ�7�q��ܾ� �Npc4-7�]��0 �קK��ܤz�Z
void CloseSevenSegment(void)
{
	PC->DOUT &= 0xFF0F;
}

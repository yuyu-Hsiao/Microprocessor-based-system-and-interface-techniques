#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NUC100Series.h"

//#define PLL_CLOCK       50000000SSS
#define PLL_CLOCK 50000000

/******************************************************************
 * dataset format setting
 ******************************************************************/

#define train_data_num 80				//Total number of training data
#define test_data_num 40					//Total number of testing data

/******************************************************************
 * Network Configuration - customized per network 
 ******************************************************************/
#define input_length 3 						//The number of input
#define HiddenNodes 10						//The number of neurons in hidden layer
#define target_num 4 						//The number of output


const float LearningRate = 0.001;					//Learning Rate
const float Momentum = 0.9;
const float InitialWeightMax = 0.5;
const float goal_acc = 	0.95;							//Target accuracy

// Create training dataset/output
float train_data_input[train_data_num][input_length] = {
//Ambient    
{ 450,  434,  464}, { 450,  434,  464}, { 446,  431,  463}, { 446,  431,  464}, { 447,  431,  463}, { 447,  429,  463}, { 448,  429,  461}, { 448,  429,  462}, 
{ 446,  430,  460}, { 444,  430,  459}, { 445,  428,  459}, { 444,  430,  458}, { 444,  430,  459}, { 444,  430,  456}, { 446,  430,  459}, { 443,  430,  460}, 
{ 442,  430,  460}, { 442,  429,  460}, { 443,  430,  461}, { 444,  428,  463}, 

//blue
{ 242,  271,  424}, { 242,  271,  424}, { 239,  271,  426}, { 239,  271,  425}, { 240,  267,  426}, { 239,  269,  426}, { 240,  269,  426}, { 240,  269,  425}, 
{ 238,  269,  425}, { 240,  269,  425}, { 240,  270,  426}, { 239,  269,  426}, { 239,  271,  424}, { 237,  271,  425}, { 236,  272,  425}, { 238,  268,  426}, 
{ 237,  268,  427}, { 235,  270,  426}, { 237,  269,  425}, { 237,  271,  424}, 

//red
{ 472,  354,  462}, { 472,  354,  462}, { 471,  354,  463}, { 471,  354,  463}, { 472,  353,  462}, { 471,  354,  462}, { 472,  354,  462}, { 472,  354,  463}, 
{ 472,  355,  463}, { 472,  355,  464}, { 473,  354,  464}, { 471,  354,  464}, { 471,  354,  463}, { 472,  355,  461}, { 474,  357,  462}, { 472,  356,  461}, 
{ 471,  356,  460}, { 470,  355,  459}, { 472,  357,  462}, { 472,  354,  461}, 

//green
{ 170,  236,  346}, { 170,  236,  346}, { 171,  234,  346}, { 172,  234,  346}, { 173,  235,  346}, { 172,  235,  346}, { 172,  235,  346}, { 172,  236,  346}, 
{ 172,  236,  347}, { 173,  236,  348}, { 172,  235,  347}, { 173,  234,  348}, { 171,  237,  345}, { 171,  237,  346}, { 170,  236,  345}, { 171,  238,  347}, 
{ 172,  238,  347}, { 171,  236,  348}, { 171,  234,  349}, { 172,  236,  348}

};	// You can put your training dataset here
int train_data_output[train_data_num][target_num] = {
{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},
{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},

{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},
{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},

{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},
{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},

{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},
{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1}
}; 		// Label of the training data

// Create testing dataset/output
float test_data_input[test_data_num][input_length] = {
{ 442,  428,  463}, { 444,  429,  464}, { 445,  430,  465}, { 443,  431,  464}, { 442,  431,  464}, { 443,  430,  464}, { 443,  432,  465}, { 445,  431,  467}, 
{ 445,  432,  468}, { 445,  431,  469},

{ 236,  270,  425}, { 237,  268,  426}, { 238,  268,  427}, { 238,  271,  427}, { 235,  268,  426}, { 234,  268,  424}, { 236,  268,  424}, { 238,  266,  424}, 
{ 235,  268,  423}, { 234,  268,  423},

{ 474,  353,  461}, { 473,  353,  461}, { 473,  353,  461}, { 473,  354,  461}, { 474,  354,  462}, { 472,  353,  460}, { 471,  355,  459}, { 473,  354,  461}, 
{ 471,  355,  460}, { 471,  355,  459},

{ 172,  235,  349}, { 172,  236,  349}, { 170,  235,  349}, { 172,  240,  349}, { 171,  235,  349}, { 172,  236,  350}, { 168,  236,  347}, { 170,  235,  347}, 
{ 172,  235,  347}, { 171,  237,  347}

};		// You can put your testing dataset here
int test_data_output[test_data_num][target_num] = {
{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},


{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},


{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0},


{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},


}; 			// Label of the testing data

/******************************************************************
 * End Network Configuration
 ******************************************************************/


int ReportEvery10;
int RandomizedIndex[train_data_num];
long  TrainingCycle;
float Rando;
float Error;
float Accum;

float data_mean[3] ={0};
float data_std[3] ={0};

float Hidden[HiddenNodes];
float Output[target_num];
float HiddenWeights[input_length+1][HiddenNodes];
float OutputWeights[HiddenNodes+1][target_num];
float HiddenDelta[HiddenNodes];
float OutputDelta[target_num];
float ChangeHiddenWeights[input_length+1][HiddenNodes];
float ChangeOutputWeights[HiddenNodes+1][target_num];

int target_value;
int out_value;
int max;


/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototypes                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void);
void UART0_Init(void);
void AdcSingleCycleScanModeTest(void);


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

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable ADC module clock */
    CLK_EnableModuleClock(ADC_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));

    /* ADC clock source is 22.1184MHz, set divider to 7, ADC clock is 22.1184/7 MHz */
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HIRC, CLK_CLKDIV_ADC(7));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;

    /* Disable the GPA0 - GPA3 digital input path to avoid the leakage current. */
    GPIO_DISABLE_DIGITAL_PATH(PA, 0xF);

    /* Configure the GPA0 - GPA3 ADC analog input pins */
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA0_Msk | SYS_GPA_MFP_PA1_Msk | SYS_GPA_MFP_PA2_Msk | SYS_GPA_MFP_PA3_Msk) ;
    SYS->GPA_MFP |= SYS_GPA_MFP_PA0_ADC0 | SYS_GPA_MFP_PA1_ADC1 | SYS_GPA_MFP_PA2_ADC2 | SYS_GPA_MFP_PA3_ADC3 ;
    SYS->ALT_MFP1 = 0;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Init UART                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void UART0_Init()
{
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

void scale_data()
{
		float sum[3] = {0};
		int i, j;
		
		// Compute Data Mean
		for(i = 0; i < train_data_num; i++){
			for(j = 0; j < input_length; j++){
				sum[j] += train_data_input[i][j];
			}
		}
		for(j = 0; j < input_length ; j++){
			data_mean[j] = sum[j] / train_data_num;
			printf("MEAN: %.2f\n", data_mean[j]);
			sum[j] = 0.0;
		}
		
		// Compute Data STD
		for(i = 0; i < train_data_num; i++){
			for(j = 0; j < input_length ; j++){
				sum[j] += pow(train_data_input[i][j] - data_mean[j], 2);
			}
		}
		for(j = 0; j < input_length; j++){
			data_std[j] = sqrt(sum[j]/train_data_num);
			printf("STD: %.2f\n", data_std[j]);
			sum[j] = 0.0;
		}
}

void normalize(float *data)
{
		int i;
	
		for(i = 0; i < input_length; i++){
			data[i] = (data[i] - data_mean[i]) / data_std[i];
		}
}

int train_preprocess()
{
    int i;
    
    for(i = 0 ; i < train_data_num ; i++)
    {
        normalize(train_data_input[i]);
    }
		
    return 0;
}

int test_preprocess()
{
    int i;

    for(i = 0 ; i < test_data_num ; i++)
    {
        normalize(test_data_input[i]);
    }
		
    return 0;
}

int data_setup()
{
    int i;
		//int j;
		int p, ret;
		unsigned int seed = 1;
	
		/* Set the ADC operation mode as single-cycle, input mode as single-end and
                 enable the analog input channel 0, 1 and 2 */
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, 0x7);

    /* Power on ADC module */
    ADC_POWER_ON(ADC);

    /* Clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

    /* Start A/D conversion */
    ADC_START_CONV(ADC);

    /* Wait conversion done */
    while(!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));
		
		for(i = 0; i < 3; i++)
    {
				seed *= ADC_GET_CONVERSION_DATA(ADC, i);
    }
		seed *= 1000;
		printf("\nRandom seed: %d\n", seed);
    srand(seed);

    ReportEvery10 = 1;
    for( p = 0 ; p < train_data_num ; p++ ) 
    {    
        RandomizedIndex[p] = p ;
    }
		
		scale_data();
    ret = train_preprocess();
    ret |= test_preprocess();
    if(ret) //Error Check
        return 1;

    return 0;
}

void run_train_data()
{
    int i, j, p;
    int correct=0;
    float accuracy = 0;
    printf("Train result:\n");
    for( p = 0 ; p < train_data_num ; p++ )
    { 
        max = 0;
        for (i = 1; i < target_num; i++) 
        {
            if (train_data_output[p][i] > train_data_output[p][max]) {
                max = i;
            }
        }
        target_value = max;
        
    /******************************************************************
    * Compute hidden layer activations
    ******************************************************************/

        for( i = 0 ; i < HiddenNodes ; i++ ) {    
            Accum = HiddenWeights[input_length][i] ;
            for( j = 0 ; j < input_length ; j++ ) {
                Accum += train_data_input[p][j] * HiddenWeights[j][i] ;
            }
            Hidden[i] = 1.0/(1.0 + exp(-Accum)) ;
        }

    /******************************************************************
    * Compute output layer activations and calculate errors
    ******************************************************************/

        for( i = 0 ; i < target_num ; i++ ) {    
            Accum = OutputWeights[HiddenNodes][i] ;
            for( j = 0 ; j < HiddenNodes ; j++ ) {
                Accum += Hidden[j] * OutputWeights[j][i] ;
            }
            Output[i] = 1.0/(1.0 + exp(-Accum)) ; 
        }
        
        max = 0;
        for (i = 1; i < target_num; i++) 
        {
            if (Output[i] > Output[max]) {
                max = i;
            }
        }
        out_value = max;

        if(out_value!=target_value)
            printf("Error --> Training Pattern: %d,Target : %d, Output : %d\n", p, target_value, out_value);
        else
            correct++;
        }
        // Calculate accuracy
        accuracy = (float)correct / train_data_num;
        printf ("Accuracy = %.2f /100 \n",accuracy*100);

}

void run_test_data()
{
    int i, j, p;
    int correct=0;
    float accuracy = 0;
    printf("Test result:\n");
    for( p = 0 ; p < test_data_num ; p++ )
    { 
        max = 0;
        for (i = 1; i < target_num; i++) 
        {
            if (test_data_output[p][i] > test_data_output[p][max]) {
                max = i;
            }
        }
        target_value = max;
        
    /******************************************************************
    * Compute hidden layer activations
    ******************************************************************/

        for( i = 0 ; i < HiddenNodes ; i++ ) {    
            Accum = HiddenWeights[input_length][i] ;
            for( j = 0 ; j < input_length ; j++ ) {
                Accum += test_data_input[p][j] * HiddenWeights[j][i] ;
            }
            Hidden[i] = 1.0/(1.0 + exp(-Accum)) ;
        }

    /******************************************************************
    * Compute output layer activations and calculate errors
    ******************************************************************/

        for( i = 0 ; i < target_num ; i++ ) {    
            Accum = OutputWeights[HiddenNodes][i] ;
            for( j = 0 ; j < HiddenNodes ; j++ ) {
                Accum += Hidden[j] * OutputWeights[j][i] ;
            }
            Output[i] = 1.0/(1.0 + exp(-Accum)) ; 
        }
        max = 0;
        for (i = 1; i < target_num; i++) 
        {
            if (Output[i] > Output[max]) {
                max = i;
            }
        }
        out_value = max;

        if(out_value!=target_value)
            printf("Error --> Training Pattern: %d,Target : %d, Output : %d\n", p, target_value, out_value);
        else
            correct++;
        }
        // Calculate accuracy
        accuracy = (float)correct / test_data_num;
        printf ("Accuracy = %.2f /100 \n",accuracy*100);
}

float Get_Train_Accuracy()
{
    int i, j, p;
    int correct = 0;
		float accuracy = 0;
    for (p = 0; p < train_data_num; p++)
    {
/******************************************************************
* Compute hidden layer activations
******************************************************************/

        for( i = 0 ; i < HiddenNodes ; i++ ) {    
            Accum = HiddenWeights[input_length][i] ;
            for( j = 0 ; j < input_length ; j++ ) {
                Accum += train_data_input[p][j] * HiddenWeights[j][i] ;
            }
            Hidden[i] = 1.0/(1.0 + exp(-Accum)) ;
        }

/******************************************************************
* Compute output layer activations
******************************************************************/

        for( i = 0 ; i < target_num ; i++ ) {    
            Accum = OutputWeights[HiddenNodes][i] ;
            for( j = 0 ; j < HiddenNodes ; j++ ) {
                Accum += Hidden[j] * OutputWeights[j][i] ;
            }
            Output[i] = 1.0/(1.0 + exp(-Accum)) ; 
        }
        //get target value
        max = 0;
        for (i = 1; i < target_num; i++) 
        {
            if (train_data_output[p][i] > train_data_output[p][max]) {
                max = i;
            }
        }
        target_value = max;
        //get output value
        max = 0;
        for (i = 1; i < target_num; i++) 
        {
            if (Output[i] > Output[max]) {
                max = i;
            }
        }
        out_value = max;
        //compare output and target
        if (out_value==target_value)
        {
            correct++;
        }
    }

    // Calculate accuracy
    accuracy = (float)correct / train_data_num;
    return accuracy;
}

void load_weight()
{
    int i,j;
    printf("\n=======Hidden Weight=======\n");
    printf("{");
    for(i = 0; i <= input_length ; i++)
    {
        printf("{");
        for (j = 0; j < HiddenNodes; j++)
        {
            if(j!=HiddenNodes-1){
                printf("%f,", HiddenWeights[i][j]);
            }else{
                printf("%f", HiddenWeights[i][j]);
            }
        }
        if(i!=input_length){
            printf("},\n");
        }else {
            printf("}");
        }
    }
    printf("}\n");

    printf("\n=======Output Weight=======\n");

    for(i = 0; i <= HiddenNodes ; i++)
    {
        printf("{");
        for (j = 0; j < target_num; j++)
        {
            if(j!=target_num-1){
                printf("%f,", OutputWeights[i][j]);
            }else{
                printf("%f", OutputWeights[i][j]);
            }
        }
        if(i!=HiddenNodes){
            printf("},\n");
        }else {
            printf("}");
        }
    }
    printf("}\n");
}

void AdcSingleCycleScanModeTest()
{
		int i, j;
    uint32_t u32ChannelCount;
    float single_data_input[3];
		char output_string[10] = {NULL};

    printf("\n");	
		printf("[Phase 3] Start Prediction ...\n\n");
		PB2=1;
    while(1)
    {
			
				/* Set the ADC operation mode as single-cycle, input mode as single-end and
                 enable the analog input channel 0, 1, 2 and 3 */
        ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, 0x7);

        /* Power on ADC module */
        ADC_POWER_ON(ADC);

        /* Clear the A/D interrupt flag for safe */
        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

        /* Start A/D conversion */
        ADC_START_CONV(ADC);

        /* Wait conversion done */
        while(!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));

        for(u32ChannelCount = 0; u32ChannelCount < 3; u32ChannelCount++)
        {
            single_data_input[u32ChannelCount] = ADC_GET_CONVERSION_DATA(ADC, u32ChannelCount);
        }
				normalize(single_data_input);
						

				// Compute hidden layer activations
				for( i = 0 ; i < HiddenNodes ; i++ ) {    
						Accum = HiddenWeights[input_length][i] ;
						for( j = 0 ; j < input_length ; j++ ) {
								Accum += single_data_input[j] * HiddenWeights[j][i] ;
						}
						Hidden[i] = 1.0/(1.0 + exp(-Accum)) ;
				}

				// Compute output layer activations
				for( i = 0 ; i < target_num ; i++ ) {    
						Accum = OutputWeights[HiddenNodes][i] ;
						for( j = 0 ; j < HiddenNodes ; j++ ) {
								Accum += Hidden[j] * OutputWeights[j][i] ;
						}
						Output[i] = 1.0/(1.0 + exp(-Accum)) ; 
				}
						
				max = 0;
				for (i = 1; i < target_num; i++) 
				{
						if (Output[i] > Output[max]) {
								max = i;
						}
				}
				out_value = max;
				
				switch(out_value){
						case 0:
								strcpy(output_string, "Ambient");
								break;
						case 1:
								strcpy(output_string, "Blue");	
								break;
						case 2:
								strcpy(output_string, "Red");	
								break;
						case 3:
								strcpy(output_string, "Green");
								break;
                        /*
						case 0:
								strcpy(output_string, "Ambient");
								break;
						case 1:
								strcpy(output_string, "Blue");	
								break;
						case 2:
								strcpy(output_string, "Magenta");	
								break;
						case 3:
								strcpy(output_string, "Red");
								break;
						case 4:
								strcpy(output_string, "Orange");	
								break;
						case 5:
								strcpy(output_string, "Yellow");	
								break;
						case 6:
								strcpy(output_string, "Green");
								break;
                        */
				}
				
				printf("\rPrediction output: %-8s", output_string);
				CLK_SysTickDelay(500000);


    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{
		int i, j, p, q, r;
    float accuracy=0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();
	
	  GPIO_SetMode(PB, BIT2, GPIO_PMD_OUTPUT);
	  PB2=0;
	
		printf("\n+-----------------------------------------------------------------------+\n");
    printf("|                        LAB8 - Machine Learning                        |\n");
    printf("+-----------------------------------------------------------------------+\n");
		printf("System clock rate: %d Hz\n", SystemCoreClock);

    printf("\n[Phase 1] Initialize DataSet ...");
	  /* Data Init (Input / Output Preprocess) */
		if(data_setup()){
        printf("[Error] Datasets Setup Error\n");
        return 0;
    }else
				printf("Done!\n\n");
		
		printf("[Phase 2] Start Model Training ...\n");
		// Initialize HiddenWeights and ChangeHiddenWeights 
    for( i = 0 ; i < HiddenNodes ; i++ ) {    
        for( j = 0 ; j <= input_length ; j++ ) { 
            ChangeHiddenWeights[j][i] = 0.0 ;
            Rando = (float)((rand() % 100))/100;
            HiddenWeights[j][i] = 2.0 * ( Rando - 0.5 ) * InitialWeightMax ;
        }
    }

    // Initialize OutputWeights and ChangeOutputWeights
    for( i = 0 ; i < target_num ; i ++ ) {    
        for( j = 0 ; j <= HiddenNodes ; j++ ) {
            ChangeOutputWeights[j][i] = 0.0 ;  
            Rando = (float)((rand() % 100))/100;        
            OutputWeights[j][i] = 2.0 * ( Rando - 0.5 ) * InitialWeightMax ;
        }
    }

    // Begin training 
    for(TrainingCycle = 1 ; TrainingCycle < 2147483647 ; TrainingCycle++)
    {
        Error = 0.0 ;

        // Randomize order of training patterns
        for( p = 0 ; p < train_data_num ; p++) {
            q = rand()%train_data_num;
            r = RandomizedIndex[p] ; 
            RandomizedIndex[p] = RandomizedIndex[q] ; 
            RandomizedIndex[q] = r ;
        }

        // Cycle through each training pattern in the randomized order
        for( q = 0 ; q < train_data_num ; q++ ) 
        {    
            p = RandomizedIndex[q];

            // Compute hidden layer activations
            for( i = 0 ; i < HiddenNodes ; i++ ) {    
                Accum = HiddenWeights[input_length][i] ;
                for( j = 0 ; j < input_length ; j++ ) {
                    Accum += train_data_input[p][j] * HiddenWeights[j][i] ;
                }
                Hidden[i] = 1.0/(1.0 + exp(-Accum)) ;
            }

            // Compute output layer activations and calculate errors
            for( i = 0 ; i < target_num ; i++ ) {    
                Accum = OutputWeights[HiddenNodes][i] ;
                for( j = 0 ; j < HiddenNodes ; j++ ) {
                    Accum += Hidden[j] * OutputWeights[j][i] ;
                }
                Output[i] = 1.0/(1.0 + exp(-Accum)) ;   
                OutputDelta[i] = (train_data_output[p][i] - Output[i]) * Output[i] * (1.0 - Output[i]) ;   
                Error += 0.5 * (train_data_output[p][i] - Output[i]) * (train_data_output[p][i] - Output[i]) ;
            }

            // Backpropagate errors to hidden layer
            for( i = 0 ; i < HiddenNodes ; i++ ) {    
                Accum = 0.0 ;
                for( j = 0 ; j < target_num ; j++ ) {
                    Accum += OutputWeights[i][j] * OutputDelta[j] ;
                }
                HiddenDelta[i] = Accum * Hidden[i] * (1.0 - Hidden[i]) ;
            }

            // Update Input-->Hidden Weights
            for( i = 0 ; i < HiddenNodes ; i++ ) {     
                ChangeHiddenWeights[input_length][i] = LearningRate * HiddenDelta[i] + Momentum * ChangeHiddenWeights[input_length][i] ;
                HiddenWeights[input_length][i] += ChangeHiddenWeights[input_length][i] ;
                for( j = 0 ; j < input_length ; j++ ) { 
                    ChangeHiddenWeights[j][i] = LearningRate * train_data_input[p][j] * HiddenDelta[i] + Momentum * ChangeHiddenWeights[j][i];
                    HiddenWeights[j][i] += ChangeHiddenWeights[j][i] ;
                }
            }

            // Update Hidden-->Output Weights
            for( i = 0 ; i < target_num ; i ++ ) {    
                ChangeOutputWeights[HiddenNodes][i] = LearningRate * OutputDelta[i] + Momentum * ChangeOutputWeights[HiddenNodes][i] ;
                OutputWeights[HiddenNodes][i] += ChangeOutputWeights[HiddenNodes][i] ;
                for( j = 0 ; j < HiddenNodes ; j++ ) {
                    ChangeOutputWeights[j][i] = LearningRate * Hidden[j] * OutputDelta[i] + Momentum * ChangeOutputWeights[j][i] ;
                    OutputWeights[j][i] += ChangeOutputWeights[j][i] ;
                }
            }
        }
        accuracy = Get_Train_Accuracy();

        // Every 10 cycles send data to terminal for display
        ReportEvery10 = ReportEvery10 - 1;
        if (ReportEvery10 == 0)
        {
            
            printf ("\nTrainingCycle: %ld\n",TrainingCycle);
            printf ("Error = %.5f\n",Error);
            printf ("Accuracy = %.2f /100 \n",accuracy*100);
            //run_train_data();

            if (TrainingCycle==1)
            {
                ReportEvery10 = 9;
            }
            else
            {
                ReportEvery10 = 10;
            }
        }

        // If error rate is less than pre-determined threshold then end
        if( accuracy >= goal_acc ) break ;
    }

    printf ("\nTrainingCycle: %ld\n",TrainingCycle);
    printf ("Error = %.5f\n",Error);
    run_train_data();
    printf ("Training Set Solved!\n");
    printf ("--------\n"); 
    printf ("Testing Start!\n ");
    run_test_data();
    printf ("--------\n"); 
    ReportEvery10 = 1;
    load_weight();
		
		printf("\nModel Training Phase has ended.\n");

    /* Start prediction */
    AdcSingleCycleScanModeTest();

    while(1);
}
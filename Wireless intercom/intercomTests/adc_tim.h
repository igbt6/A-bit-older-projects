
#include "stm_lib/inc/stm32f0xx_gpio.h"


//#ifndef ADC_TIM_H_
//#define ADC_TIM_H_



/* Private function prototypes -----------------------------------------------*/


//extern volatile uint8_t flag_ADC;
void ADC_TIM_Config(void);
void Display(void);
void Display_Init(void);
void TIM1_Conf(void);
void TIM2_Conf(void);
void ADC1_COMP_IRQHandler(void);
void DAC_Conf(void);
void Enable_TIM6_DAC(void);

volatile uint16_t buf_ADC[2][32]; // bufor na dane z ADC
volatile uint16_t ADC1ConvertedValue;
extern volatile uint8_t full_frameADC_received;
extern volatile uint8_t  num_buf;
//#endif /* ADC_TIM_H_ */

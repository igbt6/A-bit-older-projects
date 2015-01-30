
#include "stm_lib/inc/stm32f0xx_gpio.h"


//#ifndef ADC_TIM_H_
//#define ADC_TIM_H_



/* Private function prototypes -----------------------------------------------*/

#define FSAMPLE 6000
#define SIZE_BUF_ADC 32
#define SIZE_BUF_DAC 32
//extern volatile uint8_t flag_ADC;
void ADC_TIM_Config(void);
void TIM1_Conf(void);
void TIM2_Conf(void);
void DAC_Conf(void);

void Enable_TIM6_DAC(void);
void Disable_TIM6_DAC(void);

void ADC1_DMA_Config(void);
void DAC_DMA_Config(void);
/* Private function prototypes -----------------------------------------------*/


uint8_t buf_ADC[2][SIZE_BUF_ADC]; // bufor na dane z ADC
uint8_t buf_DAC[2][SIZE_BUF_DAC]; // bufor na dane z ADC
volatile uint8_t ADC1ConvertedValue;
volatile uint8_t  num_bufDAC;
volatile uint8_t flaga;
extern volatile uint8_t full_frameADC_received;
extern volatile uint8_t  num_buf;

//#endif /* ADC_TIM_H_ */

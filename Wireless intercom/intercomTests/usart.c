/*
 * usart.c
 *
 *  Created on: Oct 25, 2012
 *      Author: lukasz
 */


/**
  *****************************************************************************
  * @title   HyperTerminal_Interrupt.c
  * @author  CooCox
  * @date    28 Sep 2012
  * @brief   This example provides a description of how to use the USART1
  *          with Transmit and Receive interrupts to communicate with the
  *          hyperterminal.
  *******************************************************************************
  */
////// The above comment is automatically generated by CoIDE ///////////////////

/**
  ******************************************************************************
  * @file    USART/HyperTerminal_Interrupt/main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-May-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
#include "stm_lib/inc/stm32f0xx_gpio.h"
#include "stm_lib/inc/stm32f0xx_rcc.h"
#include "stm_lib/inc/stm32f0xx_exti.h"
#include "stm_lib/inc/stm32f0xx_syscfg.h"
#include"stm_lib/inc/stm32f0xx_misc.h"
#include "stm_lib/inc/stm32f0xx_usart.h"
#include "usart.h"
#include <stdio.h>

/** @addtogroup STM32F0xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup HyperTerminal_Interrupt
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TXBUFFERSIZE   (countof(TxBuffer) - 1)
#define RXBUFFERSIZE   0x20

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t TxBuffer[] = "\n\rUSART test AbCDEFGH IJJJJJJ BELE CO\n\r";
USART_InitTypeDef USART_InitStructure;
uint8_t RxBuffer[RXBUFFERSIZE];
uint8_t NbrOfDataToTransfer = TXBUFFERSIZE;
uint8_t NbrOfDataToRead = RXBUFFERSIZE;
__IO uint8_t TxCount = 0;
__IO uint16_t RxCount = 0;

/* Private function prototypes -----------------------------------------------*/
void NVIC_Config(void);
void USARTInit(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void HyperTerminal_Interrupt(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
  this is done through SystemInit() function which is called from startup
  file (startup_stm32f0xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32f0xx.c file
  */

  /* NVIC configuration */
 /// NVIC_Config();

  /* USARTx configuration ------------------------------------------------------*/
  USARTInit();

  /* Enable the USART1 Transmoit interrupt: this interrupt is generated when the
  USART1 transmit data register is empty */
 /// USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

  /* Wait until USART1 send the TxBuffer */
 /// while(TxCount < NbrOfDataToTransfer)
 /// {}

  /* The software must wait until TC=1. The TC flag remains cleared during all data
  transfers and it is set by hardware at the last frame's end of transmission*/
 /// while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
 /// {}

  /* Enable the USART1 Receive interrupt: this interrupt is generated when the
  USART1 receive data register is not empty */
////  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
/*
void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  //Enable the USART2 Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

*/
/**
  * @brief  Configures COM port.
  * @retval None
  */
void USARTInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    /* Enable USART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* Connect PXx to USARTx_Tx */
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);

  /* Connect PXx to USARTx_Rx */
 GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

    /* Configure USART Tx, Rx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART configuration */
    USART_Init(USART1, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);

}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
/*
int fputc(int ch, FILE *f)
{
  // Place your implementation of fputc here
  // e.g. write a character to the USART
  USART_SendData(USART1, (uint8_t) ch);

  // Loop until transmit data register is empty
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {}

  return ch;
}
*/

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */

/*
void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    // Read one byte from the receive data register
    RxBuffer[RxCount++] = (USART_ReceiveData(USART1) & 0x7F);
    GPIOC->ODR ^= GPIO_Pin_8;
  }


  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {
    // Write one byte to the transmit data register
    USART_SendData(USART1, TxBuffer[TxCount++]);

    if(TxCount == NbrOfDataToTransfer)
    {
      // Disable the USART1 Transmit interrupt
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
  }
}
*/


// implementacja funkcji itoa:

#define INT_DIGITS 5		/* enough for 16 bit integer */

char *itoa(i)
     int i;
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}

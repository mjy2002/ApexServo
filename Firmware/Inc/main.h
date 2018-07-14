/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define DSA1_Pin GPIO_PIN_13
#define DSA1_GPIO_Port GPIOC
#define DSA2_Pin GPIO_PIN_14
#define DSA2_GPIO_Port GPIOC
#define DSA3_Pin GPIO_PIN_15
#define DSA3_GPIO_Port GPIOC
#define EN_IN_Pin GPIO_PIN_0
#define EN_IN_GPIO_Port GPIOC
#define EN_IN_EXTI_IRQn EXTI0_IRQn
#define DIR_IN_Pin GPIO_PIN_1
#define DIR_IN_GPIO_Port GPIOC
#define DIR_IN_EXTI_IRQn EXTI1_IRQn
#define LED2_Pin GPIO_PIN_0
#define LED2_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOA
#define S1_Pin GPIO_PIN_2
#define S1_GPIO_Port GPIOA
#define DSB3_Pin GPIO_PIN_3
#define DSB3_GPIO_Port GPIOA
#define DSB2_Pin GPIO_PIN_6
#define DSB2_GPIO_Port GPIOA
#define DSB1_Pin GPIO_PIN_7
#define DSB1_GPIO_Port GPIOA
#define S2_Pin GPIO_PIN_4
#define S2_GPIO_Port GPIOC
#define S3_Pin GPIO_PIN_5
#define S3_GPIO_Port GPIOC
#define OLED_RES_Pin GPIO_PIN_0
#define OLED_RES_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_PIN_1
#define OLED_DC_GPIO_Port GPIOB
#define OLED_NSS_Pin GPIO_PIN_12
#define OLED_NSS_GPIO_Port GPIOB
#define AS5048A_NSS_Pin GPIO_PIN_15
#define AS5048A_NSS_GPIO_Port GPIOA
#define STEP_IN_Pin GPIO_PIN_2
#define STEP_IN_GPIO_Port GPIOD
#define STEP_IN_EXTI_IRQn EXTI2_IRQn

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */
#define Quick_GPIO_H(GPIOx, GPIO_Pin)   GPIOx->BSRR = GPIO_Pin
#define Quick_GPIO_L(GPIOx, GPIO_Pin)   GPIOx->BSRR = (uint32_t)GPIO_Pin << 16U
/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc4;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc4);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt / USART3 wake-up interrupt through EXTI line 28.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
	// Namig: Spodnjo kodo skupaj s komentarji skopirajte znotraj funkcije USART3_IRQHandler().


		// Funkcija USART3_IRQHandler() se izvede vsaki�?, ko se sproži katerakoli od prekinitev USART3 vmesnika.
		// Funkcija USART3_IRQHandler() je torej pravzaprav SPLOŠNA prekinitvena rutina USART3 vmesnika.
		// Znotraj te splošne prekinitvene rutine je potrebno ugotoviti, kateri od SPECIFIČNIH prekinitvenih
		// dogodkov je sprožil prekinitev in se potem ustrezno odzvati na ta prekinitveni dogodek
		// s "callback funkcijo".Za nas bosta klju�?na prekinitvena dogodka sprejetje novega podatka (RXNE)
		// ter sprostitev oddajnega podatkovnega registra (TXE).

		// Vir prekinitve ugotavljamo s pomo�?jo prekinitveno-statusnega registra USART vmesnika
		// ob pomo�?i LL funkcij.

		// Nujno pa je tudi, da preden se odzovemo na specifi�?ni vir prekinitve, da preverimo,
		// �?e je ta specifi�?na prekinitev sploh omogo�?ena. Če tega ne bi storili, tvegamo,
		// da se na SPLOŠNI prekinitveni dogodek odzovemo z napa�?no SPECIFIČNO prekinitveno "callback" rutino.



		// ------ Odzivanje na novo-sprejeti podatek (zastavica RXNE = Receive data register not empty) -------


		// Najprej preverimo, �?e je ta specifi�?na prekinitev sploh omogo�?ena. Uporabimo LL funkcijo.
		if( LL_USART_IsEnabledIT_RXNE_RXFNE(USART3) )		// sploh omogo�?ena prekinitev ob RXNE?
		{
			// Če je prekinitev omogo�?ena, potem preverimo še, �?e je postavljena ustrezna zastavica.
			if(  LL_USART_IsActiveFlag_RXNE_RXFNE(USART3) )	// postavljena zastavica RXNE?
			{
				// Če je ta specifi�?na prekinitev omogo�?ena in �?e je postavljena zastavica tega specifi�?nega
				// prekinitvenega dogodka, potem se odzovemo s klicem ustrezne "callback" rutine.

				SCI_receive_byte_Callback();

				// V vednost: zastavica RXNE se zbriše avtomatsko, ko preberemo sprejemni register RDR.
			}
		}


		// V premislek: ker se nahajamo znotraj USART3 splošne prekinitvene rutine (tj. USART3_IRQHandler() ),
		// je popolnoma jasno, da je smiselno preverjati le zastavice enote USART3. Vidite, da tu
		// ne potrebujemo informacije o tem, katera USARTx enota se uporablja - ker je to implicitno dolo�?eno:
		// znotraj USART3_IRQHandler() nas seveda zanimajo zastavice vmesnika USART3.





		// ------ Odzivanje na sprostitev oddajnega podatkovnega registra TDR (zastavica TXE = Transmitter Empty) -------

		// Najprej preverimo, �?e je ta specifi�?na prekinitev sploh omogo�?ena. Uporabimo LL funkcijo.
		if( LL_USART_IsEnabledIT_TXE_TXFNF(USART3) )		// sploh omogo�?ena prekinitev ob TXE?
		{
			// Če je prekinitev omogo�?ena, potem preverimo še, �?e je postavljena ustrezna zastavica.
			if(  LL_USART_IsActiveFlag_TXE_TXFNF(USART3) )		// postavljena zastavica TXE?
			{
				// Če je ta specifi�?na prekinitev omogo�?ena in �?e je postavljena zastavica tega specifi�?nega
				// prekinitvenega dogodka, potem se odzovemo s klicem ustrezne "callback" rutine.

				SCI_transmit_byte_Callback();


				// V vednost: zastavica TXE se zbriše avtomatsko, ko pišemo v oddajni podatkovni register TDR.
			}
		}


		// V premislek: ker se nahajamo znotraj USART3 splošne prekinitvene rutine (tj. USART3_IRQHandler() ),
		// je popolnoma jasno, da je smiselno preverjati le zastavice enote USART3. Vidite, da tu
		// ne potrebujemo informacije o tem, katera USARTx enota se uporablja - ker je to implicitno dolo�?eno:
		// znotraj USART3_IRQHandler() nas seveda zanimajo zastavice vmesnika USART3.


  /* USER CODE END USART3_IRQn 0 */
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1 and DAC3 channel underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */
	if (LL_TIM_IsEnabledIT_UPDATE(TIM6)){
		if (LL_TIM_IsActiveFlag_UPDATE(TIM6)){
			LL_TIM_ClearFlag_UPDATE(TIM6);

			PSERV_run_services_Callback();
		}
	}
  /* USER CODE END TIM6_DAC_IRQn 0 */

  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

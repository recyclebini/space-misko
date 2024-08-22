/*
 * periodic_services.h
 *

 */

//    Potrebovali bomo sledeče LL funkcije:
//    a) void LL_TIM_EnableCounter(TIM_TypeDef TIMx)                    Enable timer counter
//    b) void LL_TIM_EnableIT_UPDATE(TIM_TypeDefTIMx)                Enable update interrupt (UIE).
//    c) void LL_TIM_DisableIT_UPDATE(TIM_TypeDef TIMx)                Disable update interrupt (UIE).
//    d) uint32_t LL_TIM_IsEnabledIT_UPDATE(TIM_TypeDefTIMx)        Indicates whether the update interrupt (UIE) is enabled.
//    e) uint32_t LL_TIM_IsActiveFlag_UPDATE(TIM_TypeDef TIMx)        Indicate whether update interrupt flag (UIF) is set (update interrupt is pending).
//    f) void LL_TIM_ClearFlag_UPDATE(TIM_TypeDefTIMx)                Clear the update interrupt flag (UIF).
#ifndef INCLUDE_PERIODIC_SERVICES_H_
#define INCLUDE_PERIODIC_SERVICES_H_

#include "stm32g4xx_ll_tim.h"
#include "kbd.h"

typedef struct {
	TIM_TypeDef *timer;
} PSERV_handle_t;


// Inicijalizacija periodic services
void PSERV_init(void);

// Enable periodic services
void PSERV_enable(void);

// Disable periodic services
void PSERV_disable(void);

// Callback funkcija, ki se bo izvajala periodično s pomočjo prekinitev časovnika in bo poskrbela za periodično izvajanje rutin
void PSERV_run_services_Callback(void);

#endif /* INCLUDE_PERIODIC_SERVICES_H_ */

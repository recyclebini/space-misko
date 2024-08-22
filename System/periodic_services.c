/*
 * periodic_services.c
 *

 */

#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_tim.h"
#include "kbd.h"
#include "LED.h"
#include "periodic_services.h"

PSERV_handle_t PSERV;

// Kateri časovnik bo uporabljen pri implementaciji, zagon časovnika
void PSERV_init(void){
	PSERV.timer = TIM6;
	LL_TIM_EnableCounter(PSERV.timer);
}

// Omogocanje periodčnog izvajanja rutin - omogočanje prekinitve časovnika ob dogodku preliva ("update event")
void PSERV_enable(void){
	LL_TIM_EnableIT_UPDATE(PSERV.timer);
}

// Onemogočanje periodičnog izvanja rutin - onemogočanje prekinitev časovnika ob dogodku preliva
void PSERV_disable(void){
	LL_TIM_DisableIT_UPDATE(PSERV.timer);
}

// Callback funkcija, ki se bo izvajala periodično s pomočjo prekinitev časovnika in bo poskrbela za periodično izvanje rutin
void PSERV_run_services_Callback(void){

		KBD_scan();

	}


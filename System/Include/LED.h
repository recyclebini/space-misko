/*
 * LED.h
 *
 *  Created on:
 *      Author:
 */

#ifndef INCLUDE_LED_H_
#define INCLUDE_LED_H_





// ----------- Include other modules (for public) -------------


// Vključimo nizko-nivojsko LL knjižnjico, da dobimo podporo za upravljanje LEDic na strojnem nivoju preko GPIO-jev.
#include "stm32g4xx_ll_gpio.h"


// Pri implementaciji sistemskih funkcij za upravljanje LEDic bomo potrebovali sledeče nizko-nivojske funkcije:
//		- prizganje LEDice  	LL_GPIO_SetOutputPin (GPIO_TypeDef * GPIOx, uint32_t PinMask)
//		- ugasanje LEDice   	LL_GPIO_ResetOutputPin (GPIO_TypeDef * GPIOx, uint32_t PinMask)
// 		- invertiranje LEDice   LL_GPIO_TogglePin (GPIO_TypeDef * GPIOx, uint32_t PinMask)
//		--uint32_t LL_GPIO_IsOutputPinSet (GPI0_TypeDef *GPIOx, uint32_t PinMask)





// -------------------- Public definitions --------------------



// Tukaj definiramo naštevni tip, s katerim dosežemo predvsem dvoje stvari:
//
//		1. definiramo imena LEDic, ki nastopajo v našem sistemu
//
//		2. vrstni red, v katerem naštejemo imena LEDic, hkrati definira, v kakšnem vrstnem redu so urejene
//		   LEDice v naši LED skupini: od LSB LEDice (least significant bit) proti MSB LEDici (most significant bit)
//
typedef enum {LED0, LED1, LED2, LED3, LED4, LED5, LED6, LED7, NUM_OF_LEDS} LEDs_enum_t;


// Še nekaj poudarkov:
//
//		- na koncu seznama LEDic smo dodali NUM_OF_ element, ki hrani število LEDic v našem sistemu
//
//		- zgoraj definirana imena LEDic bomo uporabljali, ko bo potrebno specificirati, katero LEDico želimo upravljati
//
//		- elemente naštevnega tipa bomo uporabljali za naslavljanje celic v tabeli






// ---------------- Public function prototypes ----------------

// Implementiramo funkcije, ki upravljajo bodisi posamezno LEDico bodisi vse LEDice kot urejeno LED skupino.
// Implementiramo tudi funkcijo, ki vrača informacijo o trenutnem stanju posameznih LEDic.
// Na koncu bomo vse implementirane funkcije testirali znotraj demo funkcije.


void LED_init(void);

void LED_on(LEDs_enum_t LEDn);		// // Poglejte, kako uporabimo zgoraj definirani naštevni tip LEDs_enum_t kot vir informacije, katero LEDico želimo upravljati.
void LED_off(LEDs_enum_t LEDn);
void LED_toggle(LEDs_enum_t LEDn);
void LED_set(LEDs_enum_t LEDn, uint8_t state);

void LEDs_on(uint8_t LED_bitmask);
void LEDs_off(uint8_t LED_bitmask);

uint8_t LED_is_on(LEDs_enum_t LEDn);

void LEDs_write(uint8_t value);
uint8_t LEDs_read(void);


void LED_demo(void);




#endif /* INCLUDE_LED_H_ */

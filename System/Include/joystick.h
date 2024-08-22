/*
 * joystick.h
 *
 *  Created on: Apr 29, 2022
 *      Author: Gasper
 */

#ifndef INCLUDE_JOYSTICK_H_
#define INCLUDE_JOYSTICK_H_





// ----------- Include other modules (for public) -------------

// Vključimo knjižnice za podporo delu s HAL knjižnicami in delu z GPIO vhodi/izhodi z LL knjižnicami.
#include "stm32g4xx_hal.h"			// HAL knjižnice
#include "stm32g4xx_ll_gpio.h"		// LL GPIO knjižnice







// -------------------- Public definitions --------------------


// Spodaj sledi nekaj "javnih" definicij, ki so pomembne za uporabo "joystick" modula.



// Tu definiramo naštevni tip, s katerim pravzaprav definiramo vse osi "joysticka"
// (ednina angl. joystick axis, množina angl. joystick axes). Naš "joystick" ima le dve osi, ki jih
// tipično poimenujemo X os in Y os (mimogrede, lahko bi imeli tri-osni "joystick").
//
// ZAKAJ je smiselno vpeljati te definicje?
//
// Ker bomo te dva elementa naštevnega tipa kasneje lahko uporabljali za naslavljanje elementov v tabeli,
// kjer hranimo meritve lege osi "joysticka". Poglejte primer spodaj, kjer z naštevnim elementom "X"
// naslovimo element tabele, ki hrani izmerjeno lego X osi "joysticka":
//
// 		joystick.position_raw[X]
//
// Naštevnemu tipu je dodan tudi NUM_OF_ element, katerega vlogo pa že poznate.
typedef enum
{
	X,
	Y,
	NUM_OF_AXES

} joystick_axes_enum_t;




// Definiramo še naštevni tip, s katerim  definiramo vse tipke "joysticka".
// Postopamo popolnoma enako, kot smo to storili pri modulu kbd.c.
// Ker ima naš "joystick" le eno tipko, je definicija precej trivialna.
// Poimenujmo to tipko "JOY_BTN_FIRE", saj običajno s tipko pri "joysticku" "streljamo".
typedef enum
{
	JOY_BTN_FIRE,
	JOY_NUM_OF_BTNS,
	JOY_BTN_NONE

} joystick_buttons_enum_t;





// Podobno kot smo to storili pri tipkovnici, bomo definirali še en pomožni naštevni tip,
// ki bo nosil informacijo o tem, kakšna je logična vrednost signala na digitalnem vhodu,
// kamor je priključena tipka "joysticka". Namreč, tipka "joysticka" je uporabljena tako,
// da se ob pritisku na tipko na digitalnem vhodu pojavi logična ničla - in ne enica.
// In to je pomemben podatek, kadar delamo s stanji tipk.

typedef enum {JOY_BTN_PRESSED_SIGNAL_STATE = 0, JOY_BTN_RELEASED_SIGNAL_STATE = 1}	joystick_button_sig_state_t;










// ---------------- Public function prototypes ----------------


void JOY_init(ADC_HandleTypeDef *adc, TIM_HandleTypeDef *tim);


void JOY_scan_button(void);
joystick_buttons_enum_t JOY_get_pressed_button(void);
uint8_t JOY_is_button_state_pressed(void);
void JOY_flush(void);


void JOY_calibrate(void);
uint8_t JOY_get_axis_position(joystick_axes_enum_t axis);


void JOY_button_demo(void);
void JOY_LED_demo(joystick_axes_enum_t axis);
void JOY_SCI_send_status(void);


#endif /* INCLUDE_JOYSTICK_H_ */

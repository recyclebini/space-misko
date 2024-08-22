/*
 * lcd_backlight.c
 *
 *  Created on: Apr 13, 2022
 *      Author: Matič, Glažar
 */





/* **************** MODULE DESCRIPTION *************************

Ta modul implementira sistemske funkcije za upravljanje svetilnosti
osvetlitve zaslona.

Spreminjanje svetilnosti osvetlitve se doseže s pomočjo spreminjanja
širine pravokotnega pulza, ki krmili vklop in in izklop osvetlitve zaslona
(tj. pulzno-širinske modulacije krmilnega signala, angl. pulse width
modulation, PWM) s primerno visoko frekvenco.

Modul omogoča:
	-	nastavitev svetilnosti v smislu nastavitve relativne jakosti
		osvetlitve podane v procentih (tj. od 0% do 100%),
	-	ugašanje osvetlitve
	-	vklop osvetlitve na pred-definirano privzeto vrednost
	- 	branje trenutno nastavljene jakosti osvetlitve


POZOR: Za uspešno uporabo modula je potrebno predhodno poskrbeti
za ustrezno nizko-nivojsko inicializacijo pripadajočega časovnika,
ki je uporabljen za generiranje PWM signala.

************************************************************* */





// ----------- Include other modules (for private) -------------

#include "lcd_backlight.h"		// lastne definicije modula

#include "SCI.h"				// za demonstracijo





// ---------------------- Private definitions ------------------



// Definiramo tip "handle" strukture "LCD_BKLT_handle_t", ki jo bomo uporabili za
// upravljanje osvetlitve LCD zaslona.
// V strukturi je potrebno pomniti:
//		-	kateri časovnik ("timer") bo uporabljen za implementacijo PWM signala,
//		- 	kateri kanal časovnika ("timer channel") bo uporabljen za implementacijo PWM signala
//		-	kolikšna bo privzeta relativna jakost osvetlitve (tj. "default brightness value in %")



// Definirajmo torej podatkovni tip "handle" strukture.
typedef struct
{
	// Informacija o uporabljenem časovniku in kanalu časovnika:
	TIM_TypeDef	*timer;
	uint32_t		timer_channel;


	// Informacija o privzeti jakosti osvetlitve:
	// (ker je jakost osvetlitve na intervalu od 0 do 100, je dovolj 8-bitna spremenljivka)
	uint8_t 	default_brightness;


} LCD_BKLT_handle_t;




// In sedaj na podlagi zgoraj definiranega tipa definiramo še globalno strukturno spremenljivko "LCD_backlight".

LCD_BKLT_handle_t LCD_backlight;








// -------------- Public function implementations --------------



// Funkcija LCD_BKLT_init() poskrbi za inicializacijo "handle" strukture za upravljanje LCD osvetlitve,
// za omogočitev delovanja časovnika, omogočitev delovanja kanala časovnika ter za določitev začetne
// vrednosti jakosti osvetlitve LCD zaslona ob inicializaciji sistema.
void LCD_BKLT_init(void)
{

	// 0. Poskrbite, da je ustrezna nizko-nivojska strojna oprema že inicializirana

		// Za nizko-nivojsko inicializacijo časovnika, s katerim implementiramo PWM signal za
		// krmiljenje LCD osvetlitve, je poskrbljeno s pomočjo avtomatsko generirane CubeMX
		// programske kode znotraj funkcije MX_TIM4_Init() v "main.c"!



	// 1. Inicializacija "handle" strukture za za upravljanje LCD osvetlitve


		// --- Specifikacija časovnika ter kanala časovnika ---
		//
		// Ti dva parametra strukture določimo s pred-definiranima makrojema iz LL knjižnice,
		// podobno kot smo to storili v prejšnjih vajah.
		LCD_backlight.timer = TIM4;							// DOPOLNI: zamenjaj "x" z ustrezno vrednostjo.
		LCD_backlight.timer_channel = LL_TIM_CHANNEL_CH1;	// DOPOLNI: zamenjaj "x" z ustrezno vrednostjo.



		// --- Specifikacija privzete vrednosti za osvetlitev zaslona ---
		//
		// Nastavimo privzeto vrednost osvetlitve na 60 %.
		LCD_backlight.default_brightness = 60;		// podano v procentih




	// 2. Omogočitev delovanja časovnika, da prične s štetjem.

		// Uporabimo ustrezno LL funkcijo.

		// DOPOLNI
		LL_TIM_EnableCounter (LCD_backlight.timer);



	// 3. Vklop osvetlitve LCD zaslona s privzeto jakostjo osvetlitve.

		// Tu si lahko pomagamo kar z ustrezno LCD_BKLT_ funkcijo za vklop osvetlitve,
		// ki je implementirana v nadaljevanju spodaj.

		// DOPOLNI
		LCD_BKLT_set_brightness(LCD_backlight.default_brightness);




	// 4. Omogočitev delovanja ustreznega kanala časovnika

		// Ta kanal časovnika bo poskrbel za generacijo PWM signala na pinu mikrokrmilnika
		// po principu primerjanja vrednosti (angl. output compare functionality).
		// Uporabite ustrezno LL funkcijo.

		// DOPOLNI
		LL_TIM_CC_EnableChannel (LCD_backlight.timer, LCD_backlight.timer_channel);

}





// Funkcija LCD_BKLT_set_brightness() poskrbi za nastavitev relativne
// jakosti osvetlitve LCD zaslona. Vhodni argument funkcije je relativna
// jakost osvetlitve, podana v procentih (od 0 do 100).
//
// PREMISLITE: ker smo premišljeno definirali periodo časovnika, s katerim
// generiramo PWM signal, vhodni argument funkcije kar neposredno lahko
// uporabimo za nastavitev vrednosti za primerjanje ("compare value")
void LCD_BKLT_set_brightness(uint8_t brightness)
{
	// Za nastavitev "compare" vrednosti časovnika uprabimo ustrezno LL funkcijo.

	// DOPOLNI
	LL_TIM_OC_SetCompareCH1 (LCD_backlight.timer, brightness);

	// Mimogrede: opaziti velja, da so LL funkcije za delo s kanali časovnika specifične. Vidite
	// namreč lahko, da za delo s kanalom 1 uporabljamo funkcije, ki v imenu vsebujejo pripono CH1.
	// Če bi torej želeli uporabljati drugi kanal časovnika, bi bilo potrebno uporabiti druge LL funkcije!
}




// Funkcija LCD_BKLT_get_brightness() vrne trenutno nastavitev relativne
// jakosti osvetlitve LCD zaslona.
//
// PREMISLITE: ker smo premišljeno definirali periodo časovnika, s katerim
// generiramo PWM signal, se v vrednosti za primerjanje ("compare value")
// že kar skriva informacija o relativni jakosti osvetlitve v procentih.
uint8_t LCD_BKLT_get_brightness(void)
{
	// Za branje "compare" vrednosti časovnika uprabimo ustrezno LL funkcijo.
	return  LL_TIM_OC_GetCompareCH1 (LCD_backlight.timer);

	// Mimogrede: opaziti velja, da so LL funkcije za delo s kanali časovnika specifične. Vidite
	// namreč lahko, da za delo s kanalom 1 uporabljamo funkcije, ki v imenu vsebujejo pripono CH1.
	// Če bi torej želeli uporabljati drugi kanal časovnika, bi bilo potrebno uporabiti druge LL funkcije!
}







// Funkcija LCD_BKLT_on() prižge osvetlitev LED zaslona tako, da nastavi jakost
// osvetlitve na specificirano privzeto vrednost ("default value").
void LCD_BKLT_on(void)
{
	LCD_BKLT_set_brightness(LCD_backlight.default_brightness);
}


// Funkcija LCD_BKLT_off() ugasne osvetlitev LED zaslona.
void LCD_BKLT_off(void)
{
	LCD_BKLT_set_brightness(0);
}






// -------------- Test functions --------------


// Z demo funkcijo stestiramo in prikažemo delovanje nastavljanja jakosti
// osvetlitve LCD zaslona.
//
// PREDLOG: V prvi zanki počasi povečujte svetilnost, v drugi jo počasi zmanjšujte.
// (Namig: "počasi" pomeni, da je potrebno uporabiti zakasnitve).
// Poskusite stestirati in demonstrirati vse funkcije LCD_BKLT modula.
void LCD_BKLT_demo(void)
{
	uint8_t	compare_value;


	// "ON - OFF" demo.
	LCD_BKLT_on();
	HAL_Delay(1000);

	LCD_BKLT_off();
	HAL_Delay(1000);


	// Postopno spreminjanje osvetlitve.


		// Naraščanje jakosti osvetlitve.
		for(int val=0; val <= 100; val++)
		{
			// Nastavimo novo vrednost jakosti osvetlitve.
			LCD_BKLT_set_brightness(val);

			// DOPOLNI


			// Preberemo dejansko nastavljeno vrednost osvetlitve
			// in jo pošljemo s pomočjo printf() preko SCI vmesnika.
			compare_value = LCD_BKLT_get_brightness();
			printf("compare value = %d\n", compare_value);

			// Kratka zakasnitev.
			HAL_Delay(10);
		}


		// Daljša zakasnitev.
		HAL_Delay(1000);


		// Upadanje jakosti osvetlitve.
		for(int val=100; val >= 0; val--)
		{
			// Nastavimo novo vrednost jakosti osvetlitve.

			// DOPOLNI
			LCD_BKLT_set_brightness(val);


			// Preberemo dejansko nastavljeno vrednost osvetlitve
			// in jo pošljemo s pomočjo printf() preko SCI vmesnika.
			compare_value = LCD_BKLT_get_brightness();
			printf("compare value = %d\n", compare_value);

			// Kratka zakasnitev.
			HAL_Delay(10);
		}

		// Daljša zakasnitev.
		HAL_Delay(1000);


	// Na koncu demo funkcije vklopimo osvetlitev na privzeti vrednosti.
	LCD_BKLT_on();


}






/*
 * LED.c
 *
 *  Created on:
 *      Author:
 */



/* **************** MODULE DESCRIPTION *************************

Ta modul implementira sistemske funkcije za upravljanje LEDic
sistema. LEDice se lahko upravlja na dva načina:

	1.	upravljanje posamezne LEDice ali pa

	2.	upravljanje vseh LEDic kot urejene LED skupine.

Upravljanje stanja LEDice je doseženo preko upravljanja stanja
ustreznega digitalnega GPIO izhoda.

POZOR: za uspešno uporabo modula je potrebno predhodno poskrbeti
za ustrezno nizko-nivojsko inicializacijo digitalnih GPIO izhodov.


************************************************************* */





// ----------- Include other modules (for private) -------------

#include "LED.h"




// ---------------------- Private definitions ------------------


// Najprej pripravimo podatkovno strukturo, v kateri bomo hranili vse potrebne parametre za
// upravljanje LED skupine. To podatkovno strukturo bomo v žargonu imenovali "handle structure".

// "Handle strukturo" bomo definirali po principu "od spodaj navzgor" (angl. "bottom-up approach").


// Pričnimo torej najprej z definicijo strukture LED_handle_t, ki hrani vse potrebno, da na
// sistemskem nivoju s pomočjo nizko-nivojskih strojnih funkcij upravljamo z eno samo LEDico.
typedef struct
{
	GPIO_TypeDef   *port;		// informacija o "portu", kamor je priključena LEDica
								// NAMIG: poglejte, kako informacijo o portu posredujemo LL funkcijam (npr. funkciji LL_GPIO_SetOutputPin() )

	uint32_t 		pin;		// informacija o pinu, kamor je priključena LEDica (tj. številka pina)

} LED_handle_t;





// Sedaj pa s pomočjo zgoraj definirane strukture LED_handle_t za upravljanje ene same LEDice
// definirajmo "handle strukturo" LED_group_handle_t, s pomočjo katere pa bomo lahko upravljali
// vse LEDice v LED skupini.

typedef struct
{
	// Ker imam v LED skupini očitno več kot le eno LEDico, bomo potrebovali več kot le eno LED_handle_t strukturo.
	// In to množico LED_handle_t struktur je seveda smiselno urediti v tabelo, ki jo poimenujemo kar "LEDs[]".
	// Tako za vsako LEDico v LED skupini hranimo njeno LED_handle_t strukturo v enem od elementov tabele "LEDs[]".

	LED_handle_t 	LEDs[NUM_OF_LEDS];	// NAMIG: kje že hranimo informacijo o tem, koliko LEDic ima LED modul?


} LED_group_handle_t;





// Vse to zgoraj so bile le DEFINICIJE ZGRADBE podatkovne strukture. Sedaj pa lahko končno ustvarimo
// še strukturno spremenljivko "LED_group", ki bo imela tako zgradbo in bo za nas hranila vse potrebne
// parametre za upraljanje LED skupine.

LED_group_handle_t LED_group;		// spremenljivka "handle strukture" za LED skupino


// Vidite, da smo "handle strukturo" shranili v GLOBALNO spremenljivko znotraj "LED.c" modula.
// To je ugodno, saj bodo vse sistemske funkcije za upravljanje LEDic lahko enostavno
// dostopale do potrebnih parametrov znotraj "handle strukture".




// Zgoraj smo si pripravili "handle strukturo" za upravljanje LED skupine, vendar pa je ta struktura
// še neuporabna, saj ni inicializirana! Inicializacijo "handle strukture" bomo izvedli v sklopu
// _init() funkcije spodaj.







// -------------- Public function implementations --------------



// Funkcija LED_init() poskrbi, da se pravilno inicializira LED_group_handle_t struktura.
// Poskrbi tudi, da se določi začetno stanje LEDic.
void LED_init(void)
{

	// 0. Poskrbite, da je ustrezna nizko-nivojska strojna oprema že inicializirana

		// Za inicializacijo GPIO digitalnih izhodov, ki krmilijo LEDice, je
		// poskrbljeno s pomočjo avtomatsko generirane CubeMX programske kode
		// znotraj funkcije MX_GPIO_Init() v "main.c"!




	// 1. Inicializirajte "handle strukture" za posamezne LEDice v LED skupini

		// Definirajte, kako so posamezne LEDice sistema povezane na GPIO pine in porte.
		// To storite tako, da pravilno inicializirate posamezne LED "handle strukture" znotraj LEDs[] tabele.
		// Uporabite makroje "GPIOn" in "LL_GPIO_PIN_n" iz nizko-nivojske LL knjižnice.
		// Spodaj imate primer za inicializacijo LEDice LED0.


		// LEDice na portu F
		LED_group.LEDs[LED0].pin = LL_GPIO_PIN_2;
		LED_group.LEDs[LED0].port = GPIOF;

		// DOPOLNI
		LED_group.LEDs[LED1].pin = LL_GPIO_PIN_3;
		LED_group.LEDs[LED1].port = GPIOF;

		LED_group.LEDs[LED2].pin = LL_GPIO_PIN_4;
		LED_group.LEDs[LED2].port = GPIOF;

		LED_group.LEDs[LED3].pin = LL_GPIO_PIN_5;
		LED_group.LEDs[LED3].port = GPIOF;


		// LEDice na portu C
		LED_group.LEDs[LED4].pin = LL_GPIO_PIN_0;
		LED_group.LEDs[LED4].port = GPIOC;

		LED_group.LEDs[LED5].pin = LL_GPIO_PIN_1;
		LED_group.LEDs[LED5].port = GPIOC;

		LED_group.LEDs[LED6].pin = LL_GPIO_PIN_2;
		LED_group.LEDs[LED6].port = GPIOC;

		LED_group.LEDs[LED7].pin = LL_GPIO_PIN_3;
		LED_group.LEDs[LED7].port = GPIOC;



	// 2. Nastavite začetno stanje LEDic (t.i. privzeto stanje (angl. default state)).

		// Na začetku po inicializaciji želimo imeti vse LEDice ugasnjene.
		// Uporabi se lahko kar LEDs_write() funkcijo.
		LEDs_write(0x00);

}







// ------- Upravljanje posamezne LEDIce v LED skupini ------------



// Spodaj boste lahko videli, kako sedaj lahko uporabimo naštevni tip LEDs_enum_t
// kot vhodni argument funkcije "LEDn" tako, da specificiramo,
// katero od LEDic v LED skupini želimo uprabljati.



// Funkcija LED_on() prižge tisto LEDico v skupini, ki jo specificira
// vrednost "LEDn". Vrednost "LEDn" pa je lahko eden od elementov iz
// seznama naštevnega tipa LEDs_enum_t.
void LED_on(LEDs_enum_t LEDn)
{
	// Za prižig LEDice uporabimo sledečo nizko-nivojsko LL funkcijo:
	//
	//		void LL_GPIO_SetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
	//
	LL_GPIO_SetOutputPin( LED_group.LEDs[LEDn].port, LED_group.LEDs[LEDn].pin );


	// Poglejte, kako potrebno informacijo o portu in pinu LEDice dobimo
	// s pomočjo vhodnega argumenta LEDn. Pomagamo si s tabelo LED_handle_t
	// struktur "LED_group.LEDs[]" ter z naslavljanjem elementov tabele
	// s pomočjo naštevnega tipa.

	// Za lažjo predstavo: predstavljate si lahko, da vrednost "LEDn" zgoraj
	// zamenjate z npr. LED3 iz seznama naštevnega tipa; in tako boste s klicem zgornje
	// funkcijo postavili na visoko stanje prav tisti digitalni izhod, ki pripada
	// portu in pinu LEDice LED3.


}





// Funkcija LED_off() ugasne tisto LEDico v skupini, ki jo specificira
// vrednost "LEDn". Vrednost "LEDn" pa je lahko eden od elementov iz
// seznama naštevnega tipa LEDs_enum_t.
void LED_off(LEDs_enum_t LEDn)
{
	LL_GPIO_ResetOutputPin( LED_group.LEDs[LEDn].port, LED_group.LEDs[LEDn].pin );

}




// Funkcija LED_toggle() invertira stanje tiste LEDico v skupini, ki jo specificira
// vrednost "LEDn". Vrednost "LEDn" pa je lahko eden od elementov iz
// seznama naštevnega tipa LEDs_enum_t.
void LED_toggle( LEDs_enum_t LEDn )
{
	LL_GPIO_TogglePin( LED_group.LEDs[LEDn].port, LED_group.LEDs[LEDn].pin );
}




// Funkcija LED_set() nastavi stanje LEDice "LEDn" glede na vrednost vhodnega argumenta "state".
// Vrednost "LEDn" pa je lahko eden od elementov iz seznama naštevnega tipa LEDs_enum_t.
void LED_set(LEDs_enum_t LEDn, uint8_t state)
{
	// Preveri željeno stanje LEDice "state";
	if (state)
	{
		LED_on(LEDn);
	}
	else
	{
		// sicer pa LEDico ugasni.
		LED_off(LEDn);
	}
}








// -------- Upravljanje vseh LEDic kot urejene LED skupine ------------
//
// LED skupino lahko uprabljamo na dva načina:
//
//	1. s pomočjo bitne maske
//
//	2. neposredno s pomočjo vrednosti.



// ---- Upravljanje s pomočjo bitne maske ------


// S funkcijo "LEDs_on()" prižgemo le tiste LEDice v LED skupini, ki jih določa vhodni argument
// bitna maska "LED_bitmask": če je postavljen i-ti bit maske "LED_bitmask", potem prižgemo i-to
// LEDico v skupini; sicer ne spreminjamo stanja LEDice. Pri tem se upošteva enak vrstni red LEDic,
// kot je bil definiran v naštevnem tipu LEDs_enum_t.
void LEDs_on(uint8_t LED_bitmask)
{

	// Pomožna bitna maska, s katero bomo "brali" vrednosti posameznih bitov
	// vhodnega argumenta "LED_bitmask" od LSB bita proti MSB bitu.
	uint8_t bitmask = 0x01;

	// S "for" zanko se sprehodimo preko vseh LEDic v sistemu.
	for(uint8_t i=0; i < NUM_OF_LEDS; i++)
	{
		// S pomočjo pomožne maske "bitmask" preberemo stanje i-tega bita v vhodnem argumentu "LED_bitmask".
		if ( LED_bitmask & bitmask )
		{
			// In če je i-ti bit v vhodnem argumentu "LED_bitmask" postavljen, prižgemo i-to LEDico.
			LL_GPIO_SetOutputPin( LED_group.LEDs[i].port, LED_group.LEDs[i].pin );
		}

		// In nato si pripravimo novo pomožno bitno masko za branje naslednjega bita v vhodnem argumentu.
		bitmask <<= 1;
	}
}






// S funkcijo "LEDs_off()" ugašamo le tiste LEDice v LED skupini, ki jih določa vhodni argument
// bitna maska "LED_bitmask": če je postavljen i-ti bit maske "LED_bitmask", potem ugasnemo i-to
// LEDico v skupini; sicer ne spreminjamo stanja LEDice. Pri tem se upošteva enak vrstni red LEDic,
// kot je bil definiran v naštevnem tipu LEDs_enum_t.
void LEDs_off(uint8_t LED_bitmask)
{

	// Funkcija deluje na enak način kot LEDs_on(), le da je sedaj potrebno LEDice ugašati.

	// Pomožna bitna maska, s katero bomo "brali" vrednosti posameznih bitov
	// vhodnega argumenta "LED_bitmask" od LSB bita proti MSB bitu.
	uint8_t bitmask = 0x01;

	// S "for" zanko se sprehodimo preko vseh LEDic v sistemu.
	for(uint8_t i=0; i < NUM_OF_LEDS; i++)
	{
		// S pomočjo pomožne maske "bitmask" preberemo stanje i-tega bita v vhodnem argumentu "LED_bitmask".
		if ( LED_bitmask & bitmask )
		{
			// In če je i-ti bit v vhodnem argumentu "LED_bitmask" postavljen, ugasnemo i-to LEDico.
			LL_GPIO_ResetOutputPin( LED_group.LEDs[i].port, LED_group.LEDs[i].pin );
		}

		// In nato si pripravimo novo pomožno bitno masko za branje naslednjega bita v vhodnem argumentu.
		bitmask <<= 1;
	}


}








// ----- Upravljanje neposredno s pomočjo vrednosti -----


// V primeru funkcije LEDs_write() pa vrednost vhodnega argumenta "value"
// neposredno določa, katere LEDice v LED skupini bodo prižgane ter katere
// ugasnjene po ideji: če postavljen i-ti bit vrednosti "value", potem naj
// bo prižgana i-ta LEDica; sicer naj bo ugasnjena.
void LEDs_write(uint8_t value)
{
	uint8_t bitmask = 0x01;

	// S "for" zanko se sprehodimo preko vseh LEDic v sistemu.
	for(uint8_t i=0; i < NUM_OF_LEDS; i++)
	{
		// Preveri vrednost i-tega bita vhodnega argumenta "value".
		if (value & bitmask)
		{
			// Če je i-ti bit enak 1, potem naj se prižge i-ta LEDica, pri čemer se pa
			// upošteva vrstni red LEDic, kot je bil definiran v naštevnem tipu LEDs_enum_t;

			LL_GPIO_SetOutputPin( LED_group.LEDs[i].port, LED_group.LEDs[i].pin );

		}
		else
		{
			// sicer pa naj se i-ta LEDica ugasne.

			LL_GPIO_ResetOutputPin( LED_group.LEDs[i].port, LED_group.LEDs[i].pin );

		}


		// In nato si pripravimo novo pomožno bitno masko za branje naslednjega bita v vhodnem argumentu.
		bitmask <<= 1;
	}
}








// -------- Vračanje informacij o trenutnem stanju LEDic --------


// Funkcija LED_is_on() vrne vrednost 0, če je LEDica LEDn ugasnjena; sicer vrne
// vrednost različno od 0.
uint8_t LED_is_on(LEDs_enum_t LEDn)
{
		return LL_GPIO_IsOutputPinSet( LED_group.LEDs[LEDn].port, LED_group.LEDs[LEDn].pin );
}



// Funkcija LEDs_read() vrne 8-bitno vrednost, kjer i-ti bit odraža stanje
// i-te LEDice v LED skupini.
uint8_t LEDs_read(void)
{
	uint8_t value = 0;
	uint8_t bitmask = 0x01;

	for(uint8_t i=0; i < NUM_OF_LEDS; i++)
	{
		if ( LL_GPIO_IsOutputPinSet( LED_group.LEDs[i].port, LED_group.LEDs[i].pin ) )
			value |= bitmask;

		bitmask <<= 1;
	}

	return value;
}







// ------- Test functions ---------


// Znotraj funkcije LED_demo() boste implementirali kodo, s katero boste
// v prvi vrsti preizkusili, če ste pravilno implementirali sistemske LED
// funkcije. Hkrati pa bo ta funkcija lahko služila kot demonstracija
// funkcionalnosti sistemskih LED funkcij.
void LED_demo(void)
{
	/*LED_on(LED0);
	HAL_Delay(2000);
	LED_off(LED0);
	LED_toggle(LED5);
	HAL_Delay(2000);
	LED_set(LED2, 1);
	HAL_Delay(2000);

	LEDs_on(15);
	HAL_Delay(2000);
	LEDs_off(14);*/

	LEDs_write(15);
}






// -------------- Private function implementations -------------


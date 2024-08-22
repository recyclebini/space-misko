/*
 * timing_utils.c
 *
 *  Created on:
 *      Author:
 */


/* **************** MODULE DESCRIPTION *************************

V tem modulu so implementirana različna orodja za merjenje časa.
Implementirana je funkcionalnost ure štoparice, ki vam pomaga
meriti pretečeni čas med izvajanjem programa z milisekundno
resolucijo. Ura štoparica je implementirana na podlagi SysTick
števca milisekund, do katerega dostopamo s pomočjo HAL knjižnice.


************************************************************* */




// ----------- Include other modules (for private) -------------


#include "timing_utils.h"		// lastne definicije modula
#include "stm32g4xx_hal.h"		// podpora za delo s SysTick
#include "LED.h"				// za potrebe demonstracije





// ---------------------- Private definitions ------------------





// ------------- Public function implementations --------------



// Tu spodaj bomo spisali funkcije, s pomočjo katerih bomo implementirali
// funkcionalnost ure štoparice. Opazili boste lahko t.i. "bottom-up approach",
// kjer pričnemo z implementacijo najbolj "temeljnih" funkcij in nato z njimi
// gradimo čedalje bolj kompleksno funkcionalnost.




// Funkcija TIMUT_stopwatch_set_time_mark() si zabeleži trenutno
// vrednost SysTick števca in tako na nek način postavi "časovni zaznamek"
// (angl. time mark), v katerem trenutku smo z uro štoparico pričeli
// meriti čas (tj. "štopati").
void TIMUT_stopwatch_set_time_mark(stopwatch_handle_t *stopwatch)
{
	stopwatch->time_mark = HAL_GetTick();
}



// Funkcija TIMUT_stopwatch_update() posodobi vrednost pretečenega
// časa od trenutka, kjer smo postavili časovni zaznamek ("time mark").
// Hkrati funkcija tudi vrne vrednost pretečenega časa kot izhodni parameter.
uint32_t TIMUT_stopwatch_update(stopwatch_handle_t *stopwatch)
{
	// Poračunamo vrednost pretečenega časa od postavljenega časovnega zaznamka
	// in jo shranimo v spremenljivko "handle" strukture štoparice.
	stopwatch->elapsed_time = HAL_GetTick() - stopwatch->time_mark;

	// Vrednost pretečenega časa hkrat tudi vrnemo, kar funkciji veča
	// raznolikost uporabe.
	return stopwatch->elapsed_time;
}




// Funkcija TIMUT_stopwatch_has_X_ms_passed() preveri, ali je od postavitve
// časovnega zaznamka že preteklo "x" milisekund, kjer pa je "x" vrednost
// vhodnega parametra funkcije. Funkcija vrne vrednost 1, če je že preteklo
// "x" milisekund, sicer pa vrne 0.
uint8_t TIMUT_stopwatch_has_X_ms_passed(stopwatch_handle_t *stopwatch, uint32_t x)
{
	// Najprej je potrebno posodobiti vrednost pretečenega časa.
	// Uporabite ustrezno TIMUT_ funkcijo zgoraj.

	// DOPOLNI
	TIMUT_stopwatch_update(stopwatch);

	// Nato se pa preveri, če je pretečeni čas večji ali manjši od "x" milisekund.
	if (stopwatch->elapsed_time >= x)
	{
		return 1;	// pretečeni čas je večji od "x" milisekund
	}
	else
	{
		return 0;	// pretečeni čas je manjši od "x" milisekund
	}

}






// Funkcija TIMUT_stopwatch_has_another_X_ms_passed() je namenjena PERIODIČNEMU
// preverjanju, ali je že preteklo "x" milisekund od zadnjega časovnega zaznamka.
// Parameter "x" je vhodni argument funkcije.
// Funkcija deluje tako, da če je preteklo dovolj časa, sama poskrbi za
// ustrezen premik časovnega zaznamka ("time mark") naprej. To nam omogoči, da
// zgolj s klicem funkcije TIMUT_stopwatch_has_another_X_ms_passed() ugotavljamo,
// ali je že preteklo "x" časa od KONCA prejšnjega intervala dolžine "x".
// Povedano drugače: s to funkcijo torej časovno os razdelimo na intervale
// dolžine "x" in hkrati z njo ugotavljamo, ali se je trenutni interval
// že iztekel.
// Funkcija vrne vrednost 1, če je že preteklo "x" milisekund od konca prejšnjega
// intervala, sicer pa vrne 0.
uint8_t TIMUT_stopwatch_has_another_X_ms_passed(stopwatch_handle_t *stopwatch, uint32_t x)
{
	// Najprej preverimo, če je že preteklo "x" milisekund od trenutnega časovnega
	// zaznamka. Uporabimo ustrezno TIMUT_ zgoraj.
	TIMUT_stopwatch_update(stopwatch);
	if (stopwatch->elapsed_time >= x )
	{
		// Če je že preteklo "x" milisekund od časovnega zaznamka, potem je potrebno
		// časovni zaznamek postaviti naprej, da bomo tako z njegovo pomočjo
		// lahko ugotavljali, kdaj se izteče naslednji interval.

		// Premik časovnega zaznamka izvedemo na dva načina.

		// Če je trenutni pretečeni čas štoparice večji kot dvakratna dolžina
		// intervala "x", potem smo zamudili in spregledali vsaj en interval
		// v celoti! V tem primeru je smiselno, da časovni zaznamek ponovno
		// nastavimo kar na trenutno vrednost SysTick števca.
		if (stopwatch->elapsed_time > 2*x )
		{
			// Ponastavi časovni zaznamek na trenutno vrednost SysTick števca.
			// Uporabi ustrezno TIMUT_ funkcijo zgoraj.

			// DOPOLNI
			TIMUT_stopwatch_set_time_mark(stopwatch);
			// In vrnemo vrednost 1, saj je preteklo več kot "x" milisekund.
			return 1;
		}
		else
		{
			// V nasprotnem primeru je preteklo več kot "x" milisekund, vendar
			// manj kot "2*x" milisekund.

			// V tem primeru pa nismo zamudili nobenega intervala in je smiselno,
			// da časovni zaznamek le premaknemo za "x" dalje naprej od njegove
			// trenutne lokacije.
			stopwatch->time_mark += x;

			// In prav tako vrnemo vrednost 1, saj je preteklo več kot "x" milisekund.
			return 1;
		}

	}
	else
	{
		// V tem primeru pa ni preteklo več kot "x" milisekund in zato vrnemo 0.
		return 0;
	}

}






// Funkcija TIMUT_stopwatch_demo() demonstrira uporabo ure štoparice s
// pomočjo prižiganja LEDic.
void TIMUT_stopwatch_demo(void)
{

	// Pripravimo si "handle" strukturo, s pomočjo katere bomo implementirali
	// funkcionalnost ure štoparice.
	stopwatch_handle_t   stopwatch;




	// Najprej demonstrirajmo čakanje ("delay") s pomočjo funkcije
	// TIMUT_stopwatch_has_X_ms_passed().

	// Prižgemo LEDico in si zabeležimo trenutek prižiga (tj. postavimo
	// časovni zaznamek - "time mark").
	LED_on(LED0);										// prižig LEDice
	TIMUT_stopwatch_set_time_mark(&stopwatch); 		// postavimo časovni zaznamek s TIMUT_ funkcijo


	// Sedaj pa v "while" zanki čakamo, da mine x=1000 ms, torej ena sekunda.
	// Uporabimo funkcijo TIMUT_stopwatch_has_X_ms_passed().
	while (!TIMUT_stopwatch_has_X_ms_passed(&stopwatch, 1000))
	{
		// Mimogrede, v zanki ne počnemo ničesar. Program se torej ne izvaja dalje.
		// To pa pomeni, da je taka implementacija časovne zakasnitve v načinu "blocking mode".

	}

	// In ko smo v "while" zanki počakali, da je minila ena sekunda, pa sedaj
	// lahko LEDico ugasnemo.

	LED_off(LED0);







	// Sedaj pa demonstrirajmo še periodično preverjanje, ali je že preteklo
	// "x" milisekund od zadnjega časovnega zaznamka z uporabo funkcije
	// TIMUT_stopwatch_has_another_X_ms_passed().

	// Najprej si na novo postavimo časovni zaznamek ("time mark") - torej
	// "pričnemo s štopanjem".

	TIMUT_stopwatch_set_time_mark(&stopwatch);

	// Sedaj pa znotraj neskončne "while" zanke periodično preverjamo, ali se je že
	// iztekel trenutni interval dolžine 0,5 sekunde. Če se je, spremenimo stanje
	// LEDice LED1.
	while(1)
	{
		if ( TIMUT_stopwatch_has_another_X_ms_passed( &stopwatch, 1000) )
		{
			// Spremenimo stanje LEDice LED1.
			// DOPOLNI
			LED_toggle(LED1);
		}

	}


}







// ------- Interrupt callbacks  ---------

// ------- Test functions ---------


// -------------- Private function implementations -------------


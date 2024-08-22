/*
 * game.c
 *
 *  Created on: 10 May 2022
 *      Author: Gasper
 */


/* **************** MODULE DESCRIPTION *************************

Ta modul implementira algoritem igrice s pomočjo avtomatov stanj.
Algoritem igre za svoje delovanje potrebuje podporo sistemskih
modulov za delo z vhodno-izhodnimi napravami sistema ter podporo
aplikacijskih modulov za delo z objekti aplikacije ter podporo
za izris objekotv na zaslon.


************************************************************* */




// ----------- Include other modules (for private) -------------





// ------ Standard C libraries -----
#include <stdio.h>
#include <stdlib.h>		// support for random number generation



// POGLEJTE: igra (tj. aplikacija) je odvisna le od modulov s
// sistemskega nivoja, ne pa tudi od modulov s strojnega nivoja!


// --- Moduli sistemskega nivoja ---

#include "LED.h"				// podpora za delo z LEDicami
#include "kbd.h"				// podpora za delo s tipkovnico
#include "SCI.h"				// podpora za delo s serijskim vmesnikom
#include "joystick.h"			// podpora za delo z joystickom
#include "lcd.h"				// podpora za delo z LCDjem
#include "timing_utils.h"		// podpora za delo z orodji za merjenje časa




// --- Moduli applikacijskega nivoja ----

#include "game.h"				// lastne definicije game.c modula

#include "objects.h"				// v igro vključimo modul za delo z objekti igre
#include "graphics.h"				// v igro vključimo modul za prikaz objektov na zaslon
#include "math_utils.h"				// vključimo model za generiranje random števil




// ---------------------- Private definitions ------------------


// ----- Prototipi "privatnih" funkcij ------

uint8_t 	Intro();
uint8_t 	GamePlay();
uint8_t 	GameOver();


void GamePlay_Update_Aiming_LED_Indicator(void);





// ----- Definicija stanj avtomatov  --------


// stanja avtomata Game()
typedef enum GAME_states {
	GAME_INTRO_STATE,
	GAME_PLAY_STATE,
	GAME_OVER_STATE
} GAME_states_t;

// stanja avtomata Intro()
typedef enum INTRO_states {
	INTRO_INIT,
	INTRO_WAIT_BEFORE_KBD_ACTIVE,
	INTRO_PRESS_ANY_KEY, INTRO_WAIT_FOR_ANY_KEY,
	INTRO_LEDS_OFF
} INTRO_states_t;

// stanja avtomata GamePlay()
	// TO DO: Tu boste pri naslednji vaji definirali še stanja avtomata GamePlay().
typedef enum GAMEPLAY_states {
	GAMEPLAY_INIT,
	GAMEPLAY_COUNTDOWN_3,
	GAMEPLAY_COUNTDOWN_2,
	GAMEPLAY_COUNTDOWN_1,
	GAMEPLAY_COUNTDOWN_CLEAR,
	GAMEPLAY_START_MEASURING_GAMEPLAY_TIME,
	GAMEPLAY_SPAWN_TARGET,
	GAMEPLAY_SHOOT_TARGET,
	GAMEPLAY_BEFORE_GAME_OVER
} GAMEPLAY_states_t;
// stanja avtomata GameOver()
typedef enum GAMEOVER_states {
	GAMEOVER_SCREEN,
	GAMEOVER_WAIT_BEFORE_LEDS_OFF,
	GAMEOVER_LEDS_OFF,
	GAMEOVER_WAIT_FOR_ANY_KEY
} GAMEOVER_states_t;








// ------------- Public function implementations --------------






// Funkcija, ki implementira nad-avtomat Game().
void Game()
{
	// Stanje avtomata je potrebno pomniti tudi, ko se avtomat ne izvaja.
	// To pomeni, da je potrebno stanje avtomata hraniti v spremenljivki,
	// ki ima neskončno življensko dobo. Lokalni spremenljivki funkcije
	// lahko napravimo življensko dobo neskončno, če jo definiramo kot
	// statično spremenljivko (tj. uporabimo t.i. ključno besedo "static",
	// Osvojimo C, Fajfar et al., stran 83).
	// Hkrati z definicijo spremenljivke poskrbimo še za inicializacijo začetnega stanja avtomata.
	static GAME_states_t state = GAME_INTRO_STATE;


	// Pomožna spremenljivka, kamor bomo shranjevali vrnjeno izhodno
	// vrednost pod-avtomatov (tj. "exit value").
	// Hkrati z definicijo spremenljivke poskrbimo še za inicializacijo začetne vrednosti.
	uint8_t exit_value = 0;


	// Avtomat stanj implementiramo s pomočjo "switch-case" stavka, s katerim
	// zelo enostavno dosežemo izvajanja le tistega dela kode, ki pripada
	// točno določenemu stanju avtomata.
	switch (state)
	{

		case GAME_INTRO_STATE:

			// Znotraj tega stanja izvajamo pod-avtomat Intro().
			exit_value = Intro();

			// Če pod-avtomat vrne vrednost različno od nič,
			// avtomat Game() preide v naslednje stanje.
			if (exit_value != 0)
				state = GAME_PLAY_STATE;

		break;



		case GAME_PLAY_STATE:

			// Znotraj tega stanja izvajamo pod-avtomat GamePlay().
			exit_value = GamePlay();

			// Če pod-avtomat vrne vrednost različno od nič,
			// avtomat Game() preide v naslednje stanje.
			if (exit_value != 0)
				state = GAME_OVER_STATE;

		break;





		case GAME_OVER_STATE:

			// Znotraj tega stanja izvajamo pod-avtomat GameOver().
			exit_value = GameOver();

			// Če pod-avtomat vrne vrednost različno od nič,
			// avtomat Game() preide v naslednje stanje.
			if (exit_value != 0)
				state = GAME_INTRO_STATE;

		break;




		// Avtomatu dodamo "varovalko" za primer, da avtomat zaide v nedefinirano stanje.
		// To napako javimo preko SCI vmesnika. Vidite, da SCI vmesnik uporabljamo
		// za realno-časni nadzor nad napakami aplikacije preko serijskega terminala.
		default:

			// Javimo vir napake in vrednost nedefiniranega stanja, v katerem se je našel avtomat.
			printf("Game(): Error - undefined state (%d)", state);

			// Ustavimo izvajanje z namenom, da razvojnik lahko opazi neobičajno obnašanje aplikacije.
			HAL_Delay(5000);

			// In na koncu avomat restiramo tako, da ga postavimo v začetno stanje.
			state = GAME_INTRO_STATE;
			exit_value = 0;

		break;
	}
}








// Funkcija, ki implementira pod-avtomat Intro().
uint8_t Intro()
{

	// Definicija makro parametrov za pod-avtomat Intro().
	#define INTRO_DELAY_BEFORE_KBD_ACTIVE	4000		// časovni premor pred aktivacijo tipkovnice


	// Stanje avtomata je potrebno pomniti tudi, ko se avtomat ne izvaja.
	// To pomeni, daje potrebno stanje avtomata hraniti v spremenljivki,
	// ki ima neskončno življensko dobo. Lokalni spremenljivki funkcije
	// lahko napravimo neskončno življensko dobo, če jo definiramo kot
	// statično spremenljivko ("static").
	// Hkrati poskrbimo za inicializacijo začetnega stanja avtomata.
	static INTRO_states_t state = INTRO_INIT;


	// Pomožna spremenljivka, kamor bomo shranjevali vrnjeno izhodno
	// vrednost pod-avtomatov (tj. "return exit value").
	// Pazite: ne pozabite smiselno definirati tudi začetne vrednosti te spremenljivke.
	uint8_t exit_value = 0;

	// Pomožna spremenljivka, kjer bomo hranili informacijo o pritisnjeni tipki.
	buttons_enum_t	key;


	// Ker mora pod-avtomat implementirati časovne zakasnitve oziroma premore,
	// bomo potrebovali funkcionalnost ure štoparice iz modula timing_utils.c.
	// Premislite: tudi ura štoparica mora imeti svoje spremenljivke z neskončno
	// življensko dobo.
	static stopwatch_handle_t stopwatch;




	// Avtomat stanj implementiramo s pomočjo "switch-case" stavka, s katerim
	// zelo enostavno dosežemo izvajanja le tistega dela kode, ki pripada
	// točno določenemu stanju avtomata.
	switch (state)
	{

			// Začetno stanje pod-avtomata.
			// Inicializacija vseh podaktovnih struktur igre, risanje "splash screen" zaslona
		 	// ter prižig vseh LEDic.
			case INTRO_INIT:

				// Opravila stanja.
				OBJ_init();	// Namig: inicializacijo podatkovnih struktur (tj. objektov)
							// lahko izvedete s klicem ene same funkcije.
				LEDs_write(255);
				GFX_draw_gfx_object(&splash_screen);

				// Ob koncu tega stanja pričnemo z merjenjem časa premora, ki sledi
				// (tj. sprožimo uro štoparico tako, da postavimo časovni zaznamek - "time mark").
				TIMUT_stopwatch_set_time_mark(&stopwatch);

				// Določimo naslednje stanje ter "exit" vrednost.
				state = INTRO_WAIT_BEFORE_KBD_ACTIVE;
				exit_value = 0;

			break;




			// Premor pred aktivacijo tipkovnice, čiščenje medpomnilnika tipkovnice.
			case INTRO_WAIT_BEFORE_KBD_ACTIVE:

				// V tem stanju preverjamo, ali se je premor že iztekel.
				if (TIMUT_stopwatch_has_X_ms_passed(&stopwatch, INTRO_DELAY_BEFORE_KBD_ACTIVE) == 1)
				{
					// Če se je premor že iztekel,

					// počistimo medpomnilnik tipkovnice.
					KBD_flush();

					// ter določimo naslednje stanje in "exit" vrednost.
					state = INTRO_PRESS_ANY_KEY;
					exit_value = 0;

				}

			break;



			// Izpis "press any key".
			case INTRO_PRESS_ANY_KEY:

				// Opravila stanja.
				// Namig: Napis "Press any key" je shranjen v obliki "sprite-a". Preučite,
							// v katerem od objektov se skriva ta "sprite".
				GFX_draw_gfx_object(&press_any_key_sprite);

				// Določimo naslednje stanje ter "exit" vrednost.
				state = INTRO_WAIT_FOR_ANY_KEY;
				exit_value = 0;

			break;



			// Čakanje na pritisk tipke.
			case INTRO_WAIT_FOR_ANY_KEY:

				// V tem stanju preverjamo, če je bila pritisnjena katerakoli tipka.

				// Preberemo informacijo o pritisnjenih tipkah iz medpomnilnika tipkovnice.
				// (Skeniranje tipkovnice se izvaja avtomatsko v sklopu periodic_services.c modula.)

				key = KBD_get_pressed_button();

				// In če je bila pritisnjena katerakoli tipka
				if (key != BTN_NONE)
				{

					// določimo naslednje stanje in "exit" vrednost.
					KBD_flush();
					state = INTRO_LEDS_OFF;
					exit_value = 0;

				}

			break;




			// Ugasnitev vseh LEDic
			case INTRO_LEDS_OFF:

				// Opravila stanja.
				LEDs_write(0xFF);

				// Določimo naslednje stanje ter "exit" vrednost.

				// V tej točki se je izvajanje pod-avtomata zaključilo.
				// Stanje avtomata nastavimo na začetno stanje in nastavimo "exit" vrednost 1.
				state = INTRO_INIT;
				exit_value = 1;

			break;






			// Avtomatu dodamo "varovalko" za primer, da avtomat zaide v nedefinirano stanje.
			// To napako javimo preko SCI vmesnika. Vidite, da SCI vmesnik uporabljamo
			// za realno-časni nadzor nad napakami aplikacije preko serijskega terminala.
			default:

				// Javimo vir napake in vrednost nedefiniranega stanja, v katerem se je našel avtomat.
				printf("Intro(): Error - unknown state (%d)", state);

				// Ustavimo izvajanje z namenom, da razvojnik lahko opazi neobičajno obnašanje aplikacije.
				HAL_Delay(5000);

				// In na koncu avomat restiramo tako, da ga postavimo v začetno stanje.
				state = INTRO_INIT;
				exit_value = 0;

			break;


		}


	// Vrnemo zgoraj nastavljeno "exit_value" vrednost nad-avtomatu.
	// Tako bo nad-avtomat vedel, ali mora še zaganjati ta pod-avtomat ali 
	// pa mora preiti v naslednje stanje.
	return exit_value;

}














// Funkcija, ki implementira pod-avtomat GamePlay().




// Funkcija, ki implementira pod-avtomat GamePlay().
uint8_t GamePlay()
{



	// -------------- Definicije znotraj funkcije GamePlay() ------------------

	// Definicija makro parametrov za pod-avtomat GamePlay().
	#define GAMEPLAY_COUNTDOWN_DELAY	1000



	// Spremenljivka, ki hrani stanje avtomata.
	static GAMEPLAY_states_t state = GAMEPLAY_INIT;


	// Pomožna spremenljivka, kamor bomo shranjevali vrnjeno izhodno
	// vrednost pod-avtomatov (tj. "return exit value").
	uint8_t exit_value = 0;

	// Pomožna spremenljivka, kjer bomo hranili informacijo o pritisnjeni tipki.
	buttons_enum_t	key;


	// Spremenljivka za implementacijo ure štoparice.
	static stopwatch_handle_t   stopwatch;






	// ----------------- Ugnezdena funkcija GamePlay_UpdateChanges() ---------------------


	// Ugnezdena funkcija (angl. nested function) je funkcija, ki je definirana znotraj
	// druge funkcije. Taka ugnezdena funkcija lahko dostopa do spremenljivk te druge funkcije,
	// ki so bile definirane pred ugnezdeno funkcijo. V našem primeru bomo to izkoristili
	// tako, da bomo znotraj ugnezdene funkcije dostopali do spremenljivke "stopwatch", ki vsebuje
	// informacijo o času, ki je še na voljo za igranje.


	// Ugnezdena funkcija GamePlay_UpdateChanges() poskrbi, da se objekti, ki jih algoritem igre spreminja,
	// posodobijo: tako njihovi parametri kot njihov prikaz na zaslonu.
	void GamePlay_UpdateChanges(void)
	{

		// Spremenljivka za implementacijo dodatne ure štoparice, s katero bomo zagotovili pravo hitrost
		// izvajanja igrice.
		static stopwatch_handle_t   update_stopwatch;


		// PAZITE: parametre objektov in njihov izris osvežujemo periodično s
		// točno določeno periodo osveževanja. Tako dosežemo, da igrica vedno teče
		// z enako hitrostjo, neodvisno od hitrosti procesorja (tj. od hitrosti sistemske ure).
		if ( TIMUT_stopwatch_has_another_X_ms_passed( &update_stopwatch, settings.game_play_update_period) )
		{

			// -------- Posodobitve parametrov objektov -----------
			// Clear bullet previous position
			 GFX_clear_gfx_object_on_background(&bullet.gfx_object, &background);

			// Posodobimo parametre objektov s pomočjo GFX modula.

				// Posodobimo lokacijo tarče.
				//GFX_update_moving_gfx_object_location(&target.gfx_object );
			 for (int i = 0; i < NUM_OF_ARDUINO_TYPES; ++i) {
			                GFX_update_moving_arduino_object_location(&arduino[i]);
			            }


			 GFX_update_moving_arduino_object_location(&target);

			// Posodobimo parametre objektov s pomočjo OBJ modula.

				// Posodobimo lokacijo namerka.
				OBJ_crosshair_set_center_location_with_joystick();

				// Posodobimo razdaljo med namerkom in tarčo.
				OBJ_crosshair_update_distance_to_target();

				// Posodobimo vrednost "progress bar" indiktorja, ki kaže,
				// koliko časa je še na voljo za igranje.
				//OBJ_set_timeout_bar_value( 100 - (TIMUT_stopwatch_update(&stopwatch) * 100) / settings.game_play_time );

				// Posodobimo vrednost doseženih točk.


				//OBJ_set_score_text_value(game_status.score);


				OBJ_update_bullet();



				if (bullet.active && GFX_are_gfx_objects_overlapping(&bullet.gfx_object, &target.gfx_object)) {
				    GFX_clear_gfx_object_on_background(&target.gfx_object, &background);
				    GFX_clear_gfx_object_on_background(&bullet.gfx_object, &background);

				    game_status.score += target.points;
				    bullet.active = 0;

				    OBJ_spawn_target();
				}

				if (GFX_are_gfx_objects_overlapping(&crosshair.gfx_object, &target.gfx_object)) {
				    if (game_status.life_points > 0) {
				        game_status.life_points--;
				        OBJ_draw_life_points();
				        GFX_clear_gfx_object_on_background(&target.gfx_object, &background);

				        if (game_status.life_points == 0) {
				            state = GAMEPLAY_BEFORE_GAME_OVER;
				            exit_value = 0;
				            return;
				        } else {
				            OBJ_spawn_target();
				        }
				    }
				}

				// Izrišsemo posodobljene objekte na zaslon s pomočjo GFX modula.

				// Izris namerka in tarče na ozadje.
				GFX_draw_two_gfx_objects_on_background(&crosshair.gfx_object, &target.gfx_object, &background);

				if (bullet.active) {
				    GFX_draw_one_gfx_object_on_background(&bullet.gfx_object, &background);
				}


			// ------ Izris posodobljenih objektov na zaslon -----------




				// Izris vrednosti doseženih točk.
				 OBJ_set_score_text_value(game_status.score);
				 GFX_display_text_object(&score_box_title);
				 GFX_display_text_object(&score_text);


				 OBJ_draw_life_points();



			// -------- Posodobitev ostalih izhodnih naprav sistema -------

				// Posodobimo LED indikator točnosti namerka.
				GamePlay_Update_Aiming_LED_Indicator();

		}

	}






	// --------------- Implementacija algoritma "igranja igre" z avtomatom stanj ------------------------

	// Avtomat stanj implementiramo s pomočjo "switch-case" stavka.
	switch (state)
	{

		// Začetno stanje pod-avtomata.
		//
		// Inicializacija, ki je potrebna pred začetkom igranja igre:
		//

		case GAMEPLAY_INIT:

			// Opravila stanja.
			// 		- inicializacija generatorja naključnih števil
			MATH_init_random_generator();
			//		- izrišemo ozadje
			GFX_draw_gfx_object(&background);
			//		- izrišemo napise "TIME" in "SCORE"
			GFX_display_text_object(&score_box_title);
			//		- izrišemo "progress bar" za prikaz časa, ki je še na voljo za igranje
			//GFX_display_progress_bar(&timeout_bar);
			//		- izpišemo tenutno število točk
			GFX_display_text_object(&score_text);
			OBJ_draw_life_points();



			// Določimo naslednje stanje ter "exit" vrednost.
			state = GAMEPLAY_COUNTDOWN_3;
			exit_value = 0;

		break;






		// Odštevanje do začetka igre - 3.
		// Izrišemo številko 3.
		case GAMEPLAY_COUNTDOWN_3:

			// Opravila stanja.
			GFX_draw_one_gfx_object_on_background(&countdown_digit_3_sprite, &background);

			// Ob koncu tega stanja pričnemo z merjenjem časa premora, ki sledi
			// (tj. sprožimo uro štoparico tako, da postavimo časovni zaznamek - "time mark").
			TIMUT_stopwatch_set_time_mark(&stopwatch);



			// Določimo naslednje stanje ter "exit" vrednost.
			state = GAMEPLAY_COUNTDOWN_2;
			exit_value = 0;

		break;



		// Odštevanje do začetka igre - 2.
		// Preverimo, če že minila 1 sekunda, nato izrišemo številko 2.
		case GAMEPLAY_COUNTDOWN_2:

			// V tem stanju preverjamo, ali se je premor že iztekel.
			if ( TIMUT_stopwatch_has_X_ms_passed(&stopwatch, GAMEPLAY_COUNTDOWN_DELAY) )
			{
				// Če se je premor že iztekel,

				// izrišemo "countdown digit sprite"
				GFX_clear_gfx_object_on_background(&countdown_digit_3_sprite, &background);
				GFX_draw_one_gfx_object_on_background(&countdown_digit_2_sprite, &background);
				TIMUT_stopwatch_set_time_mark(&stopwatch);

				// ter določimo naslednje stanje in "exit" vrednost.
				state = GAMEPLAY_COUNTDOWN_1;
				exit_value = 0;

			}

		break;



		// Odštevanje do začetka igre - 1.
		// Preverimo, če že minila 1 sekunda, nato izrišemo številko 1.
		case GAMEPLAY_COUNTDOWN_1:

			// V tem stanju preverjamo, ali se je premor že iztekel.
			if ( TIMUT_stopwatch_has_X_ms_passed(&stopwatch, GAMEPLAY_COUNTDOWN_DELAY) )
			{
				// Če se je premor že iztekel,

				// izrišemo "countdown digit sprite"
				GFX_clear_gfx_object_on_background(&countdown_digit_2_sprite, &background);
				GFX_draw_one_gfx_object_on_background(&countdown_digit_1_sprite, &background);
				TIMUT_stopwatch_set_time_mark(&stopwatch);

				// ter določimo naslednje stanje in "exit" vrednost.
				state = GAMEPLAY_COUNTDOWN_CLEAR;
				exit_value = 0;

			}

		break;







		// Pobrišemo številko 1.
		// // Preverimo, če že minila 1 sekunda, nato pobrišemo številko 1.
		case GAMEPLAY_COUNTDOWN_CLEAR:

			// V tem stanju preverjamo, ali se je premor že iztekel.
			if (TIMUT_stopwatch_has_X_ms_passed(&stopwatch, GAMEPLAY_COUNTDOWN_DELAY) )
			{
				// Če se je premor že iztekel,

				// izbrišemo "countdown digit sprite"
				GFX_clear_gfx_object_on_background(&countdown_digit_1_sprite, &background);


				// ter določimo naslednje stanje in "exit" vrednost.
				state = GAMEPLAY_SPAWN_TARGET;
				exit_value = 0;

			}

		break;




		/*// Sprožimo merjenje časa, ki je na voljo za igranje igrice.
		case GAMEPLAY_START_MEASURING_GAMEPLAY_TIME:

			// Opravila stanja.
			TIMUT_stopwatch_set_time_mark(&stopwatch);


			// ter določimo naslednje stanje in "exit" vrednost.
			state = GAMEPLAY_SPAWN_TARGET;
			exit_value = 0;


		break;*/







		// Ustvarimo naključno tarčo, počistimo medpomnilnik tipkovnice ter izrišemo novo
		// tarčo na zaslon.
		case GAMEPLAY_SPAWN_TARGET:

			// Opravila stanja.
			KBD_flush();
			OBJ_spawn_target();
			GFX_draw_one_gfx_object_on_background(&target.gfx_object, &background);



			// ter določimo naslednje stanje in "exit" vrednost.
			state = GAMEPLAY_SHOOT_TARGET;
			exit_value = 0;


		break;




		// Izvajamo jedro algoritma za igranje igrice:
		//
		// 		- posodobimo spremembe (tj. spremembe parametrov objektov in spremembe na zaslonu)
		//		- preberemo informacijo o naslednji pritisnjeni tipki
		//		- preverjamo zadetek tarče:
		//			- če tarča zadeta, pobrišemo tarčo, prištejemo točke
		//			- če tarča zgrešena, odštejemo kazenske točke
		//		- preverjamo, če se je že iztekel čas za igranje igrice
		case GAMEPLAY_SHOOT_TARGET:

			// Opravila stanja.

			// Posodobimo spremembe (tj. spremembe parametrov objektov in spremembe na zaslonu).
			GamePlay_UpdateChanges();


			// Pridobimo informacijo o pritisnjenih tipkah.
			key = KBD_get_pressed_button();

			// Če je bila pritisnjena katerakoli tipka, je igralec ustrelil.
			// Sedaj je potrebno preveriti, ali je bil strel  uspsssešen ali ne.
			if ( key != BTN_NONE )
			{
				//bullet
				OBJ_shoot_bullet();
				// Če smo ustrelili, ko je bil namerek na tarči -> zadetek.
				if ( OBJ_is_crosshair_on_target() )
				{
					// Pobrišemo tarčo na ozadju.
					GFX_clear_gfx_object_on_background(&target.gfx_object, &background);

					// Povečamo število točk za toliko, kolikor je bila "vredna tarča"
					game_status.score += target.points;

					// ter določimo naslednje stanje in "exit" vrednost.
					state = GAMEPLAY_SPAWN_TARGET;		// vrnemo se v stanje, kjer ustvarimo naključno novo tarčo
					exit_value = 0;
				}

				/* Če smo ustrelili, ko je bil namerek ni bil na tarči -> zgrešimo.
				//else
				{
					// Zgrešeni streli se kaznujejo. Kazenske točke so predpisane v "settings" objektu.
					game_status.score -= settings.missed_shot_penalty;
				}*/

			}



			/*// Po koncu vseh opravil preverimo, ali se je morda že iztekel čas, ki je navoljo za igranje igre.
			if ( TIMUT_stopwatch_has_X_ms_passed(&stopwatch, settings.game_play_time) )
			{
				// ter določimo naslednje stanje in "exit" vrednost.
				state = GAMEPLAY_BEFORE_GAME_OVER;
				exit_value = 0;
			}*/

		break;





		// Postorimo še zadnje stvari pred "Game Over".
		case GAMEPLAY_BEFORE_GAME_OVER:

			// Opravila stanja.
			OBJ_draw_life_points();
			// Poskrbimo, da indikator preostalega časa resnično kaže vrednost 0,
			//OBJ_set_timeout_bar_value(0);
			GFX_display_text_object(&score_text);


			// ter določimo naslednje stanje in "exit" vrednost.
			// V tem stanju avtomat zaključi izvajanje in se vrne v začetno stanje.
			state = GAMEPLAY_INIT;
			exit_value = 1;


		break;








		// Varovalka za nedefinirana stanja.
		default:

			// Javimo vir napake in vrednost nedefiniranega stanja, v katerem se je našel avtomat.
			printf("GamePlay(): Error - unknown state (%d)", state);

			// Ustavimo izvajanje z namenom, da razvojnik lahko opazi neobičajno obnašanje aplikacije.
			HAL_Delay(5000);

			// In na koncu avomat restiramo tako, da ga postavimo v začetno stanje.
			state = GAMEPLAY_INIT;
			exit_value = 0;

		break;

	}


	// Vrnemo "exit value" pod-avtomata.
	return exit_value;


}


















// Funkcija, ki implementira pod-avtomat GameOver().
uint8_t GameOver()
{
	// Definicija makro parametrov za pod-avtomat GameOver().
	#define GAMEOVER_DELAY_BEFORE_REALIZING	3000
	#define GAMEOVER_DELAY_10s	10000
	// Po vzoru pod-avtomata Intro() implementirajte še pod-avtomat GameOver().
	//Definiramo začetno stanje pod-avtomata
	static GAMEOVER_states_t state = GAMEOVER_SCREEN;

	// Pomožna spremenljivka, kamor bomo shranjevali vrnjeno izhodno
	// vrednost pod-avtomatov (tj. "return exit value").
	// Pazite: ne pozabite smiselno definirati tudi začetne vrednosti te spremenljivke.
	static stopwatch_handle_t stopwatch;
	buttons_enum_t	key;
	uint8_t exit_value = 0;

	switch(state)
	{
		case GAMEOVER_SCREEN:

			GFX_draw_one_gfx_object_on_background(&game_over_sprite, &background);
			LEDs_on(0xFF);
			TIMUT_stopwatch_set_time_mark(&stopwatch);

			state = GAMEOVER_WAIT_BEFORE_LEDS_OFF;
			exit_value = 0;
			// V tem stanju preverjamo, ali se je premor že iztekel.
			break;

		case GAMEOVER_WAIT_BEFORE_LEDS_OFF:
			if (TIMUT_stopwatch_has_X_ms_passed(&stopwatch, GAMEOVER_DELAY_BEFORE_REALIZING))
			{
				// Če se je premor že iztekel,

				// počistimo medpomnilnik tipkovnice.
				KBD_flush();

				// ter določimo naslednje stanje in "exit" vrednost.
				state = GAMEOVER_LEDS_OFF;
				exit_value = 0;

			}
			break;
		case GAMEOVER_LEDS_OFF:
			LEDs_off(0xFF);
			TIMUT_stopwatch_set_time_mark(&stopwatch);
			// ter določimo naslednje stanje in "exit" vrednost.
			state = GAMEOVER_WAIT_FOR_ANY_KEY;
			exit_value = 0;
			break;
		case GAMEOVER_WAIT_FOR_ANY_KEY:
			key = KBD_get_pressed_button();
			if((key != BTN_NONE) || (TIMUT_stopwatch_has_another_X_ms_passed(&stopwatch, GAMEOVER_DELAY_10s)))
				{
				state = GAMEOVER_SCREEN;
				exit_value = 1;
				}
			break;

	}

	return exit_value;


}









// Funkcija GamePlay_Update_Aiming_LED_Indicator() posodobi stanje LED indikatorja
// točnosti namerka: bolj kot je namerek na tarči, več LEDic bo prižganih.
void GamePlay_Update_Aiming_LED_Indicator(void)
{

	// Definicija makro parametrov.
	#define NOMINAL_DISTANCE_BETWEEN_TARGET_AND_CROSSHAIR	150

	// Pomožne spremenljivke.
	int16_t percent_distance;
	int16_t percent_vicinity;

	uint8_t number_of_LEDs_on;

	uint8_t bitmask_right = 0;
	uint8_t bitmask_left = 0;
	uint8_t bitmask;


	// Izračunamo relativno oddaljenost tarče
	percent_distance = (crosshair.distance_to_target_center * 100) / NOMINAL_DISTANCE_BETWEEN_TARGET_AND_CROSSHAIR;


	// Porežemo prevelike vrednosti.
	if (percent_distance > 100)
		percent_distance = 100;


	// in jo pretvorimo v "relativno bližino tarče".
	percent_vicinity = 100 - percent_distance;


	// Preračunamo, koliko LEDic je potrebno prižgati za dano "relativno bližino tarče".
	number_of_LEDs_on = ( (1 + NUM_OF_LEDS/2) * percent_vicinity ) / 100;

	// Porežemo prevelike vrednosti.
	if (number_of_LEDs_on  > (NUM_OF_LEDS/2) )
		number_of_LEDs_on = (NUM_OF_LEDS/2);


	// Tvorimo bitno masko za prižig štirih LEDic na desni strani.
	for(uint8_t i = 1; i <= number_of_LEDs_on ; i++ )
	{
		bitmask_right |= 1;
		bitmask_right = bitmask_right << 1;
	}
	bitmask_right = bitmask_right >> 1;


	// Tvorimo bitno masko za prižig štirih LEDic na levi strani.
	bitmask_left = bitmask_right << (8 - number_of_LEDs_on);

	// Bitni maski združimo v eno samo masko
	bitmask = bitmask_right | bitmask_left;


	// in jo uporabimo za prižig LEDic.
	LEDs_write(bitmask);

}



















// ------- Test functions ---------


// -------------- Private function implementations -------------



/*
 * kbd.c
 *
 *  Created on:
 *      Author:
 */




/* **************** MODULE DESCRIPTION *************************

Ta modul implementira tipkovnico sistema na podlagi tipk, ki so
v sistemu razpoložljive. Modul implementira branje tipkovnice
("skeniranje"), detekcijo pritiska tipk in shranjevanje informacije
o pritisnjenih tipkah v medpomnilnik (angl. buffer).

POZOR: za uspešno uporabo modula je potrebno predhodno poskrbeti
za ustrezno nizko-nivojsko inicializacijo digitalnih GPIO vhodov.


************************************************************* */




// ----------- Include other modules (for private) -------------


#include "LED.h"		// vključimo LED modul za potrebe "keyboard demo" funkcije
#include "stm32g4xx_ll_gpio.h"





// ---------------------- Private definitions ------------------




// -------- Keyboard definitions ----------


#include "kbd.h"


// Najprej pripravimo "handle" strukturo, v kateri bomo hranili vse potrebne parametre za
// delo s tipkovnico.

// "Handle" strukturo bomo definirali po principu "od spodaj navzgor" (angl. "bottom-up approach").


// Definirajmo najprej tip "button_handle_t" strukture, ki hrani vse potrebne parametre
// za delo z eno samo tipko.
typedef struct
{
	// DOPOLNI
	GPIO_TypeDef *port;
	uint32_t pin;
	button_sig_state_t state;
		// Namig: za hranjenje informacije o "stanju tipke" uporabimo naštevni tip "button_sig_state_t" .
		// Hranimo pravzaprav STANJE SIGNALA tipke!

} button_handle_t;





// Sedaj pa s pomočjo zgoraj definirane strukture button_handle_t za delo z eno samo tipko
// definirajmo tip "handle" strukture keyboard_handle_t, ki jo bomo uporabljali za delo s
// celotno tipkovnico.
typedef struct
{
	// tabela "handle" struktur za vse tipke v sistemu
	// DOPOLNI

	button_handle_t 	buttons[NUM_OF_BTNS];


} keyboard_handle_t;




// Vse to zgoraj so bile le DEFINICIJE ZGRADBE podatkovne strukture. Sedaj pa lahko končno ustvarimo
// še strukturno spremenljivko "keyboard", ki bo imela tako zgradbo in bo za nas hranila vse potrebne
// parametre za delo s tipkovnico.

keyboard_handle_t		keyboard;		// spremenljivka "handle" strukture za tipkovnico




// Vidite, da smo "handle strukturo" shranili v GLOBALNO spremenljivko znotraj "kbd.c" modula.
// To je ugodno, saj bodo vse sistemske funkcije za delo s tipkovnico lahko enostavno
// dostopale do potrebnih parametrov znotraj "handle" strukture.


// Zgoraj smo si pripravili "handle" strukturo za delo s tipkovnico, vendar pa je ta struktura
// še neuporabna, saj ni inicializirana! Inicializacijo "handle" strukture bomo izvedli v sklopu
// _init() funkcije spodaj.







// -------- Keyboard buffer definitions ----------

// Pripraviti moramo tudi podatkovne strukture, ki bodo implementirale
// ciklični medpomnilnik (angl. cyclic buffer), kamor bomo shranjevali
// informacijo o pritisnjenih tipkah tipkovnice, katerih še nismo "sprocesirali".


// Najprej je potrebno vključiti knjižnico za delo s cikličnim medpomnilnikom.
#include "buf.h"

// Definirajmo dolžino cikličnega medpomnilnika za tipkovnico. Definirajmo
// jo kot makro parameter.
#define KBD_BUF_LEN 	32


// In sedaj še definirajmo podatkovne strukture, s katerimi bomo implementirali
// ciklični medpomnilnik. Potrebujemo tabelo, kamor bomo shranjevali informacijo
// o pritisnjenih tipkah. In potrebujemo "handle" strukturo za upravljanje
// cikličnega medpomnilnika.
uint8_t 		kbd_buffer[KBD_BUF_LEN];	// the buffer data array
buf_handle_t 	kbd_buf_handle;				// the buffer handle structure


// To je vse v smislu podatkovnih struktur, ki so potrebne za implementacijo
// cikličnega medpomnilnika. Ta medpomnilnik pa  bomo v nadaljevanju uporabljali
// s pomočjo funkcij za delo z medpomnilnikom, ki pa se nahajajo v buf.c modulu.






// -------------- Public function implementations --------------



// Funkcija KBD_init() poskrbi, da se pravilno inicializira keyboard_handle_t strukturna spremenljivka.
// Poskrbi tudi za inicializacijo medpomnilnika tipkovnice.
//
void KBD_init(void)
{

	// 0. Poskrbite, da je ustrezna nizko-nivojska strojna oprema že inicializirana

		// Za inicializacijo GPIO digitalnih vhodov, s katerimi beremo stanja tipk,
		// je poskrbljeno s pomočjo avtomatsko generirane CubeMX programske kode
		// znotraj funkcije MX_GPIO_Init() v "main.c"!



	// 1. Inicializirajte "handle" strukture za posamezne tipke v tipkovnici na strojnem nivoju

		// Definirajte, kako so posamezne tipke povezane na GPIO pine in porte.
		// To storite tako, da pravilno inicializirate "handle" strukture posameznih tipk.
		// Uporabite makroje "GPIOn" in "LL_GPIO_PIN_n" iz nizko-nivojske LL knjižnice.


		// buttons on port C
		keyboard.buttons[BTN_OK].pin = LL_GPIO_PIN_15 ;
		keyboard.buttons[BTN_OK].port = GPIOC;



		keyboard.buttons[BTN_ESC].pin = LL_GPIO_PIN_14 ;
		keyboard.buttons[BTN_ESC].port = GPIOC;



			// DOPOLNI


		// buttons on port G
		keyboard.buttons[BTN_UP].pin = LL_GPIO_PIN_0 ;
		keyboard.buttons[BTN_UP].port = GPIOG;



		keyboard.buttons[BTN_DOWN].pin = LL_GPIO_PIN_1 ;
		keyboard.buttons[BTN_DOWN].port = GPIOG;

		keyboard.buttons[BTN_LEFT].pin = LL_GPIO_PIN_6 ;
		keyboard.buttons[BTN_LEFT].port = GPIOG;



		keyboard.buttons[BTN_RIGHT].pin = LL_GPIO_PIN_8 ;
		keyboard.buttons[BTN_RIGHT].port = GPIOG;



			// DOPOLNI





	// 2. Inicializirajte začetne vrednosti "handle" strukture za posamezne tipke v tipkovnici na sistemskem nivoju


		// Sprehodimo se preko vseh "handle" struktur za delo s posameznimi tipkami.
		for(int i=0; i < NUM_OF_BTNS; i++)
		{
			// Ne pozabite, da v "handle" strukturah za posamezne tipke hranimo tudi vrednost stanja tipke!
			// In te vrednosti je potrebno na začetku nastaviti.
			// Smiselno jih je nastaviti na tako vrednost, kot da so tipke na začetku v sproščenem,
			// nestisnjenem stanju (angl. released). In tu si lahko pomagate s pomožnim
			// naštevnim tipom button_sig_value_t.

			keyboard.buttons[i].state = BTN_RELEASED_SIGNAL_STATE;
		}



	// 3. Inicializiramo še medpomnilnik tipkovnice.

		// Uporabimo funkcijo BUF_init(), ki določi, katera tabela se bo uporabljala kot
		// ciklični medpomnilnik ter kako dolg bo ta medpomnilnik.
		BUF_init( &kbd_buf_handle, kbd_buffer, KBD_BUF_LEN);

}






// Funkcija KBD_scan() prebere trenutno stanje tipk v tipkovnici: ugotovi,
// katere tipke SO BILE PRITISNJENE ter shrani to informacijo v medpomnilnik
// tipkovnice, da se bo kasneje sistem lahko odzval na pritisk teh tipk.
//
void KBD_scan(void)
{
	// Za vsako tipko v tipkovnici, preberi njeno trenutno stanje
	// in ugotovi, ali je bila pritisnjena. Če je bila pritisnjena,
	// shrani to informacijo v medpomnilnik tipkovnice, da se bo
	// kasneje lahko sistem odzval na pritisk te tipke.

	// Pripravimo si pomožno spremenljivko, ki bo začasno hranila vrednost prejšnjega,
	// torej starega stanja tipke.
	button_sig_state_t state_old;


	// Sprehodimo se preko vseh "handle" struktur za delo s posameznimi tipkami.
	for(int i=0; i < NUM_OF_BTNS; i++)
	{
		// Znotraj zanke delamo trenutno z i-to tipko. Spremenljivka "i" je pomožni števec zanke.

		// Trenutno, novo stanje tipke postane sedaj staro stanje tipke in si ga tako shranimo v
		// pomožno spremenljivko.
		state_old = keyboard.buttons[i].state;

		// In novo, trenutno stanje tipke se prebere iz ustreznega digitalnega GPIO vhoda s pomočjo LL funkcije.
		keyboard.buttons[i].state = LL_GPIO_IsInputPinSet(keyboard.buttons[i].port, keyboard.buttons[i].pin) ;



		// Sedaj pa je potrebno preveriti, ali se je za i-to tipko ZGODIL PRITISK. Pri tem si pomagajte s
		// starim in trenutnim stanjem tipke. Pomaga vam lahko tudi pomožni naštevni tip button_sig_value_t.
		if ( state_old == BTN_RELEASED_SIGNAL_STATE  && keyboard.buttons[i].state == BTN_PRESSED_SIGNAL_STATE )
		{

			// In če zaznamo pritisk i-te tipke, shranimo to informacijo v medpomnilnik tipkovnice.
			// Shranimo seveda kar vrednost elementa naštevnega tipa, ki pripada obravnavani i-ti tipki.
			// In to je seveda kar vrednost števca "i". Uporabimo funkcijo BUF_store_byte().
			BUF_store_byte(&kbd_buf_handle, i);

		}

	}

}







// Funkcija KBD_get_pressed_button() iz medpomnilnika tipkovnice vrne
// informacijo o tem, katera je naslednja pritisnjena tipka, na katero
// se še nismo odzvali (tj. je še nismo obdelali, "sprocesirali").
//
buttons_enum_t KBD_get_pressed_button(void)
{

	// Pripravimo si pomožno spremenljivko, v kateri bomo pripravili
	// vrednost, ki jo bo funkcija KBD_get_pressed_key() vrnila.
	buttons_enum_t pressed_button;

	// Vidite, da vračamo vrednosti iz seznama naštevnega tipa "buttons_enum_t".



	// Sedaj pa moramo iz medpomnilnika tipkovnice prebrati naslednji element,
	// tj. informacijo o naslednji pritisnjeni tipki, ki je še nismo obdelali.
	//
	// To storimo z uporabo "buffer" funkcije
	//
	//		BUF_get_byte(buf_handle_t *buf_handle, uint8_t *data)
	//
	// Funkciji je potrebno posredovati naslov "handle" strukture za medpomnilnik
	// (v našem primeru je to "&kbd_buf_handle") ter naslov spremenljivke,
	// kamor naj se shrani vrednost prebranega elementa iz medpomnilnika (v našem primeru
	// je to naslov pomožne spremenljivke "&pressed_button").
	//
	// Če funkcija BUF_get_byte() uspešno prebere element in medpomnilnika,
	// bo vrnila vrednost BUFFER_OK.
	// V nasprotnem primeru sklepamo, da je bilo branje elementa iz
	// medpomnilnika neuspešno, ker je medpomnilnik prazen.

	// Da bo postopek morda lažje razumljiv, si pripravimo še pomožno spremenljivko,
	// kamor bomo shranili vrnjeno vrednost funkcije BUF_get_byte().
	buf_rtrn_codes_t	return_code;



	// Torej, poskusimo prebrati nov element iz medpomnilnika in ga shraniti v
	// spremenljivko "pressed_button". Rezultat operacije shranimo v spremenljikvo "return_code".
	return_code = BUF_get_byte(&kbd_buf_handle, &pressed_button);


	// Če je bilo branje elementa iz medpomnilnika v spremenljivko "pressed_button"
	// uspešno (rezultat operacije je BUFFER_OK),
	if ( return_code == BUFFER_OK )
	{
		// potem je vrednost, ki jo mora funkcija KBD_get_pressed_button() vrniti,
		// kar enaka vrednosti pomožne spremenljivke "pressed_button".

		return pressed_button;
	}
	else
	{

		// Če pa je bilo branje elementa iz medpomnilnika v spremenljivko "pressed_button"
		// neuspešno (tj. ni bil vrnjen BUFFER_OK), potem pa sklepamo, da je medpomnilnik
		// prazen in da pravzaprav ni bila pritisnjena nobena nova tipka.
		// Funkcija KBD_get_pressed_button() mora vrniti kodo "BTN_NONE".

		return BTN_NONE;
	}

}






// Funkcija KBD_is_button_state_pressed() vrne informacijo o tem, ali je specificirana tipka
// TRENUTNO STISNJENA ali ne. Če je tipka stisnjena, vrne vrednost  1, sicer vrne vrednost 0.
// Tipko specificiramo z vhodnim argumentom funkcije "button",
// pri čemer je ta vhodni argument element naštevnega tipa "buttons_enum_t".
uint8_t KBD_is_button_state_pressed(buttons_enum_t button)
{
	// Namig: tu ni potrebno z LL funkcijo brati trenutnega stanja tipke. Trenutno stanje
	// tipke OBIobičajno ugotavljate izključno s funkcijo KBD_scan(). Na ta način namreč zagotovite
	// popoln nadzor nad osveževanjem stanja tipkovnice, ki se izvede ob točno določenih trenutkih.

	// Torej, trenutno stanje tipke vam je že na voljo in je shranjeno v spremenljivki "state"
	// ustrezne "handle" strukture tipke.


	// Preverimo torej, če je trenutno stanje tipke stisnjeno ("pressed state"),
	// in glede na stanje tvorimo ustrezno vrnjeno vrednost funkcije.
	if (keyboard.buttons[button].state == BTN_PRESSED_SIGNAL_STATE)
	{

		return 1;
	}
	else
	{
		return 0;
	}

}





// Funkcija KBD_flush() počisti medpomnilnik tipkovnice. To pomeni, da se zavrže
// vso informacijo o morebitnih pritisnjenih tipkah, ki jih še nismo obdelali.
void KBD_flush(void)
{
	BUF_flush(&kbd_buf_handle);
}


// Funkcija KBD_any_button_been_pressed() vrne infromacijo o tem, ali je bila
// pritisnjena katerakoli nova tipka, ki je še nismo obdelali. To stori tako,
// da preveri velikost podatkov, ki jih hrani medpomnilnik tipkovnice.
uint8_t KBD_any_button_been_pressed(void)
{
	return ( BUF_get_data_size( &kbd_buf_handle ) );
}






// ------- Test functions ---------



// Funkcija KBD_demo() demonstrira delovanje tipkovnice s pomočjo LED modula.
// Funkcija je implementirana tako, da jo je potrebno izvajati v neskončni while(1)
// zanki main() funkcije, saj le s periodičnim "skeniranjem" stanja tipkovnice lahko zagotovimo
// detekcijo pritisnjenih tipk in se nato na te pritiske odzivamo.
void KBD_demo(void)
{

	// Pomožna spremenljivka, ki hrani informacijo o zadnji pritisnjeni tipki.
	buttons_enum_t pressed_key;

	// "Skeniramo" stanje tipkovnice.
		// DOPOLNI
	KBD_scan();


	// Stestiramo preverjanje, če je bila pritisnjena KATERAKOLI tipka:
	// če je bila, spremenimo stanje LEDice LED6.
	if ( KBD_any_button_been_pressed() )
	{
		LED_toggle(LED6);
	}


	// Preberemo informacijo o tem, katera tipka je bila nazadnje pritisnjena.
	pressed_key = KBD_get_pressed_button();



	// V primeru, da je bila pritisnjena katerakoli tipka, se odzivamo na te pritiske
	// s spreminjanjem stanja LEDic.
	while (pressed_key != BTN_NONE)
	{
		// LEDicam spreminjamo stanje glede na to, katera tipka je bila pritisnjena.
		switch (pressed_key)
		{

		 case BTN_OK:
			  LED_toggle(LED0);
			  break;

		 case BTN_RIGHT:
			 LED_toggle(LED1);
			 break;

		 case BTN_LEFT:
				 LED_toggle(LED2);
				 break;

		 case BTN_DOWN:
				 LED_toggle(LED3);
				 break;

		 case BTN_UP:
				 LED_toggle(LED4);
				 break;

		 case BTN_ESC:
		 		 LED_toggle(LED5);
		 		 break;

		  default:
			  break;
		}


		// Nato preberemo informacijo o naslednji pritisnjeni tipki.
		pressed_key = KBD_get_pressed_button();

	}



	// Stestiramo še funkcijo, ki vrne informacijo o tem, ali je tipka TRENUTNO stisnjena:
	// Če je tipka OK stisnjena, naj sveti LED7; sicer pa naj bo ugasnjena.
	LED_set(LED7, KBD_is_button_state_pressed(BTN_OK));



	// In nekoliko počakamo, preden gremo v ponovno "skeniranje" stanja tipkovnice.
	HAL_Delay(10);

	// Vidite, da tu pravzaprav definiramo hitrost osveževanja stanja tipkovnice ("keyboard refresh rate").
}

// Funkcija KBD_demo_toggle_LEDs_if_buttons_pressed() je namenjena testiranju
// delovanja tipkovnice. Deluje tako, da ugotovi, katere tipke so bile pritisnjene
// in se na te pritiske odzove s spremembo stanja ustrezne LEDice ("toggle").
//
// PAZITE: demo funkcija ne izvaja skeniranja tipkovnice! To je potrebno
// izvesti zunaj te funkcije.
void KBD_demo_toggle_LEDs_if_buttons_pressed(void)
{

	// Pomožna spremenljivka, kjer bomo hranili informacijo o nazadnje
	// pritisnjeni tipki.
	buttons_enum_t pressed_key;


	// Preberemo, katera tipka je bila pritisnjena nazadnje
	// (uporabite ustrezno KBD_ funkcijo).
	pressed_key = KBD_get_pressed_button();


	// Če je v pomožni spremenljivki informacija o pritisku dejanske tipke,
	while (pressed_key != BTN_NONE)
	{
		// spremenimo stanje ustrezne LEDice glede na to, katera tipka
		// je bila pritisnjena.
		switch ( pressed_key )
		{
		  case BTN_OK:
			  LED_toggle(LED0);
			  break;

		  case BTN_ESC:
			  LED_toggle(LED1);
			  break;

		  case BTN_UP:
			  LED_toggle(LED2);
			  break;

		  case BTN_DOWN:
			  LED_toggle(LED3);
			  break;

		  case BTN_LEFT:
			  LED_toggle(LED4);
			  break;

		  case BTN_RIGHT:
			  LED_toggle(LED5);
			  break;


		  default:
			  break;
		}


		// Preberemo, če je bila morda pritisnjena še kaka tipka,
		// ki še ni bila obdelana.
		pressed_key = KBD_get_pressed_button();

	}

}





// -------------- Private function implementations -------------



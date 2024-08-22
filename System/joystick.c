// ----------- Include other modules (for private) -------------


#include "joystick.h"

// Vključimo še LED in SCI sistemska modula, ki bosta pomagala
// pri demonstraciji delovanja "joysticka".
#include "LED.h"
#include "SCI.h"




// ---------------------- Private definitions ------------------


// Na enak način kot smo to storili pri tipkovnici, definirajmo najprej tip
// "joystick_button_handle_t" strukture, ki hrani vse potrebne parametre
// za delo s tipko "joysticka".

typedef struct
{
	GPIO_TypeDef 						*port;
	uint32_t 							pin;
	joystick_button_sig_state_t			state;

} joystick_button_handle_t;





// Sedaj pa definirajmo tip "handle" strukture joystick_handle_t, ki jo bomo uporabljali
// za delo z "joystickom".
typedef struct
{
	// Dodamo ji "handle" strukturo za delo s tipko "joysticka".
	joystick_button_handle_t	button;


	// Dodamo sistemske spremenljivke za delo z osmi "joysticka".
	// VEDITE: podatke, ki se tičejo osi "joysticka", hranimo v TABELAH, kjer vsak element tabele hrani
	// informacijo v zvezi z eno od osi "joysticka".
	uint16_t 	position_raw[NUM_OF_AXES];				// "surova" neobdelana meritev trenutne pozicije osi "joysticka"

	uint16_t 	position_raw_min[NUM_OF_AXES];			// informacija o najmanjšem odklonu osi "joysticka"
	uint16_t 	position_raw_max[NUM_OF_AXES];;			// informacija o največjem odklonu osi "joysticka"
	uint16_t 	position_raw_range[NUM_OF_AXES];;		// informacija o razponu odklona osi "joysticka" (angl. axis range)


	// Za merjenje lege osi "joysticka" je potrebno upravljati še z časovnikom, ki
	// periodično proži meritve lege, in DMA enoto, ki skrbi za avtomatsko shranjevanje
	// izvedenih meritev. To pa pomeni, da je potrebno v "handle" strukturi hraniti
	// tudi informacijo o tem, kateri časovnik in katero DMA enoto bomo uporabili
	// za delo z "joystickom". Ti dve informaciji bomo hranili tako, da bomo
	// shranili KAZALCA na "handle" sktrukturo časovnika in "handle strukturo" DMA enote.
	// Saj če imamo dostop do teh "handle" struktur, potem lahko po potrebi s pomočjo
	// HAL funkcij upravljamo s temi perifernimi enotami.
	TIM_HandleTypeDef 	*timer;
	ADC_HandleTypeDef 	*ADC;


} joystick_handle_t;





// Vse to zgoraj so bile le DEFINICIJE ZGRADBE podatkovne strukture. Sedaj pa lahko končno ustvarimo
// še strukturno SPREMENLJIVKO "joystick", ki bo imela tako zgradbo in bo za nas hranila vse potrebne
// parametre za delo z "joystickom".

joystick_handle_t joystick;







// -------- Joystick button buffer ----------

// "Joystick" ima tudi tipke. In pri delu s tipkami je smiselno uporabiti
// medpomnilnik, ki hrani informacijo o tem, katere tipke so bile pritisnjene,
// a se na njihov pritisk še nismo odzvali. Čeprav ima naš "joystick" le eno
// tipko, je uporaba medpomnilnika smiselna. Zakaj? Ker lahko to tipko pritisnemo
// večkrat zapored in se je potrebno na vsak pritisk odzvati posebej
// (primer: s tipko "joysticka" streljate in vsak pritisk pomeni en strel).

// Medpomnilnik uporabimo na popolnoma enak način, kot smo to storili v primeru
// tipkovnice.


// Najprej je potrebno vključiti knjižnico za delo s cikličnim medpomnilnikom.
#include "buf.h"

// Definirajmo dolžino cikličnega medpomnilnika za tipke "joysticka". Definirajmo
// jo kot makro parameter.
#define JOY_BTN_BUF_LEN 	16


// In sedaj še definirajmo podatkovne strukture, s katerimi bomo implementirali
// ciklični medpomnilnik. Potrebujemo tabelo, kamor bomo shranjevali informacijo
// o pritisnjenih tipkah "joysticka". In potrebujemo "handle" strukturo za upravljanje
// cikličnega medpomnilnika.
uint8_t 		joy_btn_buffer[JOY_BTN_BUF_LEN];	// the buffer data array
buf_handle_t 	joy_btn_buf_handle;					// the buffer handle structure










// ------------- Public function implementations --------------




// Funkcija JOY_init() poskrbi za inicializacijo "joystick" modula na sistemskem
// nivoju: inicializira parametre "joystick" handle strukture, inicializira
// medpomnilnik "joysticka" ter inicializira in zažene periferne enote za
// merjenje signalov "joysticka".
//
// POUDARITI velja, da se spodnja inicializacijska funkcija razlikuje od preteklih inicializacijskih
// funkcij v tem smislu, da kot vhodni parameter prejme kazalce na "handle" strukture,
// ki so potrebne za upravljanje perifernih enot s pomočjo HAL funkcij. V našem primeru je torej
// potrebno inicializacijski funkciji posredovati NASLOV "handle" strukture za delo s časovnikom, ki
// proži meritve lege osi "joysticka", ter NASLOV "handle" sktrukture za delo z DMA enoto, ki poskrbi za
// shranjevanje teh meritev v pomnilnik.
void JOY_init(ADC_HandleTypeDef *ADC_handle, TIM_HandleTypeDef *timer_handle)
{

	// 0. Poskrbite, da je ustrezna nizko-nivojska strojna oprema že inicializirana

		// Za merjenje pozicije "joysticka" uporabljamo pristop z DMA enoto in proženje
		// AD pretvornika s pomočjo časovnika. Da bo "joystick" modul pravilno deloval,
		// je potrebno torej poskrbeti za inicializacijo GPIO digitalnega vhoda,
		// časovnika, AD pretvornika ter DMA enote, za kar pa je poskrbljeno s pomočjo
		// avtomatsko generirane CubeMX programske kode v "main.c".



	 // 1. Inicializacija "handle" strukture za tipke "joysticka" na strojnem nivoju

		// Definirajte, kako so posamezne tipke povezane na GPIO pine in porte.
		// To storite tako, da pravilno inicializirate "handle" strukturo tipke "joysticka".
		// Uporabite makroje "GPIOx" in "LL_GPIO_PIN_x" iz nizko-nivojske LL knjižnice.
		// Postopate popolnoma enako, kot smo to storili v primeru tipkovnice.
		joystick.button.pin = LL_GPIO_PIN_13;	// DOPOLNI: zamenjaj "x" z ustrezno vrednostjo.
		joystick.button.port = GPIOC;			// DOPOLNI: zamenjaj "x" z ustrezno vrednostjo.




	// 2. Nastavitev začetnih vrednosti "joystick" sistemskih spremenljivk

		// Začetne vrednosti tipke "joysticka".
			joystick.button.state = JOY_BTN_RELEASED_SIGNAL_STATE;


		// Smiselno nastavimo začetne ekstremne vrednosti pozicije "joysticka".

			// Ideja: če za minimalno vrednost nastavimo začetno vrednost na navečjo še dovoljeno vrednost,
			// potem bodo pri kalibraciji meritve zagotovo manjše od te začetne vrednosti.
			joystick.position_raw_min[X] = 820;
			joystick.position_raw_min[Y] = 739;


			// Ideja: če za maksimalno vrednost nastavimo začetno vrednost na najmenjšo vrednost,
			// potem bodo pri kalibraciji meritve zagotovo večje od te začetne vrednosti.
			joystick.position_raw_max[X] = 3101;
			joystick.position_raw_max[Y] = 3100;

			joystick.position_raw_range[X] = joystick.position_raw_max[X] - joystick.position_raw_min[X];
			joystick.position_raw_range[Y] =  joystick.position_raw_max[Y] - joystick.position_raw_min[Y];


		// Shranimo si kazalce na "handle" strukturi za AD pretvornik in časovnik,
		// ki ju uporabljamo pri implementaciji "joystick" modula. Ti dva kazalca sta
		// posredovana inicializacijski funkciji kot vhodna argumenta funkcije.
		// Ti dve infromaciji seveda shranimo v spremenljivki "joystick handle" strukture.
		joystick.ADC = ADC_handle;
		joystick.timer = timer_handle;



	// 3. Inicializiramo medpomnilnik za tipke "joysticka"

		// Uporabimo funkcijo BUF_init(), ki določi, katera tabela se bo uporabljala kot
		// ciklični medpomnilnik ter kako dolg bo ta medpomnilnik.
		BUF_init( &joy_btn_buf_handle, joy_btn_buffer, JOY_BTN_BUF_LEN);




	// 4. Poskrbimo za zagon strojne opreme za merjenje signalov "joysticka"

		// Pri tem uporabljamo funkcije HAL knjižnice.


		// Najprej kalibriramo AD pretvornik, da bodo meritve pozicije "joysticka" bolj točne.
		// Kalibracijo izvedemo za "single-ended" vhodne signale.
		HAL_ADCEx_Calibration_Start( joystick.ADC, ADC_SINGLE_ENDED);


		// Nato omogočimo DMA enoto v navezi z AD pretvornikom (rečemo tudi, da zaženemo
		// AD pretovrnik v DMA načinu).
		//
		// To storimo s klicem spodnje HAL funkcije, ki DMA enoti "sporoči",
		// s katerim AD pretvornikom bo delala, na katero mesto v pomnilniku
		// naj shranjuje rezultate AD pretvorbe ter koliko teh rezultatov bo morala prenesti
		// ob vsaki končani AD pretvorbi.
		// Namig: meritve AD pretvornika želimo shraniti v ustrezno tabelo znotraj
		// "joystick" handle strukture. Število rezultatev AD pretvorbe pa je 
		// enako dolžini te tabele.
		HAL_ADC_Start_DMA(joystick.ADC, (uint32_t*)joystick.position_raw, NUM_OF_AXES);


		// Na koncu pa še zaženemo časovnik, ki bo prožil AD pretvorbe.
		// S tem smo pravzaprav sprožili "avtomatsko" merjenje pozicije "joysticka".
		HAL_TIM_Base_Start( joystick.timer );


	// 5. Počakamo na prvo meritev pozicije "joysticka"

		// Časovnik proži AD pretvorbe vsakih 10 milisekund.
		// Torej je smiselno počakati milisekundo dlje.
		HAL_Delay(1);

}








// ------- Funkcionalnost povezana s tipkami "joysticka" ----------


// Funkcionalnost "joystickovih" tipk implementiramo na popolnoma enak način,
// kot smo to storili v primeru tipkovnice.


// Funkcija JOY_scan_button() prebere trenutno stanje tipke "joysticka"; ugotovi,
// ali je bila tipka pritisnjena ter shrani to informacijo v medpomnilnik
// "joysticka", da se bo kasneje lahko sistem odzval na pritisk te tipke.
void JOY_scan_button(void)
{
	// Ker ima naš "joystick" le eno samo tipko, se funkcija poenostavi.

	// Pripravimo si pomožno spremenljivko, ki bo začasno hranila vrednost prejšnjega,
    // torej starega stanja tipke.
	joystick_button_sig_state_t state_old;


	// Trenutno, novo stanje tipke postane sedaj staro stanje tipke in si ga tako shranimo v
	// pomožno spremenljivko.
	state_old = joystick.button.state;

	// In novo, trenutno stanje tipke se prebere iz ustreznega digitalnega GPIO vhoda s pomočjo LL funkcije.
	joystick.button.state = LL_GPIO_IsInputPinSet( joystick.button.port, joystick.button.pin );



	// Sedaj je potrebno preveriti, ali se je za to tipko zgodil pritisk. Pri tem si pomagajte s starim in
	// trenutnim stanjem tipke. Pomaga vam lahko tudi pomožni naštevni tip "joystick_button_sig_state_t".
	if ( ( state_old == JOY_BTN_RELEASED_SIGNAL_STATE) && ( joystick.button.state == JOY_BTN_PRESSED_SIGNAL_STATE ) )
	{

		// In če zaznamo pritisk tipke, shranimo to informacijo v medpomnilnik "joystick" tipkovnice.
		// Ker ima "joystick" le eno samo tipko, shranimo vedno isto informacijo: JOY_BTN_FIRE.
		// Uporabimo funkcijo BUF_store_byte().
		BUF_store_byte( &joy_btn_buf_handle, JOY_BTN_FIRE);

	}

}



// Funkcija JOY_get_pressed_button() iz medpomnilnika "joysticka" vrne
// informacijo o tem, katera je naslednja pritisnjena tipka, na katero
// se še nismo odzvali (tj. je nismo obdelali, "sprocesirali").
joystick_buttons_enum_t JOY_get_pressed_button(void)
{
	// Ustvarimo pomožno spremenljivko, v kateri bomo pripravili
	// vrednost, ki jo bo funkcija JOY_get_pressed_button() vrnila.
	// Ustvarimo tudi pomožno spremenljivko, ki bo hranila informacijo
	// o uspešnosti branja iz medpomnilnika.
	joystick_buttons_enum_t 		pressed_button;
	buf_rtrn_codes_t				return_code;


	// Sedaj poskusimo prebrati nov element iz medpomnilnika in ga shraniti v spremenljivko "pressed_button"
	// Hkrati si v spremenljivko "return_code" zabeležimo vrnjeno kodo "buffer" funkcije.
	return_code = BUF_get_byte(&joy_btn_buf_handle, &pressed_button);


	// Če je bilo branje elementa iz medpomnilnika v spremenljivko "pressed_button"
	// uspešno (tj. vrnjen BUFFER_OK),
	if ( return_code == BUFFER_OK )
	{
		//potem je vrednost, ki jo mora funkcija JOY_get_pressed_button() vrniti
		// kar enaka vrednosti pomožne spremenljivke "pressed_button".
		return pressed_button;
	}
	else
	{
		// Če pa je bilo branje elementa iz medpomnilnika v spremenljivko "pressed_button"
		// neuspešno (tj. ni bil vrnjen BUFFER_OK), potem pa sklepamo, da je medpomnilnik
		// prazen in da pravzaprav ni bila pritisnjena nobena nova tipka.
		// Funkcija JOY_get_pressed_button() mora vrniti kodo "JOY_BTN_NONE".
		return JOY_BTN_NONE;
	}

}



// Funkcija JOY_get_button_state() vrne TRENUTNO stanje tipke "joysticka".
// Ker ima "joystick" eno samo tipko, je funkcija poenostavljena.
uint8_t JOY_is_button_state_pressed(void)
{

	if ( joystick.button.state == JOY_BTN_PRESSED_SIGNAL_STATE )
	{

		return 1 ;
	}
	else
	{
		return 0;
	}

}



// Funkcija JOY_flush() počisti medpomnilnik za tipke "joysticka". To pomeni, da se zavrže
// vso informacijo o morebitnih pritisnjenih tipkah, ki jih še nismo obdelali.
void JOY_flush(void)
{
	BUF_flush(&joy_btn_buf_handle);
}







// ------- Funkcionalnost povezana z osmi "joysticka" ----------




// Funkcija JOY_calibrate() kalibrira "joystick". To pomeni, da
// funkcija spremlja meritve lege osi "joysticka" in si beleži,
// kolikšna je bila najmanjša oziroma največja vrednost meritve.
// Z drugimi besedami - pomerijo se ekstremne vrednosti za lege osi "joysticka".
// Kalibrirane ekstremne vrednosti bomo nato uporabili za izračun
// relativne pozicije osi "joysticka".
// Kalibracijo zaključimo s pritiskom na tipko "joysticka".
void JOY_calibrate(void)
{

	// Pomožna spremenljivka, kjer hranimo informacijo o pritisnjeni tipki "joysticka".
	// Sprva privzamemo, da tipka "joysticka" še ni bila pritisnjena.
	joystick_buttons_enum_t pressed_key = JOY_get_pressed_button();


	// Dokler ni pritisnjena tipka "joysticka", izvajmo kalibracijo.
	while (pressed_key == JOY_BTN_NONE)
	{

		// Beležimo si minimalne vrednosti (najmanši odklon osi).
		// Da spomnimo: parametre za posamezne osi "joysticka" hranimo v tabelah.
		// In za naslavljanje elementov v tabelah uporabljamo naštevna elementa "X" in "Y"
		// tipa "joystick_axes_enum_t".

			// Za X os "joysticka". 
			// Če je trenutna meritev odklona manjša od zabeležene minimalne vrednosti, 
			if ( joystick.position_raw[X] < joystick.position_raw_min[X] )
			{
				// to vrednost zabeležimo kot novo minimalno vrednost.
				joystick.position_raw_min[X] = joystick.position_raw[X];
			}

			// Za Y os "joysticka".
			// Če je trenutna meritve odklona manjša od zabeležene minimalne vrednosti, 
			if ( joystick.position_raw[Y] < joystick.position_raw_min[Y] )
			{
				// to vrednost zabeležimo kot novo minimalno vrednost.
				joystick.position_raw_min[Y] = joystick.position_raw[Y];
			}


		// Beležimo si maksimalne vrednosti (največji odklon osi).

			// Za X os "joysticka".
			// Če je trenutna meritev odklona večja od zabeležene maksimalne vrednosti, 
			if ( joystick.position_raw[X] > joystick.position_raw_max[X])
			{
				// to vrednost zabeležimo kot novo maksimalno vrednost.
				joystick.position_raw_max[X] = joystick.position_raw[X];
			}

			// Za Y os "joysticka".
			// Če je trenutna meritev odklona večja od zabeležene maksimalne vrednosti, 
			if ( joystick.position_raw[Y] > joystick.position_raw_max[Y] )
			{
				// to vrednost zabeležimo kot novo maksimalno vrednost.
				joystick.position_raw_max[Y] = joystick.position_raw[Y];
			}


		// Iz ekstremnim vrednosti lahko poračunamo razpon odklona (angl. axis range).
		joystick.position_raw_range[X] = joystick.position_raw_max[X] - joystick.position_raw_min[X];
		joystick.position_raw_range[Y] =  joystick.position_raw_max[Y] - joystick.position_raw_min[Y];


		// Preverimo, če ni morda medtem bila pritisnjena tipka "joysticka", ki ustavi izvajanje
		// kalibracije.
		JOY_scan_button();	// DOPOLNI			// skeniramo stanje tipke "joysticka"
		pressed_key = JOY_get_pressed_button();		// preberemo informacijo o zadnji pritisnjeni tipki


		// Izpišemo status "joysticka" preko SCI vmesnika, kar nam pomaga
		// pri "debuggiranju" kalibracije "joysticka".
		printf("******** CALIBRATION IN PROGRESS ********\n");
		JOY_SCI_send_status();


		// Dodamo časovni zamik, preden se ponovno lotimo beleženja ekstremnih vrednosti.
		HAL_Delay(100);

	}

}








// Funkcija JOY_get_axis_position() vrne RELATIVNO pozicijo osi "joysticka".
// Relativno pozicijo "joysticka" izračunamo na podlagi trenutne "surove" meritve
// pozicije osi ter ekstremnih vrednosti osi, ki jih hranimo v "handle" strukturi
// "joysticka". Os "joysticka" specificiramo kot vhodni argument funkcije "axis".
// PAZITE: funkcija mora poskrbeti tudi, da so vrednosti, ki jih vračamo, vedno smiselno
// omejene na interval [0..100].
uint8_t JOY_get_axis_position(joystick_axes_enum_t axis)
{

	// Pomožna spremenljivka, kjer bomo hranili izračunano relativno pozicijo osi.
	// POZOR: uporabimo predznačeni celoštevilski tip, ki nam bo pomagal zaznati
	// primer, da je trenutna meritev pozicije manjša od kalibrirane minimalne vrednosti.
	int16_t	relative_position;


	// Izračun relativne pozicije osi na procent natančno.
	//
	// PAZITE #1: uporablja se celoštevilska aritmetika! Operacija deljenja vrne celoštevilski
	// količnik brez decimalnega deleža zaradi ostanka! Zato je vrstni red izvajanja operacij pomemben:
	// če izmerjeni odklon od minimalne lege "prehitro" delite z razponom osi ("range-om"),
	// boste vedno dobili rezultat enak 0 ali kvečjemu 1. Ideja: kaj pa, če bi izmerjeni odklon
	// pred deljenjem z "range-om" pomnožili s 100, kar moramo že tako ali tako storiti? Premislite.
	//
	// PAZITE #2: pri izračunu relativne lege je potrebno deliti z razponom osi "joysticka" ("range-om").
	// Pri tem lahko pride do sledečega problema: če je razpon osi "joysticka" napačno kalibriran tako,
	// da je enak 0, lahko to povzroči deljenje z ničlo! To pa bi "vrglo" procesor v posebno stanje
	// napake ("fault state") in celoten program bi se vam obesil. Zato je smiselno pred izračunom preveriti
	// če razpon osi "joysticka" različen od 0.
	if (joystick.position_raw_range[axis] != 0)
		relative_position = (((int32_t)joystick.position_raw[axis] - joystick.position_raw_min[axis]) * 100) / joystick.position_raw_range[axis];
	// ne smemo dovoliti, da vrne vrednost večjo od 100 oziroma manjšo od 0. V primeru
	// takih nedovoljenih izračunanih vrednosti to vrednost zaokrožimo na 100 oziroma 0.


		// Obravnavamo primer, ko je trenutna meritev pozicije manjša od kalibrirane minimalne vrednosti.
		if (relative_position < 0)
		{
			return 0;
		}

		// Obravnavamo primer, ko je trenutna meritev pozicije večja od kalibrirane maksimalne vrednosti.
		else if (relative_position > 100)
		{
			return 100;
		}

		// Ostane le še primer, ko se izračunana relativna pozicija nahaja na intervalu [0..100].
		else
		{
			return relative_position;
		}

}









// ----------------------- Test functions -------------------------------


// Funkcija JOY_SCI_send_status() je pomožna "debug" funkcija, ki nam pomaga izpisovati
// stanje "joysticka" preko serijskega vmesnika SCI. Tako lahko v terminalu opazujete
// meritve osi "joysticka" v realnem času.
void JOY_SCI_send_status(void)
{
	printf("--- Joystick status ---\n");

	// Sistemske spremenljivke "joystick" modula, ki jih hranimo
	// v tabelah "joystick handle" strukture.

	printf("X_RAW = %d\n", joystick.position_raw[X]);
	printf("Y_RAW = %d\n", joystick.position_raw[Y]);

	printf("X_RAW_MIN = %d\n", joystick.position_raw_min[X]);
	printf("X_RAW_MAX = %d\n", joystick.position_raw_max[X]);

	printf("Y_RAW_MIN = %d\n", joystick.position_raw_min[Y]);
	printf("Y_RAW_MAX = %d\n", joystick.position_raw_max[Y]);

	printf("X_RAW_RANGE = %d\n", joystick.position_raw_range[X]);
	printf("Y_RAW_RANGE = %d\n", joystick.position_raw_range[Y]);


	// Trenutna relativna lega osi "joysticka".
	printf("X = %d\n", JOY_get_axis_position(X));
	printf("Y = %d\n", JOY_get_axis_position(Y) );

	printf("\n");
}




// Funkcija JOY_button_demo() demonstrira delovanje tipke "joysticka".
// Demonstracija je izvedena na popolnoma enak način, kot smo to storili
// v primeru tipkovnice.
void JOY_button_demo(void)
{

	joystick_buttons_enum_t pressed_key;


	// Skeniraj stanje tipke "joysticka".
	JOY_scan_button();

	// Preberi informacijo o zadnji pritisnjeni tipki.
	pressed_key = JOY_get_pressed_button();  // DOPOLNI


	// Dokler je zadnja pritisnjena tipka še dejanska tipka, na katero se še nismo odzvali.
	while (pressed_key != JOY_BTN_NONE)
	{
		// Spremeni stanje ustrezne LEDice.
		switch (pressed_key)
		{
		  case JOY_BTN_FIRE:

			  LED_toggle(LED0);
			  break;

		  default:
			  break;
		}

		// Preberi informacijo o zadnji pritisnjeni tipki.
		pressed_key = JOY_get_pressed_button();

	}

	// Demonstriraj odzivanje na stanje tipke.
	LED_set(LED7, JOY_is_button_state_pressed() );


	// Majnha zakasnitev, ki določa hitrost osveževanja branja tipke.
	HAL_Delay(10);
}





// Funkcija JOY_LED_demo() demonstrira funkcionalnost "joysticka" s pomočjo
// prižiganja LEDice glede na trenutno relativno lego osi joysticka: če je os
// "joysticka" porinjena v skrajno pozicijo, potem sveti ustrezna "krajna" LEDica.
// Vhodni argument funkcije specificira, katera os "joysticka" naj se uporabi za
// demonstracijo.
void JOY_LED_demo(joystick_axes_enum_t axis)
{

	// DODATNA NEOBVEZNA NALOGA

	// DOPOLNI

}



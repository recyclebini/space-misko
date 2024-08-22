/*
 * SCI.h
 *
 *  Created on:
 *      Author:
 */

#ifndef INCLUDE_SCI_H_
#define INCLUDE_SCI_H_



// ----------- Include other modules (for public) -------------

// Vključimo nizko-nivojsko LL knjižnjico, da dobimo podporo za delo z USART vmesnikom.

	// DOPOLNI
#include  "stm32g4xx_ll_usart.h"


// Pri implementaciji sistemskih funkcij serijskega vmesnika SCI bomo potrebovali sledeče nizko-nivojske funkcije
// za upravljanje USART periferne enote:
//
//	   DOPOLNI
// __STATIC_INLINE void LL_USART_TransmitData8 (USART_TypeDef * USARTx, uint8_t Value)
//__STATIC_INLINE uint8_t LL_USART_ReceiveData8 (USART_TypeDef * USARTx)
//_STATIC_INLINE uint32_t LL_USART_IsActiveFlag_TXE_TXFNF (USART_TypeDef * USARTx)
//__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_RXNE_RXFNE (USART_TypeDef * USARTx)

// Iz zgornjega seznama funkcij lahko vidimo, da bo potrebno v "handle" strukturi SCI vmesnika hraniti
// samo en parameter, ki določa, katero USARTx periferno enoto bomo uporabili.



// "Javno" vključimo še standardno "stdio" knjižnico, v kateri se nahaja printf() funkcija.
// Tako zagotovimo, da bodo lahko tudi ostali moduli, ki uporabljajo SCI modul, lahko uporabljali
// prirejeno printf() funkcijo (ki smo jo priredili znotraj SCI modula).
#include <stdio.h>



// -------------------- Public definitions --------------------


// "Javno" definirajmo še pomožne "vrnjene kode" (angl. return codes), s katerimi bodo
// SCI funkcije sporočale, ali je bila operacija uspešna ali ne.
// Uporabimo naštevni tip.
typedef enum { SCI_ERROR = -1, SCI_NO_ERROR = 0} SCI_rtrn_codes_t;



// ******** Dopolniti je potrebno seznam uporabljanih LL funkcij:

//	Za uporabo USART vmesnika s pomočjo prekinitev bomo potrebovali sledeče LL funkcije:
//		- void LL_USART_EnableIT_RXNE_RXFNE(USART_TypeDef *USARTx)
//
//			DOPOLNI seznam z novimi LL funkcijami,
//				s pomočjo katerih uporabljamo USART prekinitve




// Pri uporabi USART vmesnika s prekinitvami je smiselno uporabiti sprejemni (RX) ciklični medpomnilnik,
// ki bo hranil sprejete, a še neobdelane podatke, ter SCI oddajni (TX) medpomnilnik, ki pa bo
// hranil podatke, ki jih je še potrebno poslati. Zato vključimo še modul "buf".
#include "buf.h"






// *********** Dodati je potrebno dodatne "javne" prototipe funkcij:


// --------- Prototipi funkcij za delo v načinu s prekinitvami ----------


// SCI modulu bomo dodali novo funkcionalnost, ki temelji na uporabi prekinitev:


// - funkciji za pošiljanje niza znakov oziroma skupine binarnih podatkov:
void SCI_send_string_IT(char *str);
void SCI_send_bytes_IT(uint8_t *data, uint32_t size);

	// Mimogrede, končnica "_IT" v imenu funkcij namiguje, da sta ti dve funkciji implementirani
	// s pomočjo prekinitev.



// - "callback" funkciji za implementacijo sprejema in pošiljanja enega bajta

void SCI_receive_byte_Callback(void);
void SCI_transmit_byte_Callback(void);


	// "Callback" funkcije so funkcije, ki jih pokličemo, ko se zgodi ustrezna prekinitev,
	// in pravzaprav realizirajo vsebino prekinitvene rutine.
	//
	// Prvo funkcijo bomo s pomočjo prekinitev poklicali vsakič, ko bo USART sprejemnik
	// sprejel nov podatek in bo poskrbela, da se ta podatek shrani v
	// sprejemni RX sistemski medpomnilnik.
	//
	// Drugo funkcijo pa bomo s pomočjo prekinitev poklicali vsakič, ko bo USART oddajnik
	// pripravljen sprejeti nov podatek za pošiljanje. Ta funkcija bo poskrbela, da se
	// pošljejo vsi podatki, ki čakajo v oddajnem TX sistemskem medpomnilniku.



// -  funkcije za delo z SCI sprejemnim RX in oddajnim TX medpomnilnikom

uint32_t SCI_RX_buffer_get_data_size(void);
uint32_t SCI_is_RX_buffer_not_empty(void);
buf_rtrn_codes_t SCI_RX_buffer_get_byte(uint8_t *data);
buf_rtrn_codes_t SCI_RX_buffer_get_bytes(uint8_t *data,  uint32_t size);
buf_rtrn_codes_t SCI_RX_buffer_flush(void);

buf_rtrn_codes_t SCI_TX_buffer_flush(void);



// - "demo" funkcije za demonstracijo komunikacije z uporabo prekinitev

void SCI_demo_receive_with_interrupts(void);
void SCI_demo_transmit_with_interrupts(void);
void SCI_demo_echo_with_interrupts(void);






// ---------------- Public function prototypes ----------------

void SCI_init(void);

void SCI_send_char(char c);
void SCI_send_byte(uint8_t data);

uint8_t SCI_is_data_waiting(void);

SCI_rtrn_codes_t SCI_read_char(char *c);
SCI_rtrn_codes_t SCI_read_byte(uint8_t *data);


void SCI_send_string(char *str);
void SCI_send_bytes(uint8_t *data, uint32_t size);


void SCI_demo_Hello_world(void);
void SCI_demo_echo_with_polling(void);
void SCI_demo_Hello_world_printf(void);


#endif /* INCLUDE_SCI_H_ */

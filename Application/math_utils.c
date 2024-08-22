/*
 * math_utils.c
 *
 *  Created on: 18 May 2022
 *      Author: Gasper
 */


/* **************** MODULE DESCRIPTION *************************

This module implements various math functions that the application
might need.

************************************************************* */


// ----------- Include other modules (for private) -------------


#include "math_utils.h"


// ------ Standard C libraries -----

#include <stdlib.h>		// support for random number generation
#include <math.h>



// -------- System level modules -------

#include "timing_utils.h"

#include "SCI.h"	// for demo only
#include "kbd.h"	// for demo only





// ---------------------- Private definitions ------------------


// ------------- Public function implementations --------------




void MATH_init_random_generator(void)
{
	// use the current millisecond tick counter as a seed for random generator
	srand( HAL_GetTick() );
}





int32_t MATH_random_integer_number_from_interval(int32_t x_min, int32_t x_max)
{
	uint32_t	interval_length;



	interval_length = x_max - x_min;

	return   ( x_min + ( rand() % (interval_length + 1) ) );

}


int8_t MATH_random_sign(void)
{
	if ( rand() > (RAND_MAX/2) )
	{
		return 1;
	}
	else
	{
		return -1;
	}

}




uint32_t MATH_calculate_distance_between_points(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint32_t dx, dy, distance;

	dx = x1 - x2;
	dy = y1 - y2;


	distance = ( (uint32_t) sqrt( dx*dx + dy*dy ) );

	return distance;

}










// ------- Test functions ---------



void MATH_demo_random_integer_number_from_interval(void)
{
	int32_t rnd;

	while( KBD_get_pressed_button() == BTN_NONE );	// wait for button pressed


	MATH_init_random_generator();


	while(1)
	{
		rnd = MATH_random_integer_number_from_interval(0,2);
		printf("random number = %li\n" ,rnd);

		HAL_Delay(500);

	}

}


// -------------- Private function implementations -------------

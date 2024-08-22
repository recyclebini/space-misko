/*
 * math_utils.h
 *
 *  Created on: 18 May 2022
 *      Author: Gasper
 */

#ifndef INCLUDE_MATH_UTILS_H_
#define INCLUDE_MATH_UTILS_H_



// ----------- Include other modules (for public) -------------

// ------ Standard C libraries -----

#include <stdio.h>		// data types definitions
#include <stdint.h>   // fixed-width integer types




// -------------------- Public definitions --------------------


// ---------------- Public function prototypes ----------------


void MATH_init_random_generator(void);
int32_t MATH_random_integer_number_from_interval(int32_t x_min, int32_t x_max);
int8_t MATH_random_sign(void);

uint32_t MATH_calculate_distance_between_points(int16_t x1, int16_t y1, int16_t x2, int16_t y2);


void MATH_demo_random_integer_number_from_interval(void);


#endif /* INCLUDE_MATH_UTILS_H_ */

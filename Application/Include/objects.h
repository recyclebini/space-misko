/*
 * objects.h
 *
 *  Created on: 10 May 2022
 *      Author: Gasper
 */

#ifndef INCLUDE_OBJECTS_H_
#define INCLUDE_OBJECTS_H_






// ----------- Include other modules (for public) -------------




// ------ Standard C libraries -----

#include "stdint.h"		// support for data type definitions
#include <stdio.h>		// add sprintf() functionality




// -------- Application level modules -------

#include "images.h"			// definitions for image objects
#include "graphics.h"		// support for graphic objects

#include "math_utils.h"		// different math functions (random number generation etc.)







// -------------------- Public definitions --------------------


#define MAX_LIFE_POINTS 3
#define MAX_Y_OFFSET 50
#define MIN_Y_OFFSET 200


typedef struct
{
	// min/max limits for velocity of moving objects
	velocity_t	abs_velocity_min;
	velocity_t	abs_velocity_max;

	// timing parameters
	uint32_t		game_play_time;					// duration for game play (in milliseconds)
	uint32_t		game_play_update_period;		// duration between game play updates (in milliseconds)

	// missed shot penalty
	int16_t		missed_shot_penalty;		// how many points are taken if a shot is missed

} settings_t;



typedef struct
{
	int16_t	score;
	uint8_t life_points;

} game_status_t;






// canvas object (parameters about area where graphics is drawn)
typedef struct
{
	location_t	whole_area;
	location_t	above_ground;
	location_t	below_ground;

} canvas_t;





// ------ COMPOUND OBJECTS TYPE DEFINITIONS ---------


// ---- Arduino object type definitions ----
typedef enum {UNO, MEGA, NANO, INVADER,    NUM_OF_ARDUINO_TYPES} arduino_types_enum_t;

typedef enum {MOVE_RIGHT, MOVE_LEFT} movement_direction_t;

typedef struct arduino_object_t
{
	// graphic object
	graphic_object_t	gfx_object;


	// arduino specific parameters
	uint8_t 		points;				// points earned when destroyed
	movement_direction_t direction;


} arduino_object_t;




// ---- Crosshair object type definitions ----

typedef struct
{
	// graphic object
	graphic_object_t	gfx_object;

	// crosshair specific parameters
	uint16_t	distance_to_target_center;		// distance in pixels

} crosshair_object_t;



// --- Bullet ---
typedef struct
{
	// graphic object
	graphic_object_t gfx_object;

	// bullet specific parameters
	uint16_t active; // flag to check if bullet is active
} bullet_object_t;











// ------------- OBJECT ARE EXTERNAL GLOBAL VARIABLES  -----------

// This simplifies inter-module data transfer at the risk of exposing data.

extern settings_t 		settings;
extern game_status_t	game_status;
extern canvas_t			canvas;



extern graphic_object_t	splash_screen;
extern graphic_object_t	background;

extern graphic_object_t	game_over_sprite;
extern graphic_object_t	press_any_key_sprite;

extern graphic_object_t	countdown_digit_1_sprite;
extern graphic_object_t	countdown_digit_2_sprite;
extern graphic_object_t	countdown_digit_3_sprite;



extern arduino_object_t		arduino[NUM_OF_ARDUINO_TYPES];

extern crosshair_object_t	crosshair;
extern arduino_object_t		target;
extern bullet_object_t bullet;


extern text_object_t	score_box_title;
extern text_object_t	score_text;

extern text_object_t	time_box_title;
extern progress_bar_t	timeout_bar;
extern graphic_object_t life_points[MAX_LIFE_POINTS];







// ---------------- Public function prototypes ----------------

void OBJ_init(void);

void OBJ_init_settings(void);
void OBJ_init_game_status(void);
void OBJ_init_canvas(void);

void OBJ_init_splash_screen(void);
void OBJ_init_background(void);

void OBJ_init_game_over_sprite(void);
void OBJ_init_press_any_key_sprite(void);
void OBJ_init_countdown_digit_sprites(void);

void OBJ_init_arduinos(void);

void OBJ_init_crosshair(void);
void OBJ_crosshair_set_center_location_with_joystick(void);
void OBJ_crosshair_update_distance_to_target(void);
uint8_t OBJ_is_crosshair_on_target(void);
void OBJ_init_bullet(void);
void OBJ_update_bullet(void);
void OBJ_shoot_bullet(void);
void OBJ_init_life_points(void);

void OBJ_init_target(arduino_types_enum_t type);
void OBJ_spawn_target(void);
void OBJ_spawn_two_targets(void);

void OBJ_init_score_box_title(void);
void OBJ_init_score_text(void);
void OBJ_set_score_text_value(int16_t score);

void OBJ_init_time_box_title(void);
void OBJ_init_timeout_bar(void);
void OBJ_set_timeout_bar_value(uint8_t value);




void OBJ_demo_spawn_target(void);


#endif /* INCLUDE_OBJECTS_H_ */

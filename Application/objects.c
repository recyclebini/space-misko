/*
 * objects.c
 *
 *  Created on: 10 May 2022
 *      Author: Gasper
 */



/* **************** MODULE DESCRIPTION *************************

This module defines application objects and functions that manipulate
the object parameters.


************************************************************* */





// ----------- Include other modules (for private) -------------


// ------ Standard C libraries -----

#include <string.h>		// support for memory copy





// -------- System level modules -------

#include "joystick.h"

#include "kbd.h"				// for demo only
//#include "timing_utils.h"		// for demo only



// -------- Application level modules -------

#include "objects.h"






// ---------------------- Private definitions ------------------



// ---- Object for defining application settings and status ------


settings_t 		settings;
game_status_t	game_status;

canvas_t		canvas;



// --- Graphical objects ----

graphic_object_t	splash_screen;
graphic_object_t	background;
graphic_object_t	game_over_sprite;
graphic_object_t	press_any_key_sprite;
graphic_object_t 	life_points[MAX_LIFE_POINTS];

graphic_object_t	countdown_digit_1_sprite;
graphic_object_t	countdown_digit_2_sprite;
graphic_object_t	countdown_digit_3_sprite;

progress_bar_t	timeout_bar;




// ------- Compound objects -------

arduino_object_t	arduino[NUM_OF_ARDUINO_TYPES];

crosshair_object_t	crosshair;
arduino_object_t	target;
bullet_object_t bullet;



// ----- Text objects ------

text_object_t	score_box_title;
text_object_t	score_text;

text_object_t	time_box_title;







// ------------- Public function implementations --------------

// init all the objects
void OBJ_init(void)
{
	// init settings
	OBJ_init_settings();
	OBJ_init_game_status();
	OBJ_init_canvas();

	// full screen images
	OBJ_init_splash_screen();
	OBJ_init_background();

	// sprites
	OBJ_init_game_over_sprite();
	OBJ_init_press_any_key_sprite();
	OBJ_init_arduinos();
	OBJ_init_crosshair();
	OBJ_init_countdown_digit_sprites();
	OBJ_init_bullet();
	OBJ_init_life_points();


	// text boxes
	OBJ_init_score_box_title();
	OBJ_init_score_text();

	//OBJ_init_time_box_title();


	// init progress bar
	//OBJ_init_timeout_bar();


}






// ---------------- OBJECTS for SETTINGS ----------------



void OBJ_init_settings(void)
{
	// set the min/max limits for velocity of moving objects
	settings.abs_velocity_min.x = 2;
	settings.abs_velocity_min.y = 4;

	settings.abs_velocity_max.x = 5;
	settings.abs_velocity_max.y = 4;

	// set timing parameters
	//settings.game_play_time = 20000;			// duration for game play (in milliseconds)
	settings.game_play_update_period = 10;		// duration between game play updates (in milliseconds)

	// missed shot penalty
	settings.missed_shot_penalty = 50;

}

void OBJ_init_life_points(void)
	    {
		 for (int i = 0; i < MAX_LIFE_POINTS; i++)
		    {
		        life_points[i].image.image_array = (uint16_t*) life_points_img;
		        life_points[i].image.size_x = 20;
		        life_points[i].image.size_y = 20;
		        life_points[i].image.size = life_points[i].image.size_x * life_points[i].image.size_y;

		        GFX_init_location_restrictions(&life_points[i], &canvas.whole_area);
		        GFX_init_gfx_object_location(&life_points[i], 10 + i * 30, 10);
		        GFX_set_gfx_object_velocity(&life_points[i], 0, 0);
		    }
		}


void OBJ_draw_life_points(void)
{
    for (int i = 0; i < MAX_LIFE_POINTS; i++)
    {
    	 if (i < game_status.life_points)
    	        {
    	            GFX_draw_one_gfx_object_on_background(&life_points[i], &background);
    	        }
    	        else
    	        {
    	            GFX_clear_gfx_object_on_background(&life_points[i], &background);
    	        }
    	    }
    	}

void OBJ_init_game_status(void)
{
	game_status.score = 0;
	game_status.life_points = MAX_LIFE_POINTS;
}






// object "constructor"
void OBJ_init_canvas(void)
{
	// whole area
		canvas.whole_area.x_min = 0;
		canvas.whole_area.y_min = 0;

		canvas.whole_area.x_max = DISPLAY_SIZE_X - 1;
		canvas.whole_area.y_max = DISPLAY_SIZE_Y - 1;

		canvas.whole_area.x_center = DISPLAY_CENTER_X;
		canvas.whole_area.y_center = DISPLAY_CENTER_Y;



	// above ground area
		canvas.above_ground.x_min = 0;
		canvas.above_ground.y_min = 0;

		canvas.above_ground.x_max = canvas.whole_area.x_max;
		canvas.above_ground.y_max = 230;

		canvas.above_ground.x_center = DISPLAY_CENTER_X;
		canvas.above_ground.y_center = canvas.above_ground.y_max/2;


	// below ground area
		canvas.below_ground.x_min = 0;
		canvas.below_ground.y_min = canvas.above_ground.y_max + 1;

		canvas.below_ground.x_max = canvas.whole_area.x_max;
		canvas.below_ground.y_max = canvas.whole_area.y_max;

		canvas.below_ground.x_center = DISPLAY_CENTER_X;
		canvas.below_ground.y_center = (canvas.below_ground.y_max - canvas.below_ground.y_min) / 2;
}









// ------------------ FULL SCREEN IMAGES ----------------------


// object "constructor"
void OBJ_init_splash_screen(void)
{
	// init graphic object

		// init image
		splash_screen.image.image_array = (uint16_t*) splash_screen_img;

		splash_screen.image.size_x = 320;
		splash_screen.image.size_y = 240;
		splash_screen.image.size = splash_screen.image.size_x * splash_screen.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&splash_screen, &canvas.whole_area );
		GFX_init_gfx_object_location( &splash_screen, 0, 0);


		// set the initial object velocity
		GFX_set_gfx_object_velocity(&splash_screen, 0, 0);

		// set edge parameters
		splash_screen.edge_behavior = EDGE_IGNORE;



}



// object "constructor"
void OBJ_init_background(void)
{
	// init graphic object

		// init image
		background.image.image_array = (uint16_t*) background_img;

		background.image.size_x = 320;
		background.image.size_y = 240;
		background.image.size = background.image.size_x * background.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&background, &canvas.whole_area );
		GFX_init_gfx_object_location( &background, 0, 0);


		// set the initial object velocity
		GFX_set_gfx_object_velocity(&background, 0, 0);

		// set edge parameters
		background.edge_behavior = EDGE_IGNORE;


}









// ------------------ SPRITES ----------------------




// In computer graphics, a sprite is a two-dimensional bitmap that is
// integrated into a larger scene, most often in a 2D video game.





// object "constructor"
void OBJ_init_game_over_sprite(void)
{
	// init graphic object

		// init image
		game_over_sprite.image.image_array = (uint16_t*) game_over_img;

		game_over_sprite.image.size_x = 242;
		game_over_sprite.image.size_y = 81;
		game_over_sprite.image.size = game_over_sprite.image.size_x * game_over_sprite.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions ( &game_over_sprite, &canvas.whole_area );
		GFX_init_gfx_object_location( &game_over_sprite, 39, 53 );



		// set the initial object velocity
		GFX_set_gfx_object_velocity(&game_over_sprite, 0, 0);


		// set edge parameters
		game_over_sprite.edge_behavior = EDGE_IGNORE;
}




// object "constructor"
void OBJ_init_press_any_key_sprite(void)
{
	// init graphic object

		// init image
		press_any_key_sprite.image.image_array = (uint16_t*) press_any_key_img;

		press_any_key_sprite.image.size_x = 260;
		press_any_key_sprite.image.size_y = 23;
		press_any_key_sprite.image.size = press_any_key_sprite.image.size_x * press_any_key_sprite.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&press_any_key_sprite, &canvas.whole_area );
		GFX_init_gfx_object_location( &press_any_key_sprite, 30, 150);


		// set the initial object velocity
		GFX_set_gfx_object_velocity(&press_any_key_sprite, 0, 0);


		// set edge parameters
		press_any_key_sprite.edge_behavior = EDGE_IGNORE;

}




// object "constructor"
void OBJ_init_countdown_digit_sprites(void)
{
	// init graphic object

		// init image
		countdown_digit_1_sprite.image.image_array = (uint16_t*) digit_1_img;

		countdown_digit_1_sprite.image.size_x = 33;
		countdown_digit_1_sprite.image.size_y = 70;
		countdown_digit_1_sprite.image.size = countdown_digit_1_sprite.image.size_x * countdown_digit_1_sprite.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&countdown_digit_1_sprite, &canvas.whole_area );
		GFX_init_gfx_object_center_location( &countdown_digit_1_sprite, canvas.above_ground.x_center, canvas.above_ground.y_center);


		// set the initial object velocity
		GFX_set_gfx_object_velocity(&countdown_digit_1_sprite, 0, 0);


		// set edge parameters
		countdown_digit_1_sprite.edge_behavior = EDGE_IGNORE;





	// init graphic object

		// init image
		countdown_digit_2_sprite.image.image_array = (uint16_t*) digit_2_img;

		countdown_digit_2_sprite.image.size_x = 70;
		countdown_digit_2_sprite.image.size_y = 70;
		countdown_digit_2_sprite.image.size = countdown_digit_2_sprite.image.size_x * countdown_digit_2_sprite.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&countdown_digit_2_sprite, &canvas.whole_area );
		GFX_init_gfx_object_center_location( &countdown_digit_2_sprite, canvas.above_ground.x_center, canvas.above_ground.y_center);


		// set the initial object velocity
		GFX_set_gfx_object_velocity(&countdown_digit_2_sprite, 0, 0);


		// set edge parameters
		countdown_digit_2_sprite.edge_behavior = EDGE_IGNORE;





	// init graphic object

		// init image
		countdown_digit_3_sprite.image.image_array = (uint16_t*) digit_3_img;

		countdown_digit_3_sprite.image.size_x = 69;
		countdown_digit_3_sprite.image.size_y = 70;
		countdown_digit_3_sprite.image.size = countdown_digit_3_sprite.image.size_x * countdown_digit_3_sprite.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&countdown_digit_3_sprite, &canvas.whole_area );
		GFX_init_gfx_object_center_location( &countdown_digit_3_sprite, canvas.above_ground.x_center, canvas.above_ground.y_center);


		// set the initial object velocity
		GFX_set_gfx_object_velocity(&countdown_digit_3_sprite, 0, 0);


		// set edge parameters
		countdown_digit_3_sprite.edge_behavior = EDGE_IGNORE;



}









void OBJ_init_arduinos(void)
{
	// --- arduino UNO ---

		// init graphic object

			// init image
			arduino[UNO].gfx_object.image.image_array = (uint16_t*) arduino_uno_img;

			arduino[UNO].gfx_object.image.size_x = 40;
			arduino[UNO].gfx_object.image.size_y = 45;
			arduino[UNO].gfx_object.image.size = arduino[UNO].gfx_object.image.size_x * arduino[UNO].gfx_object.image.size_y;

			// init the graphic object location
			GFX_init_location_restrictions(&arduino[UNO].gfx_object, &canvas.above_ground );
			GFX_init_gfx_object_center_location(&arduino[UNO].gfx_object, canvas.above_ground.x_center, canvas.above_ground.y_center);

			// set the initial object velocity
			GFX_set_gfx_object_velocity(&arduino[UNO].gfx_object, 2, 0);


			// set edge parameters
			arduino[UNO].gfx_object.edge_behavior = EDGE_BOUNCE;



		// init arduino specific parameters
			arduino[UNO].points = 100;

		// initial movement direction
			arduino[UNO].direction = MOVE_RIGHT;







	// --- arduino MEGA ---

		// init graphic object

			// init image
			arduino[MEGA].gfx_object.image.image_array = (uint16_t*) arduino_mega_img;

			arduino[MEGA].gfx_object.image.size_x = 45;
			arduino[MEGA].gfx_object.image.size_y = 45;
			arduino[MEGA].gfx_object.image.size =  arduino[MEGA].gfx_object.image.size_x * arduino[MEGA].gfx_object.image.size_y;

			// init the graphic object location
			GFX_init_location_restrictions(&arduino[MEGA].gfx_object, &canvas.above_ground );
			GFX_init_gfx_object_center_location(&arduino[MEGA].gfx_object, canvas.above_ground.x_center, canvas.above_ground.y_center);



			// set the initial object velocity
			GFX_set_gfx_object_velocity(&arduino[MEGA].gfx_object, 2, 0);


			// set edge parameters
			arduino[MEGA].gfx_object.edge_behavior = EDGE_BOUNCE;


		// init arduino specific parameters
			arduino[MEGA].points = 100;

		// initial movement direction
			arduino[MEGA].direction = MOVE_RIGHT;




	// --- arduino NANO ---

		// init graphic object

			// init image
			arduino[NANO].gfx_object.image.image_array = (uint16_t*) arduino_nano_img;

			arduino[NANO].gfx_object.image.size_x = 45;
			arduino[NANO].gfx_object.image.size_y = 45;
			arduino[NANO].gfx_object.image.size =  arduino[NANO].gfx_object.image.size_x * arduino[NANO].gfx_object.image.size_y;

			// init the graphic object location
			GFX_init_location_restrictions (&arduino[NANO].gfx_object, &canvas.above_ground );
			GFX_init_gfx_object_center_location(&arduino[NANO].gfx_object, canvas.above_ground.x_center, canvas.above_ground.y_center);



			// set the initial object velocity
			GFX_set_gfx_object_velocity(&arduino[NANO].gfx_object, 2, 0);


			// set edge parameters
			arduino[NANO].gfx_object.edge_behavior = EDGE_BOUNCE;



		// init arduino specific parameters
			arduino[NANO].points = 100;


			// init image
						arduino[INVADER].gfx_object.image.image_array = (uint16_t*) arduino_invader_img;

						arduino[INVADER].gfx_object.image.size_x = 40;
						arduino[INVADER].gfx_object.image.size_y = 35;
						arduino[INVADER].gfx_object.image.size = arduino[INVADER].gfx_object.image.size_x * arduino[INVADER].gfx_object.image.size_y;

						// init the graphic object location
						GFX_init_location_restrictions(&arduino[INVADER].gfx_object, &canvas.above_ground );
						GFX_init_gfx_object_center_location(&arduino[INVADER].gfx_object, canvas.above_ground.x_center, canvas.above_ground.y_center);

						// set the initial object velocity
						GFX_set_gfx_object_velocity(&arduino[INVADER].gfx_object, 2, 0);


						// set edge parameters
						arduino[INVADER].gfx_object.edge_behavior = EDGE_BOUNCE;



					// init arduino specific parameters
						arduino[INVADER].points = 100;

					// initial movement direction
						arduino[INVADER].direction = MOVE_RIGHT;


}

void OBJ_init_bullet(void){

	    bullet.gfx_object.image.image_array = (uint16_t*) bullet_img;
	    bullet.gfx_object.image.size_x = 10;
	    bullet.gfx_object.image.size_y = 18;
	    bullet.gfx_object.image.size = bullet.gfx_object.image.size_x * bullet.gfx_object.image.size_y;

	    GFX_init_location_restrictions(&bullet.gfx_object, &canvas.whole_area);


	    GFX_init_gfx_object_location(&bullet.gfx_object, -20, -20);

	    bullet.active = 0;
}

void OBJ_update_bullet(void)
{
	  if (bullet.active) {

		  	GFX_clear_gfx_object_on_background(&bullet.gfx_object, &background);

	        GFX_save_gfx_object_location(&bullet.gfx_object);

	        GFX_move_location(&bullet.gfx_object.location, &bullet.gfx_object.location_new, bullet.gfx_object.velocity.x, bullet.gfx_object.velocity.y);
	        uint8_t placement_successful = GFX_set_gfx_object_location(&bullet.gfx_object, bullet.gfx_object.location_new.x_min, bullet.gfx_object.location_new.y_min);

	               if (!placement_successful || bullet.gfx_object.location.x_min > DISPLAY_SIZE_X || bullet.gfx_object.location.x_max < 0 || bullet.gfx_object.location.y_min > DISPLAY_SIZE_Y
	            		   || bullet.gfx_object.location.y_max < 0) {
	                   bullet.active = 0;

	                   GFX_clear_gfx_object_on_background(&bullet.gfx_object, &background);
	               } else {

	                    GFX_draw_one_gfx_object_on_background(&bullet.gfx_object, &background);
	                }
	            }
}

void OBJ_shoot_bullet(void){
	 if (!bullet.active)
	    {
	        GFX_set_gfx_object_center_location(&bullet.gfx_object, crosshair.gfx_object.location.x_center, crosshair.gfx_object.location.y_center);

	        GFX_set_gfx_object_velocity(&bullet.gfx_object, 0, -7);

	        bullet.active = 1;
	    }
}
// --------- CROSSHAIR -----------


// object "constructor"
void OBJ_init_crosshair(void)
{
	// init graphic object

		// init image
		crosshair.gfx_object.image.image_array = (uint16_t*) crosshair_img;

		crosshair.gfx_object.image.size_x = 45;
		crosshair.gfx_object.image.size_y = 45;
		crosshair.gfx_object.image.size = crosshair.gfx_object.image.size_x * crosshair.gfx_object.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&crosshair.gfx_object, &canvas.whole_area );
		GFX_init_gfx_object_center_location(&crosshair.gfx_object, canvas.below_ground.x_center, canvas.below_ground.y_max- crosshair.gfx_object.image.size_y / 2);

		// set the initial object velocity
		GFX_set_gfx_object_velocity(&crosshair.gfx_object, 0, 0);


		// set edge parameters
		crosshair.gfx_object.edge_behavior = EDGE_IGNORE;


	// init crosshair specific parameters
		crosshair.distance_to_target_center = 0;

}




void OBJ_crosshair_set_center_location_with_joystick(void)
{
	   int16_t x, y;
	   int16_t last_x = -1;

	    uint8_t joystick_x = JOY_get_axis_position(X);

	    if (joystick_x < 45) // levo
	    {

	        x = crosshair.gfx_object.location.x_center - 4;
	    }
	    else if (joystick_x > 55) // desno
	    {
	        x = crosshair.gfx_object.location.x_center + 4;
	    }
	    else
	    {
	        x = crosshair.gfx_object.location.x_center;
	    }

	    y = crosshair.gfx_object.location.y_center;

	    if (x < crosshair.gfx_object.center_limits.X_MIN)
	        x = crosshair.gfx_object.center_limits.X_MIN;
	    else if (x > crosshair.gfx_object.center_limits.X_MAX)
	        x = crosshair.gfx_object.center_limits.X_MAX;


	    if (last_x != x) {
	        GFX_move_gfx_object_center(&crosshair.gfx_object, x, y);
	        last_x = x;
	    }
	}





void OBJ_crosshair_update_distance_to_target(void)
{
	crosshair.distance_to_target_center =
			MATH_calculate_distance_between_points(	crosshair.gfx_object.location.x_center,
													crosshair.gfx_object.location.y_center,
													target.gfx_object.location.x_center,
													target.gfx_object.location.y_center);

}



uint8_t OBJ_is_crosshair_on_target(void)
{
	return GFX_is_point_inside_location( &target.gfx_object.location, crosshair.gfx_object.location.x_center, crosshair.gfx_object.location.y_center);
}












// object "constructor"
void OBJ_init_target(arduino_types_enum_t type)
{
	// copy the arduino object into the target object
	memcpy( &target, &arduino[type], sizeof(arduino_object_t) );
}




void OBJ_spawn_target(void)
{
	arduino_types_enum_t arduino_type;
	int16_t x, y;

	// generate a target from a random arduino
	arduino_type = MATH_random_integer_number_from_interval(0, NUM_OF_ARDUINO_TYPES - 1);

	OBJ_init_target(arduino_type);


	// randomize the target position
	x = MATH_random_integer_number_from_interval(target.gfx_object.center_limits.X_MIN, target.gfx_object.center_limits.X_MAX);
	y = MATH_random_integer_number_from_interval(30, 150);
	//GFX_init_location_restrictions(&target.gfx_object, (&canvas.above_ground+MIN_Y_OFFSET));

	GFX_init_gfx_object_center_location( &target.gfx_object, x, y );




	// randomize the target velocity
	x = MATH_random_sign() * MATH_random_integer_number_from_interval(settings.abs_velocity_min.x, settings.abs_velocity_max.x);
	y = MATH_random_sign() * MATH_random_integer_number_from_interval(settings.abs_velocity_min.y, settings.abs_velocity_max.y);

	GFX_set_gfx_object_velocity( &target.gfx_object, x, y );

}










// ------------ TEXT OBJECTS -------------


// ------- Score text --------

void OBJ_init_score_box_title(void)
{
	static char str[]= "SCORE";

	score_box_title.text = str;

	score_box_title.x_min = 243;
	score_box_title.y_min = 10;

	score_box_title.fore_color = C_WHITE;
	score_box_title.back_color = C_BLACK;

	score_box_title.font = (UG_FONT*) &FONT_7X12;
}



void OBJ_set_score_text_value(int16_t score)
{
	static char str[6];

	sprintf(str, "%5i", score);
	score_text.text = str;

}


void OBJ_init_score_text(void)
{
	OBJ_set_score_text_value( game_status.score );

	score_text.x_min = 242;
	score_text.y_min = 30;

	score_text.fore_color = C_WHITE;
	score_text.back_color = C_BLACK;

	score_text.font = (UG_FONT*) &FONT_8X8;
}



// ---- Time box text ------


void OBJ_init_time_box_title(void)
{
	static char str[]= "TIME";

	time_box_title.text = str;

	time_box_title.x_min = 57;
	time_box_title.y_min = score_box_title.y_min;

	time_box_title.fore_color = C_WHITE;
	time_box_title.back_color = C_BLACK;

	time_box_title.font = (UG_FONT*) &FONT_7X12;
}





// ---- Timeout progress bar ------


void OBJ_init_timeout_bar(void)
{
	timeout_bar.value_percent = 100;
	timeout_bar.full_length_px = 137;


	timeout_bar.bar.x_min = 54;
	timeout_bar.bar.y_min = 217;

	timeout_bar.bar.length = timeout_bar.full_length_px;
	timeout_bar.bar.height = 10;

	timeout_bar.bar.x_max = timeout_bar.bar.x_min + timeout_bar.bar.length;
	timeout_bar.bar.y_max = timeout_bar.bar.y_min + timeout_bar.bar.height;

	timeout_bar.bar.color = C_RED;

	timeout_bar.background_color = C_BLACK;

}




void OBJ_set_timeout_bar_value(uint8_t value)
{
	if (value <= 100 )
	{
		timeout_bar.value_percent = value;

		timeout_bar.bar.length = (timeout_bar.full_length_px * timeout_bar.value_percent) / 100;

		timeout_bar.bar.x_max = timeout_bar.bar.x_min + timeout_bar.bar.length;

	}
}

















// ------- Test functions ---------




void OBJ_demo_spawn_target(void)
{
	OBJ_init();	// init all the objects

	// display background
	GFX_draw_gfx_object(&background);



	while( KBD_get_pressed_button() == BTN_NONE );	// wait for button pressed

	MATH_init_random_generator();



	OBJ_spawn_target();

	while(1)
	{

		GFX_draw_one_gfx_object_on_background( &target.gfx_object, &background);

		HAL_Delay(20);


		GFX_update_moving_gfx_object_location( &target.gfx_object );
	}

}





// -------------- Private function implementations -------------



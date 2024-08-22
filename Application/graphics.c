/*
 * graphics.c
 *
 *  Created on: 10 May 2022
 *      Author: Gasper
 */





/* **************** MODULE DESCRIPTION *************************

This module implements functions for displaying various objects
via the GUI (i.e. the LCD screen).

// NOTE: all coordinates (x,y) (if not specified otherwise), represent
// the top left corner of objects (i.e. the x_min, y_min).

// We can use different types of coordinate systems:
//
//	1) the absolute coordinate system with the origin at the top left corner of the display ("_abs" suffix )
//
//	2) the image coordinate system with the origin at the top left corner of the image  ("_img" suffix )


************************************************************* */





// ----------- Include other modules (for private) -------------


// ------ Standard C libraries -----

#include <stdlib.h>		// support for dynamic memory allocation
#include <math.h>		// support for math basic functions



// -------- System level modules -------

#include "kbd.h"		// for demo



// ------ System level modules ---------



// ------ Application level modules ---------
#include "graphics.h"
#include "objects.h"
#include "images.h"






// ---------------------- Private definitions ------------------


// an uxiliary graphic object is used as a partial frame buffer
graphic_object_t	partial_frame_buffer;








// ------------- Public function implementations --------------






// ----------- IMAGE RELATED FUNCTIONALITY ------------



void GFX_display_image_array(uint16_t *img_array, int16_t x, int16_t y, uint16_t size_x, uint16_t size_y)
{
	ILI9341_SetDisplayWindow( x, y, size_x, size_y);
	ILI9341_SendData((LCD_IO_Data_t *) img_array, size_x*size_y );
}



void GFX_display_image_object(image_object_t *image, int16_t x, int16_t y)
{
	ILI9341_SetDisplayWindow( x, y, image->size_x, image->size_y);
	ILI9341_SendData((LCD_IO_Data_t *) image->image_array, image->size );
}



// coordinates (x_img,y_img) are given in the image coordinate system
uint16_t GFX_get_image_pixel(image_object_t *img, int16_t x_img, int16_t y_img)
{
	uint32_t i;

	// coordinates (x_img,y_img) are from image coordinate system
	if ( ( (x_img>=0) && (x_img < img->size_x) ) && ( (y_img>=0) && (y_img < img->size_y) ) )
	{

		// calculate the "linear index" of the pixel at (x, y) coordinate
		i = y_img * img->size_x + x_img;

		// return the pixel at that index from the image array
		return img->image_array[i];
	}

	else
	{
		// return transparent pixel if pixel location lies outside the image
		return IMG_TRANSPARENT_COLOR_CODE;
	}

}


//todo: tole spodaj ne bo delovalo dobro za negativne x oz. y
void GFX_get_image_part(image_object_t *img, int16_t x, int16_t y, uint16_t size_x, uint16_t size_y, uint16_t *sub_image_array)
{
	uint32_t i = 0;


	// make sure image part lies inside the image
	if(y + size_y > img->size_y  || x + size_x > img->size_x)
		return;



	// go over all required rows
	for( uint16_t row = y; row < (y + size_y); row++ )
	{
		// for each row, go over all required columns
		for(uint16_t column = x; column < (x + size_x); column++ )
		{

			sub_image_array[i] = GFX_get_image_pixel(img, column, row);
			i++;
		}

	}

}







uint16_t* GFX_allocate_image_buffer(uint32_t size_px)
{
	return ( (uint16_t *) malloc(size_px * sizeof(uint16_t) ) );
}


void GFX_deallocate_image_buffer(uint16_t *ptr)
{
	free(ptr);
}









// ----------- GRAPHICAL OBJECT RELATED FUNCTIONALITY ---------





void GFX_draw_gfx_object(graphic_object_t *gfx_object)
{

	ILI9341_SetDisplayWindow( gfx_object->location.x_min, gfx_object->location.y_min, gfx_object->image.size_x, gfx_object->image.size_y);

	ILI9341_SendData((LCD_IO_Data_t *) gfx_object->image.image_array, gfx_object->image.size );
}





void GFX_save_gfx_object_location(graphic_object_t *gfx_object)
{
	gfx_object->location_old.x_min = gfx_object->location.x_min;
	gfx_object->location_old.y_min = gfx_object->location.y_min;

	gfx_object->location_old.x_max = gfx_object->location.x_max;
	gfx_object->location_old.y_max = gfx_object->location.y_max;

	gfx_object->location_old.x_center = gfx_object->location.x_center;
	gfx_object->location_old.y_center = gfx_object->location.y_center;

}



void GFX_restore_gfx_object_old_location(graphic_object_t *gfx_object)
{
	gfx_object->location.x_min = gfx_object->location_old.x_min;
	gfx_object->location.y_min = gfx_object->location_old.y_min;

	gfx_object->location.x_max = gfx_object->location_old.x_max;
	gfx_object->location.y_max = gfx_object->location_old.y_max;

	gfx_object->location.x_center = gfx_object->location_old.x_center;
	gfx_object->location.y_center = gfx_object->location_old.y_center;


}



// the limiting values are still allowed
void GFX_init_location_restrictions(graphic_object_t *gfx_object, location_t *canvas_location)
{
	// top left corner restrictions
	gfx_object->top_left_limits.X_MIN = canvas_location->x_min;
	gfx_object->top_left_limits.Y_MIN = canvas_location->y_min;

	gfx_object->top_left_limits.X_MAX = canvas_location->x_max - gfx_object->image.size_x;
	gfx_object->top_left_limits.Y_MAX = canvas_location->y_max - gfx_object->image.size_y;

	gfx_object->top_left_limits.X_RANGE = gfx_object->top_left_limits.X_MAX - gfx_object->top_left_limits.X_MIN;
	gfx_object->top_left_limits.Y_RANGE = gfx_object->top_left_limits.Y_MAX - gfx_object->top_left_limits.Y_MIN;


	// center point restrictions
	gfx_object->center_limits.X_MIN = canvas_location->x_min + ceil( gfx_object->image.size_x / 2 );
	gfx_object->center_limits.Y_MIN = canvas_location->y_min + ceil( gfx_object->image.size_y / 2 );

	gfx_object->center_limits.X_MAX = canvas_location->x_max - ceil( gfx_object->image.size_x / 2 );
	gfx_object->center_limits.Y_MAX = canvas_location->y_max - ceil( gfx_object->image.size_y / 2 );

	gfx_object->center_limits.X_RANGE = gfx_object->center_limits.X_MAX - gfx_object->center_limits.X_MIN;
	gfx_object->center_limits.Y_RANGE = gfx_object->center_limits.Y_MAX - gfx_object->center_limits.Y_MIN;

}







uint8_t GFX_is_point_inside_location(location_t *location, int16_t x, int16_t y)
{
	if( ( x >= location->x_min ) &&
		( x <= location->x_max ) &&
		( y >= location->y_min ) &&
		( y <= location->y_max )    )
	{
		return 1;	// the point (x,y) is inside location
	}
	else
		return 0;	// the point (x,y) is outside location
}




uint8_t GFX_is_location_inside_object_restrictions(location_restriction_t *restrictions, int16_t x, int16_t y)
{
	if( ( x >= restrictions->X_MIN ) &&
		( x <= restrictions->X_MAX ) &&
		( y >= restrictions->Y_MIN ) &&
		( y <= restrictions->Y_MAX )    )
	{
		return 1;	// the location is inside restrictions
	}
	else
		return 0;	// the location is not inside restrictions
}









uint8_t GFX_set_gfx_object_location(graphic_object_t *gfx_object, int16_t x, int16_t y)
{

	// check if the new location is within restrictions
	if( GFX_is_location_inside_object_restrictions( &gfx_object->top_left_limits, x, y ) )
	{
		// within restrictions -> object can be placed

		// set the new object location
		gfx_object->location.x_min = x;
		gfx_object->location.y_min = y;


		gfx_object->location.x_max = gfx_object->location.x_min + gfx_object->image.size_x;
		gfx_object->location.y_max = gfx_object->location.y_min + gfx_object->image.size_y;


		gfx_object->location.x_center = gfx_object->location.x_min + gfx_object->image.size_x / 2;
		gfx_object->location.y_center = gfx_object->location.y_min + gfx_object->image.size_y / 2;


		return 1;	// placement successful
	}
	else
	{
		// new location is outside the restrictions -> object will not be placed

		return 0;	// placement not successful
	}


}




uint8_t GFX_init_gfx_object_location(graphic_object_t *gfx_object, int16_t x, int16_t y)
{
	uint8_t return_value;

	return_value = GFX_set_gfx_object_location(gfx_object, x, y);

	if( return_value  )
	{
		// remember the initial object location
		GFX_save_gfx_object_location(gfx_object);
	}

	return return_value;

}




uint8_t GFX_set_gfx_object_center_location(graphic_object_t *gfx_object, int16_t x, int16_t y)
{
	// check if the new location is within restrictions
	if( GFX_is_location_inside_object_restrictions( &gfx_object->center_limits, x, y ) )
	{
		// within restrictions -> object can be placed

		// set the new object location
		gfx_object->location.x_center = x;
		gfx_object->location.y_center = y;


		gfx_object->location.x_min = gfx_object->location.x_center - gfx_object->image.size_x/2;
		gfx_object->location.y_min = gfx_object->location.y_center - gfx_object->image.size_y/2;

		gfx_object->location.x_max = gfx_object->location.x_min +  gfx_object->image.size_x;
		gfx_object->location.y_max = gfx_object->location.y_min +  gfx_object->image.size_y;

		return 1;	// placement successful

	}
	else
	{
		// new location is outside the restrictions -> object will not be placed

		return 0;	// placement not successful
	}
}



uint8_t GFX_init_gfx_object_center_location(graphic_object_t *gfx_object, int16_t x, int16_t y)
{
	uint8_t return_value;

	return_value = GFX_set_gfx_object_center_location(gfx_object, x, y);

	if ( return_value )
	{
		// remember the initial object location
		GFX_save_gfx_object_location(gfx_object);
	}

	return return_value;
}





uint8_t GFX_move_gfx_object(graphic_object_t *gfx_object, int16_t x, int16_t y)
{

	// remember the current object location
	GFX_save_gfx_object_location(gfx_object);


	// set the new object location
   return GFX_set_gfx_object_location(gfx_object, x, y);

}



uint8_t GFX_move_gfx_object_center(graphic_object_t *gfx_object, int16_t x, int16_t y)
{

	// remember the current object location
	GFX_save_gfx_object_location(gfx_object);


	// set the new object center location
  return GFX_set_gfx_object_center_location(gfx_object, x, y);

}










void GFX_set_gfx_object_velocity(graphic_object_t *gfx_object, int8_t velocity_x, int8_t velocity_y)
{
	gfx_object->velocity.x = velocity_x;
	gfx_object->velocity.y = velocity_y;
}


void GFX_move_location(location_t *location, location_t *location_new, int16_t dx, int16_t dy)
{
	location_new->x_min 	= location->x_min + dx;
	location_new->x_max 	= location->x_max + dx;
	location_new->x_center 	= location->x_center + dx;

	location_new->y_min 	= location->y_min + dy;
	location_new->y_max 	= location->y_max + dy;
	location_new->y_center 	= location->y_center + dy;
}




uint8_t GFX_update_moving_gfx_object_location(graphic_object_t *gfx_object)
{

	uint8_t object_placement_successful;

	// remember the current object location
	GFX_save_gfx_object_location(gfx_object);

	   // calculate the new object location according to the object velocity
	    // simplified for now
	    // mind the inversion of the y-axis!
	    GFX_move_location(&gfx_object->location, &gfx_object->location_new, gfx_object->velocity.x, -gfx_object->velocity.y);

	    // set the new object location
	    object_placement_successful = GFX_set_gfx_object_location(gfx_object, gfx_object->location_new.x_min, gfx_object->location_new.y_min);

	    // if object placement not successful (i.e. new location is not within object location restrictions)
	    if (!object_placement_successful) {
	        switch (gfx_object->edge_behavior) {
	            case EDGE_BOUNCE:
	                object_placement_successful = GFX_bounce_moving_object_from_edge(gfx_object);
	                return object_placement_successful;
	                break;
	            case EDGE_CROSS:
	                return object_placement_successful;
	                break;
	            case EDGE_IGNORE:
	                return object_placement_successful;
	                break;
	            default:
	                return object_placement_successful;
	                break;
	        }
	    } else {
	        return object_placement_successful;
	    }

}


uint8_t GFX_update_moving_arduino_object_location(arduino_object_t *arduino_object) {
    uint8_t object_placement_successful;

    // save current location
    GFX_save_gfx_object_location(&arduino_object->gfx_object);


    // calculate new location based on direction
     switch (arduino_object->direction) {
         case MOVE_RIGHT:
             GFX_move_location(&arduino_object->gfx_object.location, &arduino_object->gfx_object.location_new, arduino_object->gfx_object.velocity.x, 0);
             break;
         case MOVE_LEFT:
             GFX_move_location(&arduino_object->gfx_object.location, &arduino_object->gfx_object.location_new, -arduino_object->gfx_object.velocity.x, 0);
             break;
     }

     // new object location
     object_placement_successful = GFX_set_gfx_object_location(&arduino_object->gfx_object, arduino_object->gfx_object.location_new.x_min, arduino_object->gfx_object.location_new.y_min);

     if (!object_placement_successful) {
         switch (arduino_object->gfx_object.edge_behavior) {
             case EDGE_BOUNCE:

                 object_placement_successful = GFX_bounce_moving_object_from_edge(&arduino_object->gfx_object);

                 // change direction
                 if (arduino_object->gfx_object.location.x_min <= arduino_object->gfx_object.top_left_limits.X_MIN) {
                     arduino_object->direction = MOVE_RIGHT;
                     arduino_object->gfx_object.velocity.x += 1;

                 } else if (arduino_object->gfx_object.location.x_max >= arduino_object->gfx_object.top_left_limits.X_MAX) {
                     arduino_object->direction = MOVE_LEFT;
                     arduino_object->gfx_object.velocity.x += 1;


                 }
                 GFX_move_location(&arduino_object->gfx_object.location, &arduino_object->gfx_object.location_new, 0, 20); // Move down
                 GFX_set_gfx_object_location(&arduino_object->gfx_object, arduino_object->gfx_object.location_new.x_min, arduino_object->gfx_object.location_new.y_min);

                 return object_placement_successful;
             case EDGE_CROSS:
             case EDGE_IGNORE:
             default:
                 return object_placement_successful;
         }
     } else {
         return object_placement_successful;
     }
 }



uint8_t GFX_bounce_moving_object_from_edge(graphic_object_t *gfx_object)
{
    int16_t x, y;
    int16_t dx, dy;

    // determine the location of the bounced object

    // considering max X restrictions
    dx = gfx_object->location_new.x_min - gfx_object->top_left_limits.X_MAX;
    if (dx >= 0)
    {
        // calculate the coordinate of the bounced object
        x = gfx_object->location_new.x_min - 2 * dx;
        y = gfx_object->location_new.y_min;

        // change the direction of the velocity component
        GFX_set_gfx_object_velocity(gfx_object, -gfx_object->velocity.x, gfx_object->velocity.y);
    }
    else
    {
        // considering min X restrictions
        dx = gfx_object->location_new.x_min - gfx_object->top_left_limits.X_MIN;
        if (dx <= 0)
        {
            // calculate the coordinate of the bounced object
            x = gfx_object->location_new.x_min - 2 * dx;
            y = gfx_object->location_new.y_min;

            // change the direction of the velocity component
            GFX_set_gfx_object_velocity(gfx_object, -gfx_object->velocity.x, gfx_object->velocity.y);
        }
        else
        {
            // if not hitting the edge, maintain current x position
            x = gfx_object->location_new.x_min;
        }
    }

    // considering max Y restrictions
    dy = gfx_object->location_new.y_min - gfx_object->top_left_limits.Y_MAX;
    if (dy >= 0)
    {
        // calculate the coordinate of the bounced object
        x = gfx_object->location_new.x_min;
        y = gfx_object->location_new.y_min - 2 * dy;

        // change the direction of the velocity component
        GFX_set_gfx_object_velocity(gfx_object, gfx_object->velocity.x, -gfx_object->velocity.y);
    }
    else
    {
        // considering min Y restrictions
        dy = gfx_object->location_new.y_min - gfx_object->top_left_limits.Y_MIN;
        if (dy <= 0)
        {
            // calculate the coordinate of the bounced object
            x = gfx_object->location_new.x_min;
            y = gfx_object->location_new.y_min - 2 * dy;

            // change the direction of the velocity component
            GFX_set_gfx_object_velocity(gfx_object, gfx_object->velocity.x, -gfx_object->velocity.y);
        }
        else
        {
            // if not hitting the edge, maintain current y position
            y = gfx_object->location_new.y_min;
        }
    }

    // place the bounced object in this new location
    return GFX_set_gfx_object_location(gfx_object, x, y);
}






void GFX_get_object_movement_area(graphic_object_t *gfx_object, location_t *object_movement_area)
{
	object_movement_area->x_min = (gfx_object->location.x_min < gfx_object->location_old.x_min) ? gfx_object->location.x_min : gfx_object->location_old.x_min;
	object_movement_area->y_min = (gfx_object->location.y_min < gfx_object->location_old.y_min) ? gfx_object->location.y_min : gfx_object->location_old.y_min;

	object_movement_area->x_max = (gfx_object->location.x_max > gfx_object->location_old.x_max) ? gfx_object->location.x_max : gfx_object->location_old.x_max;
	object_movement_area->y_max = (gfx_object->location.y_max > gfx_object->location_old.y_max) ? gfx_object->location.y_max : gfx_object->location_old.y_max;


	object_movement_area->x_center = (object_movement_area->x_max - object_movement_area->x_min) / 2;
	object_movement_area->y_center = (object_movement_area->y_max - object_movement_area->y_min) / 2;
}








void GFX_absolute_coordinates_to_object_coordinates(graphic_object_t *object, int16_t x_abs, int16_t y_abs, int16_t *x_obj, int16_t *y_obj)
{

	*x_obj = x_abs - object->location.x_min;
	*y_obj = y_abs - object->location.y_min;
}






uint8_t GFX_are_locations_overlapping(location_t *location_A, location_t *location_B)
{

	if ( 	(location_A->x_min > location_B->x_max) ||	// Cond1. If A's left edge is to the right of the B's right edge, - then A is Totally to right Of B
			(location_A->x_max < location_B->x_min) ||	// Cond2. If A's right edge is to the left of the B's left edge, - then A is Totally to left Of B
			(location_A->y_max < location_B->y_min) ||	// Cond3. If A's top edge is below B's bottom edge, - then A is Totally below B
			(location_A->y_min > location_B->y_max) )		// Cond4. If A's bottom edge is above B's top edge, - then A is Totally above B
	{
		return 0;	// not overlapping
	}
	else
		return 1;	// overlapping


}




uint8_t GFX_are_gfx_objects_overlapping(graphic_object_t *object_A, graphic_object_t *object_B)
{

	return GFX_are_locations_overlapping( &object_A->location, &object_B->location );

}




void GFX_allocate_partial_frame_buffer_for_display_area(location_t	*area_location )
{

	// set the location of the partial frame buffer to coincide with the area location
	partial_frame_buffer.location.x_min = area_location->x_min;
	partial_frame_buffer.location.x_max = area_location->x_max;

	partial_frame_buffer.location.y_min = area_location->y_min;
	partial_frame_buffer.location.y_max = area_location->y_max;

	partial_frame_buffer.location.x_center = area_location->x_center;
	partial_frame_buffer.location.y_center = area_location->y_center;



	// set the partial frame image buffer size
	partial_frame_buffer.image.size_x = partial_frame_buffer.location.x_max - partial_frame_buffer.location.x_min;
	partial_frame_buffer.image.size_y = partial_frame_buffer.location.y_max - partial_frame_buffer.location.y_min;
	partial_frame_buffer.image.size = partial_frame_buffer.image.size_x * partial_frame_buffer.image.size_y;

	// allocate memory for the part of the frame, i.e. prepare the image buffer
	partial_frame_buffer.image.image_array = GFX_allocate_image_buffer(partial_frame_buffer.image.size);

}


void GFX_copy_image_part_to_partial_frame_buffer( image_object_t *image )
{
	GFX_get_image_part( image, partial_frame_buffer.location.x_min, partial_frame_buffer.location.y_min, partial_frame_buffer.image.size_x, partial_frame_buffer.image.size_y, partial_frame_buffer.image.image_array);
}




void GFX_partial_frame_buffer_overlay_object(graphic_object_t *object)
{

	uint32_t i = 0;

	int16_t x_obj;
	int16_t y_obj;

	uint16_t pixel;



	// overlay the graphical object image over the part of the existing image in the partial frame buffer

	// go over all the required rows of the partial frame buffer image
	for( uint16_t y_abs = partial_frame_buffer.location.y_min; y_abs < partial_frame_buffer.location.y_max; y_abs++ )
	{
		// for each row, go over all required columns
		for( uint16_t x_abs = partial_frame_buffer.location.x_min; x_abs < partial_frame_buffer.location.x_max; x_abs++ )
		{
			// overlay the object pixel

			// get the graphical object image pixel at this current (column, row)
			GFX_absolute_coordinates_to_object_coordinates(object, x_abs, y_abs, &x_obj, &y_obj);
			pixel = GFX_get_image_pixel( &object->image, x_obj, y_obj);

			// overlay the pixel only if it is not transparent
			if ( pixel != IMG_TRANSPARENT_COLOR_CODE)
				partial_frame_buffer.image.image_array[i] = pixel;

			// update the buffer linear index
			i++;
		}
	}

}








void GFX_deallocate_partial_frame_buffer(void)
{
	// de-allocate memory for the partial frame buffer
	GFX_deallocate_image_buffer(partial_frame_buffer.image.image_array);
}









void GFX_draw_one_gfx_object_on_background(graphic_object_t *object, graphic_object_t *bckgnd)
{

	// the information about the display area that will use the partial frame buffer
	location_t	area;





	// If the object did not move much, the new object location might overlap with
	// the old object location. This means that we must redraw the entire movement are
	// and cannot treat the new location separately from the old location.
	if( GFX_are_locations_overlapping( &object->location_old, &object->location ) )
	{

		// specify the frame buffer graphic object parameters
		// consider that the object can be moving! -> the buffer area must
		// contain both old object area and new object area!
		GFX_get_object_movement_area(object, &area );


		// allocate the partial frame buffer for the required area of the display
		GFX_allocate_partial_frame_buffer_for_display_area(&area);


			// copy the appropriate part of the background image to the buffer
			GFX_copy_image_part_to_partial_frame_buffer(&bckgnd->image);


			// and now overlay the object image over the partial frame buffer image
			GFX_partial_frame_buffer_overlay_object(object);



			// display the partial frame buffer object
			GFX_draw_gfx_object(&partial_frame_buffer);


		// de-allocate memory for the partial frame buffer
		GFX_deallocate_partial_frame_buffer();


	}
	else
	{
		// In this case the new object location is so far away that it does not overlap
		// with the old object location. This means that we can treat the old object location
		// and the new object location separately.

		// Clear the old object location.
		GFX_clear_location_on_background( &object->location_old, bckgnd );


		// Draw the object at the new location.

			// allocate the partial frame buffer for the required area of the display
			GFX_allocate_partial_frame_buffer_for_display_area( &object->location );


				// copy the appropriate part of the background image to the buffer
				GFX_copy_image_part_to_partial_frame_buffer(&bckgnd->image);


				// and now overlay the object image over the partial frame buffer image
				GFX_partial_frame_buffer_overlay_object(object);

				// display the partial frame buffer object
				GFX_draw_gfx_object(&partial_frame_buffer);


			// de-allocate memory for the partial frame buffer
			GFX_deallocate_partial_frame_buffer();


	}



}













void GFX_draw_two_gfx_objects_on_background(graphic_object_t *front_object, graphic_object_t *middle_object, graphic_object_t *bckgnd)
{

	// the information about the display area that will use the partial frame buffer
	location_t	area;

	// the information about the movement areas of the two objects
	location_t	front_object_movement_area;
	location_t	middle_object_movement_area;

	// determine movement areas for each object
	GFX_get_object_movement_area( front_object, &front_object_movement_area);
	GFX_get_object_movement_area( middle_object, &middle_object_movement_area);



	// if movement areas of the objects are not overlapping
	if ( ! GFX_are_locations_overlapping( &front_object_movement_area, &middle_object_movement_area) )
	{
		// display these two objects on the background separately
		GFX_draw_one_gfx_object_on_background(front_object, bckgnd);
		GFX_draw_one_gfx_object_on_background(middle_object, bckgnd);

	}
	else
	{
		// redraw the movement area of the each object, considering the second object
		// in this way you minimize the


		// ---- redraw movement area of the middle object first, overlaying the front object -----

			// specify the frame buffer graphic object parameters
			// consider that the object can be moving! -> the buffer area must
			// contain both old object area and new object area!
			GFX_get_object_movement_area(middle_object, &area );


			// allocate the partial frame buffer for the required area of the display
			GFX_allocate_partial_frame_buffer_for_display_area(&area);


				// copy the appropriate part of the background image to the buffer
				GFX_copy_image_part_to_partial_frame_buffer(&bckgnd->image);


				// and now overlay the object image over the partial frame buffer image
				GFX_partial_frame_buffer_overlay_object(middle_object);
				GFX_partial_frame_buffer_overlay_object(front_object);

				// display the partial frame buffer object
				GFX_draw_gfx_object(&partial_frame_buffer);


			// de-allocate memory for the partial frame buffer
			GFX_deallocate_partial_frame_buffer();





		// ---- redraw movement area of the the front object, considering the middle object -----

			// specify the frame buffer graphic object parameters
			// consider that the object can be moving! -> the buffer area must
			// contain both old object area and new object area!
			GFX_get_object_movement_area(front_object, &area );


			// allocate the partial frame buffer for the required area of the display
			GFX_allocate_partial_frame_buffer_for_display_area(&area);


				// copy the appropriate part of the background image to the buffer
				GFX_copy_image_part_to_partial_frame_buffer(&bckgnd->image);


				// and now overlay the object image over the partial frame buffer image
				GFX_partial_frame_buffer_overlay_object(middle_object);
				GFX_partial_frame_buffer_overlay_object(front_object);

				// display the partial frame buffer object
				GFX_draw_gfx_object(&partial_frame_buffer);


			// de-allocate memory for the partial frame buffer
			GFX_deallocate_partial_frame_buffer();

	}

}















void GFX_clear_area_on_background(int16_t x_min, int16_t y_min, int16_t x_max, int16_t y_max, graphic_object_t *bckgnd)
{
	// specify the frame buffer graphic object parameters
	// the buffer image dimensions is the same as current object dimensions
	partial_frame_buffer.location.x_min = x_min;
	partial_frame_buffer.location.y_min = y_min;

	partial_frame_buffer.location.x_max = x_max;
	partial_frame_buffer.location.y_max = y_max;


	// prepare the partial frame image buffer for the area of the current object
	partial_frame_buffer.image.size_x = partial_frame_buffer.location.x_max - partial_frame_buffer.location.x_min;
	partial_frame_buffer.image.size_y = partial_frame_buffer.location.y_max - partial_frame_buffer.location.y_min;
	partial_frame_buffer.image.size = partial_frame_buffer.image.size_x * partial_frame_buffer.image.size_y;



	// allocate memory for the part of the frame, i.e. prepare the image buffer
	partial_frame_buffer.image.image_array = GFX_allocate_image_buffer(partial_frame_buffer.image.size);



	// copy the appropriate part of the background image to the buffer
	GFX_get_image_part( &bckgnd->image, partial_frame_buffer.location.x_min, partial_frame_buffer.location.y_min, partial_frame_buffer.image.size_x, partial_frame_buffer.image.size_y, partial_frame_buffer.image.image_array);

	// display the partial frame buffer
	GFX_draw_gfx_object(&partial_frame_buffer);


	// de-allocate memory for the partial frame buffer
	GFX_deallocate_image_buffer(partial_frame_buffer.image.image_array);

}



void GFX_clear_location_on_background( location_t *location, graphic_object_t *bckgnd )
{
	GFX_clear_area_on_background(location->x_min, location->y_min, location->x_max, location->y_max, bckgnd);
}





void GFX_clear_gfx_object_on_background(graphic_object_t *object, graphic_object_t *bckgnd)
{

	// specify the frame buffer graphic object parameters
	// the buffer image dimensions is the same as current object dimensions
	partial_frame_buffer.location.x_min = object->location.x_min;
	partial_frame_buffer.location.y_min = object->location.y_min;

	partial_frame_buffer.location.x_max = object->location.x_max;
	partial_frame_buffer.location.y_max = object->location.y_max;


	// prepare the partial frame image buffer for the area of the current object
		partial_frame_buffer.image.size_x = partial_frame_buffer.location.x_max - partial_frame_buffer.location.x_min;
		partial_frame_buffer.image.size_y = partial_frame_buffer.location.y_max - partial_frame_buffer.location.y_min;
		partial_frame_buffer.image.size = partial_frame_buffer.image.size_x * partial_frame_buffer.image.size_y;

		// allocate memory for the part of the frame, i.e. prepare the image buffer
		partial_frame_buffer.image.image_array = GFX_allocate_image_buffer(partial_frame_buffer.image.size);



	// copy the appropriate part of the background image to the buffer
	GFX_get_image_part( &bckgnd->image, partial_frame_buffer.location.x_min, partial_frame_buffer.location.y_min, partial_frame_buffer.image.size_x, partial_frame_buffer.image.size_y, partial_frame_buffer.image.image_array);



	// display the partial frame buffer
	GFX_draw_gfx_object(&partial_frame_buffer);


	// de-allocate memory for the partial frame buffer
	GFX_deallocate_image_buffer(partial_frame_buffer.image.image_array);


}







void GFX_update_two_gfx_objects_on_background(graphic_object_t *front_object, graphic_object_t *middle_object, graphic_object_t *bckgnd)
{
	// move objects according to the velocity
	GFX_update_moving_gfx_object_location(front_object);
	GFX_update_moving_gfx_object_location(middle_object);


	// draw objects in their new location
	GFX_draw_two_gfx_objects_on_background(front_object, middle_object, bckgnd);

}






void GFX_display_text_object(text_object_t *txt_object)
{

	UG_FontSelect(txt_object->font);
	UG_SetForecolor(txt_object->fore_color);
	UG_SetBackcolor(txt_object->back_color);

	UG_PutString(txt_object->x_min, txt_object->y_min, txt_object->text );

}



void GFX_display_progress_bar(progress_bar_t *progress_bar)
{

	UG_FillFrame( progress_bar->bar.x_min, progress_bar->bar.y_min, progress_bar->bar.x_max, progress_bar->bar.y_max, progress_bar->bar.color );

	UG_FillFrame( progress_bar->bar.x_max, progress_bar->bar.y_min, progress_bar->bar.x_min + progress_bar->full_length_px, progress_bar->bar.y_max, progress_bar->background_color );
}













// ------- Test functions ---------




void GFX_demo_sub_image(void)
{
	uint16_t sub_x = 40;
	uint16_t sub_y = 40;


	uint16_t sub_image[5000];

	OBJ_init();	// init all the objects

	GFX_get_image_part( &crosshair.gfx_object.image, 0, 0, sub_x, sub_y, sub_image );

	GFX_display_image_array(sub_image, 200, 120, sub_x, sub_y);
}



void GFX_demo_sub_image_with_malloc(void)
{
	uint16_t sub_x = 40;
	uint16_t sub_y = 40;
	uint16_t sub_size = sub_x*sub_y;


	uint16_t *sub_image;


	OBJ_init();	// init all the objects


	sub_image = GFX_allocate_image_buffer(sub_size);

	GFX_get_image_part( &crosshair.gfx_object.image, 0, 0, sub_x, sub_y, sub_image );

	GFX_display_image_array(sub_image, 200, 120, sub_x, sub_y);

	GFX_deallocate_image_buffer(sub_image);;
}


void GFX_demo_draw_one_object_on_backround(graphic_object_t *object)
{
	OBJ_init();	// init all the objects

	GFX_draw_one_gfx_object_on_background( object , &background);

}




void GFX_demo_arduino_objects(void)
{
	OBJ_init();	// init all the objects

	// display background
	GFX_draw_gfx_object(&background);


	// test arduino objects

	GFX_init_gfx_object_center_location( &arduino[MEGA].gfx_object, 50, 100 );
	GFX_draw_one_gfx_object_on_background( &arduino[MEGA].gfx_object, &background);

	GFX_init_gfx_object_center_location( &arduino[UNO].gfx_object, 130, 100 );
	GFX_draw_one_gfx_object_on_background( &arduino[UNO].gfx_object, &background);

	GFX_init_gfx_object_center_location( &arduino[NANO].gfx_object, 200, 100 );
	GFX_draw_one_gfx_object_on_background( &arduino[NANO].gfx_object, &background);


}



void GFX_demo_objects_overlapping_delete_draw(void)
{
	OBJ_init();	// init all the objects

	// display background
	GFX_draw_gfx_object(&background);

	// test objects overlapping

	GFX_init_gfx_object_center_location(&crosshair.gfx_object, 25, 25);
	GFX_draw_two_gfx_objects_on_background( &crosshair.gfx_object, &arduino[MEGA].gfx_object, &background);

	// wait for button pressed
	while( KBD_get_pressed_button() == BTN_NONE );

	GFX_clear_gfx_object_on_background( &crosshair.gfx_object, &background );

	GFX_init_gfx_object_center_location(&crosshair.gfx_object, canvas.above_ground.x_center, canvas.above_ground.y_center);
	GFX_draw_two_gfx_objects_on_background( &crosshair.gfx_object, &arduino[MEGA].gfx_object, &background);

}



void GFX_demo_move_one_object_delete_draw(graphic_object_t *object)
{
	OBJ_init();	// init all the objects

	// display background
	GFX_draw_gfx_object(&background);


	uint8_t x = 30;


	while(1)
	{
	  GFX_init_gfx_object_center_location(object, x, 100);

	  GFX_draw_one_gfx_object_on_background(object, &background);


	  HAL_Delay(100);

	  GFX_clear_gfx_object_on_background(object, &background);

	  x++;

	}
}








void GFX_demo_update_two_gfx_objects_on_background_delete_draw(graphic_object_t *front_object, graphic_object_t *middle_object, graphic_object_t *bckgnd)
{

	// delete objects
	GFX_clear_gfx_object_on_background(front_object, bckgnd);
	GFX_clear_gfx_object_on_background(middle_object, bckgnd);

	// move objects
	GFX_update_moving_gfx_object_location(front_object);	// move according to the velocity
	GFX_save_gfx_object_location(front_object);		// overwrite the old location with the current one

	GFX_update_moving_gfx_object_location(middle_object);	// move according to the velocity
	GFX_save_gfx_object_location(middle_object);	// overwrite the old location with the current one

	// draw objects in their new location
	GFX_draw_two_gfx_objects_on_background(front_object, middle_object, bckgnd);

}




void GFX_demo_two_objects_moving_delete_draw(void)
{
	OBJ_init();	// init all the objects

	// display background
	GFX_draw_gfx_object(&background);


	// demo objects moving

	GFX_init_gfx_object_center_location(&crosshair.gfx_object, 30, 100);
	GFX_set_gfx_object_velocity( &crosshair.gfx_object, 1, 0 );

	GFX_init_gfx_object_center_location(&arduino[UNO].gfx_object, 160, 100);
	GFX_set_gfx_object_velocity( &arduino[UNO].gfx_object, -1, 0 );


	GFX_draw_two_gfx_objects_on_background(&crosshair.gfx_object, &arduino[UNO].gfx_object, &background);


	// wait for button pressed
	while( KBD_get_pressed_button() == BTN_NONE );



	while(1)
	{
		  GFX_demo_update_two_gfx_objects_on_background_delete_draw(&crosshair.gfx_object, &arduino[UNO].gfx_object, &background);

		  HAL_Delay(100);

	}

}



void GFX_demo_move_one_object_using_movement_area(graphic_object_t *object)
{
	OBJ_init();	// init all the objects

	// display background
	GFX_draw_gfx_object(&background);



	GFX_init_gfx_object_center_location(object, 50, 100);
	GFX_set_gfx_object_velocity(object, 1, 0);


	while(1)
	{

		GFX_draw_one_gfx_object_on_background(object, &background);

		HAL_Delay(100);

		GFX_update_moving_gfx_object_location(object);
	}
}




void GFX_demo_two_objects_moving_using_movement_areas(void)
{
	OBJ_init();	// init all the objects

	// display background
	GFX_draw_gfx_object(&background);


	// demo objects moving

	GFX_init_gfx_object_center_location(&crosshair.gfx_object, 50, 100);
	GFX_set_gfx_object_velocity( &crosshair.gfx_object, 2, 0 );

	GFX_init_gfx_object_center_location(&arduino[UNO].gfx_object, 270, 100);
	GFX_set_gfx_object_velocity( &arduino[UNO].gfx_object, -2, 0 );


	GFX_draw_two_gfx_objects_on_background(&crosshair.gfx_object, &arduino[UNO].gfx_object, &background);


	// wait for button pressed
	while( KBD_get_pressed_button() == BTN_NONE );



	while(1)
	{
		  GFX_update_two_gfx_objects_on_background(&crosshair.gfx_object, &arduino[UNO].gfx_object, &background);

		  HAL_Delay(20);

	}

}




void GFX_demo_joystick_controlled_crosshair_arduino_bouncing(void)
{
	OBJ_init();	// init all the objects

	// display background
	GFX_draw_gfx_object(&background);


	GFX_init_gfx_object_center_location( &arduino[UNO].gfx_object, arduino[UNO].gfx_object.center_limits.X_MIN, canvas.above_ground.y_center);
	GFX_set_gfx_object_velocity(&arduino[UNO].gfx_object, 1, 0);

	//GFX_init_gfx_object_center_location( &arduino[UNO].gfx_object, arduino[UNO].gfx_object.center_limits.X_MAX-50, arduino[UNO].gfx_object.center_limits.Y_MAX-50);

	GFX_draw_two_gfx_objects_on_background( &crosshair.gfx_object, &arduino[UNO].gfx_object, &background);




	while(1)
	{
	  OBJ_crosshair_set_center_location_with_joystick();

	  if ( ! GFX_update_moving_gfx_object_location(&arduino[UNO].gfx_object) )
		  GFX_set_gfx_object_velocity( &arduino[UNO].gfx_object, -arduino[UNO].gfx_object.velocity.x, 0);


	  GFX_draw_two_gfx_objects_on_background( &crosshair.gfx_object, &arduino[UNO].gfx_object, &background);

	  //JOY_SCI_send_status();
	  //HAL_Delay(100);;

	}

}









// -------------- Private function implementations -------------



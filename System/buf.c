/*
 * buf.c
 *
 *  Created on: Jan 20, 2022
 *  Last revision: Apr 7, 2022
 *      Authors: Gasper, Bostjan, Matija
 */

/* **************** MODULE DESCRIPTION *************************

The buffer module is intended to help you use any array as a circular buffer.
You must therefore provide an array that will be treated as a buffer and a
buffer handle data structure where all the required parameters regarding
the buffer are kept.

Operation:
Pointer "rear" points to the first free location (to store the next byte).
Pointer "front" points to the first occupied location (i.e. the next byte to be read).
Empty buffer condition: rear = front.
Full buffer condition: rear = front - 1 (i.e. one location is actually free when buffer is full).

"rear" pointer is only modified when storing data.
"front" pointer is only modified when reading data.
Pointer variable values are always valid, i.e. the pointers point to a valid location inside
the buffer at all times. This means that errors cannot occur due to interrupts
and read/write buffer race conditions.

************************************************************* */




// ----------- Include other modules (for private) -------------

#include "buf.h"


// ---------------------- Private definitions ------------------


// Macro functions that help updating the buffer rear/front pointers and help
// determine the buffer data size.

// Returns the value "x" incremented by 1 using a modulo "mod" arithmetics.
// NOTE: the value "x" should be bounded to the modulo (i.e. less than modulo value "mod").
#define BUF_INC_MOD(x, mod) ( (x)+1 >= (mod) ? 0 : (x)+1 )

// Returns the difference "x - y" using modulo "mod". This means that the difference is bounded by
// the limit values 0 and "mod - 1".
// NOTE: arguments should be between 0 and "mod - 1".
#define BUF_DIF_MOD(x, y, mod) ( (x)<(y) ? (x)-(y)+(mod) : (x)-(y) )




// -------------- Public function implementations --------------


// Initialize the buffer handle according to given parameters.
// buf_length is length of buffer given by buffer_ptr,
// capacity of the buffer will be one location (byte) less.
void BUF_init(buf_handle_t *buf_handle, uint8_t *buffer_ptr, uint32_t buf_length)
{
	buf_handle->buffer = buffer_ptr;
	buf_handle->length = buf_length;

	// And flush the buffer.
	BUF_flush(buf_handle);
}

// Flush the buffer
buf_rtrn_codes_t BUF_flush(buf_handle_t *buf_handle)
{
	buf_handle->front = 0;
	buf_handle->rear = 0;

	return BUFFER_OK;
}




// Store one byte of data.
buf_rtrn_codes_t BUF_store_byte(buf_handle_t *buf_handle, uint8_t data)
{
	// check if buffer already full
	// this happens when incremented rear pointer is equal to the front pointer
	if ( BUF_INC_MOD(buf_handle->rear, buf_handle->length) == buf_handle->front )
	{
		return BUFFER_FULL;	//-> if so, return error
	}
	else
	{
		// store data
		buf_handle->buffer[buf_handle->rear] = data;

		// increment the rear pointer, pointing to the next location in the buffer
		buf_handle->rear = BUF_INC_MOD(buf_handle->rear, buf_handle->length);
		
		return BUFFER_OK;
	}
}

// Store multiple bytes
buf_rtrn_codes_t BUF_store_bytes(buf_handle_t *buf_handle, uint8_t *data, uint32_t size)
{
	buf_rtrn_codes_t rtrn_code=BUFFER_OK;
	
	// First, check if there is even enough free space to store new data.
	if ( size > BUF_get_free_size(buf_handle) )
	{
		return BUFFER_NOT_ENOUGH_SPACE;
	}
	else
	{
		// store the data cluster in the buffer
		for(uint32_t i=0; i<size; i++)
		{
			// store i-th byte
			rtrn_code = BUF_store_byte(buf_handle, data[i]);

			// if the return code not OK, break the operation with the error code
			if (rtrn_code != BUFFER_OK)
				return rtrn_code;
		}

		// if all data store successfully, return OK
		return BUFFER_OK;
	}
}







// Read one byte of data and store it to the given location.
buf_rtrn_codes_t BUF_get_byte(buf_handle_t *buf_handle, uint8_t *data)
{
	// first check if buffer empty
	if (buf_handle->front == buf_handle->rear)
	{
		return BUFFER_EMPTY;	// return error code and exit this function
	}

	// else read and return data from the buffer "front"
	*data = buf_handle->buffer[buf_handle->front];	//Get data from the queue "front"

	// increment the front pointer, releasing the location in the buffer
	buf_handle->front = BUF_INC_MOD(buf_handle->front, buf_handle->length);

	return BUFFER_OK;	// return BUFFER_OK error code

}


// Read the specified number of bytes (size) from the buffer and store it to the &data location.
buf_rtrn_codes_t BUF_get_bytes(buf_handle_t *buf_handle, uint8_t *data,  uint32_t size)
{
	buf_rtrn_codes_t	rtnr_code;

	// check if enough bytes available in the buffer
	if (BUF_get_data_size(buf_handle) < size)
	{
		return BUFFER_NOT_ENOUGH_DATA;
	}

	// get all the data byte by byte and store in the &data location
	for(int i=0; i<size; i++ )
	{
		// get one byte and remember the return code
		rtnr_code = BUF_get_byte( buf_handle, &data[i] );
		
		// if something went wrong reading this byte
		if ( rtnr_code != BUFFER_OK )
			return rtnr_code;
	}

	// if we are here, everything went OK
	return BUFFER_OK;
}



// Get the number of bytes currently in the buffer queue.
uint32_t BUF_get_data_size(buf_handle_t *buf_handle)
{
	// NOTE: you must first store the front/rear pointers in the local variables
	// and only then use these values for data size calculation. This is how you
	// avoid race conditions during data size calculation!
	uint32_t rear = buf_handle->rear;
	uint32_t front = buf_handle->front;

	// Calculate the size using modulus arithmetics.
	return BUF_DIF_MOD(rear, front, buf_handle->length);

}


// Get the size of the free space in the buffer in bytes.
uint32_t BUF_get_free_size(buf_handle_t *buf_handle)
{
	// NOTE: you must first store the front/rear pointers in the local variables
	// and only then use these values for free size calculation. This is how you
	// avoid race conditions during free size calculation!
	uint32_t rear = buf_handle->rear;
	uint32_t front = buf_handle->front;

	// Calculate the size using modulus arithmetics.
	return (buf_handle->length - BUF_DIF_MOD(rear, front, buf_handle->length) - 1);
}





// -------------- Private function implementations -------------

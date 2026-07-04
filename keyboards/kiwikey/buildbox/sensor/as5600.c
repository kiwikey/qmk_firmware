#include "as5600.h"
#include "magnetic_encoder.h"

static uint16_t last_pos = 0;

void magnetic_encoder_update_user(magnetic_encoder_kind_t magnetic_encoder, bool direction) {
	// int16_t angle = as5600read_angle();
	uint16_t pos = as5600read_angle();
	int16_t delta = (int16_t)pos - (int16_t)last_pos;

	// Handle wrap-around
	// if ()
	
	printf("+ last_pos = %5d  pos = %5d  delta = %5d \n", last_pos, pos, delta);

	last_pos = pos;

	// printf("+ direction = %5s", direction? "DOWN" : "UP");
	// printf("+ get_distance = %u \n", get_distance(magnetic_encoder));
	// printf("+ get_movement = %u \n", get_movement(100, magnetic_encoder));
}

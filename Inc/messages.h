#ifndef __MESSAGES_H
#define __MESSAGES_H

/* STM send requests and VMA send responses */
#define REQUEST_LENGTH   12  
#define RESPONSE_LENGTH  10
#define RECEIVE_TIMEOUT  5

#include "stdint.h"

struct Request
{
	uint8_t AA;
	uint8_t type; // 0x01
	uint8_t address;
	uint8_t update_base_vector; // true or false
	uint8_t position_setting; // enabling of position_setting
	uint16_t angle; // angle - 0..359;
	uint8_t velocity;
	uint8_t frequency;
	int16_t outrunning_angle;
	uint8_t crc;
};

struct Response
{
    uint8_t AA;
    uint8_t type;
    uint8_t address;
    uint8_t state;
    uint8_t position_code;
    uint16_t cur_angle;
    uint16_t current;
    uint8_t crc;
};

#endif //__MESSAGES_H

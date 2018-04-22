#ifndef __MESSAGES_H
#define __MESSAGES_H

/* STM send requests and VMA send responses */
#define VMA_DEV_REQUEST_LENGTH              7

#define RECEIVE_TIMEOUT                     5

#define VMA_DEV_REQUEST_AA1                 0
#define VMA_DEV_REQUEST_AA2                 1   // address
#define VMA_DEV_REQUEST_ADDRESS             2   // position setting
#define VMA_DEV_REQUEST_SETTING             3   // position
#define VMA_DEV_REQUEST_VELOCITY1           4   // pwm duty
#define VMA_DEV_REQUEST_VELOCITY2           5   // period frequency
#define VMA_DEV_REQUEST_CHECKSUM            6

#define VMA_DEV_RESPONSE_LENGTH             10

#define VMA_DEV_RESPONSE_AA                 0
#define VMA_DEV_RESPONSE_ADDRESS            1
#define VMA_DEV_RESPONSE_ERRORS             2
#define VMA_DEV_RESPONSE_CURRENT_1H         3
#define VMA_DEV_RESPONSE_CURRENT_1L         4
#define VMA_DEV_RESPONSE_CURRENT_2H         5   // current gray code
#define VMA_DEV_RESPONSE_CURRENT_2L         6   // current state
#define VMA_DEV_RESPONSE_VELOCITY1	        7   // period for next commutation
#define VMA_DEV_RESPONSE_VELOCITY2          8
#define VMA_DEV_RESPONSE_CHECKSUM           9

#define VMA_NUMBER                          8

#endif //__MESSAGES_H

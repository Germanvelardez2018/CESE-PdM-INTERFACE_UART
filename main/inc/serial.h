/*
 * serial.h
 *
 *  Created on: 21 abr. 2021
 *      Author: root
 */

#ifndef MAIN_INC_SERIAL_H_
#define MAIN_INC_SERIAL_H_




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "driver/uart.h"
#include "soc/uart_struct.h"


#include "system.h"


#define BUF_SIZE 256
#define BUF_RX (BUF_SIZE)
#define BUF_TX	(BUF_SIZE)


// patter definition: CCC

#define PATTERN_CHR_NUM    (3)
#define PATTERN_CHR       ('C')


//public functions

void serial_init( serial_app* serial_app);
void serial_deinit();
void serial_load_baudrate(); //param from flash memory
void serial_out(char* string);
void serial_print(char* string);
void serial_task_input(void *pvParameter);




#endif /* MAIN_INC_SERIAL_H_ */

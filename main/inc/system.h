/*
 * system.h
 *
 *  Created on: 21 abr. 2021
 *      Author: root
 */

#ifndef MAIN_INC_SYSTEM_H_
#define MAIN_INC_SYSTEM_H_




#include <stdio.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "mqtt_client.h"

struct memory_app{
	 nvs_handle_t memory_handle;
	 int8_t memory_initialized ;
};

typedef struct memory_app memory_app;

//processador



typedef enum{
	WAITING,
	WORKING,    //workin tiene substados
	ENDING,


	UNDEFINED
} State_t;

typedef enum {
	EVENT_NEXT_COMMAND,
	EVENT_EMPTY_QUEUE,
	EVENT_REPONDED,
	EVENT_ENDED,
	EVENT_ERROR

} Event_t;


typedef Event_t (*Runnable_t)();

struct buffer{
	char* text;
	int8_t size;
};

typedef struct buffer buffer;






typedef struct {
	State_t current;
	Event_t event;
	State_t next;
	Runnable_t action;
} FSMUser_interface_t;



struct processor_app{
	int8_t mode;
	char* buffer_in;
	uint8_t state_size;
	Event_t ev;
	State_t state_processor;
	QueueHandle_t userinterface_in;   // QUEUE del uart
	QueueHandle_t userinterface_out;   // QUEUE del uart
};

typedef struct processor_app processor_app;

struct serial_app {
	int32_t baudrate;
	int8_t uart;
	QueueHandle_t serial_in;   // QUEUE del uart
	QueueHandle_t serial_out;   // QUEUE del uart

	int8_t queue_elements;
};

typedef struct serial_app serial_app;

struct wifi_app {
	char *wifi_id;
	char *wifi_password;
	int8_t wifi_state;
};

typedef struct wifi_app wifi_app;

struct mqtt_app {
	char *mqtt_url;
	char *mqtt_tag_pub;
	char *mqtt_tag_sub;
	esp_mqtt_client_handle_t client;
	int8_t mqtt_state;
	wifi_app wifi_app;
};

typedef struct mqtt_app mqtt_app;


struct display_app {
	QueueHandle_t display_out;   // QUEUE del uart
	int8_t lcd_state;
};

typedef struct display_app display_app;



struct command_response{
	int8_t remote_in;
	int8_t serial_out;
	int8_t mqtt_out;
	char* command;
	char* param_1;
	char* param_2;
	int32_t baud;
	char* final_res;

};

void  extract_1_parameter(char* command);
void  extract_2_parameter(char* command);
void  extract_baudrate(char* command);
void system_init();
void system_task_out(void *pvParameter);
void system_state_task(void *pvParameter);
void system_send_queue_input(char * command);


#endif /* MAIN_INC_SYSTEM_H_ */

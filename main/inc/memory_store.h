/*
 * memory_store.h
 *
 *  Created on: 22 abr. 2021
 *      Author: root
 */

#ifndef MAIN_INC_MEMORY_STORE_H_
#define MAIN_INC_MEMORY_STORE_H_



#include "nvs_flash.h"
#include "nvs.h"
#include "memory_table.h"
#include "system.h"
#include "serial.h"



 void ksi_memory_load();


void ksi_memory_init(serial_app *serial, wifi_app *wifi, mqtt_app *mqtt);
void ksi_memory_deinit();
void ksi_memory_set_wifiid(char *wifi_id);
void ksi_memory_set_wifipass(char *wifi_password);
void ksi_memory_set_wifi_state(int8_t state);
void ksi_memory_set_mqtturl(char* url);
void ksi_memory_set_mqtt_pub(char* tag);
void ksi_memory_set_mqtt_sub(char* tag);
void ksi_memory_set_mqtt_state(int8_t state);
void ksi_memory_set_baud(int32_t baud);



int32_t ksi_memory_get_baud();
char* ksi_memory_get_wifiid();
char* ksi_memory_get_wifipass();
int8_t ksi_memory_get_wifi_state();

char* ksi_memory_get_mqtturl();
char* ksi_memory_get_mqtt_pub();
char* ksi_memory_get_mqtt_sub();
int8_t ksi_memory_get_mqtt_state();



#endif /* MAIN_INC_MEMORY_STORE_H_ */

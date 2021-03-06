/*
 * system.c
 *
 *  Created on: 21 abr. 2021
 *      Author: German Velardez
 */


#include "system.h"
#include "serial.h"
#include "memory_store.h"

static processor_app* _pro;



//format commandf

// funcion_number +  (without parameters)  .  dont forget insert '+' in the last position
// function_number + parameter_1+
// function_number + parameter_1 + parameter_2



static char* extract_parameters_str(char *buffer_in, int num_param) {

	int space = (*_pro).fun_number > 9 ? 2 : 1;        //size     '9'    '10'
	char *params = buffer_in + space + 1; //offset fun_number
	char *param_2 = strcasestr(params, "+");  //pass empezara con +,borrar
	int8_t len_param_1 = param_2 - params;
	char *param_1 = malloc(len_param_1 + 1);
	strncpy(param_1, params, len_param_1);
	param_1[len_param_1] = '\0';

	if (num_param == 2) {
		return param_2 + 1;  //add 1 for offset '+' (+param_2)
	}
	return param_1;

}

static int32_t extract_int32() {
	char *param = (*_pro).buffer_in + 2; //offset   1+baudrate   fun_number=1 is setbaudrate
	int32_t param_value = (int32_t) strtol(param, NULL, 10);
	return param_value;
}


void get_baudrate(){
	system_print("FUNCION: GET BAUDRATE\n");
	//leer desde flash baudrate almacenado
	system_print("BAUDRATE:%d\n",ksi_memory_get_baud());

}

void set_baudrate(){
	system_print("FUNCION: SET BAUDRATE\n");
	int32_t baudrate  =extract_int32();   // if extrac is error, than response.baud=0 ignore;
	if(baudrate ==0){
		system_print("FUNCION: SET BAUDRATE ERROR, PARAM IS INVALID \n");
	}
	else{
		system_print("FUNCION: SET BAUDRATE:%d\n",baudrate);
		vTaskDelay(500 / portTICK_PERIOD_MS);
		ksi_memory_set_baud(baudrate);

	}
}

void get_wifi_params() {
	system_print("FUNCION: GET WiFi PARAMS\n");
	char *p_wifi_id = ksi_memory_get_wifiid();
	if (p_wifi_id != NULL) {
		system_print("--WiFi-ID: %s\n", p_wifi_id);
	} else {
		system_print("--WiFi-ID: UNDEFINED\n");

	}
	char *p_wifi_password = ksi_memory_get_wifipass();
	if (p_wifi_password != NULL) {
		system_print("--WiFi-PASSWORD: %s\n", p_wifi_password);
	} else {
		system_print("--WiFi-PASSWORD: UNDEFINED\n");

	}

}

void set_wifi_params() {
	system_print("FUNCION: SET WiFi PARAMS\n");
	char *param_1 = extract_parameters_str((*_pro).buffer_in, 1);
	if (param_1 == NULL) {
		system_print("ERROR PARAMETERS'n");
		return;
	}
	char *param_2 = extract_parameters_str((*_pro).buffer_in, 2);
	if (param_2 == NULL) {
		system_print("ERROR PARAMETERS'n");
		return;
	}
	ksi_memory_set_wifiid(param_1);
	ksi_memory_set_wifipass(param_2);
	system_print("--PARAMETER 1 %s\n", param_1);
	system_print("--PARAMETER 2 %s\n", param_2);

}

void get_mqtt_url() {
	system_print("FUNCION: GET MQTT URL\n");
	char *p_mqtt_url = ksi_memory_get_mqtturl();
	if (p_mqtt_url != NULL) {
		system_print("--MQTT-URL: %s\n", p_mqtt_url);
	} else {
		system_print("--MQTT-URL: UNDEFINED\n");

	}
}

void set_mqtt_url() {
	system_print("FUNCION: SET MQTT URL\n");
	char *param_1 = extract_parameters_str((*_pro).buffer_in, 1);
	if (param_1 == NULL) {
		system_print("ERROR PARAMETERS'n");
		return;
	}
	ksi_memory_set_mqtturl(param_1);
	system_print("--PARAMETER 1: %s\n", param_1);
}


static Executable functions_table[]={

	    &get_baudrate,
	    &set_baudrate,
	    &get_wifi_params,
	    &set_wifi_params,
	    &get_mqtt_url,
	    &set_mqtt_url,
};



void system_send_queue_input(char* buffer_in)
{
	xQueueSendToBack((*_pro).userinterface_in, &(buffer_in),(portTickType) portMAX_DELAY);
}



static int8_t get_function(char* buffer_in){
	// command -->   "fun_number+param1+param2 (SET FUNCTION WITH TWO PARAMETERS STRINGS)
	// command -->   "fun_number+param1+ 	   (SET FUNCTION WITH ONE PARAMETER  STRINGS)
	// command -->   "fun_number+              (GET FUNCTION)

	char* rest = strcasestr(buffer_in,"+");
	int size_funct = rest -buffer_in ;
	char* function = malloc(size_funct+1);
	strncpy(function,buffer_in,size_funct);
	function[size_funct]='\0';// ending the string
	int8_t fun_number = (int8_t) strtol(function, NULL, 10);
	return fun_number;
}

static void readCommand(char *buffer_in) {

	int8_t fun_number =get_function(buffer_in);
	(*_pro).fun_number = fun_number;  // load in response the fun_number
	(*functions_table[fun_number])(buffer_in);

}

static void EndingResponse() { // put here your ending functions
	system_print("Finalizando la respuesta\n ");
}

   //Process the command into the buffer
static Event_t ProcessCommand() {
	readCommand((*_pro).buffer_in);
	return EVENT_REPONDED;
}



static Event_t ProcessFinally() {
	EndingResponse();
	return EVENT_ENDED;
}

FSMUser_interface_t fsmTable[] = {

		{ WAITING, EVENT_NEXT_COMMAND,WORKING, &ProcessCommand },
		{ WORKING,EVENT_REPONDED, ENDING, &ProcessFinally },
		{ ENDING, EVENT_ENDED,WAITING, NULL }
};

void processor_init() //Modo 0 interfaz usuario pc , modo 1 interfaz microcontrolador wentux
{
	(*_pro).mode =0;
	(*_pro).state_processor = WAITING;
	(*_pro).state_size = sizeof(fsmTable) / sizeof(FSMUser_interface_t);
}


void system_init (processor_app *procesor){
	_pro = procesor;
	processor_init(0);
	(*_pro).userinterface_in = xQueueCreate(10, sizeof(char*)); // ten elements in the input queue
}



void system_state_task(void *pvParameter) {
	for (;;) {
		   (*_pro).ev =xQueueReceive((*_pro).userinterface_in, (&((*_pro).buffer_in)),(portTickType) portMAX_DELAY) 
			   ?VENT_NEXT_COMMAND : EVENT_EMPTY_QUEUE;
		   if ((*_pro).ev == EVENT_NEXT_COMMAND) {
		     for (uint8_t i = 0; i < (*_pro).state_size; i++) {
				if (fsmTable[i].current == (*_pro).state_processor
						&& fsmTable[i].event == (*_pro).ev) {
						if (fsmTable[i].action != NULL) {
							(*_pro).ev = (Event_t) (*(fsmTable[i].action))(); // start command processing
						}
						(*_pro).state_processor = fsmTable[i].next;
					}
				}
			}
			vTaskDelay(200 / portTICK_PERIOD_MS);
		}
}








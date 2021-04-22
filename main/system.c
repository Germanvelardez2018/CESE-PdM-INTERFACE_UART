/*
 * system.c
 *
 *  Created on: 21 abr. 2021
 *      Author: root
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

static int32_t extract_baudrate() {
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
	int32_t baudrate  =extract_baudrate();   // if extrac is error, than response.baud=0 ignore;
	if(baudrate ==0){
		system_print("FUNCION: SET BAUDRATE ERROR, PARAM IS INVALID \n");
	}
	else{
		system_print("FUNCION: SET BAUDRATE:%d\n",baudrate);
		vTaskDelay(500 / portTICK_PERIOD_MS);
		ksi_memory_set_baud(baudrate);

	}
}
void get_wifi_params(){
	system_print("FUNCION: GET WiFi PARAMS\n");
	system_print("--WiFi-ID: mi_wifi\n");
	system_print("--WiFi-PASSWORD: *********\n");




}
void set_wifi_params(){
	system_print("FUNCION: SET WiFi PARAMS\n");
	system_print("--PARAMETER 1 %s\n", extract_parameters_str((*_pro).buffer_in,1));
	system_print("--PARAMETER 2 %s\n",  extract_parameters_str((*_pro).buffer_in,2));


}

void get_mqtt_url(){
	system_print("FUNCION: GET MQTT URL\n");
	system_print("--MQTT-URL: www.mqtt.org\n");
}
void set_mqtt_url(){
	system_print("FUNCION: SET MQTT URL\n");
	system_print("--PARAMETER 1: %s\n",extract_parameters_str((*_pro).buffer_in,1));

}





static Executable functions_table[]={

		&get_baudrate,
	    &set_baudrate,
		&get_wifi_params,
	    &set_wifi_params,
	    &get_mqtt_url,
	    &set_mqtt_url,

};


void system_send_queue_input(char* buffer_in)  // for user interface mqtt
{
	xQueueSendToBack((*_pro).userinterface_in, &(buffer_in),(portTickType) portMAX_DELAY);
	}



static int8_t get_function(char* buffer_in){
	// command -->   "fun_number+param1+param2   or fun_number only

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

static void EndingResponse() { // Aqui irian funciones de cierre a los comandos realizados
	system_print("Finalizando la respuesta\n ");
}

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
	(*_pro).userinterface_in = xQueueCreate(10, sizeof(char*)); // 10 mensaje de 20 char cada uno


}



void system_state_task(void *pvParameter) {
	for (;;) {
			(*_pro).ev =
					xQueueReceive((*_pro).userinterface_in, (&((*_pro).buffer_in)),
							(portTickType) portMAX_DELAY) ?
							EVENT_NEXT_COMMAND : EVENT_EMPTY_QUEUE;


			if ((*_pro).ev == EVENT_NEXT_COMMAND) {

				for (uint8_t i = 0; i < (*_pro).state_size; i++) {

					if (fsmTable[i].current == (*_pro).state_processor
							&& fsmTable[i].event == (*_pro).ev) {

						if (fsmTable[i].action != NULL) {
							(*_pro).ev = (Event_t) (*(fsmTable[i].action))(); // supuestemnte estoy llamoando al callback
						}
						(*_pro).state_processor = fsmTable[i].next;
					}
				}

			}
			vTaskDelay(200 / portTICK_PERIOD_MS);

		}
}








/*
 * system.c
 *
 *  Created on: 21 abr. 2021
 *      Author: root
 */


#include "system.h"
#include "serial.h"


static processor_app* _pro;

void system_send_queue_input(char* buffer_in)
{
	xQueueSendToBack((*_pro).userinterface_in, &buffer_in,(portTickType) portMAX_DELAY);
	}

static void pro_print(char *out) {


	xQueueSendToBack((*_pro).userinterface_out, &out, 500/portTICK_RATE_MS);
}

static void vprint(const char *fmt, va_list argp) {
	char string[200];
	if (0 < vsprintf(string, fmt, argp)) // build string
			{
				pro_print(string);
	}
}

void processor_print(const char *fmt, ...) // custom printf() function
{
	va_list argp;
	va_start(argp, fmt);
	vprint(fmt, argp);
	va_end(argp);
}


static void readCommand(char *buffer_in) {

}

static void EndingResponse() { // Aqui irian funciones de cierre a los comandos realizados
	processor_print("Finalizando la respuesta\n ");
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

{ WAITING, EVENT_NEXT_COMMAND, WORKING, &ProcessCommand }, { WORKING,
		EVENT_REPONDED, ENDING, &ProcessFinally }, { ENDING, EVENT_ENDED,
		WAITING, NULL } };

void processor_init() //Modo 0 interfaz usuario pc , modo 1 interfaz microcontrolador wentux
{
	(*_pro).mode =0;
	(*_pro).state_processor = WAITING;
	(*_pro).state_size = sizeof(fsmTable) / sizeof(FSMUser_interface_t);
}


void system_init (processor_app *procesor){
	_pro = procesor;
	processor_init(0);
	(*_pro).userinterface_out = xQueueCreate(15, sizeof(buffer)); // 10 mensaje de 20 char cada uno
	(*_pro).userinterface_in = xQueueCreate(15, sizeof(buffer)); // 10 mensaje de 20 char cada uno


}

static void  extract_1_parameter(char* command);
static void  extract_2_parameter(char* command);
static void  extract_baudrate(char* command);




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




 void system_task_out(void *pvParameter) {
	char* buffer_out;
	for (;;) {
		if (xQueueReceive((*_pro).userinterface_out,&buffer_out,
				(portTickType) portMAX_DELAY)) {

			serial_out( buffer_out);
		}
		vTaskDelay(200 / portTICK_PERIOD_MS);

	}
}





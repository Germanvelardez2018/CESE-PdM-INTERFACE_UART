/*
 * serial.c
 *
 *  Created on: 21 abr. 2021
 *      Author: root
 */


#include "serial.h"



static serial_app* _serial;




void serial_print(char* string){
	xQueueSendToBack((*_serial).serial_out, &string, 500/portTICK_RATE_MS);

}


void serial_out(char* string){
	uart_write_bytes((*_serial).uart, (const char*) string, strlen(string));
}




void serial_load_baud(){
	printf("se cargo baud en hardware: %d\n",(*_serial).baudrate);
	uart_set_baudrate((*_serial).uart,(*_serial).baudrate);
}




void serial_init(serial_app *serial_app) {

	_serial = serial_app;

	uart_config_t uart_config =
			{ .baud_rate = serial_app->baudrate, .data_bits = UART_DATA_8_BITS,
					.parity = UART_PARITY_DISABLE,
					.stop_bits = UART_STOP_BITS_1, .flow_ctrl =
							UART_HW_FLOWCTRL_DISABLE, //UART_HW_FLOWCTRL_DISABLE    UART_HW_FLOWCTRL_CTS_RTS

			};
	ESP_ERROR_CHECK(uart_param_config((*_serial).uart, &uart_config));
	ESP_ERROR_CHECK(
			uart_set_pin((*_serial).uart,16,17,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE)); //18 Y 19     , 17 Y 18   RX Y TX
	ESP_ERROR_CHECK(
			uart_driver_install((*_serial).uart,(const int)2*BUF_RX,(const int)2*BUF_TX,20,&((*_serial).serial_in),0));
	//Set uart pattern detect function.
	uart_enable_pattern_det_baud_intr((*_serial).uart, PATTERN_CHR,
			PATTERN_CHR_NUM, 9, 0, 0); // pattern is LF
	//Reset the pattern queue length to record at most 20 pattern positions.
	uart_pattern_queue_reset((*_serial).uart, (*_serial).queue_elements); //Se resetea la cola de posiciones en 20


	// start entry and exit queues

	serial_out("mensaje fin de configuracion aurt\n");


}

void serial_deinit() {
	uart_driver_delete((*_serial).uart);
}


void serial_task_input(void *pvParameter){

	printf("iniciando tarea input serrial\n");

	uart_event_t event; //Estructura para gestionar evento de uart, es el objeto que se usa en la QueueUart
	size_t buffered_size;
	uint8_t *input_data = (uint8_t*) malloc(BUF_SIZE);
	char *com;    //BUFFER ESCLUSIVO PARA COMANDOS

	for (;;) {
		if (xQueueReceive((*_serial).serial_in, (void*) &event,
				(portTickType) portMAX_DELAY))
		printf("el evento es: %d\n",event.type);
		{
			switch (event.type) {
			case UART_DATA:
				uart_read_bytes((*_serial).uart, input_data, event.size, portMAX_DELAY); //Leo un comando
				uart_write_bytes((*_serial).uart, (const char*) input_data, event.size);
				serial_out(" -> No es un comando valido \n");
				break;

			case UART_FIFO_OVF:
				serial_print("hw fifo overflow");
				uart_flush_input((*_serial).uart);
				xQueueReset((*_serial).serial_in);
				break;

			case UART_BUFFER_FULL:
				serial_print("ring buffer full");
				uart_flush_input((*_serial).uart);
				xQueueReset((*_serial).serial_in);
				break;

			case UART_BREAK:
				serial_print("uart rx break");
				break;
			case UART_PARITY_ERR:
				serial_print("uart parity error");
				break;

			case UART_FRAME_ERR:
				serial_out("uart frame error");
				break;

			case UART_PATTERN_DET:
				uart_get_buffered_data_len((*_serial).uart, &buffered_size);
				int pos = uart_pattern_pop_pos((*_serial).uart);
				if (pos == -1) {
					uart_flush_input((*_serial).uart);
				} else {

					com = (char*) malloc(pos + 1);
					uart_read_bytes((*_serial).uart, com, pos, 100 / portTICK_PERIOD_MS); //lectura del comando
					uint8_t pat[PATTERN_CHR_NUM + 1];
					memset(pat, 0, sizeof(pat)); //sive para agregar final a cadenas
					uart_read_bytes((*_serial).uart, pat, PATTERN_CHR_NUM,200 / portTICK_PERIOD_MS);
					uart_flush((*_serial).uart);
					com[pos] = '\0';
					serial_out(com);
					printf("comando recibido: %s",com);
				// implementar urgente	ksi_processor_send_queue(com);
				}
				break;
				//Others
			default:
				printf("defaut case event type: %d", event.type);
				break;
			}
		}  //if

	} //for


}

































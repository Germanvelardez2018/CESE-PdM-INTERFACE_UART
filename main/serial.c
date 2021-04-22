/*
 * serial.c
 *
 *  Created on: 21 abr. 2021
 *      Author: root
 */


#include "serial.h"



static serial_app* _serial;

void serial_out(char *string) {
	uart_write_bytes((*_serial).uart, (const char*) string, strlen(string));
}

static void vprint(const char *fmt, va_list argp) {
	char string[100];
	if (0 < vsprintf(string, fmt, argp)) // build string
			{
		serial_out(string);
	}
}

void system_print(const char *fmt, ...) // custom print() function for the system
{
	va_list argp;
	va_start(argp, fmt);
	vprint(fmt, argp);
	va_end(argp);
}

void serial_load_baudrate() {
	printf("se cargo baud en hardware: %d\n", (*_serial).baudrate);
	uart_set_baudrate((*_serial).uart, (*_serial).baudrate);
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
	uart_pattern_queue_reset((*_serial).uart, (*_serial).queue_elements);
}

void serial_deinit() {
	uart_driver_delete((*_serial).uart);
}

void serial_task_input(void *pvParameter) {

	printf("iniciando tarea input serrial\n");

	uart_event_t event; //Estructura para gestionar evento de uart, es el objeto que se usa en la QueueUart
	size_t buffered_size;
	uint8_t *input_data = (uint8_t*) malloc(BUF_SIZE);
	char *com;    //BUFFER ESCLUSIVO PARA COMANDOS

	for (;;) {
		if (xQueueReceive((*_serial).serial_in, (void*) &event,
				(portTickType) portMAX_DELAY))
			printf("el evento es: %d\n", event.type);
		{
			switch (event.type) {
			case UART_DATA:
				uart_read_bytes((*_serial).uart, input_data, event.size,
						portMAX_DELAY); //Leo un comando
				uart_write_bytes((*_serial).uart, (const char*) input_data,
						event.size);
				serial_out(" -> No es un comando valido \n");
				break;

			case UART_FIFO_OVF:
				serial_out("hw fifo overflow");
				uart_flush_input((*_serial).uart);
				xQueueReset((*_serial).serial_in);
				break;

			case UART_BUFFER_FULL:
				serial_out("ring buffer full");
				uart_flush_input((*_serial).uart);
				xQueueReset((*_serial).serial_in);
				break;

			case UART_BREAK:
				serial_out("uart rx break");
				break;
			case UART_PARITY_ERR:
				serial_out("uart parity error");
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
					uart_read_bytes((*_serial).uart, com, pos,
							100 / portTICK_PERIOD_MS); //lectura del comando
					uint8_t pat[PATTERN_CHR_NUM + 1];
					memset(pat, 0, sizeof(pat)); //sive para agregar final a cadenas
					uart_read_bytes((*_serial).uart, pat, PATTERN_CHR_NUM,
							200 / portTICK_PERIOD_MS);
					uart_flush((*_serial).uart);
					com[pos] = '\0';
					printf("comando recibido: %s", com);
					system_send_queue_input(com);
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

































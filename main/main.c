#include "freertos/FreeRTOS.h"
#include "freertos/task.h"




#include "system.h"
#include "serial.h"
#include "memory_store.c"




/* DEFAULT VALUES
 * */

static processor_app processor ={
		.mode=0,
		.state_processor=WAITING,
};

static serial_app serial = {
		.baudrate=9600,
		.uart=2,
		.queue_elements=10
};

static wifi_app wifi = {
		.wifi_id=NULL,
		.wifi_password=NULL,
		.wifi_state=0,
};
static mqtt_app mqtt={
		.mqtt_url=NULL,
		.mqtt_tag_pub=NULL,
		.mqtt_tag_sub=NULL,
		.mqtt_state=0,
};


static display_app display={};   //future implementation



void app_main(void)
{
	printf("iniciamos el programa\n");

	ksi_memory_init(&serial, &wifi, &mqtt);
	ksi_memory_load();  // load states of the structs

	serial_init(&serial);   //starting the serial port

	system_init(&processor); //starting the user interfaces

	xTaskCreatePinnedToCore(serial_task_input, "serial in", 1024 * 30, NULL, 1,NULL, 1);   //Uart interface esta implementada en UART2.
	xTaskCreatePinnedToCore(system_state_task, "system state", 1024 * 40, NULL, 1,NULL, 1);   //Uart interface esta implementada en UART2.

	while (true) {

		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}


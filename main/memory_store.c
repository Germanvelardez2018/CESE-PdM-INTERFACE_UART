/*
 * memory_store.c
 *
 *  Created on: 22 abr. 2021
 *      Author: root
 */


#include "memory_store.h"


serial_app*  _m_serial;
wifi_app*    _m_wifi;
mqtt_app*    _m_mqtt;



static memory_app memory={
		 .memory_initialized=0,
		 .memory_handle=NULL
};




static void ksi_memory_open() {
	esp_err_t err = nvs_open(CONFIG_REG, NVS_READWRITE,
			&(memory.memory_handle));
	if (err != ESP_OK) {
		system_print("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		printf("Open config success\n");
	}

}

static void ksi_memory_close() {
	nvs_commit((memory.memory_handle));
	nvs_close(memory.memory_handle);
	printf("Close config success\n");
}

void ksi_memory_deinit() {
	nvs_flash_deinit();
	memory.memory_initialized = 0;

}

void ksi_memory_set_wifiid(char *wifi_id) {
	if (wifi_id != NULL) {
		ksi_memory_open();
		esp_err_t err = nvs_set_str(memory.memory_handle, ID_WIFI, wifi_id);

		if (err != ESP_OK) {
			printf("Error al conectarse a red %s\n", wifi_id);
		} else {
			printf("Se configuro ID_WIFI: %s\n", wifi_id);
			(*_m_wifi).wifi_id = malloc(strlen(wifi_id) + 1);
			strcpy((*_m_wifi).wifi_id, wifi_id);
		}
		ksi_memory_close();
	} else {
		printf("(SETWIFI) Parametro invalido ignorado %s\n", wifi_id);
	}

}

void ksi_memory_set_wifipass(char *wifi_password) {
	if (wifi_password != NULL) {
		ksi_memory_open();
		esp_err_t err = nvs_set_str(memory.memory_handle, PASSWORD_WIFI,
				wifi_password);

		if (err != ESP_OK) {
			printf("Error al guarda password: %s\n", wifi_password);
		} else {
			printf("Se configuro PASSWORD ID\n");
			(*_m_wifi).wifi_password = malloc(strlen(wifi_password) + 1);
			strcpy((*_m_wifi).wifi_password, wifi_password);

		}
		ksi_memory_close();
	} else {
		printf("(SETWIFI) Parametro invalido ignorado %s\n", wifi_password);
	}

}
void ksi_memory_set_mqtturl(char *url) {
	if (url != NULL) {
		ksi_memory_open();
		esp_err_t err = nvs_set_str(memory.memory_handle, MQTT_URL, url);

		if (err != ESP_OK) {
			printf("Error al conectarse a red %s\n", url);
			(*_m_mqtt).mqtt_url=NULL;

		} else {
			printf("Se configuro MQTT url: %s\n", url);
			strcpy((*_m_mqtt).mqtt_url, url);

		}
		ksi_memory_close();
	} else {
		printf("Parametro invalido ignorado %s\n", url);
	}

}
void ksi_memory_set_mqtt_pub(char *tag_pub) {
	if (tag_pub != NULL) {
		ksi_memory_open();
		esp_err_t err = nvs_set_str(memory.memory_handle, MQTT_TAG_PUB,
				tag_pub);

		if (err != ESP_OK) {
			printf("Error AL GUARDAR TAG PUB: %s\n", tag_pub);
			(*_m_mqtt).mqtt_tag_pub=NULL;
		} else {
			printf("Se configuro MQTT TAG PUB: %s\n", tag_pub);
			strcpy((*_m_mqtt).mqtt_tag_pub, tag_pub);
		}
		ksi_memory_close();
	} else {
		printf("Parametro invalido ignorado TAG PUB %s\n", tag_pub);
	}

}

void ksi_memory_set_mqtt_sub(char *tag_sub) {
	if (tag_sub != NULL) {
		ksi_memory_open();
		esp_err_t err = nvs_set_str(memory.memory_handle, MQTT_TAG_SUB,
				tag_sub);

		if (err != ESP_OK) {
			printf("Error AL GUARDAR TAG SUB: %s\n", tag_sub);
			(*_m_mqtt).mqtt_tag_sub=NULL;
		} else {
			printf("Se configuro MQTT TAG SUB: %s\n", tag_sub);
			strcpy((*_m_mqtt).mqtt_tag_sub, tag_sub);
		}
		ksi_memory_close();
	} else {
		printf("Parametro invalido ignorado TAG SUB %s\n", tag_sub);
	}

}

void ksi_memory_set_baud(int32_t baudrate) {
	if ((baudrate >= 9600) && (baudrate <= 119200)) {
		ksi_memory_open();
		esp_err_t err = nvs_set_i32(memory.memory_handle, BAUDRATE, baudrate);

		if (err != ESP_OK) {
			printf("Error al configurar el baudrate\n");

		} else {
			printf("Se configuro exitosamente  baudrate: %d\n", baudrate);
			(*_m_serial).baudrate = baudrate;
			serial_load_baudrate(); // load baud in the hardware
		}
		ksi_memory_close();
	} else {
		printf("(SETBAUD) Parametro invalido ignorado %d\n", baudrate);
	}

}

int32_t ksi_memory_get_baud() {

	ksi_memory_open();
	esp_err_t err = nvs_get_i32(memory.memory_handle, BAUDRATE,
			&((*_m_serial).baudrate));

	if (err != ESP_OK) {
		printf("Error al leer el baudrate\n");
	}

	ksi_memory_close();
	return (*_m_serial).baudrate;

}

char* ksi_memory_get_wifiid() {
	ksi_memory_open();
	size_t required_size;
	nvs_get_str(memory.memory_handle, ID_WIFI, NULL, &required_size);
	(*_m_wifi).wifi_id = (char*) malloc(required_size);
	esp_err_t err = nvs_get_str(memory.memory_handle, ID_WIFI,
			(*_m_wifi).wifi_id, &required_size);

	if (err != ESP_OK) {
		printf("Error al leer el el WIFI_ID\n");
		(*_m_wifi).wifi_id = NULL;
	} else {
		printf("EL WIDIID es : %s\n", (*_m_wifi).wifi_id);
	}
	ksi_memory_close();
	return (*_m_wifi).wifi_id;

}
char* ksi_memory_get_wifipass() {
	ksi_memory_open();
	size_t required_size;
	nvs_get_str(memory.memory_handle, PASSWORD_WIFI, NULL, &required_size);
	(*_m_wifi).wifi_password = (char*) malloc(required_size);
	esp_err_t err = nvs_get_str(memory.memory_handle, PASSWORD_WIFI,
			(*_m_wifi).wifi_password, &required_size);
	if (err != ESP_OK) {
		printf("Error al leer el el PASS_ID\n");
		(*_m_wifi).wifi_password = NULL;
	} else {
		printf("EL WIFIPASS es : %s\n", (*_m_wifi).wifi_password);
	}
	ksi_memory_close();
	return (*_m_wifi).wifi_password;
}

char* ksi_memory_get_mqtturl() {
	ksi_memory_open();
	size_t required_size;
	nvs_get_str(memory.memory_handle, MQTT_URL, NULL, &required_size);
	(*_m_mqtt).mqtt_url = (char*) malloc(required_size);
	esp_err_t err = nvs_get_str(memory.memory_handle, MQTT_URL,
			(*_m_mqtt).mqtt_url, &required_size);
	if (err != ESP_OK) {
		printf("Error al cargar MQTT_URL\n");
		(*_m_mqtt).mqtt_url = NULL;
	} else {
		printf("MQTT_URL: %s\n", (*_m_mqtt).mqtt_url);
	}
	ksi_memory_close();
	return (*_m_mqtt).mqtt_url;

}
char* ksi_memory_get_mqtt_pub() {
	ksi_memory_open();
	size_t required_size;
	nvs_get_str(memory.memory_handle, MQTT_TAG_PUB, NULL, &required_size);
	(*_m_mqtt).mqtt_tag_pub = (char*) malloc(required_size);
	esp_err_t err = nvs_get_str(memory.memory_handle, MQTT_TAG_PUB,
			(*_m_mqtt).mqtt_tag_pub, &required_size);
	if (err != ESP_OK) {
		printf("Error al cargar MQTT_TAG_PUB\n");
		(*_m_mqtt).mqtt_tag_pub = NULL;
	} else {
		printf("MQTT_PUB: %s\n", (*_m_mqtt).mqtt_tag_pub);
	}
	ksi_memory_close();
	return (*_m_mqtt).mqtt_tag_pub;

}
char* ksi_memory_get_mqtt_sub() {
	ksi_memory_open();
	size_t required_size;
	nvs_get_str(memory.memory_handle, MQTT_TAG_SUB, NULL, &required_size);
	(*_m_mqtt).mqtt_tag_sub = (char*) malloc(required_size);
	esp_err_t err = nvs_get_str(memory.memory_handle, MQTT_TAG_SUB,
			(*_m_mqtt).mqtt_tag_sub, &required_size);
	if (err != ESP_OK) {
		printf("Error al cargar MQTT_TAG_SUB\n");
		(*_m_mqtt).mqtt_tag_sub = NULL;
	} else {
		printf("MQTT_SUB: %s\n", (*_m_mqtt).mqtt_tag_sub);
	}
	ksi_memory_close();
	return (*_m_mqtt).mqtt_tag_sub;

}

void ksi_memory_set_wifi_state(int8_t state) {

	ksi_memory_open();
	esp_err_t err = nvs_set_i8(memory.memory_handle, WIFI_STATE, state);

	if (err == ESP_OK) {
		(*_m_wifi).wifi_state = state;

	}

	ksi_memory_close();
}

int8_t ksi_memory_get_wifi_state() {
	ksi_memory_open();
	esp_err_t err = nvs_get_i8(memory.memory_handle, WIFI_STATE,
			&((*_m_wifi).wifi_state));

	if (err != ESP_OK) {
		//	printf("Error al leer el STATE WIFI\n");
		(*_m_wifi).wifi_state = 0;

	} else {
		//	printf(" STATE WIFI: %d\n",(*_m_wifi).wifi_state);
	}

	ksi_memory_close();
	return (*_m_wifi).wifi_state;
}

int8_t ksi_memory_get_mqtt_state() {
	ksi_memory_open();
	esp_err_t err = nvs_get_i8(memory.memory_handle, MQTT_STATE,
			&((*_m_mqtt).mqtt_state));

	if (err != ESP_OK) {
		//	printf("Error al leer el STATE WIFI\n");
		(*_m_mqtt).mqtt_state = 0;

	} else {
		//	printf(" STATE WIFI: %d\n",(*_m_wifi).wifi_state);
	}

	ksi_memory_close();
	return (*_m_mqtt).mqtt_state;

}
void ksi_memory_set_mqtt_state(int8_t state) {
	ksi_memory_open();
	esp_err_t err = nvs_set_i8(memory.memory_handle, MQTT_STATE, state);

	if (err != ESP_OK) {
		//	printf("Error al leer el STATE WIFI\n");
		(*_m_mqtt).mqtt_state = state;
	}

	ksi_memory_close();

}

 void ksi_memory_load() {

//load seial
	ksi_memory_get_baud();
//load wifi
	ksi_memory_get_wifiid();
	ksi_memory_get_wifipass();
	ksi_memory_get_wifi_state();
//load mqtt
	ksi_memory_get_mqtturl();
	ksi_memory_get_mqtt_pub();
	ksi_memory_get_mqtt_sub();
	printf("finalizamos carga de datos\n");
}

void ksi_memory_init(serial_app *serial, wifi_app *wifi, mqtt_app *mqtt){
	_m_serial= serial;
	_m_wifi = wifi;
	_m_mqtt = mqtt;
	esp_err_t ret = nvs_flash_init();
		if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
			//	ESP_ERROR_CHECK(nvs_flash_erase());
			//	ret = nvs_flash_init();
			system_print("ERROR GRAVE DE MEMORIA\n");
		}
		ESP_ERROR_CHECK(ret);
		memory.memory_initialized = 1;
		//ksi_memory_load();

}




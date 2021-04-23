
#Primeras implementacion de interfaz de usuario mediante UART en ESP-32


# Se utilizo una tabla para las funciones que ejecutan los comandos

# La posicion de la funcion en la tabla determina el fun_number del comando asociado


```c

functions_table[]={

	    &get_baudrate,      ------>fun_number =0
	    &set_baudrate,      ------>fun_number =1
	    &get_wifi_params,   ------>fun_number =2
	    &set_wifi_params,   ------>fun_number =3
	    &get_mqtt_url,      ------>fun_number =4
	    &set_mqtt_url,      ------>fun_number =5
};
```


# Formato de los comandos
Se soportan 3 tipos de comandos:

## comandos sin parametros ( funciones GET)

 comando = "fun_number+"
                           *no olvidar el signo '+'

##  comandos con un solo parametro (funciones SET)

 comando = "fun_number+parametro1+" 

##  comandos con dos parametros (funciones SET)

 comando = "fun_number+parametro1+parameter2"


 # Todos los comandos deben terminar con La inclusion de la secuencia 'CCC'
 Esto es necesario porque se implementaron funciones de deteccion 
 de patrones mediante UART.
 En caso de no agregar la secuencia el comando sera invalido y la interfaz lo notificara


  

# Agregar nuevas funciones:
 Para implemenarlas estan deben tener el formato de Executable  return void  params (char*)
 Se deben agregar en la tabla functions_table
 #Se recomienda utilizar las funciones de soporte para extraer parametros en caso de necesitarlos.
 Estas funciones solo serviran si se cumple el formato anteriormente explicado.

 ```c
    static char* extract_parameters_str(char *buffer_in, int num_param)
 ``` 
    Sirve para extraer el parametro uno o  o el  parametro   de un buffer string  recibido
 
 
 ```c
    static int32_t extract_int32()
 ```
    sirve para extraerl el primero y unico dato de un buffer string recibido. Utilizar solo  cuando se requiera setear valores int.
    

/*! @mainpage Proyecto Integrador - ConfortMed
 *
 * @section genDesc General Description
 *
 * El presente programa corresponde a un prototipo de administración de consultorios de atención médica denominado "ConfortMed".
 * El dispositivo se controla mediante una aplicación Bluetooth en la cual se seleccionan los días de atención correspondiente a dicho consultorio, con los correspondientes horarios.
 * Al comenzar la jornada de atención, se enciende el PC del consultorio a través de un GPIO que activa un relé conectado al power switch de la misma. 
 * Constantemente se sensa la temperatura del lugar utilizando un sensor de temperatura analógico LM35C. Si la temperatura promedio del mismo alcanza una temperatura mayor a 23°C, se enciende el aire acondicionado del lugar, a través de un GPIO 
 * conectado a una Arduino Nano encargado de emitir señales infrarrojas que enciendan/apaguen el artefacto según corresponda.
 * A través de la aplicación se pueden observar la temperatura actual sensada, la temperatura promedio y las temperaturas máximas y mínimas. A su vez se puede observar si actualmente el consultorio
 * se encuentra ocupado y cuándo finaliza el turno. En caso de encontrarse el consultorio desocupado, informa cuando comienza y finaliza el próximo turno.
 * Fuera de los horarios de atención, se enciende una lámpara UV encargada de desinfectar el lugar.
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  	 |        ESP32   	    |
 * |:-------------------:|:---------------------|
 * | 	LM35C	 		 | 	 GPIO_01 (CH1)		|
 * |	RELÉ LÁMPARA UV	 |	 GPIO_22			|
 * |	RELÉ PC			 |	 GPIO_21			|
 * |	ARDUINO NANO	 |	 GPIO_20			|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | 						Description                             |
 * |:----------:|:--------------------------------------------------------------|
 * | 16/05/2024 | Creación del Documento                         				|
 * | 19/06/2024	| Finalización. Se comprueba correcto funcionamiento del mismo	|
 * | 21/06/2024	| Documentación													|
 *
 * @author Wendler Tatiana Ailen (ailuwendler@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <lm35c.h>
#include <uart_mcu.h>
#include <analog_io_mcu.h>
#include <ble_mcu.h>
#include <time.h>

/*==================[macros and definitions]=================================*/
/**
 * @def CONFIG_MEASURE_PERIOD
 * @brief Periodo en milisegundos cada cuál se ejecutarán las tareas MeasureTemperature() y ReadTime()
 * 
 */
#define CONFIG_MEASURE_PERIOD 10000

/**
 * @def CONFIG_CONTROL_PERIOD
 * @brief Periodo en milisegundos cada cuál se ejecutará la tarea DeviceControl()
 * 
 */
#define CONFIG_CONTROL_PERIOD 30000

/**
 * @def CONFIG_AIR_PERIOD
 * @brief Periodo en milisegundos cada cuál se ejecutará la tarea IRControl()
 * 
 */
#define CONFIG_AIR_PERIOD 120000

#define GPIO_PC GPIO_22
#define GPIO_UV GPIO_21
#define GPIO_ARD GPIO_20
#define GPIO_SERVO GPIO_19

/*==================[internal data definition]===============================*/
/**
 * @var uint16_t temperature
 * @brief Variable global en la cual se almacenará la última temperatura sensada
 */
uint16_t temperature = 0;

/**
 * @var bool UV
 * @brief Variable de control para encender/apagar la lámpara UV
 * 
 */
bool UV = true;

/**
 * @var bool PC
 * @brief Variable de control para encender/apagar la PC
 * 
 */
bool PC = true;

/**
 * @var bool PC
 * @brief Variable de control para encender/apagar el Aire Acondicionado
 * 
 */
bool AIR = false;

/**
 * @var bool today
 * @brief Variable de control que indica si el día de hoy hay atención en el consultorio o no
 * 
 */
bool today = false;

/**
 * @var bool now
 * @brief Variable de control que indica si nos encontramos en horario de atención o no
 * 
 */
bool now = false;

/**
 * @var int hours
 * @brief Indica la hora actual
 * 
 */
int hours;

/**
 * @var int minutes
 * @brief Indica los minutos actuales de la hora actual.
 * 
 */
int minutes;

/**
 * @var int hours_start
 * @brief Indica la hora de inicio de atención.
 * 
 */
int hours_start;

/**
 * @var int minutes_start
 * @brief Indica los minutos de la hora de inicio de atención.
 * 
 */
int minutes_start;

/**
 * @var int hours_end
 * @brief Indica la hora de finalización de atención.
 * 
 */
int hours_end;

/**
 * @var int minutes_end
 * @brief Indica los minutos de la hora de finalización de atención.
 * 
 */
int minutes_end;

/**
 * @var char day[3]
 * @brief Indica el día actual en formato Sun, Mon, Tue, Wed, Thu, Fri, Sat según corresponda
 * 
 */
char day[3];


uint16_t temperature_max = 0;
uint16_t temperature_min = 1000;
float temperature_prom = 0;
TaskHandle_t MeasureTemperature_task_handle = NULL;
TaskHandle_t DeviceControl_task_handle = NULL;
TaskHandle_t IRControl_task_handle = NULL;
TaskHandle_t ReadTime_task_handle = NULL;

/*==================[internal functions declaration]=========================*/

/**
 * @brief Tarea encargada de sensar la temperatura actual y calcular la temperatura máxima, la temperatura mínima y la temperatura promedio.
 * 		  Envía todos los datos a través de la UART y a la aplicación bluetooth. 
 * 		  Si la temperatura promedio es mayor a 23°C, habilita la variable de control que permite encender el Aire Acondicionado.
 */
void MeasureTemperature(){
	while (true){

	temperature = lm35cMeasureTemperature();
	UartSendString(UART_PC, (char *)UartItoa(temperature, 10));
	UartSendString(UART_PC, " °C ");
	UartSendString(UART_PC, "\r\n" );

	if (temperature>temperature_max){
			temperature_max = temperature;
		}

	if (temperature<temperature_min){
			temperature_min = temperature;
		}

	temperature_prom = (temperature_prom+temperature)/2;


	UartSendString(UART_PC, "Temperatura Promedio:");
	UartSendString(UART_PC, (char *)UartItoa(temperature_prom, 10));
	UartSendString(UART_PC, " °C ");
	UartSendString(UART_PC, "\r\n" );

	UartSendString(UART_PC, "Temperatura Minima:");
	UartSendString(UART_PC, (char *)UartItoa(temperature_min, 10));
	UartSendString(UART_PC, " °C ");
	UartSendString(UART_PC, "\r\n" );

	UartSendString(UART_PC, "Temperatura Maxima:");
	UartSendString(UART_PC, (char *)UartItoa(temperature_max, 10));
	UartSendString(UART_PC, " °C ");
	UartSendString(UART_PC, "\r\n" );

	char msg[30];
	sprintf(msg, "*T%u\n*", temperature); //Termómetro de temperatura actual
	BleSendString(msg);
	sprintf(msg, "*N%u\n*", temperature); //Temperatura actual
	BleSendString(msg);
	sprintf(msg, "*M%.2f\n*", temperature_prom); //Temperatura promedio
	BleSendString(msg);
	sprintf(msg, "*R%u\n*", temperature_max); //Temperatura maxima
	BleSendString(msg);
	sprintf(msg, "*L%u\n*", temperature_min); //Temperatura minima
	BleSendString(msg);

	if(temperature_prom>23){
		AIR = true;
	}

	else{
		AIR = false;
	}

	vTaskDelay(CONFIG_MEASURE_PERIOD / portTICK_PERIOD_MS);
}}

/**
 * @brief Tarea encargada de encender/apagar la PC y la lámpara UV dependiendo de si hoy hay o no atención, y si nos encontramos
 * 		  en horario de atención o no. Se indica por la UART si los dispositivos se encuentran encedidos o apagados.
 */
void DeviceControl(){
	while (true){

	if (now==false && today==true){
	if (UV){
		GPIOOn(GPIO_UV);
		UartSendString(UART_PC, "Lampara UV Apagada");
		UartSendString(UART_PC, "\r\n" );
	}

	else{
		GPIOOff(GPIO_UV);
		UartSendString(UART_PC, "Lampara UV Encendida");
		UartSendString(UART_PC, "\r\n" );
	}}

	if (today && now){
	if (PC){
		GPIOOn(GPIO_PC);
		UartSendString(UART_PC, "PC Encendida");
		UartSendString(UART_PC, "\r\n" );
	}

	else{
		GPIOOff(GPIO_PC);
		UartSendString(UART_PC, "PC Apagada");
		UartSendString(UART_PC, "\r\n" );
	}}

	vTaskDelay(CONFIG_CONTROL_PERIOD / portTICK_PERIOD_MS);

}}

/**
 * @brief Tarea encargada de, mediante un GPIO, indicarle al arduino nano si debe emitir la señal IR para encender
 * 		  o apagar el Aire Acondicionado. 
 */
void IRControl(){
	while (true){

	if (AIR){
		GPIOOn(GPIO_ARD);
	}

	else{
		GPIOOff(GPIO_ARD);
	}

	vTaskDelay(CONFIG_AIR_PERIOD / portTICK_PERIOD_MS);
}}

/**
 * @brief Tarea que lee el día y la hora y minutos actuales. Indica si nos encontramos
 * 		  en horario de atención o no, de acuerdo a los datos ingresados en la aplicación bluetooth.
 * 		  Envía a la aplicación bluetooth si el consultorio actualmente se encuentra disponible o desocupado.
 * 		  En caso de encontrarse ocupado, también envía la hora de finalización. En caso de encontrarse disponible, 
 * 		  envía la hora de inicio y de finalización del próximo turno.
 */
void ReadTime(){
	while (true){

	char msg[50];	
	//Extraigo la hora y minutos
	time_t now = time(NULL);
  	struct tm *local = localtime(&now);

    hours = (local->tm_hour)-3; //Calcula la hora según el uso horario de mi sistema, debo restarle 3 para ajustarlo a la hora Argentina
    minutes = local->tm_min;

	//Extraigo el día de la semana en formato Sun, Mon, Tue, Wed, Thu, Fri, Sat segun corresponda
	for (int i = 0; i<4; i++){
        day[i]= ctime(&now)[i];
    }


	if (hours_start <= hours && minutes_start <= minutes){

		now = true;

		sprintf(msg, "*G%u\n*", 1); //Barra disponibilidad
		BleSendString(msg);
		sprintf(msg, "*D%d\n", 'Ocu'); //Mensaje disponibilidad. Consultorio Ocupado
		BleSendString(msg);
	}

	if(hours_end <= hours && minutes_end <= minutes){

		now = false;

		sprintf(msg, "*G%u\n*", 0); //Barra disponibilidad
		BleSendString(msg);
		sprintf(msg, "*D%d\n", 'Dis'); //Mensaje disponibilidad. Consultorio Disponible
		BleSendString(msg);
	}

	if(now){
		sprintf(msg, "*P%u\n*", hours_end); //Hora de finalización
		BleSendString(msg);
		sprintf(msg, "*Q%u\n*", minutes_end); //Minutos de finalización
		BleSendString(msg);}

	else{
		sprintf(msg, "*F%u\n*", hours_start); //Hora de inicio del próximo turno
		BleSendString(msg);
		sprintf(msg, "*J%u\n*", minutes_start); //Minutos de inicio del próximo turno
		BleSendString(msg);

		sprintf(msg, "*P%u\n*", hours_end); //Hora de finalización del próximo turno
		BleSendString(msg);
		sprintf(msg, "*Q%u\n*", minutes_end); //Minutos de finalización del próximo turno
		BleSendString(msg);
	}

	vTaskDelay(CONFIG_MEASURE_PERIOD / portTICK_PERIOD_MS);
	
}}

/**
 * @brief Tarea que se ejecuta al recibir datos a través de la aplicación bluetooth. 
 * 		  Determina si hoy hay atención o no, dependiendo del día actual y los datos obtenidos.
 * 
 * @param data Datos enviados desde la aplicación
 */
void ReadApp(uint8_t * data, uint8_t length){
	
	uint8_t i = 1;
	
	//Recibo los días de atención en formato Sun, Mon, Tue, Wed, Thu, Fri, Sat. Si coincide con el día actual, significa que hoy habrá atención
	if(data[0]==day[0] && data[1]==day[1]){
		today=true;
	}
	
	else{
		today=false;
	}

	if(data[0]=='I'){
		hours_start = 0;
		while(data[i] != 'I'){
			hours_start = hours_start * 10;
			hours_start = hours_start + (data[i] - '0');
			i++;
		}
	}

	if(data[0]=='S'){
		minutes_start = 0;
		while(data[i] != 'S'){
			minutes_start = minutes_start * 10;
			minutes_start = minutes_start + (data[i] - '0');
			i++;
		}
	}

	if(data[0]=='F'){
		hours_end = 0;
		while(data[i] != 'F'){
			hours_end = hours_end * 10;
			hours_end = hours_end + (data[i] - '0');
			i++;
		}
	}

	if(data[0]=='F'){
		minutes_end = 0;
		while(data[i] != 'F'){
			minutes_end = minutes_end * 10;
			minutes_end = minutes_end + (data[i] - '0');
			i++;
		}
	}
}

/**
 * @brief Tarea que se ejecuta al recibir caracteres a través de la UART. Se utiliza para corroborar funcionamiento de
 * 		  de los dispositivos.
 */
void ReadUart(){

	uint8_t lectura; 
	UartReadByte(UART_PC, &lectura);

	if (lectura == 'U'){
		UV = !UV;
	}

	if (lectura == 'P'){
		PC = !PC;
	}

	if (lectura == 'A'){
		AIR = !AIR;
	}

}

/*==================[external functions definition]==========================*/
void app_main(void){

	lm35cInit(CH1); //Sensor de temperatura

	GPIOInit(GPIO_PC, GPIO_OUTPUT); //GPIO que activará el relé que encederá la Lampara UV
	GPIOInit(GPIO_UV, GPIO_OUTPUT); //GPIO que activará el relé que encenderá la PC
	GPIOInit(GPIO_ARD, GPIO_OUTPUT); //GPIO que activará el Arduino Nano que controla el Aire Acondicionado

	ble_config_t ble_configuration = {
        "CONSULTORIO_1",
        ReadApp
    };
	
	serial_config_t serial_pc ={
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = ReadUart,
		.param_p = NULL,
	};

	UartInit(&serial_pc);

	BleInit(&ble_configuration);

	xTaskCreate(&MeasureTemperature, "Medir Temperatura", 2048, NULL, 5, &MeasureTemperature_task_handle);
	xTaskCreate(&DeviceControl, "Controla la PC y la lampara UV", 2048, NULL, 5, &DeviceControl_task_handle);
	xTaskCreate(&IRControl, "Controla el Aire Acondicionado", 2048, NULL, 5, &IRControl_task_handle);
	xTaskCreate(&ReadTime, "Lee el día de hoy, hora y minutos actuales", 2048, NULL, 5, &ReadTime_task_handle);

	
}
/*==================[end of file]============================================*/
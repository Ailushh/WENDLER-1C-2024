/*! @mainpage Proyecto Integrador
 *
 * @section genDesc General Description
 *
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
#include <led.h>
#include "servo_sg90.h"
/*==================[macros and definitions]=================================*/

#define CONFIG_MEASURE_PERIOD 10000
#define CONFIG_CONTROL_PERIOD 10000
#define CONFIG_AIR_PERIOD	120000
#define GPIO_PC GPIO_22
#define GPIO_UV GPIO_21
#define GPIO_ARD GPIO_20
#define GPIO_SERVO GPIO_19

/*==================[internal data definition]===============================*/

uint16_t temperature = 0;
bool UV = true;
bool PC = true;
bool AIR = false;
bool today = false;
bool now = false;
int hours, minutes;
int hours_start;
int minutes_start;
int hours_end;
int minutes_end;
char day[3];
uint16_t temperature_max = 0;
uint16_t temperature_min = 1000;
float temperature_prom = 0;
TaskHandle_t MeasureTemperature_task_handle = NULL;
TaskHandle_t DeviceControl_task_handle = NULL;
TaskHandle_t IRControl_task_handle = NULL;
TaskHandle_t ReadTime_task_handle = NULL;

/*==================[internal functions declaration]=========================*/


static void MeasureTemperature(){
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

static void DeviceControl(){
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

static void IRControl(){
	while (true){

	if (AIR){
		GPIOOn(GPIO_ARD);
	}

	else{
		GPIOOff(GPIO_ARD);
	}

	vTaskDelay(CONFIG_AIR_PERIOD / portTICK_PERIOD_MS);
}}


static void ReadTime(){
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


	if (hours_start == hours && minutes_start == minutes){

		now = true;

		sprintf(msg, "*G%u\n*", 1); //Barra disponibilidad
		BleSendString(msg);
		sprintf(msg, "*D%u\n*", 'Ocu'); //Mensaje disponibilidad. Consultorio Ocupado
		BleSendString(msg);
	}

	if(hours_end == hours && minutes_end == minutes){

		now = false;

		sprintf(msg, "*G%u\n*", 0); //Barra disponibilidad
		BleSendString(msg);
		sprintf(msg, "*D%u\n*", 'Dis'); //Mensaje disponibilidad. Consultorio Disponible
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

void ReadApp(uint8_t * data, uint8_t length){
	
	uint8_t i = 1;
	
	//Recibo los días de atención en formato Sun, Mon, Tue, Wed, Thu, Fri, Sat. Si coincide con el día actual, hoy habrá atención
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
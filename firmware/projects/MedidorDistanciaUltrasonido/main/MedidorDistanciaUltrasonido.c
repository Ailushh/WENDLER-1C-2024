/*! @mainpage Medidor de Distancia por Ultrasonido
 *
 * @section genDesc General Description
 *
 * El programa permite medir la distancia a través de un sensor de ultrasonido HC-SR04 y mostrarla por pantalla en cm. 
 * Con el switch 1 se inicia/detiene la medición. 
 * Los leds se irán encendiendo/apagando a medida que aumenta/disminuye la distancia medida.
 * Con el switch 2 se congela la medida actual en el LCD.
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	LCD 	 	| 	GPIO_19     |
 * |				|	GPIO_18		|
 * |				|	GPIO_9		|
 * |	HC-SR04		|	GPIO_03		|
 * |				|	GPIO_02		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 04/04/2024 | Creación del Documento		                 |
 * | 11/04/2024 | Finalización y documentación					 |
 *
 * @author Tatiana Ailen Wendler (ailuwendler@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <led.h>
#include <switch.h>
#include <hc_sr04.h>
#include <lcditse0803.h>
/*==================[macros and definitions]=================================*/
#define CONFIG_MEASURE_PERIOD 1000
#define CONFIG_LED_PERIOD 1000
#define CONFIG_LCD_PERIOD 1000
#define CONFIG_SWITCH_PERIOD 200
/*==================[internal data definition]===============================*/
bool medir_distancia = false;
bool hold_medicion = false;
uint16_t distancia = 0;
TaskHandle_t MedirDistancia_task_handle = NULL;
TaskHandle_t EncenderLedsSegunDistancia_task_handle = NULL;
TaskHandle_t MostrarDistanciaLCD_task_handle = NULL;
TaskHandle_t LeerSwitches_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

/**
 * @brief Función que permite obtener la distancia detectada por el sensor en cm, con una taza
 * 		  de actualización de 1s.
*/
static void MedirDistancia (void *pvParameter){

 while(true){

	if (medir_distancia){

	distancia = HcSr04ReadDistanceInCentimeters();}

	vTaskDelay(CONFIG_MEASURE_PERIOD / portTICK_PERIOD_MS);
	}

}
/**
 * @brief Función que enciende los leds de acuerdo a la distancia medida.
 * 		  Si la distancia<10cm, se apagan todos los leds.
 * 		  Si 10<=distancia<20, se enciende el Led 1.
 * 		  Si 20<=distancia<30, se encienden los leds 1 y 2.
 * 		  Si distancia=>30, se encienden todos los leds.
*/
static void EncenderLedsSegunDistancia (void *pvParameter){

 while(true){

	if (medir_distancia){

		if (distancia<10){
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}

		else if (10<=distancia && distancia<20){
			LedOn(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}

		else if(20<=distancia && distancia<30){
			LedOn(LED_1);
			LedOn(LED_2);
			LedOff(LED_3);
		}

		else {
			LedOn(LED_1);
			LedOn(LED_2);
			LedOn(LED_3);
		}}

	else{
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}

	vTaskDelay(CONFIG_LED_PERIOD / portTICK_PERIOD_MS);

}}

/**
 * @brief Función que permite mostrar la distancia en cm por el LCD. Puede mostrar la distancia medida
 * 		  en tiempo real o holdear la distancia actual.
*/

void static MostrarDistanciaLCD (void *pvParameter){

 while(true){
	
	if(medir_distancia){

		if (!hold_medicion){

			LcdItsE0803Write(distancia);
	}}

	else{ 
		LcdItsE0803Write(0);
	}

	vTaskDelay(CONFIG_LCD_PERIOD / portTICK_PERIOD_MS);
}}

/**
 * @brief Función que permite detectar si se activa el Switch 1 o Switch 2.
 * 		  Para el caso del Switch 1 modifica un booleano que inicia/finaliza la medición.
 * 		  Para el caso del Switch 2 modifica un booleano que holdea la medición actual en el LCD.
*/

void static LeerSwitches (void *pvParameter){

 while(true){

	uint8_t teclas = SwitchesRead();

	switch (teclas)
	{
		case SWITCH_1:
			medir_distancia = !medir_distancia;
		break;
	
		case SWITCH_2:
			hold_medicion = !hold_medicion;
		break;
	}

	vTaskDelay(CONFIG_SWITCH_PERIOD / portTICK_PERIOD_MS); 

}}

/*==================[external functions definition]==========================*/
void app_main(void){

	LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();

	xTaskCreate(&LeerSwitches, "Leer_Teclas", 512, NULL, 5, &LeerSwitches_task_handle);
	xTaskCreate(&MedirDistancia, "Medir_Distancia", 512, NULL, 5, &MedirDistancia_task_handle);
	xTaskCreate(&EncenderLedsSegunDistancia, "Encender/Apagar_Leds", 512, NULL, 5, &EncenderLedsSegunDistancia_task_handle);
	xTaskCreate(&MostrarDistanciaLCD, "Mostrar_distancia_por_LCD", 512, NULL, 5, &MostrarDistanciaLCD_task_handle);


}
/*==================[end of file]============================================*/
/*! @mainpage Guia 1 - Ejercicio 6
 *
 * @section genDesc General Description
 *
 * El programa permite mostrar datos numéricos a través de una pantalla LCD.
 *
 * 
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	LCD 	 	| 	GPIO_19     |
 * |				|	GPIO_18		|
 * |				|	GPIO_9		|
 * |	BCD INPUT	|	GPIO_20		|
 * |				|	GPIO_21		|
 * |				|	GPIO_22		|
 * |				|	GPIO_23		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 21/03/2024 | Creación del documento	                     |
 * | 03/04/2024 | Finalización y documentación					 |
 *
 * @author Tatiana Ailen Wendler (ailuwendler@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <gpio_mcu.h>

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/*==================[internal functions declaration]=========================*/

/**
 @brief Función que permite visualizar por el display de LCD un número elegido.
 @param data Número decimal de 32 bits.
 @param digits Cantidad de dígitos del número decimal.
 @param ptr_bits Puntero a un vector de struct de gpioConf_t que almacena los pines correspondientes a los bits de
				cada cada dígito a mostrar.
 @param ptr_lcd Puntero a un vector de struct de gpioConf_t que almacena los pines correspondientes a cada 7 segmentos
				del lcd.
 * 
*/
void MostrarDisplay (uint32_t data, uint8_t digits, gpioConf_t *ptr_bits, gpioConf_t *ptr_lcd ){
	
	for(int i = 0; i<4; i++){
		GPIOInit(ptr_bits[i].pin, ptr_bits[i].dir);}

	for(int i = 0; i<3; i++){
		GPIOInit(ptr_lcd[i].pin, ptr_lcd[i].dir);}

	uint8_t digito;
	uint8_t bcd[digits];

	for (int i=(digits-1); i>=0; i--){
		digito = data%10;
		bcd[i] = digito;
		data = data/10;
	}

	for (int j = 0; j<3; j++){

		GPIOOff(ptr_lcd[j].pin);
		GPIOOn(ptr_lcd[j].pin);

		for (int i = 0; i<4; i++){
		
			if( (bcd[j] & (1<<i)) == 0){
			GPIOOff(ptr_bits[i].pin);

			}
			else{
			GPIOOn(ptr_bits[i].pin);
			}
		}

		GPIOOff(ptr_lcd[j].pin);

		}

}
/*==================[external functions definition]==========================*/
void app_main(void){

	gpioConf_t bits[4] = {{GPIO_20,GPIO_OUTPUT},{GPIO_21,GPIO_OUTPUT},{GPIO_22,GPIO_OUTPUT},{GPIO_23,GPIO_OUTPUT}};
	gpioConf_t LCD[3] = {{GPIO_19,GPIO_OUTPUT},{GPIO_18,GPIO_OUTPUT},{GPIO_9,GPIO_OUTPUT}};

	uint32_t numero = 345;
	uint8_t digitos = 3;

	MostrarDisplay(numero, digitos, bits, LCD);

}
/*==================[end of file]============================================*/
/*! @mainpage Guia 1 - Ejercicio 4
 *
 * @section genDesc General Description
 *
 * Programa que recibe un número y lo convierte en BCD, agregando cada dígito dentro de un arreglo.
 *
 * 
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 21/03/2024 | Document creation		                         |
 *
 * @author Ailén Wendler (ailuwendler@gmail.com.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
void convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	// Extraígo cada dígito del dato por separado y lo agrego en el vector

	uint8_t digito;

	for (int i=(digits-1); i>=0; i--){
		digito = data%10;
		bcd_number[i] = digito;
		data = data/10;
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){

	uint8_t bcd[3];

	uint32_t numero = 123;
	
	uint8_t digitos = 3;

	convertToBcdArray(numero, digitos, bcd);

	printf("Muestro por consola: ");
	printf("%d",bcd[0]);
	printf("%d",bcd[1]);
	printf("%d",bcd[2]);
	printf("\n");

}
/*==================[end of file]============================================*/
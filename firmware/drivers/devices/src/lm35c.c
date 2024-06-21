/**
 * @file lm35c.c
 * @author Wendler Tatiana Ailen (ailuwendler@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include "lm35c.h"
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
analog_input_config_t temp_config;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
bool lm35cInit(adc_ch_t canal){
	

	temp_config.input = canal;
	temp_config.mode = ADC_SINGLE;

	AnalogInputInit(&temp_config);  

	return true;
}

float lm35cMeasureTemperature(void){

	uint16_t value;
	uint16_t temperature = 0;
	
	AnalogInputReadSingle(temp_config.input, &value);
	temperature = value/10;
	
	return temperature;
}
/*==================[end of file]============================================*/

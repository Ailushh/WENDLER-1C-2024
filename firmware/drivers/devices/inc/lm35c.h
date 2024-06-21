#ifndef LM35C_H
#define LM35C_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup Sensor_t Sensor de Temperatura
 ** @{ */

/** \brief Sensor_t driver for the ESP-EDU Board.
 *
 * 
 * @author Wendler Tatiana Ailen
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 23/05/2023 | Creación del Documento                        						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>
#include "gpio_mcu.h"
#include "analog_io_mcu.h"
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @fn bool lm35cInit(adc_ch_t canal);
 * @brief Initializes lmc5c driver
 * @param[in] canal 
 * @return TRUE if no error 
 */
bool lm35cInit(adc_ch_t canal);

/** @fn lm35cMeasureTemperature(void);
 * @brief Measures the current temperature
 * @return value of temperature in °C 
 */
float lm35cMeasureTemperature(void);

#endif /* #ifndef LM35C_H */

/*==================[end of file]============================================*/


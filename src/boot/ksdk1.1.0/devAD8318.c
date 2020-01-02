#include <stdint.h>

#include "fsl_spi_master_driver.h"
#include "fsl_port_hal.h"

#include "SEGGER_RTT.h"
#include "gpio_pins.h"
#include "warp.h"
#include "devAD8318.h"
// For use of "writeText"
#include "devSSD1331.h"

volatile uint8_t	inBuffer[1];


/*
 *	Override Warp firmware's use of these pins and define new aliases.
 */
enum
{
	kAD8318Pin	= GPIO_MAKE_PIN(HW_GPIOB, 0),
};





int
devAD8318init(void)
{
	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Using PTB0 for ADC measurement
	 */

	PORT_HAL_SetMuxMode(PORTB_BASE, 0, kPortPinDisabled);
	writeText("Setup PTB0 as ADC");


	return 0;
}

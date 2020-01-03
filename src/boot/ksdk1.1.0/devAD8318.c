#include <stdint.h>

#include "fsl_port_hal.h"
#include "fsl_adc16_driver.h"

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

// Function copied from Kinetis SDK 1.1 API Reference Manual
void ADC16_TEST_OneTimeTrigger(uint32_t instance, uint32_t chnGroup, uint8_t chn)
{
#if FSL_FEATURE_ADC16_HAS_CALIBRATION
	// The MKL03Z4 does have calibration and this is defined in line 85 of:
	// Warp-firmware/tools/sdk/ksdk1.1.0/platform/CMSIS/Include/device/MKL03Z4/MKL03Z4_features.h
	// Also have hardware averaging, 12 bit max resolution
	adc16_calibration_param_t MyAdcCalibraitionParam;
#endif // FSL_FEATURE_ADC16_HAS_CALIBRATION //

	adc16_user_config_t MyAdcUserConfig;
	adc16_chn_config_t MyChnConfig;
	int32_t MyAdcValue;
	uint32_t i;

#if FSL_FEATURE_ADC16_HAS_CALIBRATION
	// Auto calibration. //
	ADC16_DRV_GetAutoCalibrationParam(instance, &MyAdcCalibraitionParam);
	ADC16_DRV_SetCalibrationParam(instance, &MyAdcCalibraitionParam);
#endif // FSL_FEATURE_ADC16_HAS_CALIBRATION //

	// Initialize the ADC converter. //
	ADC16_DRV_StructInitUserConfigDefault(&MyAdcUserConfig);
	ADC16_DRV_Init(instance, &MyAdcUserConfig);

	// Configuration for ADC channel. //
	MyChnConfig.chnNum = chn;
	MyChnConfig.diffEnable= false;
	MyChnConfig.intEnable = false;

	#if FSL_FEATURE_ADC16_HAS_MUX_SELECT
	MyChnConfig.chnMux = kAdcChnMuxOfA;
	#endif // FSL_FEATURE_ADC16_HAS_MUX_SELECT //

	for (i = 0U; i < 4U; i++)
	{
		// Trigger the conversion with indicated channel’s configuration. //
		ADC16_DRV_ConfigConvChn(instance, chnGroup, &MyChnConfig);
		// Wait for the conversion to be done. //
		ADC16_DRV_WaitConvDone(instance, chnGroup);
		// Fetch the conversion value and format it. //
		MyAdcValue = ADC16_DRV_GetConvValueRAW(instance, chnGroup);
		// ADC16_DRV_ConvRAWData(MyAdcValue, false, kAdcResolutionBitOfSingleEndAs12) );
	}
	// Pause the conversion after testing. //
	ADC16_DRV_PauseConv(instance, chnGroup);
	// Disable the ADC. //
	ADC16_DRV_Deinit(instance);
}


int
devAD8318init(void)
{
	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Using PTB0 for ADC measurement
	 */

	PORT_HAL_SetMuxMode(PORTB_BASE, 0, kPortPinDisabled);
	writeText("Setup PTB0 as ADC\n");
	writeText("Running ADC Test:\n");
	ADC16_TEST_OneTimeTrigger();
	writeText("Run Sucessfully\n");

	return 0;
}

// typedef enum
// {
// 	kAD8318ColororderRGB		= 1,
// 	kAD8318DelaysHWFILL		= 3,
// 	kAD8318DelaysHWLINE		= 1,
// } AD8318Constants;
#ifndef WARP_BUILD_ENABLE_DEVAD8318
#define WARP_BUILD_ENABLE_DEVAD8318
#endif
// #ifndef __ADC16_TEST_H__
// #define __ADC16_TEST_H__
// #include
// #include
// #include
// #include
// #include
// #include
// <stdio.h>
// <stdbool.h>
// <stdint.h>
// "board.h"
// "fsl_os_abstraction.h"
// "fsl_debug_console.h"
// #include "fsl_adc16_driver.h"
// #define ADC16_TEST_CHN_TEMPERATURE
// #define ADC16_TEST_CHN_GROUT_NUM (26U) // Temperature Sensor. //
// (0U)
// #define ADC16_TEST_LOOP_CYCLE (4U)

// int32_t ADC16_TEST_OneTimeTrigger(uint32_t instance, uint32_t chnGroup, uint8_t chn);
// int32_t readADC();
void printSensorDataAD8318(bool hexModeFlag);
// #endif // __ADC16_TEST_H__ //

void printPowerToScreen(void);
int	devAD8318init(void);

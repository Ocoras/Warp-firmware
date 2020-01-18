## Overview

This fork of the WARP Firmware implements the use of an AD8318 RF Logarithmic Power Measurement IC connected to PTB0. Although not added to the WARP menu (due to lacking SPI or I2C interfaces and thus there being few configuration options), the raw ADC reading will be printed when the function `printAllSensors` is called.


My instantiation of the `adc_16` driver is taken from the KSDK API Manual, the Font implementation from the Adafruit GFX library (although adjusted not to be object-oriented as it would not fit the design paradigm for WARP), and the float-to-string conversion from geeksforgeeks
<!-- Insert URLS above ! -->

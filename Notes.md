## Overview

For my 4B25 project I am implementing the use of an AD8308 RF Logarithmic Power Measurement IC to measure the power in the RFID frequency range of the RF spectrum.

From a software perspective, I aim to accomplish this by reading a voltage from the ADC, converting the value into a decibel power reading (using conversion values provided by the datasheet), performing some signal processing such as averaging, and showing a final value on the commandline and the screen.

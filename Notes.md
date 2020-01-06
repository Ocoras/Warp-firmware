## Overview

For my 4B25 project I am implementing the use of an AD8318 RF Logarithmic Power Measurement IC to measure the power in the RFID frequency range of the RF spectrum.

From a software perspective, I aim to accomplish this by reading a voltage from the ADC, converting the value into a decibel power reading (using conversion values provided by the datasheet), performing some signal processing such as averaging, and showing a final value on the commandline and the screen.


Floating voltage = 2.93V on ADC, gives a value of 4060 ish
2^12 = 4096 Is the maximum value of the ADC, thus this means our floating voltage is close to the supply rails of 3.3V (for the processor), thus we can only really measure up to 3V-ish maximum.

Thankfully, the output of the measurement chip has been measured to have a voltage between 0.81 and 2.02 in our region of interest, thus the maximum of the ADC will be unlikely to be reached. In brief testing I have found the value of the ADC to remain in 1700-2000 region when connected to a standard radio aerial. This corresponds to a minimum voltage of 1.3V, approximately -30dBm.

It is key to make sure that the RF chip and the board have a common ground, in testing I have accomplished this by connecting the - side of the battery to GND on the development board, however when a 9V power supply clip is acquired, we can accomplish this through powering the whole board via the VIN pin at 9V, connecting our sensor power supply to the same rail. This 9V supply is regulated to 5V on the measurement board, and to 3.3V on the development board.

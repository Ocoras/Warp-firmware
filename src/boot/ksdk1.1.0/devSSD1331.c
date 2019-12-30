#include <stdint.h>

#include "fsl_spi_master_driver.h"
#include "fsl_port_hal.h"

#include "SEGGER_RTT.h"
#include "gpio_pins.h"
#include "warp.h"
#include "devSSD1331.h"

// #include "gfxfont.h"
#include "devSSD1331_font.h"

volatile uint8_t	inBuffer[1];
volatile uint8_t	payloadBytes[1];

/*
 *	Override Warp firmware's use of these pins and define new aliases.
 */
enum
{
	kSSD1331PinMOSI		= GPIO_MAKE_PIN(HW_GPIOA, 8),
	kSSD1331PinSCK		= GPIO_MAKE_PIN(HW_GPIOA, 9),
	kSSD1331PinCSn		= GPIO_MAKE_PIN(HW_GPIOB, 13),
	kSSD1331PinDC		= GPIO_MAKE_PIN(HW_GPIOA, 12),
	kSSD1331PinRST		= GPIO_MAKE_PIN(HW_GPIOB, 0),
};


uint8_t width = 0x5F;
uint8_t height = 0x3F;
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;
uint8_t textsize_x = 1;
uint8_t textsize_y = 1;
bool wrap = true;

// Colours stored in arrays
uint8_t textcolor[3];
uint8_t textbg[3];






static int
writeCommand(uint8_t commandByte)
{
	spi_status_t status;

	/*
	 *	Drive /CS low.
	 *
	 *	Make sure there is a high-to-low transition by first driving high, delay, then drive low.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	OSA_TimeDelay(10);
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	/*
	 *	Drive DC low (command).
	 */
	GPIO_DRV_ClearPinOutput(kSSD1331PinDC);

	payloadBytes[0] = commandByte;
	status = SPI_DRV_MasterTransferBlocking(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)&payloadBytes[0],
					(uint8_t * restrict)&inBuffer[0],
					1		/* transfer size */,
					1000		/* timeout in microseconds (unlike I2C which is ms) */);

	/*
	 *	Drive /CS high
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);

	return status;
}



// static const uint8_t WIDTH = 0x5F;
// static const uint8_t HIEGHT = 0x3F;

void
drawLine(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y, uint8_t * colours){
	uint8_t red = colours[0];
	uint8_t green = colours[1];
	uint8_t blue = colours[2];
	writeCommand(kSSD1331CommandDRAWLINE);
	writeCommand(start_x);
	writeCommand(start_y);
	writeCommand(end_x);
	writeCommand(end_y);
	writeCommand(blue);
	writeCommand(green);
	writeCommand(red);

}

void
drawRect(uint8_t start_x, uint8_t start_y, uint8_t width_x, uint8_t width_y, uint8_t * colours) {
	uint8_t red = colours[0];
	uint8_t green = colours[1];
	uint8_t blue = colours[2];

	writeCommand(kSSD1331CommandDRAWRECT);
	writeCommand(start_x);
	writeCommand(start_y);
	writeCommand(start_x + width_x);
	writeCommand(start_y + width_y);
	// Border - assume same as rect.
	writeCommand(blue);
	writeCommand(green);
	writeCommand(red);
	// Rect
	writeCommand(blue);
	writeCommand(green);
	writeCommand(red);

}



// Implementing writing to screen based on implementation in Adafruit_GFX Library:
void
drawChar(uint8_t x, uint8_t y, uint16_t c, uint8_t * colour, uint8_t * bg, uint8_t size_x, uint8_t size_y){

	if((x >= width)            || // Clip right - Assuming horizontal screen
		 (y >= height)           || // Clip bottom
		 ((x + 6 * size_x - 1) < 0) || // Clip left
		 ((y + 8 * size_y - 1) < 0))   // Clip top
			return;

	for(int8_t i=0; i<5; i++ ) { // Char bitmap = 5 columns
			// ToDo : Store font in program memory & implement the line reader function from the program memory:
			uint8_t line = font[c * 5 + i];
			for(int8_t j=0; j<8; j++, line >>= 1) {
					if(line & 1) {
						if (size_x ==1 && size_y==1){
							drawLine(x+i, y+j,x+i,y+j,colour);
						} else{
							drawRect(x+i*size_x, y+j*size_y, size_x, size_y, colour);
						}
					} //else if(bg != colour) {
						//drawRect(x+i*size_x, y+j*size_y, size_x, size_y, bg);
					//}
			}
			// Commenting out all background colour lines to speed up writing
			// if(bg != colour) { // If opaque, draw vertical line for last column
			// 		drawRect(x+5*size_x, y, size_x, 8*size_y, bg);
			// }
	}
}


void
writeText(uint8_t * txt) {
	for (uint8_t i = 0; txt[i]; i++){
		uint8_t c = txt[i];
		if (c=='\n'){
			cursor_x = 0;
			cursor_y += textsize_y*8;
		} else if(c!= '\r') {                 // Ignore carriage returns
				if(wrap && ((cursor_x + textsize_x * 6) > width)) { // Off right?
						cursor_x  = 0;                 // Reset x to zero,
						cursor_y += textsize_y * 8;    // advance y one line
				}
				drawChar(cursor_x, cursor_y, c, textcolor, textbg, textsize_x, textsize_y);
				cursor_x += textsize_x * 6;          // Advance x one char
		}
	}

}





int
devSSD1331init(void)
{
	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Re-configure SPI to be on PTA8 and PTA9 for MOSI and SCK respectively.
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 8u, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 9u, kPortMuxAlt3);

	enableSPIpins();

	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Reconfigure to use as GPIO.
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 13u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTA_BASE, 12u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTB_BASE, 0u, kPortMuxAsGpio);


	/*
	 *	RST high->low->high.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);
	GPIO_DRV_ClearPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);

	/*
	 *	Initialization sequence, borrowed from https://github.com/adafruit/Adafruit-SSD1331-OLED-Driver-Library-for-Arduino
	 */
	writeCommand(kSSD1331CommandDISPLAYOFF);	// 0xAE
	writeCommand(kSSD1331CommandSETREMAP);		// 0xA0
	writeCommand(0x72);				// RGB Color
	writeCommand(kSSD1331CommandSTARTLINE);		// 0xA1
	writeCommand(0x0);
	writeCommand(kSSD1331CommandDISPLAYOFFSET);	// 0xA2
	writeCommand(0x0);
	writeCommand(kSSD1331CommandNORMALDISPLAY);	// 0xA4
	writeCommand(kSSD1331CommandSETMULTIPLEX);	// 0xA8
	writeCommand(0x3F);				// 0x3F 1/64 duty
	writeCommand(kSSD1331CommandSETMASTER);		// 0xAD
	writeCommand(0x8E);
	writeCommand(kSSD1331CommandPOWERMODE);		// 0xB0
	writeCommand(0x0B);
	writeCommand(kSSD1331CommandPRECHARGE);		// 0xB1
	writeCommand(0x31);
	writeCommand(kSSD1331CommandCLOCKDIV);		// 0xB3
	writeCommand(0xF0);				// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
	writeCommand(kSSD1331CommandPRECHARGEA);	// 0x8A
	writeCommand(0xFF);
	// writeCommand(0x64);
	writeCommand(kSSD1331CommandPRECHARGEB);	// 0x8B
	writeCommand(0xFF);
	// writeCommand(0x78);
	writeCommand(kSSD1331CommandPRECHARGEA);	// 0x8C
	writeCommand(0xFF);
	// writeCommand(0x64);
	writeCommand(kSSD1331CommandPRECHARGELEVEL);	// 0xBB
	writeCommand(0x3E);
	writeCommand(kSSD1331CommandVCOMH);		// 0xBE
	writeCommand(0x3E);
	writeCommand(kSSD1331CommandMASTERCURRENT);	// 0x87
	// Making the master current to the top value:
	// writeCommand(0x06);
	writeCommand(0x0F);
	writeCommand(kSSD1331CommandCONTRASTA);		// 0x81
	// Setting all contrasts to maximum 80h
	// writeCommand(0x91);
	writeCommand(0xFF);
	writeCommand(kSSD1331CommandCONTRASTB);		// 0x82
	// writeCommand(0x50);
	writeCommand(0xFF);
	writeCommand(kSSD1331CommandCONTRASTC);		// 0x83
	// writeCommand(0x7D);
	writeCommand(0xFF);
	writeCommand(kSSD1331CommandDISPLAYON);		// Turn on oled panel

	/*
	 *	To use fill commands, you will have to issue a command to the display to enable them. See the manual.
	 */
	writeCommand(kSSD1331CommandFILL);
	writeCommand(0x11);

	/*
	 *	Clear Screen
	 */
	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(0x00);
	writeCommand(0x00);
	writeCommand(0x5F);
	writeCommand(0x3F);

		/*
		 *	Any post-initialization drawing commands go here.
		 */
// Three sub-pixels for color A, B and C have 6 bits, 5 bits and 6 bits respectively.
// 00h to 5Fh for columns, 00h to 3Fh for rows
	// writeCommand(kSSD1331CommandDRAWRECT);
	// writeCommand(0x00);
	// writeCommand(0x00);
	// writeCommand(0x5F);
	// writeCommand(0x3F);
	// writeCommand(0x00);
	// writeCommand(0x3E);
	// writeCommand(0x00);
	// writeCommand(0x00);
	// writeCommand(0x3E);
	// writeCommand(0x00);

	//...

	textcolor[0] = 0x3E ;
	textcolor[1] = 0x3E ;
	textcolor[2] = 0x3E ;
	textbg[0] = 0x00;
	textbg[1] = 0x00;
	textbg[2] = 0x00;

	unsigned char text[] = "kill\n";
	writeText(text);
	textsize_x=2;
	textsize_y=2;
	unsigned char text2[] = "me\n";
	writeText(text2);

	textsize_x=3;
	textsize_y=3;
	unsigned char text3[] = "now";
	writeText(text3);
	// textcolor[0] = 0x3E ;
	// textcolor[1] = 0x3E ;
	// textcolor[2] = 0x00 ;
	// writeText('m');
	// textcolor[0] = 0x30 ;
	// textcolor[1] = 0x30 ;
	// textcolor[2] = 0x30 ;
	// writeText('s');


	return 0;
}

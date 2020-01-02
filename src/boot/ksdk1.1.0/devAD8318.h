/*
 *	See https://github.com/adafruit/Adafruit-SSD1331-OLED-Driver-Library-for-Arduino for the Arduino driver.
 */

// typedef enum
// {
// 	kAD8318ColororderRGB		= 1,
// 	kAD8318DelaysHWFILL		= 3,
// 	kAD8318DelaysHWLINE		= 1,
// } AD8318Constants;

int	devAD8318init(void);
// void writeText(uint8_t c);

//
// class SSD1331 {
// private:
// 	uint8_t width;
// 	uint8_t height;
// 	uint8_t cursor_x;
// 	uint8_t cursor_y;
//
// 	int writeCommand(uint8_t commandByte);
// 	void drawRect(uint8_t start_x, uint8_t start_y, uint8_t width_x, uint8_t width_y, uint8_t * color_pointer);
// 	void drawChar(uint8_t x, uint8_t y, uint16_t c, uint8_t * colour, uint8_t * bg, uint8_t size_x, uint8_t size_y);
//
//
// public:
// 	void writeText(uint8_t c);
// 	int init(void);
// 	uint8_t textsize_x;
// 	uint8_t textsize_y;
// 	uint8_t textcolor[3];
// 	uint8_t textbg[3];
// 	bool wrap;
//
// 	Screen(uint8_t w,	uint8_t h){
// 		width = w;
// 		height = h;
// 		cursor_x = 0;
// 		cursor_y = 0;
// 		textsize_x = 1;
// 		textsize_y = 1;
// 		wrap = true;
// 		textcolor[0] = 0x00 ;
// 		// Green for hacker vibes
// 		textcolor[1] = 0x3E ;
// 		textcolor[2] = 0x00 ;
// 		textbg[0] = 0x00;
// 		textbg[1] = 0x00;
// 		textbg[2] = 0x00;
//
// 	}
// }

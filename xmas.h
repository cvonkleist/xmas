// GE Christmas light control for Arduino
// Ported by Scott Harris <scottrharris@gmail.com>
// scottrharris.blogspot.com

#include "WProgram.h"


// Based on this code:

/*!     Christmas Light Control
 **     By Robert Quattlebaum <darco@deepdarc.com>
 **     Released November 27th, 2010
 **
 **     For more information,
 **     see <http://www.deepdarc.com/2010/11/27/hacking-christmas-lights/>.
 **
 **     Originally intended for the ATTiny13, but should
 **     be easily portable to other microcontrollers.
 */

#define xmas_color_t uint16_t // typedefs can cause trouble in the Arduino environment

// Eliminate the .h file

#define LIGHT_COUNT          (50) //I only have a 36 light strand. Should be 50 or 36
#define CHANNEL_MAX          (0xF)
#define DEFAULT_INTENSITY     (0xCC)
#define HUE_MAX               ((CHANNEL_MAX+1)*6-1)
#define COLOR(r,g,b)     ((r)+((g)<<4)+((b)<<8))
#define COLOR_WHITE     COLOR(CHANNEL_MAX,CHANNEL_MAX,CHANNEL_MAX)
#define COLOR_BLACK     COLOR(0,0,0)
#define COLOR_RED          COLOR(CHANNEL_MAX,0,0)
#define COLOR_GREEN     COLOR(0,CHANNEL_MAX,0)
#define COLOR_BLUE          COLOR(0,0,CHANNEL_MAX)
#define COLOR_CYAN          COLOR(0,CHANNEL_MAX,CHANNEL_MAX)
#define COLOR_MAGENTA     COLOR(CHANNEL_MAX,0,CHANNEL_MAX)
#define COLOR_YELLOW     COLOR(CHANNEL_MAX,CHANNEL_MAX,0)

// Pin setup
#define STRAND_COUNT 2
#define STRAND_PINS {4, 6}
#define STATUSPIN 13 // The LED

const int strand_pin[STRAND_COUNT] = STRAND_PINS;

// The delays in the begin, one, and zero functions look funny, but they give the correct
// pulse durations when checked with a logic analyzer. Tested on an Arduino Uno.

void xmas_begin(int strand);
void xmas_one(int strand);
void xmas_zero(int strand);
void xmas_end(int strand);
void xmas_fill_color(int strand, uint8_t begin,uint8_t count,uint8_t intensity,xmas_color_t color);
void xmas_fill_color_same(int strand, uint8_t begin,uint8_t count,uint8_t intensity,xmas_color_t color);
void xmas_set_color(int strand, uint8_t led,uint8_t intensity,xmas_color_t color);
xmas_color_t xmas_color(uint8_t r,uint8_t g,uint8_t b);
xmas_color_t xmas_color_hue(uint8_t h);
void clear();

#define WIDTH 10
#define HEIGHT 10

// figured out how to inline stuff from http://www.velocityreviews.com/forums/t437213-extern-inline.html
inline uint8_t translate_strand(int x, int y) {
	return x % STRAND_COUNT;
}
inline uint8_t translate(int x, int y) {
	int n;
	n = (x - translate_strand(x, y)) / STRAND_COUNT * HEIGHT;
	n += (n / HEIGHT) % 2 == 1 ?  y : (HEIGHT - y - 1);
	return n;
}
inline void set_pixel(uint8_t x,uint8_t y, uint8_t intensity, xmas_color_t color) {
	xmas_set_color(translate_strand(x, y), translate(x, y), intensity, color);
}
static inline void swap(uint8_t *a, uint8_t *b);
inline void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t intensity, xmas_color_t c);

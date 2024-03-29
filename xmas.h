// ge g-35 xmas library
// cvk/2011
//
// based on scott harris' code, which was based on deepdarc's code. these
// notices appeared in this file:
//
//  // GE Christmas light control for Arduino
//  // Ported by Scott Harris <scottrharris@gmail.com>
//  // scottrharris.blogspot.com
//  //
//  /*!     Christmas Light Control
//   **     By Robert Quattlebaum <darco@deepdarc.com>
//   **     Released November 27th, 2010
//   **
//   **     For more information,
//   **     see <http://www.deepdarc.com/2010/11/27/hacking-christmas-lights/>.
//   **
//   **     Originally intended for the ATTiny13, but should
//   **     be easily portable to other microcontrollers.
//   */

#include "Arduino.h"

#define xmas_color_t uint16_t

#define LIGHT_COUNT (50)
#define CHANNEL_MAX (0xF)
#define DEFAULT_INTENSITY (0xCC)
#define HUE_MAX ((CHANNEL_MAX+1)*6-1)
#define COLOR(r,g,b) ((r)+((g)<<4)+((b)<<8))
#define COLOR_WHITE COLOR(CHANNEL_MAX,CHANNEL_MAX,CHANNEL_MAX)
#define COLOR_BLACK COLOR(0,0,0)
#define COLOR_RED COLOR(CHANNEL_MAX,0,0)
#define COLOR_GREEN COLOR(0,CHANNEL_MAX,0)
#define COLOR_BLUE COLOR(0,0,CHANNEL_MAX)
#define COLOR_CYAN COLOR(0,CHANNEL_MAX,CHANNEL_MAX)
#define COLOR_MAGENTA COLOR(CHANNEL_MAX,0,CHANNEL_MAX)
#define COLOR_YELLOW COLOR(CHANNEL_MAX,CHANNEL_MAX,0)

// Pin setup
#define STATUSPIN 13 // The LED

extern const int width;
extern const int height;
extern const int strand_count;
extern const int strand_pins[];
extern const int strand_pin_lookup_table[];
extern const int bulb_lookup_table[];

void xmas_begin(int strand);
void xmas_one(int strand);
void xmas_zero(int strand);
void xmas_end(int strand);
void xmas_fill_color(int strand, uint8_t begin, uint8_t count, uint8_t intensity, xmas_color_t color);
void xmas_fill_color_same(int strand, uint8_t begin, uint8_t count, uint8_t intensity, xmas_color_t color);
void xmas_set_color(int strand, uint8_t led, uint8_t intensity, xmas_color_t color);
xmas_color_t xmas_color(uint8_t r, uint8_t g, uint8_t b);
void clear();

// figured out my problem with inlining with
// http://www.velocityreviews.com/forums/t437213-extern-inline.html

// returns the strand number for coordinate (x, y)
inline uint8_t translate_strand(int x, int y) {
  return strand_pin_lookup_table[y * width + x];
}

// returns the bulb number that corresponds to coordinate (x, y)
inline uint8_t translate(int x, int y) {
  return bulb_lookup_table[y * width + x];
}

// turn the pixel (x, y) on to +intensity+ with +color+
inline void set_pixel(uint8_t x, uint8_t y, uint8_t intensity, xmas_color_t color) {
  //Serial.print("set_pixel");
  //Serial.print(" x = ");
  //Serial.println(x);
  //Serial.print(" y = ");
  //Serial.println(y);
  //Serial.print(" translate_strand(x,y) = ");
  //Serial.println(translate_strand(x,y));
  //Serial.print(" translate(x,y) = ");
  //Serial.println(translate(x,y));
  xmas_set_color(translate_strand(x, y), translate(x, y), intensity, color);
}
static inline void swap(uint8_t *a, uint8_t *b);
inline void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t intensity, xmas_color_t c);

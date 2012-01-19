#include "Arduino.h"
#include "xmas.h"
#include <util/delay.h>

#define PIN_HIGH(x) PORTD |= (1 << x)
#define PIN_LOW(x) PORTD &= ~(1 << x)

// begin a serial packet on +strand+
void xmas_begin(int strand_pin) {
  PIN_HIGH(strand_pin);
  _delay_us(10);
  PIN_LOW(strand_pin);
}

// send a serial 1 on +strand+
void xmas_one(int strand_pin) {
  PIN_LOW(strand_pin);
  _delay_us(20);
  PIN_HIGH(strand_pin);
  _delay_us(8);
  PIN_LOW(strand_pin);
}

// send a serial 0 on +strand+
void xmas_zero(int strand_pin) {
  PIN_LOW(strand_pin);
  _delay_us(10);
  PIN_HIGH(strand_pin);
  _delay_us(20);
  PIN_LOW(strand_pin);
}

// end serial packet on +strand+
void xmas_end(int strand_pin) {
  PIN_LOW(strand_pin);
  _delay_us(30);
}


// set +count+ bulbs starting at +begin+ to +color+ at +intensity+
void xmas_fill_color(int strand, uint8_t begin, uint8_t count, uint8_t intensity, xmas_color_t color) {
  while(count--)
    xmas_set_color(strand, begin++, intensity, color);
}

// set pixel number +led+ along a +strand+ to +color+ at +intensity+
void xmas_set_color(int strand, uint8_t led, uint8_t intensity, xmas_color_t color) {
  uint8_t i;
  int strand_pin = strand_pins[strand];
  xmas_begin(strand_pin);
  for(i = 6; i; i--, (led <<= 1))
    if(led & (1 << 5))
      xmas_one(strand_pin);
    else
      xmas_zero(strand_pin);
  for(i = 8; i; i--, (intensity <<= 1))
    if(intensity & ( 1 << 7))
      xmas_one(strand_pin);
    else
      xmas_zero(strand_pin);
  for(i = 12; i; i--, (color <<= 1))
    if(color & (1 << 11))
      xmas_one(strand_pin);
    else
      xmas_zero(strand_pin);
  xmas_end(strand_pin);
}

// returns a color value that represents an RGB triple
xmas_color_t xmas_color(uint8_t r, uint8_t g, uint8_t b) {
  return COLOR(r, g, b);
}

// clear the board
//
// when the pixels are freshly plugged in, this has the side effect of giving
// each pixel an address, since a fresh pixel assumes the address of whatever
// command it first hears (and then doesn't pass the command on, while it does
// pass on all future commands it receives that are not addressed to it).
void clear() {
  for(int i = 0; i < strand_count; i++) {
    xmas_fill_color(i, 0, LIGHT_COUNT, DEFAULT_INTENSITY, COLOR_BLACK);
  }
}

inline uint8_t translate(int x, int y);
inline uint8_t translate_strand(int x, int y);
extern inline void set_pixel(uint8_t x, uint8_t y, uint8_t intensity, xmas_color_t color);

// swap the variables pointed at by *a and *b
static inline void swap(uint8_t *a, uint8_t *b) {
  uint8_t t;
  t = *a;
  *b = *a;
  *a = t;
}

// draw a line from (x0, y0) to (x1, y1)
//
// efficient line implementation found in wikipedia
inline void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t intensity, xmas_color_t c) {
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);
  int sy, sx;
  if(x0 < x1) sx = 1; else sx = -1;
  if(y0 < y1) sy = 1; else sy = -1;
  int err = dx - dy;

  while(1) {
    set_pixel(x0, y0, intensity, c);
    if(x0 == x1 && y0 == y1) break;
    int e2 = 2 * err;
    if(e2 > -dy) {
      err = err - dy;
      x0 = x0 + sx;
    }
    if(e2 < dx) {
      err = err + dx;
      y0 = y0 + sy;
    }
  }
}

#include "WProgram.h"
#include "xmas.h"
// The delays in the begin, one, and zero functions look funny, but they give the correct
// pulse durations when checked with a logic analyzer. Tested on an Arduino Uno.
void xmas_begin(int strand)
{
	digitalWrite(strand_pin[strand], 1);
	delayMicroseconds(7); //The pulse should be 10 uS long, but I had to hand tune the delays. They work for me
	digitalWrite(strand_pin[strand], 0);
}

void xmas_one(int strand)
{
	digitalWrite(strand_pin[strand], 0);
	delayMicroseconds(11); //This results in a 20 uS long low
	digitalWrite(strand_pin[strand], 1);
	delayMicroseconds(7);
	digitalWrite(strand_pin[strand], 0);
}

void xmas_zero(int strand)
{
	digitalWrite(strand_pin[strand], 0);
	delayMicroseconds(2);
	digitalWrite(strand_pin[strand], 1);
	delayMicroseconds(20 - 3);
	digitalWrite(strand_pin[strand], 0);
}

void xmas_end(int strand)
{
	digitalWrite(strand_pin[strand],0);
	delayMicroseconds(40); // Can be made shorter
}


// The rest of Robert's code is basically unchanged

void xmas_fill_color(int strand, uint8_t begin,uint8_t count,uint8_t intensity,xmas_color_t color)
{
	while(count--)
	{
		xmas_set_color(strand, begin++,intensity,color);
	}
}

void xmas_fill_color_same(int strand, uint8_t begin,uint8_t count,uint8_t intensity,xmas_color_t color)
{
	while(count--)
	{
		xmas_set_color(strand, 0,intensity,color);
	}
}


void xmas_set_color(int strand, uint8_t led,uint8_t intensity,xmas_color_t color) {
	uint8_t i;
	xmas_begin(strand);
	for(i=6;i;i--,(led<<=1))
		if(led&(1<<5))
			xmas_one(strand);
		else
			xmas_zero(strand);
	for(i=8;i;i--,(intensity<<=1))
		if(intensity&(1<<7))
			xmas_one(strand);
		else
			xmas_zero(strand);
	for(i=12;i;i--,(color<<=1))
		if(color&(1<<11))
			xmas_one(strand);
		else
			xmas_zero(strand);
	xmas_end(strand);
}


xmas_color_t
xmas_color(uint8_t r,uint8_t g,uint8_t b) {
	return COLOR(r,g,b);
}

xmas_color_t
xmas_color_hue(uint8_t h) {
	switch(h>>4) {
		case 0:     h-=0; return xmas_color(h,CHANNEL_MAX,0);
		case 1:     h-=16; return xmas_color(CHANNEL_MAX,(CHANNEL_MAX-h),0);
		case 2:     h-=32; return xmas_color(CHANNEL_MAX,0,h);
		case 3:     h-=48; return xmas_color((CHANNEL_MAX-h),0,CHANNEL_MAX);
		case 4:     h-=64; return xmas_color(0,h,CHANNEL_MAX);
		case 5:     h-=80; return xmas_color(0,CHANNEL_MAX,(CHANNEL_MAX-h));
	}
}

void clear() {
	for(int i = 0; i < STRAND_COUNT; i++) {
		pinMode(strand_pin[i], OUTPUT);
		xmas_fill_color(i, 0, LIGHT_COUNT, DEFAULT_INTENSITY, COLOR_BLACK); //Enumerate all the lights
	}
}

#define WIDTH 10
#define HEIGHT 10

inline uint8_t translate(int x, int y);
inline uint8_t translate_strand(int x, int y);
extern inline void set_pixel(uint8_t x,uint8_t y, uint8_t intensity, xmas_color_t color);

static inline void swap(uint8_t *a, uint8_t *b) {
	uint8_t t;
	t = *a;
	*b = *a;
	*a = t;
}

inline void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t intensity, xmas_color_t c) {
	int dx = abs(x1-x0);
	int dy = abs(y1-y0);
	int sy, sx;
	if(x0 < x1)sx = 1;else sx = -1;
	if(y0 < y1)sy = 1;else sy = -1;
	int err = dx-dy;

	while(1) {
		set_pixel(x0,y0, intensity, c);
		if(x0 == x1 && y0 == y1) break;
		int e2 = 2*err;
		if(e2 > -dy) {
			err = err - dy;
			x0 = x0 + sx;
		}
		if(e2 <  dx) {
			err = err + dx;
			y0 = y0 + sy;
		}
	}
}

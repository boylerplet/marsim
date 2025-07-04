#ifndef MARSIM_IMPLEMENTATION
#define MARSIM_IMPLEMENTATION
#include "raylib.h"
#include <stddef.h>

#define SCALE_FACTOR    100
#define WINDOW_WIDTH    (16 * SCALE_FACTOR)
#define WINDOW_HEIGHT   (9 * SCALE_FACTOR)

#define MARGIN_LEFT     50
#define MARGIN_RIGHT    80
#define MARGIN_TOP      50
#define MARGIN_BOTTOM   60

#define VIEWPORT_RIGHT  (WINDOW_WIDTH - MARGIN_RIGHT)
#define VIEWPORT_LEFT   (MARGIN_LEFT)
#define VIEWPORT_BOTTOM (WINDOW_HEIGHT - MARGIN_BOTTOM)
#define VIEWPORT_TOP    (MARGIN_TOP)
#define VIEWPORT_WIDTH  (VIEWPORT_RIGHT - VIEWPORT_LEFT)
#define VIEWPORT_HEIGHT (VIEWPORT_BOTTOM - VIEWPORT_TOP)

#define CUSTOM_COLOR_SCHEME

#ifndef CUSTOM_COLOR_SCHEME
/* UI */
#define CLR_VIEWPORT_BG           0x181818FF
#define CLR_VIEWPORT_BORDER_BG    0xFFFFFFFF
#define CLR_PANEL_BG              0x222222FF
/* CANDLESTICK */
#define CLR_UP_CANDLESTICK_BODY   0xFFFFFFFF
#define CLR_DOWN_CANDLESTICK_BODY 0xFFFFFFFF
#define CLR_TOP_WICK_UPTREND      0xFFFFFFFF
#define CLR_BOTTOM_WICK_UPTREND   0xFFFFFFFF
#define CLR_TOP_WICK_DOWNTREND    0xFFFFFFFF
#define CLR_BOTTOM_WICK_DOWNTREND 0xFFFFFFFF

#define UP_OUTLINE
/*#define DOWN_OUTLINE*/

#else // CUSTOM_COLOR_SCHEME
/* UI */
#define CLR_VIEWPORT_BG           0x181818FF
#define CLR_VIEWPORT_BORDER_BG    0xFFFFFFFF
#define CLR_PANEL_BG              0x222222FF
/* CANDLESTICK */
#define CLR_UP_CANDLESTICK_BODY   0x7BC043FF
#define CLR_DOWN_CANDLESTICK_BODY 0xEE4035FF
#define CLR_TOP_WICK_UPTREND      0x7BC043FF
#define CLR_BOTTOM_WICK_UPTREND   0x7BC043FF
#define CLR_TOP_WICK_DOWNTREND    0xEE4035FF
#define CLR_BOTTOM_WICK_DOWNTREND 0xEE4035FF
/*#define UP_OUTLINE*/
/*#define DOWN_OUTLINE*/

#endif // CUSTOM_COLOR_SCHEME

typedef struct {
    float   x_offset;
	float   y_offset;
	float   y_scale;
	float   candlestick_width;
    bool    is_dragging;
    Vector2 drag_start;
} ViewState;

typedef struct {
	float base_price_min;
	float base_price_max;
	float price_min;
	float price_max;
	float price_range;
} PriceConfig;

typedef struct {
	float open;
	float low;
	float high;
	float close;
} CandleStick;

typedef struct {
	CandleStick *items;
	size_t count;
	size_t capacity;
} CandleSticks;

extern const float scale_intervals[];

extern const int   num_scale_intervals;

void draw_dotted_lines_h(float src, float dst, float y, float dash_size, float gap_size, float line_thickness, Color color);

bool in_y_axis();
bool in_viewport_or_x_axis();

#endif // MARSIM_IMPLEMENTATION

#include <math.h>
#include <stdio.h>
#include "raylib.h"
#include "marsim.h"
#include "nob.h"

// Declarations
void draw_dotted_lines_h(float src, float dst, float y, float dash_size, float gap_size, float line_thickness, Color color);
bool in_y_axis();
bool in_viewport_or_x_axis();

// Functions
bool read_data_from_file(CandleSticks *xs, const char *filename) {
	FILE *fptr = fopen(filename, "r");
	if (fptr == NULL) {
		TraceLog(LOG_FATAL, "RF: Could not read file: %s", filename);
		return false;
	} else {
		TraceLog(LOG_INFO, "RF: Opened file '%s'", filename);
	}

	float open;
	float low;
	float close;
	float high;

	// Read lines into xs
	while(fscanf(fptr, "%f, %f, %f, %f,", &open, &low, &high, &close) == 4) {
		CandleStick temp = {
			.open  = open,
			.low   = low,
			.high  = high,
			.close = close
		};

		nob_da_append(xs, temp);
	};

	TraceLog(LOG_INFO, "RF: %d values read from file '%s'", xs->count, filename);

	fclose(fptr);

	return true;
}

void render_panel() {
	// Left   panel
	DrawRectangle(0             , 0              , MARGIN_LEFT   , WINDOW_HEIGHT, GetColor(CLR_PANEL_BG));
	// Top    panel
	DrawRectangle(VIEWPORT_LEFT , 0              , VIEWPORT_WIDTH, MARGIN_TOP   , GetColor(CLR_PANEL_BG));
	// Right  panel
	DrawRectangle(VIEWPORT_RIGHT, 0              , MARGIN_RIGHT  , WINDOW_HEIGHT, GetColor(CLR_PANEL_BG));
	// Bottom panel
	DrawRectangle(VIEWPORT_LEFT , VIEWPORT_BOTTOM, VIEWPORT_WIDTH, MARGIN_BOTTOM, GetColor(CLR_PANEL_BG));
}

void render_axes(ViewState *view, const PriceConfig *pc, float time_window) {
	render_panel();

	if ((pc->price_max - pc->price_min) == 0) TraceLog(LOG_FATAL, "Divide By zero error when setting y-axis");

	// y-axis
	// Tick spacing
	/*float price_range = pc->price_max - pc->price_min;*/
	float pixels_per_unit = VIEWPORT_HEIGHT / pc->price_range;
	float label_step      = scale_intervals[0];
	int label_height      = 15;
	// Find appropriate interval
	for (int i = 0; i < num_scale_intervals; i++) {
		float tick_spacing = scale_intervals[i] * pixels_per_unit;
		// Check for overlap
		if (tick_spacing < label_height && i < num_scale_intervals - 1) {
			label_step = scale_intervals[i + 1];
			continue;
		}
		// Check if smaller interval fits
		if (i > 0) {
			float smaller_spacing = scale_intervals[i - 1] * pixels_per_unit;
			if (tick_spacing > 5 * smaller_spacing) {
				label_step = scale_intervals[i - 1];
				break;
			}
		}
		label_step = scale_intervals[i];
		break;
	}

	for (float price = floor(pc->price_min / label_step) * label_step;
		price <= pc->price_max;
		price += label_step
	) {
		float dy = (VIEWPORT_HEIGHT) * ((pc->price_max - price) / (pc->price_max - pc->price_min));
		float y  = MARGIN_TOP + dy;
		if (y >= MARGIN_TOP && y <= VIEWPORT_BOTTOM) {
			DrawText(
				TextFormat("%.3f", price),
				VIEWPORT_RIGHT + 20,
				y - 5,
				10,
				WHITE
			); // Tick label
			DrawLine(
				VIEWPORT_RIGHT    , y,
				VIEWPORT_RIGHT + 5, y,
				WHITE
			); // Tick
		}
	}

	// x-axis
	for (float time = 0; time <= time_window; time += 60.0f) {
		float x = MARGIN_LEFT + (time / 60.0f) * view->candlestick_width - view->x_offset;
		if (x >= MARGIN_LEFT && x <= VIEWPORT_RIGHT && ((int)(time / 60.0f) % 5 == 0)) {
			const int hours      = (9 + ((int)(time / 60) / 60)) % 12;
			const int minutes    = (int)(time / 60.0f) % 60;
			const int text_width = MeasureText("00:00", 16);
			DrawText(
				TextFormat("%2.2d:%2.2d", hours, minutes),
				x - (text_width / 2.0f),
				VIEWPORT_BOTTOM + 10,
				16,
				WHITE
			);
			DrawLine(
				x,
				VIEWPORT_BOTTOM,
				x,
				VIEWPORT_BOTTOM + 5,
				WHITE
			); // Tick
		}
	}
}

float get_cs_top(const CandleStick *cs) {
	return fmaxf(cs->open, cs->close);
}

float get_cs_bottom(const CandleStick *cs) {
	return fminf(cs->open, cs->close);
}

float norm_dy(const PriceConfig *pc, float offset) {
	return MARGIN_TOP + (VIEWPORT_HEIGHT) * ((pc->price_max - offset) / pc->price_range);

}

float norm_dx(const ViewState *view, int index, float offset) {
	return (VIEWPORT_LEFT + view->candlestick_width * index - view->x_offset) + offset ;
}

bool isuptrend(const CandleStick *cs) {
	return cs->open < cs->close;
}

void render_candlesticks(const ViewState *view, const PriceConfig *pc, const CandleSticks *cs_a) {
	BeginScissorMode(VIEWPORT_LEFT, VIEWPORT_TOP, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	for (size_t i = 0; i < cs_a->count; i++) {

		Rectangle body = {
			norm_dx(view, i, 0),
			norm_dy(pc, get_cs_top(&cs_a->items[i])),
			view->candlestick_width - 2.0f,
			fabsf(cs_a->items[i].open - cs_a->items[i].close) * VIEWPORT_HEIGHT / pc->price_range,
		};

		if (isuptrend(&cs_a->items[i])) {
#ifdef UP_OUTLINE
			DrawRectangleLinesEx(
				body,
				1.0f,
				GetColor(CLR_UP_CANDLESTICK_BODY)
			);
#else // UP_OUTLINE
			DrawRectangleRec(
				body,
				GetColor(CLR_UP_CANDLESTICK_BODY)
			);
#endif // UP_OUTLINE
			DrawLine(
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, cs_a->items[i].high),
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, get_cs_top(&cs_a->items[i])),
				GetColor(CLR_TOP_WICK_UPTREND)
			); // Top wick uptrend
			DrawLine(
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, get_cs_bottom(&cs_a->items[i])),
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, cs_a->items[i].low),
				GetColor(CLR_BOTTOM_WICK_UPTREND)
			); // Bottom wick uptrend
		} else {
#ifdef DOWN_OUTLINE
			DrawRectangleLinesEx(
				body,
				1.0f,
				GetColor(CLR_DOWN_CANDLESTICK_BODY),
			);
#else // DOWN_OUTLINE
			DrawRectangleRec(
				body,
				GetColor(CLR_DOWN_CANDLESTICK_BODY)
			);
#endif // DOWN_OUTLINE
			DrawLine(
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, cs_a->items[i].high),
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, get_cs_top(&cs_a->items[i])),
				GetColor(CLR_TOP_WICK_DOWNTREND)
			); // Top wick uptrend
			DrawLine(
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, get_cs_bottom(&cs_a->items[i])),
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, cs_a->items[i].low),
				GetColor(CLR_BOTTOM_WICK_DOWNTREND)
			); // Bottom wick uptrend
		}
	}

	EndScissorMode();
}

void mark_last_candle(const ViewState *view, const PriceConfig *pc, CandleSticks *cs_a) {
	CandleStick last = nob_da_last(cs_a);

	if (view->is_dragging) {};
	/*last.close = 1024.0f;*/

	float TEXT_HEIGHT     = 16.0f;
	float TEXT_BOX_HEIGHT = TEXT_HEIGHT + 8.0f;
	float TEXT_MARGIN     = 6.0f; // Inline
	float MARK_HEIGHT     = TEXT_BOX_HEIGHT + 6.0f;
	float MARK_WIDTH      = 0.0f;
	float MARK_LEFT       = 0.0f;
	float MARK_RIGHT      = VIEWPORT_RIGHT + MARGIN_RIGHT - 5.0f;
	float BOX_PADDING_R   = 6.0f;
	float BOX_PADDING_L   = 10.0f;

	char price_text[25] = {0};
	int  clr_price_text = 0;

	if (last.open == last.close) {
		clr_price_text = 0xFFFFFFFF;
	} else if (last.open > last.close) {
		clr_price_text = CLR_DOWN_CANDLESTICK_BODY;
	} else {
		clr_price_text = CLR_UP_CANDLESTICK_BODY;
	}

	snprintf(price_text, 24, "%c %.3f", last.close < 0 ? '-' : '+', fabsf(last.close));

	Vector2 text_dimensions = MeasureTextEx(GetFontDefault(), price_text, 16.0f, 1.0f);
	MARK_LEFT               = MARK_RIGHT - BOX_PADDING_R - (text_dimensions.x + 2 * TEXT_MARGIN) - BOX_PADDING_L;
	MARK_WIDTH              = MARK_RIGHT - MARK_LEFT;
	/*TraceLog(LOG_INFO, "MARK: Height: %.3f, Width: %.3f", text_dimensions.y, text_dimensions.x);*/

	BeginScissorMode(VIEWPORT_LEFT, VIEWPORT_TOP, VIEWPORT_WIDTH + MARGIN_RIGHT, VIEWPORT_HEIGHT);

	DrawRectangle(
		MARK_LEFT,
		norm_dy(pc, last.close) - (MARK_HEIGHT / 2),
		MARK_WIDTH,
		MARK_HEIGHT,
		GetColor(0xF2C811FF) // Yellow shade
	); // Mark border
	DrawRectangle(
		MARK_LEFT + BOX_PADDING_L,
		norm_dy(pc, last.close) - (TEXT_BOX_HEIGHT / 2),
		MARK_WIDTH - BOX_PADDING_L - BOX_PADDING_R,
		TEXT_BOX_HEIGHT,
		GetColor(CLR_PANEL_BG)
	); // Mark text area
	DrawText(
		price_text,
		MARK_LEFT + BOX_PADDING_L + TEXT_MARGIN,
		norm_dy(pc, last.close) - (TEXT_HEIGHT / 2),
		TEXT_HEIGHT,
		GetColor(clr_price_text)
	); // Price

	draw_dotted_lines_h(
		/*norm_dx(view, cs_a->count - 1, 0) + (view->candlestick_width / 2),*/
		VIEWPORT_LEFT,
		MARK_LEFT,
		norm_dy(pc, last.close) + 0.5f,
		3.0f,
		2.0f,
		1.0f,
		GetColor(0x777777FF)
	);

	EndScissorMode();
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Marsim - A stock market simulator");
    SetTargetFPS(30);

    ViewState view         = {0};
    view.x_offset          = -60.0f;
    view.y_offset          = 0.0f;
	view.y_scale           = 1.0f;
    view.candlestick_width = 8.0f;
    view.is_dragging       = false;

	PriceConfig price_c = {
		.base_price_min = 0.0f,
		.base_price_max = 1000.0f,
		.price_max      = 0.0f,
		.price_min      = 0.0f,
	};

	CandleSticks candlesticks = {0};
	char *filename            = "./STE";
	if (!read_data_from_file(&candlesticks, filename)) return 1;

    float time_window   = 60 * 60 * 24.0f;
	float tick_timer    = 0.0f;
	float tick_interval = 0.7f;
	float min_timer     = 0.0f;

    while (!WindowShouldClose()) {
		float dt    = GetFrameTime();
		tick_timer += dt;
		min_timer  += dt;

        // Handle mouse interaction
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (!view.is_dragging) {
                view.is_dragging = true;
                view.drag_start  = GetMousePosition();
            } else {
                Vector2 mouse_pos = GetMousePosition();
                float dx          = mouse_pos.x - view.drag_start.x;
				float dy          = mouse_pos.y - view.drag_start.y;
				float price_delta = dy * (price_c.price_range / VIEWPORT_HEIGHT); // Normalize to viewport coords
                view.x_offset    -= dx;
				view.y_offset    += price_delta;
                view.drag_start   = mouse_pos;
            }
        }
		if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
			view.is_dragging = false;
		}

		// Handle Scroll (y-axis zoom)
		if (in_y_axis()) {
			float wheel = GetMouseWheelMove();
			if (wheel != 0) {
				float zoom_factor = (wheel > 0) ? 1.1f : 0.9f;
				view.y_scale     *= zoom_factor;
				if (view.y_scale < 0.001f) view.y_scale = 0.001f;
				if (view.y_scale > 1000.0f) view.y_scale = 1000.0f;
			}
		}

		if (in_viewport_or_x_axis()) {
			float wheel = GetMouseWheelMove();
			if (wheel != 0) {
				float zoom_factor       = (wheel > 0) ? 1.1f : 0.9f;
				view.candlestick_width *= zoom_factor;
				if (view.candlestick_width < 4.0f)   view.candlestick_width = 4.0f;
				if (view.candlestick_width > 100.0f) view.candlestick_width = 100.0f;
				view.x_offset *= zoom_factor;
				view.x_offset += (wheel > 0 ? 1.0f : -1.0f) * (GetMousePosition().x - MARGIN_LEFT) * 0.1f;

			}
		}

		// Clamp offsets
        if (view.x_offset < -60.0f) view.x_offset = -60.0f;
		price_c.price_range = (price_c.base_price_max - price_c.base_price_min) / view.y_scale;

		if (IsKeyPressed(KEY_R)) {
			view.x_offset          = -60.0f;
			view.y_offset          = 0.0f;
			view.y_scale           = 5.0f;
			view.candlestick_width = 8.0f;
			view.is_dragging       = false;
		}


		price_c.price_min = view.y_offset;
		price_c.price_max = view.y_offset + price_c.price_range;

        BeginDrawing();
        ClearBackground(GetColor(CLR_VIEWPORT_BG));

        // ViewArea Border
        Rectangle viewport = {
			.x      = VIEWPORT_LEFT,
		    .y      = VIEWPORT_TOP,
		    .width  = VIEWPORT_WIDTH,
		    .height = VIEWPORT_HEIGHT
		};

		render_axes(&view, &price_c, time_window);

		if (tick_timer >= tick_interval) {
			CandleStick last = nob_da_last(&candlesticks);

			float new_close = last.close + (((float)rand() / RAND_MAX) * 5.0f * ((float)rand() / RAND_MAX > 0.5f ? 1.0f : -1.0f));

			candlesticks.items[candlesticks.count - 1].close = new_close;
			if (new_close >= candlesticks.items[candlesticks.count - 1].high) {
				candlesticks.items[candlesticks.count - 1].high = new_close;
			}
			if (new_close <= candlesticks.items[candlesticks.count - 1].low) {
				candlesticks.items[candlesticks.count - 1].low = new_close;
			}

			tick_timer = 0.0f;
		}

		if (min_timer >= 60.0f) {
			// Generate next candle
			CandleStick last = nob_da_last(&candlesticks);
			CandleStick new  = { last.close, last.close, last.close, last.close };

			nob_da_append(&candlesticks, new);

			min_timer = 0.0f;
		}

		render_candlesticks(&view, &price_c, &candlesticks);
        DrawRectangleLinesEx(viewport, 1.0f, GetColor(CLR_VIEWPORT_BORDER_BG));
		mark_last_candle(&view, &price_c, &candlesticks);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}


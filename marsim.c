#include "raylib.h"
#include "marsim.h"
#include "candlesticks.h"
#include <math.h>

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
	return cs->open > cs->close;
}

void render_candlesticks(const ViewState *view, const PriceConfig *pc, const CandleStick cs_a[250]) {
	BeginScissorMode(VIEWPORT_LEFT, VIEWPORT_TOP, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	for (int i = 0; i < 250; i++) {

		Rectangle body = {
			norm_dx(view, i, 0),
			norm_dy(pc, get_cs_top(&cs_a[i])),
			view->candlestick_width - 2.0f,
			fabsf(cs_a[i].open - cs_a[i].close) * VIEWPORT_HEIGHT / pc->price_range,
		};

		if (isuptrend(&cs_a[i])) {
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
				norm_dy(pc, cs_a[i].high),
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, get_cs_top(&cs_a[i])),
				GetColor(CLR_TOP_WICK_UPTREND)
			); // Top wick uptrend
			DrawLine(
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, get_cs_bottom(&cs_a[i])),
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, cs_a[i].low),
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
				norm_dy(pc, cs_a[i].high),
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, get_cs_top(&cs_a[i])),
				GetColor(CLR_TOP_WICK_DOWNTREND)
			); // Top wick uptrend
			DrawLine(
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, get_cs_bottom(&cs_a[i])),
				norm_dx(view, i, view->candlestick_width / 2),
				norm_dy(pc, cs_a[i].low),
				GetColor(CLR_BOTTOM_WICK_DOWNTREND)
			); // Bottom wick uptrend
		}
	}

	EndScissorMode();
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Marsim - A stock market simulator");
    SetTargetFPS(30);

    ViewState view         = {0};
    view.x_offset          = -60.0f;
    view.y_offset          = 0.0f;
	view.y_scale           = 5.0f;
    view.candlestick_width = 8.0f;
    view.is_dragging       = false;

	PriceConfig price_c = {
		.base_price_min = 0.0f,
		.base_price_max = 1000.0f,
		.price_max      = 0.0f,
		.price_min      = 0.0f,
	};

    float time_window = 60 * 60 * 24.0f;

    while (!WindowShouldClose()) {
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
		float wheel = GetMouseWheelMove();
		if (wheel != 0) {
			float zoom_factor = (wheel > 0) ? 1.1f : 0.9f;
			view.y_scale *= zoom_factor;
			if (view.y_scale < 0.001f) view.y_scale = 0.001f;
			if (view.y_scale > 1000.0f) view.y_scale = 1000.0f;
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
        DrawRectangleLinesEx(viewport, 1.0f, GetColor(CLR_VIEWPORT_BORDER_BG));

		render_candlesticks(&view, &price_c, candlesticks);
		render_axes(&view, &price_c, time_window);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

#include "marsim.h"

const float scale_intervals[]   = {
	0.005f, 0.01f, 0.02f, 0.05f,
	0.1f, 0.5f, 1.0f, 5.0f,
	10.0f, 20.0f, 25.0f, 40.0f, 50.0f,
	100.0f, 200.0f, 250.0f, 400.0f, 500.0f,
	1000.0f, 5000.0f, 10000.0f
};

const int   num_scale_intervals = sizeof(scale_intervals) / 
	sizeof(scale_intervals[0]);

void draw_dotted_lines_h(float src, float dst, float y, float dash_size, float gap_size, float line_thickness, Color color) {
	int dir = 1;
	if (dst < src) dir *= -1;

	float dx = 0;

	if (dir == 1) {
		// Draw towards right
		while (src + dx <= dst) {
			Vector2 start = { src + dx            , y };
			Vector2 end   = { src + dx + dash_size, y };
			DrawLineEx(start, end, line_thickness, color);
			dx += (dash_size + gap_size);
		}
	} else {
		// Draw towards left
		while (src - dx >= dst) {
			Vector2 start = { .x = (src - dx)            , .y = y };
			Vector2 end   = { .x = (src - dx - dash_size), .y = y };
			DrawLineEx(start, end, line_thickness, color);
			dx += (dash_size + gap_size);
		}
	}
}

bool in_y_axis() {
	Vector2 mouse_pos = GetMousePosition();

	if (
		mouse_pos.x >= VIEWPORT_RIGHT &&
		mouse_pos.x <= VIEWPORT_RIGHT + MARGIN_RIGHT &&
		mouse_pos.y >= VIEWPORT_TOP &&
		mouse_pos.y <= VIEWPORT_BOTTOM
    ) {
		return true;
	}

	return false;
}

bool in_viewport_or_x_axis() {
	Vector2 mouse_pos = GetMousePosition();

	if (
		mouse_pos.x >= VIEWPORT_LEFT &&
		mouse_pos.x <= VIEWPORT_RIGHT &&
		mouse_pos.y >= VIEWPORT_TOP &&
		mouse_pos.y <= VIEWPORT_BOTTOM + MARGIN_BOTTOM
    ) {
		return true;
	}

	return false;
}

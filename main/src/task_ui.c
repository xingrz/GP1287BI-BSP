#define TAG "UI"

#include "esp_log.h"
#include "esp_system.h"
#include "lvgl.h"
#include "tasks.h"

void
task_ui(void *arg)
{
	uint32_t count = 0U;
	char count_str[11] = {0};

	static lv_style_t style_line;
	lv_style_init(&style_line);
	lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 1);
	lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, LV_COLOR_WHITE);

	lv_coord_t w = lv_obj_get_width(lv_scr_act());
	lv_coord_t h = lv_obj_get_height(lv_scr_act()) - 6;
	ESP_LOGI(TAG, "screen size w: %d, h: %d", w, h);

	lv_point_t p_tl[] = {{0, 5}, {0, 0}, {5, 0}};
	lv_obj_t *line_tl = lv_line_create(lv_scr_act(), NULL);
	lv_obj_add_style(line_tl, LV_LINE_PART_MAIN, &style_line);
	lv_line_set_points(line_tl, p_tl, 3);

	lv_point_t p_tr[] = {{w - 1, 5}, {w - 1, 0}, {w - 5, 0}};
	lv_obj_t *line_tr = lv_line_create(lv_scr_act(), NULL);
	lv_obj_add_style(line_tr, LV_LINE_PART_MAIN, &style_line);
	lv_line_set_points(line_tr, p_tr, 3);

	lv_point_t p_bl[] = {{0, h - 5}, {0, h - 1}, {5, h - 1}};
	lv_obj_t *line_bl = lv_line_create(lv_scr_act(), NULL);
	lv_obj_add_style(line_bl, LV_LINE_PART_MAIN, &style_line);
	lv_line_set_points(line_bl, p_bl, 3);

	lv_point_t p_br[] = {{w, h - 5}, {w - 1, h - 1}, {w - 5, h - 1}};
	lv_obj_t *line_br = lv_line_create(lv_scr_act(), NULL);
	lv_obj_add_style(line_br, LV_LINE_PART_MAIN, &style_line);
	lv_line_set_points(line_br, p_br, 3);

	lv_obj_t *label_tl = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(label_tl, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
	lv_obj_set_auto_realign(label_tl, true);
	lv_label_set_text(label_tl, "TL");

	lv_obj_t *label_tr = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(label_tr, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
	lv_obj_set_auto_realign(label_tr, true);
	lv_label_set_text(label_tr, "TR");

	lv_obj_t *label_bl = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(label_bl, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, -6);
	lv_obj_set_auto_realign(label_bl, true);
	lv_label_set_text(label_bl, "BL");

	lv_obj_t *label_br = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(label_br, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, -6);
	lv_obj_set_auto_realign(label_br, true);
	lv_label_set_text(label_br, "BR");

	lv_obj_t *label_count = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(label_count, NULL, LV_ALIGN_CENTER, 0, -6);
	lv_obj_set_auto_realign(label_count, true);

	while (1) {
		sprintf(count_str, "count: %04d", count);
		lv_label_set_text(label_count, count_str);
		count = (count + 1) % 10000;
		vTaskDelay(pdMS_TO_TICKS(100));
	}

	vTaskDelete(NULL);
}

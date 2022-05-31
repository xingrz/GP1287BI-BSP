#define TAG "UI"

#include "esp_log.h"
#include "esp_system.h"
#include "lvgl.h"
#include "tasks.h"

extern lv_font_t noto_48;

void
task_ui(void *arg)
{
	lv_coord_t w = lv_obj_get_width(lv_scr_act());
	lv_coord_t h = lv_obj_get_height(lv_scr_act()) - 6;
	ESP_LOGI(TAG, "screen size w: %d, h: %d", w, h);

	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &noto_48);
	lv_label_set_text(label, "极其优秀，弹性很足。");
	lv_label_set_long_mode(label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_anim_speed(label, 100);
	lv_obj_set_pos(label, 2, 3);
	lv_obj_set_size(label, w - 4, h - 4);

	vTaskDelete(NULL);
}

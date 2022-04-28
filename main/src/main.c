#define TAG "MAIN"

#include "display.h"
#include "esp_log.h"
#include "esp_system.h"
#include "lvgl.h"
#include "tasks.h"

void
app_main()
{
	lv_init();
	display_init();

	assert(pdPASS == xTaskCreatePinnedToCore(
						 task_lv, "task_lv", TASK_LV_STACK_SIZE, NULL, TASK_LV_PRIO, NULL, 0));
	vTaskDelay(200 / portTICK_PERIOD_MS);
	assert(pdPASS == xTaskCreatePinnedToCore(
						 task_ui, "task_ui", TASK_UI_STACK_SIZE, NULL, TASK_UI_PRIO, NULL, 0));

	ESP_LOGI(TAG, "SYSTEM READY");
}

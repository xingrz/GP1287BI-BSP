#define TAG "DISPLAY"
#define DISP_DEBUG 0

#include "display.h"

#include <stdint.h>

#include "device/gp1287bi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "lvgl.h"

static void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
static void display_rounder(lv_disp_drv_t *disp, lv_area_t *area);
static void display_set_px(lv_disp_drv_t *disp_drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x,
	lv_coord_t y, lv_color_t color, lv_opa_t opa);
#if DISP_DEBUG
static void display_monitor(lv_disp_drv_t *disp_drv, uint32_t time, uint32_t px);
#endif  // DISP_DEBUG

void
display_init(void)
{
	ESP_LOGI(TAG, "Initializing display");

	gp1287bi_init();

	ESP_LOGI(TAG, "Setting up LVGL");

	static lv_disp_buf_t disp_buf;
	static uint8_t buf[GP1287BI_BUF_LEN] = {0};
	lv_disp_buf_init(&disp_buf, buf, NULL, GP1287BI_WIDTH * GP1287BI_HEIGHT);

	lv_disp_drv_t disp;
	lv_disp_drv_init(&disp);
	disp.flush_cb = display_flush;
	disp.rounder_cb = display_rounder;
	disp.set_px_cb = display_set_px;
#if DISP_DEBUG
	disp.monitor_cb = display_monitor;
#endif  // DISP_DEBUG
	disp.buffer = &disp_buf;
	lv_disp_drv_register(&disp);

	ESP_LOGI(TAG, "Display initialized");
}

static void IRAM_ATTR
display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
	ESP_LOGD(TAG, "Flushing x1:%d, y1:%d, x2:%d, y2:%d", area->x1, area->y1, area->x2, area->y2);
	gp1287bi_draw(area->x1, area->y1, area->x2, area->y2, (uint8_t *)color_p);
	lv_disp_flush_ready(disp);
}

static void
display_rounder(lv_disp_drv_t *disp, lv_area_t *area)
{
	area->y1 = 0;
	area->y2 = GP1287BI_HEIGHT - 1;
	ESP_LOGD(TAG, "Rounding display %d, %d, %d, %d", area->x1, area->y1, area->x2, area->y2);
}

static void
display_set_px(lv_disp_drv_t *disp, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
	lv_color_t color, lv_opa_t opa)
{
	uint8_t w = color.full != 0x00;
	uint16_t i = (GP1287BI_HEIGHT >> 3) * x + (y >> 3);
	uint8_t b = y & 0x7;
	buf[i] &= ~(1 << b);
	buf[i] |= w << b;
}

#if DISP_DEBUG
static void
display_monitor(lv_disp_drv_t *disp_drv, uint32_t time, uint32_t px)
{
	ESP_LOGD(TAG, "%d px refreshed in %d ms", px, time);
}
#endif  // DISP_DEBUG

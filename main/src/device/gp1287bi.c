#define TAG "GP1287BI"

#include "gp1287bi.h"

#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pinout.h"

#define CMD_SOFT_RESET 0xAA
#define CMD_CLEAR_GRAM 0x55
#define CMD_VFD_MODE_SET 0xCC
#define CMD_DIP_AREA_SET 0xE0
#define CMD_INT_SPD_SET 0xB1
#define CMD_DIM_LVL_SET 0xA0
#define CMD_WRITE_GRAM 0xF0
#define CMD_DIP_POS_OFFSET 0xC0
#define CMD_DIP_MODE_SET 0x80
#define CMD_FRM_SYNC_SET 0x08
#define CMD_OSCILLATON_SET 0x78
#define CMD_EXIT_STBY_MODE 0x6D
#define CMD_ENTR_STBY_MODE 0x61

static spi_device_handle_t spi;

static struct {
	uint8_t cmd;
	uint8_t x;
	uint8_t y;
	uint8_t rows;
	uint8_t buf[GP1287BI_BUF_LEN];
} gp1287bi_write_gram;

static void IRAM_ATTR write_data(void *buf, uint32_t len);

static void gp1287bi_hard_reset(void);

void
gp1287bi_init(void)
{
	ESP_LOGI(TAG, "Init interfaces...");

	gpio_config_t output = {
		.pin_bit_mask = (1UL << PIN_VFD_RESET) | (1UL << PIN_FLMENT_EN),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
	};

	ESP_ERROR_CHECK(gpio_config(&output));

	spi_bus_config_t bus = {
		.mosi_io_num = PIN_VFD_SDA,
		.miso_io_num = -1,
		.sclk_io_num = PIN_VFD_SCK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = GP1287BI_BUF_LEN,
		.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_IOMUX_PINS,
	};

	ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus, SPI_DMA_CH_AUTO));

	spi_device_interface_config_t dev = {
		.flags = SPI_DEVICE_BIT_LSBFIRST,
		.clock_speed_hz = 4 * 1000 * 1000,
		.mode = 3,
		.spics_io_num = PIN_VFD_CS,
		.queue_size = 1,
	};

	ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev, &spi));

	gpio_set_level(PIN_FLMENT_EN, 0);

	ESP_LOGI(TAG, "Reset panel...");

	gp1287bi_hard_reset();

	ESP_LOGI(TAG, "Light up...");

	gpio_set_level(PIN_FLMENT_EN, 1);

	uint8_t tmp[8];

	ESP_LOGI(TAG, "Init GP1287BI...");

	tmp[0] = CMD_SOFT_RESET;
	write_data(tmp, 1);

	tmp[0] = CMD_VFD_MODE_SET;
	tmp[1] = 0x02;
	tmp[2] = 0x00;
	write_data(tmp, 3);

	tmp[0] = CMD_DIP_AREA_SET;
	tmp[1] = 0xFF;
	tmp[2] = 0x31;
	tmp[3] = 0x00;
	tmp[4] = 0x20;
	tmp[5] = 0x00;
	tmp[6] = 0x00;
	tmp[7] = 0x80;
	write_data(tmp, 8);

	tmp[0] = CMD_INT_SPD_SET;
	tmp[1] = 0x20;
	tmp[2] = 0x3F;
	tmp[3] = 0x00;
	tmp[4] = 0x01;
	write_data(tmp, 5);

	tmp[0] = CMD_DIM_LVL_SET;
	tmp[1] = 0x00;
	tmp[2] = 0x66;
	write_data(tmp, 3);

	tmp[0] = CMD_CLEAR_GRAM;
	write_data(tmp, 1);

	tmp[0] = CMD_DIP_MODE_SET;
	tmp[1] = 0;
	write_data(tmp, 2);

	tmp[0] = CMD_FRM_SYNC_SET;
	tmp[1] = 0;
	write_data(tmp, 2);

	ESP_LOGI(TAG, "Init done");

	vTaskDelay(400 / portTICK_PERIOD_MS);
}

static void
gp1287bi_hard_reset(void)
{
	gpio_set_level(PIN_VFD_RESET, 0);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	gpio_set_level(PIN_VFD_RESET, 1);
	vTaskDelay(10 / portTICK_PERIOD_MS);
}

void
gp1287bi_draw(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t *pixels)
{
	ESP_LOGD(TAG, "Drawing partial (%d, %d) -> (%d, %d)", x1, y1, x2, y2);

	uint16_t rows = y2 - y1 + 1;
	uint16_t cols = x2 - x1 + 1;

	uint32_t size = (rows >> 3) * cols;

	gp1287bi_write_gram.cmd = CMD_WRITE_GRAM;
	gp1287bi_write_gram.x = x1;
	gp1287bi_write_gram.y = y1;
	gp1287bi_write_gram.rows = rows - 1;
	memcpy(gp1287bi_write_gram.buf, pixels, size);
	write_data(&gp1287bi_write_gram, 4 + size);
}

static void IRAM_ATTR
hspi_write(void *buf, uint32_t len)
{
	spi_transaction_t trans = {
		.tx_buffer = buf,
		.length = len * 8,
	};

	ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &trans));
}

static void IRAM_ATTR
write_data(void *buf, uint32_t len)
{
	hspi_write(buf, len);
}

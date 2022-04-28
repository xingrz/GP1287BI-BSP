#ifndef __DEVICE_GP1287BI__
#define __DEVICE_GP1287BI__

#include <stdint.h>

#define GP1287BI_WIDTH 256
#define GP1287BI_HEIGHT 56

#define GP1287BI_BUF_CNT (GP1287BI_WIDTH * GP1287BI_HEIGHT)
#define GP1287BI_BUF_LEN (GP1287BI_WIDTH * (GP1287BI_HEIGHT >> 3))

void gp1287bi_init(void);
void gp1287bi_draw(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t *pixels);

#endif  // __DEVICE_GP1287BI__

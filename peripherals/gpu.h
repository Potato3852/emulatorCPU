#ifndef GPU_H
#define GPU_H

#include <stdint.h>

#define GPU_WIDTH 64
#define GPU_HEIGHT 32
#define GPU_VRAM_START 0xF000
#define GPU_VRAM_SIZE (GPU_WIDTH * GPU_HEIGHT / 8)  // 256B

struct GPU {
    uint8_t vram[GPU_VRAM_SIZE];
    uint8_t dirty;
};

void gpu_init(struct GPU* gpu);
void gpu_clear(struct GPU* gpu);
void gpu_draw_pixel(struct GPU* gpu, uint8_t x, uint8_t y, uint8_t color);
void gpu_render_console(struct GPU* gpu);

#endif
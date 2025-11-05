#include "gpu.h"
#include <stdio.h>
#include <string.h>

void gpu_init(struct GPU* gpu) {
    memset(gpu->vram, 0, GPU_VRAM_SIZE);
    gpu->dirty = 1;
}

void gpu_clear(struct GPU* gpu) {
    memset(gpu->vram, 0, GPU_VRAM_SIZE);
    gpu->dirty = 1;
}

void gpu_draw_pixel(struct GPU* gpu, uint8_t x, uint8_t y, uint8_t color) {
    if (x >= GPU_WIDTH || y >= GPU_HEIGHT) return;
    
    uint16_t byte_index = (y * (GPU_WIDTH / 8)) + (x / 8);
    uint8_t bit_index = 7 - (x % 8);
    
    if (color) {
        gpu->vram[byte_index] |= (1 << bit_index);
    } else {
        gpu->vram[byte_index] &= ~(1 << bit_index);
    }
    gpu->dirty = 1;
}

void gpu_render_console(struct GPU* gpu) {
    printf("\n=== GPU DISPLAY (64x32) ===\n");
    for (int y = 0; y < GPU_HEIGHT; y++) {
        for (int x = 0; x < GPU_WIDTH; x += 8) {
            uint16_t byte_index = (y * (GPU_WIDTH / 8)) + (x / 8);
            uint8_t byte = gpu->vram[byte_index];
            
            for (int bit = 0; bit < 8; bit++) {
                printf("%c", (byte >> (7 - bit)) & 1 ? '#' : ' ');
            }
        }
        printf("\n");
    }
    printf("===========================\n");
}
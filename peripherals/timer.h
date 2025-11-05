#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TIMER_MMIO 0xFF00

#define TIMER_CONTROL  0x00
#define TIMER_INTERVAL 0x01
#define TIMER_COUNTER  0x03

#define TIMER_ENABLE (1 << 0)
#define TIMER_IRQ_ENABLE (1 << 1)
#define TIMER_IRQ_PENDING (1 << 2)

struct Timer {
    uint16_t interval;
    uint16_t counter;
    uint8_t control;
    uint8_t irq_vector;
};

void timer_init(struct Timer* timer, uint8_t irq_vector);
void timer_tick(struct Timer* timer);
uint8_t timer_read(struct Timer* timer, uint16_t addr);
void timer_write(struct Timer* timer, uint16_t addr, uint8_t value);

#endif
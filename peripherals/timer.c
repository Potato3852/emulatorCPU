#include "timer.h"
#include <stdio.h>

void timer_init(struct Timer* timer, uint8_t irq_vector) {
    timer->interval = 100;
    timer->counter = 100;
    timer->control = 0;
    timer->irq_vector = irq_vector;
}

void timer_tick(struct Timer* timer) {
    if (!(timer->control & TIMER_ENABLE)) {
        printf("TIMER: disabled\n");
        return;
    }
    
    if (timer->counter > 0) {
        timer->counter--;

        // DEBUG::
        printf("TIMER TICK: counter=%d\n", timer->counter); 
        
        if (timer->counter == 0) {
            // DEBUG::
            printf("TIMER EXPIRED! IRQ pending\n"); 

            if (timer->control & TIMER_IRQ_ENABLE) {
                timer->control |= TIMER_IRQ_PENDING;
            }

            timer->counter = timer->interval;
        }
    }
}

uint8_t timer_read(struct Timer* timer, uint16_t addr) {
    uint16_t reg = addr - TIMER_MMIO;
    
    switch (reg) {
        case TIMER_CONTROL:
            return timer->control;
        case TIMER_INTERVAL:
            return timer->interval & 0xFF;
        case TIMER_INTERVAL + 1:
            return (timer->interval >> 8) & 0xFF;
        case TIMER_COUNTER:
            return timer->counter & 0xFF;
        case TIMER_COUNTER + 1:
            return (timer->counter >> 8) & 0xFF;
        default:
            return 0;
    }
}

void timer_write(struct Timer* timer, uint16_t addr, uint8_t value) {
    uint16_t reg = addr - TIMER_MMIO;
    // DEBUG::
    // printf("TIMER WRITE: reg=0x%02X, value=0x%02X\n", reg, value);

    if (reg == TIMER_CONTROL) {
        printf(">>> TIMER CONTROL: enable=%d, irq=%d\n", 
               (value & TIMER_ENABLE) ? 1 : 0,
               (value & TIMER_IRQ_ENABLE) ? 1 : 0);
    }
    
    switch (reg) {
        case TIMER_CONTROL: {
            timer->control = value;
            if (value & TIMER_ENABLE && timer->counter == 0) {
                timer->counter = timer->interval;
            }
            break;
        }

        case TIMER_INTERVAL: {
            timer->interval = (timer->interval & 0xFF00) | value;
            break;
        }

        case TIMER_INTERVAL + 1: {
            timer->interval = (timer->interval & 0x00FF) | (value << 8);
            break;
        }

        case TIMER_COUNTER: {
            timer->counter = (timer->counter & 0xFF00) | value;
            break;
        }

        case TIMER_COUNTER + 1: {
            timer->counter = (timer->counter & 0x00FF) | (value << 8);
            break;
        }
    }
}
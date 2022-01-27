#include <kernel/hal/common/pit.h>
#include <kernel/core/kernel.h>

/// @internal pit frequency range
#define PIT_FREQ_MIN  30
#define PIT_FREQ_MAX  5000

/// @internal pit ports
#define PIT_CMD  0x43
#define PIT_DATA 0x40

/// @internal pit properties
irq_handler_t _pit_handler;
uint32_t      pit_freq;
uint64_t      pit_ticks;
uint32_t      pit_switch_freq;
uint32_t      pit_switch_timer;
uint32_t      pit_timer;
uint32_t      pit_millis, pit_seconds;
uint32_t      pit_millis_total;
uint32_t      pit_precise_timer;

void pit_init(uint32_t freq, irq_handler_t handler)
{
    // validate handler
    if (handler == NULL) { debug_error("Tried to initialize PIT with null handler"); return; }
    _pit_handler = handler;

    // clamp and set frequency 
    pit_freq         = clamp(freq, PIT_FREQ_MIN, PIT_FREQ_MAX);

    // reset other values
    pit_ticks        = 0;
    pit_timer        = 0;
    pit_switch_timer = 0;
    pit_switch_freq  = pit_freq / 8;

    // calculate actual frequency
    uint32_t f = 1193180 / pit_freq;
    uint8_t high = (uint8_t)((f & 0xFF00) >> 8);
    uint8_t low  = (uint8_t)((f & 0x00FF));

    // send frequency to pit
    port_outb(PIT_CMD, 0x36);
    port_outb(PIT_DATA, low);
    port_outb(PIT_DATA, high);

    // register interrupt
    irq_register(IRQ0, _pit_handler);

    // finished
    debug_ok("Initialized PIT at %d Hz", pit_freq);
}

void pit_disable()
{
    irq_unregister(IRQ0);
}

void pit_callback(idt_regs_t* regs)
{
    pit_calculate();

    pit_switch_timer++;
    if (pit_switch_timer >= pit_switch_freq)
    {
        pit_switch_timer = 0;
        if (PROCMGR.pit_ready) 
        { 
            //printf("FORCED SWITCH\n");
            procmgr_schedule(false); 
        }
    }
}

void pit_calculate()
{
    pit_ticks++;

    if (pit_freq < 1000)
    {
        pit_timer++;

        uint32_t millis_inc = 1000 / pit_freq;
        pit_millis += millis_inc;
        pit_millis_total += millis_inc;

        if (pit_timer >= pit_freq) { pit_millis = 0; pit_timer = 0; }
    }
    else
    {
        pit_precise_timer++;
        if (pit_precise_timer >= pit_freq / 1000) 
        { 
            pit_millis++;
            pit_millis_total++;
            pit_timer++; 
            pit_precise_timer = 0; 
        }

        if (pit_timer >= 1000) 
        { 
            pit_seconds++;
            pit_millis = 0; 
            pit_timer = 0; 
        }
    }
}

uint64_t pit_get_ticks() { return pit_ticks; }

uint32_t pit_get_millis() { return pit_millis; }

uint64_t pit_get_millis_total() { return pit_millis_total; }

uint32_t pit_get_seconds() { return pit_seconds; }
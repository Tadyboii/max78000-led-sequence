#include <stdio.h>
#include "mxc_device.h"
#include "gpio.h"
#include "mxc_delay.h"
#include <stdlib.h>

// -----------------------------
// LED Structure
// -----------------------------
typedef struct {
    mxc_gpio_regs_t* port;
    uint32_t pin;
} LED_t;

// -----------------------------
// LED Pin Assignments
// -----------------------------
LED_t leds[9] = {
    { MXC_GPIO2, MXC_GPIO_PIN_3 },
    { MXC_GPIO2, MXC_GPIO_PIN_4 },
    { MXC_GPIO1, MXC_GPIO_PIN_1 },
    { MXC_GPIO1, MXC_GPIO_PIN_0 },
    { MXC_GPIO0, MXC_GPIO_PIN_7 },
    { MXC_GPIO0, MXC_GPIO_PIN_5 },
    { MXC_GPIO0, MXC_GPIO_PIN_6 },
    { MXC_GPIO2, MXC_GPIO_PIN_6 },
    { MXC_GPIO2, MXC_GPIO_PIN_7 }
};

// -----------------------------
void setLED(int index, int state) {
    if (index < 1 || index > 9) return;
    LED_t led = leds[index - 1];
    if (state) MXC_GPIO_OutSet(led.port, led.pin);
    else       MXC_GPIO_OutClr(led.port, led.pin);
}

void turnLEDs(int start, int size) {
    for (int i = 1; i <= 9; i++) setLED(i, 0);
    for (int i = start; i < start + size && i <= 9; i++) setLED(i, 1);
}

// -----------------------------
// PB1 = P0.2 (active LOW)
int readButton(void) {
    return MXC_GPIO_InGet(MXC_GPIO0, MXC_GPIO_PIN_2);
}

// -----------------------------
// GLOBAL STATE
int mode = 0;
int lastBtn = 1;

// -----------------------------
// MAIN
int main(void) {
    srand(12345);

    // Configure LEDs
    MXC_GPIO_Config(&(mxc_gpio_cfg_t){ MXC_GPIO0,
        MXC_GPIO_PIN_7 | MXC_GPIO_PIN_5 | MXC_GPIO_PIN_6,
        MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE,
        MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 });

    MXC_GPIO_Config(&(mxc_gpio_cfg_t){ MXC_GPIO1,
        MXC_GPIO_PIN_1 | MXC_GPIO_PIN_0,
        MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE,
        MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 });

    MXC_GPIO_Config(&(mxc_gpio_cfg_t){ MXC_GPIO2,
        MXC_GPIO_PIN_3 | MXC_GPIO_PIN_4 | MXC_GPIO_PIN_6 | MXC_GPIO_PIN_7,
        MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE,
        MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 });

    // Configure PB1 button
    MXC_GPIO_Config(&(mxc_gpio_cfg_t){
        MXC_GPIO0,
        MXC_GPIO_PIN_2,
        MXC_GPIO_FUNC_IN,
        MXC_GPIO_PAD_PULL_UP,
        MXC_GPIO_VSSEL_VDDIOH,
        MXC_GPIO_DRVSTR_0 });

    // Mode 0 state
    int pos = 5;

    // Mode 1 state
    int b_pos = 1, direction = 1;
    int size = 1, growing = 1;

    while (1) {
        // -----------------------------
        // Check button every loop
        int btn = readButton();
        if (lastBtn == 4 && btn == 0) {
            mode ^= 1;          // toggle mode
            MXC_Delay(150000);  // simple debounce

            // RESET ALL LEDS
            for (int i = 1; i <= 9; i++) {
                setLED(i, 0);
            }

            // RESET mode-specific states
            if (mode == 0) {
                pos = 5;        // reset random walker position
            } else {
                b_pos = 1;      // reset ping-pong block start
                direction = 1;
                size = 1;
                growing = 1;
            }
        }
        lastBtn = btn;

        //serial print the button state
        // printf("Button state: %d\n", btn);

        // -----------------------------
        // Mode 0: Random walker
        if (mode == 0) {
            setLED(pos, 1);
            MXC_Delay(120000);
            setLED(pos, 0);

            int dir = rand() % 2;
            if (dir == 0) { if (pos > 1) pos--; else pos++; }
            else          { if (pos < 9) pos++; else pos--; }
        }

        // -----------------------------
        // Mode 1: Ping-pong block
        else {
            turnLEDs(b_pos, size);
            MXC_Delay(100000);

            b_pos += direction;

            if (direction == 1 && b_pos + size - 1 >= 9) {
                direction = -1;
                if (growing) { if (size < 9) size++; else growing = 0; }
                else         { size--; if (size == 1) growing = 1; }
                b_pos = 9 - size + 1;
            }
            else if (direction == -1 && b_pos <= 1) {
                direction = 1;
                if (growing) { if (size < 9) size++; else growing = 0; }
                else         { size--; if (size == 1) growing = 1; }
                b_pos = 1;
            }
        }
    }

    return 0;
}

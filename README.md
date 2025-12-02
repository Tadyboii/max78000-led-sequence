# MAX78000 LED Sequence Project

This project runs two LED animation modes on the MAX78000 using 9 GPIO-controlled LEDs and one button (PB1 – P0.2). Pressing PB1 toggles between modes.

## Modes
**Mode 0 – Random Walker**  
A single LED moves left/right randomly across LEDs 1–9.

**Mode 1 – Ping-Pong Block**  
A block of LEDs moves left ↔ right, expanding at edges until size 9, then shrinking.

## Hardware (MAX78000 EVKIT)
**LED Mapping:**  
1: P2.3, 2: P2.4, 3: P1.1, 4: P1.0, 5: P0.7, 6: P0.5, 7: P0.6, 8: P2.6, 9: P2.7  
**Button:** PB1 → P0.2 (active LOW)

## Behavior
- PB1 toggles between Mode 0 and Mode 1  
- LEDs update immediately; debouncing included  

## Build
Use the MSDK User Guide:  
https://analogdevicesinc.github.io/msdk/USERGUIDE/

## Notes
All logic (GPIO config, LED control, animations, button handling) is implemented in `main.c`.

Bloenk Firmware
========
This is the firmware for https://github.com/johannes85/bloenk-hardware (or https://github.com/jabertwo/bloenk-hardware). It is at its base a rewrite of the old firmware.

# Building
## Requirements
- [PlatformIO](https://platformio.org/)
- git

## Targets
There are two targets currently.
- **Generic**: This version can handle at most 128 LEDs. The actual count you can control via USB and change dynamically.
- **Fixed**: This version has a static LED count.  
  - **Setting the LED count**  
    To control the LED count change the `led_count` in the `[fixed]` section of the `platformio.ini` file.

## Compiling
- **To build all targets**  
  `pio run`
- **Build the generic version**  
  `pio run -e attiny85-20su`
- **Build the fixed LED count version**  
  `pio run -e attiny85-20su-fixed`

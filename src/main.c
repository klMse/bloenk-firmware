#include <avr/wdt.h>

int __attribute__((noreturn)) main(void) {
    wdt_enable(WDTO_1S);

    while (1) {
        wdt_reset();
    }
}

#include "light_ws2812.h"
#include "requests.h"
#include "usbdrv.h"

#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>

#ifndef BLOENK_LED_COUNT
#    define LED_MAX_COUNT 128
#    define LED_DEFAULT_COUNT 4
#else
#    define LED_MAX_COUNT BLOENK_LED_COUNT
#    define LED_DEFAULT_COUNT BLOENK_LED_COUNT
#endif

enum bloenk_version {
    BLOENK_LEGACY,
};

struct bloenk_legacy {
    uint8_t current_led;
};

struct bloenk {
    struct cRGB leds[LED_MAX_COUNT];
    uint8_t led_count;
    enum bloenk_version version;
    union {
        struct bloenk_legacy legacy;
    };
};

uint8_t EE_led_count EEMEM = 1;
static struct bloenk bloenk = {
    .version = BLOENK_LEGACY
};

static usbMsgLen_t handle_legacy(const usbRequest_t* const req) {
    uint8_t ret = 0;
    uint8_t val;

    switch (req->bRequest) {
    case RQ_SET_CURRENT_LED:
        val = req->wValue.bytes[0];
        if (val < bloenk.led_count)
            bloenk.legacy.current_led = val;
        break;

    case RQ_SET_COLOR_LED_R:
        val = req->wValue.bytes[0];
        bloenk.leds[bloenk.legacy.current_led].r = val;
        break;

    case RQ_SET_COLOR_LED_G:
        val = req->wValue.bytes[0];
        bloenk.leds[bloenk.legacy.current_led].g = val;
        break;

    case RQ_SET_COLOR_LED_B:
        val = req->wValue.bytes[0];
        bloenk.leds[bloenk.legacy.current_led].b = val;
        break;

    case RQ_WRITE_TO_LEDS:
        ws2812_setleds(bloenk.leds, bloenk.led_count);
        break;

#ifndef BLOENK_LED_COUNT
    case RQ_SET_LED_COUNT:
        val = req->wValue.bytes[0];
        if (val == 0 || val >= LED_MAX_COUNT)
            break;

        if (bloenk.led_count != val) {
            bloenk.led_count = val;
            eeprom_update_byte(&EE_led_count, val);
        }
        break;
#endif

    case RQ_GET_LED_COUNT:
        usbMsgPtr = &bloenk.led_count;
        ret = 1;
        break;

    default:
        break;
    }

    return ret;
}

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
    // TODO: Modify the V-USB lib
    usbRequest_t* req = (usbRequest_t*) data;

    if ((req->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_VENDOR)
        return 0;

    if (bloenk.version == BLOENK_LEGACY)
        return handle_legacy(req);

    return 0;
}

int main(void) {
    wdt_enable(WDTO_1S);

    usbInit();
    sei();

#ifndef BLOENK_LED_COUNT
    eeprom_busy_wait();
    bloenk.led_count = eeprom_read_byte(&EE_led_count);
    if (bloenk.led_count == 0xFF)
        bloenk.led_count = LED_DEFAULT_COUNT;
#else
    bloenk.led_count = LED_DEFAULT_COUNT;
#endif

    while (1) {
        wdt_reset();
        usbPoll();
    }
}

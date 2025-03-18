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

struct LED {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct bloenk_legacy {
    uint8_t current_led;
};

struct LEDs {
    struct LED leds[LED_MAX_COUNT];
    uint8_t led_count;
    enum bloenk_version version;
    union {
        struct bloenk_legacy legacy;
    };
};

uint8_t EE_led_count EEMEM = 1;
static struct LEDs leds;

static usbMsgLen_t handle_legacy(usbRequest_t* req) {
    uint8_t ret = 0;
    uint8_t val;

    switch (req->bRequest) {
    case RQ_SET_CURRENT_LED:
        val = req->wValue.bytes[0];
        if (val < leds.led_count)
            leds.legacy.current_led = val;
        break;

    case RQ_SET_COLOR_LED_R:
        val = req->wValue.bytes[0];
        leds.leds[leds.legacy.current_led].r = val;
        break;

    case RQ_SET_COLOR_LED_G:
        val = req->wValue.bytes[0];
        leds.leds[leds.legacy.current_led].g = val;
        break;

    case RQ_SET_COLOR_LED_B:
        val = req->wValue.bytes[0];
        leds.leds[leds.legacy.current_led].b = val;
        break;

    case RQ_WRITE_TO_LEDS:
        break;

#ifndef BLOENK_LED_COUNT
    case RQ_SET_LED_COUNT:
        val = req->wValue.bytes[0];
        if (val == 0 || val >= LED_MAX_COUNT)
            break;

        if (leds.led_count != val) {
            leds.led_count = val;
            eeprom_update_byte(&EE_led_count, val);
        }
        break;
#endif

    case RQ_GET_LED_COUNT:
        usbMsgPtr = &leds.led_count;
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

    if (leds.version == BLOENK_LEGACY)
        return handle_legacy(req);

    return 0;
}

int main(void) {
    wdt_enable(WDTO_1S);

    usbInit();
    sei();

#ifndef BLOENK_LED_COUNT
    eeprom_busy_wait();
    leds.led_count = eeprom_read_byte(&EE_led_count);
    if (leds.led_count == 0xFF)
        leds.led_count = LED_DEFAULT_COUNT;
#else
    leds.led_count = LED_DEFAULT_COUNT;
#endif

    while (1) {
        wdt_reset();
        usbPoll();
    }
}

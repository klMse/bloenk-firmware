#ifndef __REQUESTS_H__
#define __REQUESTS_H__

enum Requests {
    RQ_SET_CURRENT_LED = 0,
    RQ_SET_COLOR_LED_R = 1,
    RQ_SET_COLOR_LED_G = 2,
    RQ_SET_COLOR_LED_B = 3,
    RQ_WRITE_TO_LEDS = 4,
    RQ_SET_LED_COUNT = 10,
    RQ_GET_LED_COUNT = 20,
};

#endif /* __REQUESTS_H__ */

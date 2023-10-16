#pragma once

#include "gps_status.h"

#include <furi_hal.h>
#include <notification/notification_messages.h>

#define RX_BUF_SIZE 1024

static const int gps_baudrates[6] = {4800, 9600, 19200, 38400, 57600, 115200};
static int current_gps_baudrate = 1;

typedef enum { KNOTS, KPH, MPH, INVALID } SpeedUnit;

typedef struct {
    FuriMutex* mutex;
    FuriThread* thread;
    FuriStreamBuffer* rx_stream;
    uint8_t rx_buf[RX_BUF_SIZE];

    NotificationApp* notifications;

    uint32_t baudrate;
    bool changing_baudrate;

    bool gpx_recording;
    float gpx_last_lat;
    float gpx_last_lon;

    bool backlight_on;

    SpeedUnit speed_units;

    GpsStatus status;
} GpsUart;

void gps_uart_init_thread(GpsUart* gps_uart);
void gps_uart_deinit_thread(GpsUart* gps_uart);

GpsUart* gps_uart_enable();

void gps_uart_disable(GpsUart* gps_uart);

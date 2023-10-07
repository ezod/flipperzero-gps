#pragma once

#include <furi_hal.h>
#include <minmea.h>

typedef struct {
    bool valid;
    float latitude;
    float longitude;
    float speed;
    float course;
    float altitude;
    char altitude_units;
    int fix_quality;
    int satellites_tracked;
    struct minmea_date date;
    struct minmea_time time;
} GpsStatus;

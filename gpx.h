#pragma once

#include "gps_status.h"

#define GPX_DISTANCE_THRESHOLD 5.0l

bool gpx_start_recording(GpsStatus* status);
void gpx_stop_recording();

bool gpx_update(GpsStatus* status);

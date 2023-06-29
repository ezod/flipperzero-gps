#include "gps_uart.h"
#include "gps.h"
#include "LatLong-UTMconversion.h"
#include <assets_icons.h>
#include <gps_nmea_icons.h>

#include <furi.h>
#include <gui/gui.h>
#include <string.h>

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

void DrawCoordinatesElements(Canvas* const canvas, GpsUart* gps_uart) {
    switch(coordinateSystem) {
    case CS_LatLong: {
        DrawLatLongCoordinatesElements(canvas, gps_uart);
        break;
    }
    case CS_UTM: {
        DrawUTMCoordinatesElements(canvas, gps_uart);
        break;
    }
    case CS_MGRS:
    case CS_MAX:
    default: {
        break;
    }
    };
};

void DrawLatLongCoordinatesElements(Canvas* const canvas, GpsUart* gps_uart) {
    // Headers
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str_aligned(
        canvas,
        LatitudeHeaderElement.x,
        LatitudeHeaderElement.y,
        AlignCenter,
        AlignBottom,
        LatitudeHeaderElement.str);

    canvas_draw_str_aligned(
        canvas,
        LongitudeHeaderElement.x,
        LongitudeHeaderElement.y,
        AlignCenter,
        AlignBottom,
        LongitudeHeaderElement.str);

    // Values
    canvas_set_font(canvas, FontSecondary);
    char buffer[64];

    if(isnan(gps_uart->status.latitude)) {
        LatitudeValueElement.str = "0.0";
    } else {
        snprintf(buffer, 64, "%f", (double)gps_uart->status.latitude);
        LatitudeValueElement.str = buffer;
    }

    canvas_draw_str_aligned(
        canvas,
        LatitudeValueElement.x,
        LatitudeValueElement.y,
        AlignCenter,
        AlignBottom,
        LatitudeValueElement.str);

    if(isnan(gps_uart->status.longitude)) {
        LongitudeValueElement.str = "0.0";
    } else {
        snprintf(buffer, 64, "%f", (double)gps_uart->status.longitude);
        LongitudeValueElement.str = buffer;
    }

    canvas_draw_str_aligned(
        canvas,
        LongitudeValueElement.x,
        LongitudeValueElement.y,
        AlignCenter,
        AlignBottom,
        LongitudeValueElement.str);
};

void DrawUTMCoordinatesElements(Canvas* const canvas, GpsUart* gps_uart) {
    // Headers
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str_aligned(
        canvas,
        UTMZoneHeaderElement.x,
        UTMZoneHeaderElement.y,
        AlignLeft,
        AlignBottom,
        UTMZoneHeaderElement.str);

    canvas_draw_str_aligned(
        canvas,
        UTMNorthingHeaderElement.x,
        UTMNorthingHeaderElement.y,
        AlignLeft,
        AlignBottom,
        UTMNorthingHeaderElement.str);

    canvas_draw_str_aligned(
        canvas,
        UTMEastingHeaderElement.x,
        UTMEastingHeaderElement.y,
        AlignRight,
        AlignBottom,
        UTMEastingHeaderElement.str);

    // Values
    canvas_set_font(canvas, FontSecondary);

    char buffer[64];

    if(isnan(gps_uart->status.latitude) || isnan(gps_uart->status.longitude)) {
        UTMZoneValueElement.str = "00";
        UTMNorthingValueElement.str = "0000000";
        UTMEastingValueElement.str = "0000000";
    } else {
        // Convert from LatLong to UTM
        struct UTM utm = LLtoUTM(23, gps_uart->status.latitude, gps_uart->status.longitude);

        UTMZoneValueElement.str = &utm.Zone;

        snprintf(buffer, 64, "%i", (int)(utm.Northing));
        UTMNorthingValueElement.str = buffer;

        snprintf(buffer, 64, "%i", (int)(utm.Easting));
        UTMEastingValueElement.str = buffer;
    }

    canvas_draw_str_aligned(
        canvas,
        UTMZoneValueElement.x,
        UTMZoneValueElement.y,
        AlignCenter,
        AlignBottom,
        UTMZoneValueElement.str);

    canvas_draw_str_aligned(
        canvas,
        UTMNorthingValueElement.x,
        UTMNorthingValueElement.y,
        AlignCenter,
        AlignBottom,
        UTMNorthingValueElement.str);

    canvas_draw_str_aligned(
        canvas,
        UTMEastingValueElement.x,
        UTMEastingValueElement.y,
        AlignRight,
        AlignBottom,
        UTMEastingValueElement.str);
};

void DrawCourseElements(Canvas* const canvas, GpsUart* gps_uart) {
    // Header
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas,
        CourseHeaderElement.x,
        CourseHeaderElement.y,
        AlignCenter,
        AlignBottom,
        CourseHeaderElement.str);

    // Value
    char buffer[64];
    canvas_set_font(canvas, FontSecondary);
    snprintf(buffer, 64, "%.1f", (double)gps_uart->status.course);
    CourseValueElement.str = buffer;
    canvas_draw_str_aligned(
        canvas,
        CourseValueElement.x,
        CourseValueElement.y,
        AlignCenter,
        AlignBottom,
        CourseValueElement.str);
};

void DrawSpeedElements(Canvas* const canvas, GpsUart* gps_uart) {
    // Header
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas,
        SpeedHeaderElement.x,
        SpeedHeaderElement.y,
        AlignCenter,
        AlignBottom,
        SpeedHeaderElement.str);

    // Value
    canvas_set_font(canvas, FontSecondary);
    char buffer[64];

    if(!gps_uart->speed_in_kms) {
        snprintf(buffer, 64, "%.2f kn", (double)gps_uart->status.speed);
    } else {
        snprintf(buffer, 64, "%.2f km", (double)(gps_uart->status.speed * 1.852));
    }

    SpeedValueElement.str = buffer;
    canvas_draw_str_aligned(
        canvas,
        SpeedValueElement.x,
        SpeedValueElement.y,
        AlignCenter,
        AlignBottom,
        SpeedValueElement.str);
};

void DrawAltitudeElements(Canvas* const canvas, GpsUart* gps_uart) {
    // Header
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas,
        AltitudeHeaderElement.x,
        AltitudeHeaderElement.y,
        AlignCenter,
        AlignBottom,
        AltitudeHeaderElement.str);

    // Value
    char buffer[64];
    canvas_set_font(canvas, FontSecondary);

    if(isnan(gps_uart->status.altitude) || gps_uart->status.altitude <= 0.0) {
        snprintf(buffer, 64, "0 %c", tolower(gps_uart->status.altitude_units));
    } else {
        snprintf(
            buffer,
            64,
            "%.1f %c",
            (double)gps_uart->status.altitude,
            tolower(gps_uart->status.altitude_units));
    };

    AltitudeValueElement.str = buffer;
    canvas_draw_str_aligned(
        canvas,
        AltitudeValueElement.x,
        AltitudeValueElement.y,
        AlignCenter,
        AlignBottom,
        AltitudeValueElement.str);
};

void DrawSatelliteElements(Canvas* const canvas, GpsUart* gps_uart) {
    // Header
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas,
        SatellitesHeaderElement.x,
        SatellitesHeaderElement.y,
        AlignLeft,
        AlignBottom,
        SatellitesHeaderElement.str);

    // Value
    char buffer[64];
    canvas_set_font(canvas, FontSecondary);
    snprintf(buffer, 64, "%d", gps_uart->status.satellites_tracked);
    SatellitesValueElement.str = buffer;
    canvas_draw_str_aligned(
        canvas,
        SatellitesValueElement.x,
        SatellitesValueElement.y,
        AlignCenter,
        AlignBottom,
        SatellitesValueElement.str);
};

void DrawLastFixElements(Canvas* const canvas, GpsUart* gps_uart) {
    // Header
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas,
        LastFixHeaderElement.x,
        LastFixHeaderElement.y,
        AlignLeft,
        AlignBottom,
        LastFixHeaderElement.str);

    // Value
    char buffer[64];
    canvas_set_font(canvas, FontSecondary);
    snprintf(
        buffer,
        64,
        "%02d:%02d:%02d UTC",
        gps_uart->status.time_hours,
        gps_uart->status.time_minutes,
        gps_uart->status.time_seconds);

    LastfixValueElement.str = buffer;
    canvas_draw_str_aligned(
        canvas,
        LastfixValueElement.x,
        LastfixValueElement.y,
        AlignCenter,
        AlignBottom,
        LastfixValueElement.str);
};

void DrawAcquiringFix(Canvas* const canvas, GpsUart* gps_uart) {
    canvas_draw_icon_animation(canvas, 114, 52, gps_uart->Icon);
    icon_animation_start(gps_uart->Icon);
};

void DrawAcquiredFix(Canvas* const canvas) {
    canvas_draw_icon(canvas, 114, 52, &I_GPS_10px);
};

void SetupCanvasElements() {
    LatitudeHeaderElement.x = 32;
    LatitudeHeaderElement.y = 8;
    LatitudeHeaderElement.str = "Latitude";

    LongitudeHeaderElement.x = 96;
    LongitudeHeaderElement.y = 8;
    LongitudeHeaderElement.str = "Longitude";

    UTMZoneHeaderElement.x = 2;
    UTMZoneHeaderElement.y = 8;
    UTMZoneHeaderElement.str = "Zone";

    UTMNorthingHeaderElement.x = 32;
    UTMNorthingHeaderElement.y = 8;
    UTMNorthingHeaderElement.str = "Northing";

    UTMEastingHeaderElement.x = 124;
    UTMEastingHeaderElement.y = 8;
    UTMEastingHeaderElement.str = "Easting";

    CourseHeaderElement.x = 21;
    CourseHeaderElement.y = 30;
    CourseHeaderElement.str = "Course";

    SpeedHeaderElement.x = 64;
    SpeedHeaderElement.y = 30;
    SpeedHeaderElement.str = "Speed";

    AltitudeHeaderElement.x = 107;
    AltitudeHeaderElement.y = 30;
    AltitudeHeaderElement.str = "Altitude";

    SatellitesHeaderElement.x = 2;
    SatellitesHeaderElement.y = 52;
    SatellitesHeaderElement.str = "Satellites";

    LastFixHeaderElement.x = 60;
    LastFixHeaderElement.y = 52;
    LastFixHeaderElement.str = "Last Fix";

    LatitudeValueElement.x = 32;
    LatitudeValueElement.y = 18;
    LatitudeValueElement.str = "Latitude:";

    LongitudeValueElement.x = 96;
    LongitudeValueElement.y = 18;
    LongitudeValueElement.str = "Longitude";

    UTMZoneValueElement.x = 10;
    UTMZoneValueElement.y = 18;
    UTMZoneValueElement.str = "";

    UTMNorthingValueElement.x = 52;
    UTMNorthingValueElement.y = 18;
    UTMNorthingValueElement.str = "";

    UTMEastingValueElement.x = 122;
    UTMEastingValueElement.y = 18;
    UTMEastingValueElement.str = "";

    CourseValueElement.x = 21;
    CourseValueElement.y = 40;
    CourseValueElement.str = "Course";

    SpeedValueElement.x = 64;
    SpeedValueElement.y = 40;
    SpeedValueElement.str = "Speed";

    AltitudeValueElement.x = 107;
    AltitudeValueElement.y = 40;
    AltitudeValueElement.str = "Altitude";

    SatellitesValueElement.x = 20;
    SatellitesValueElement.y = 62;
    SatellitesValueElement.str = "Satellites";

    LastfixValueElement.x = 80;
    LastfixValueElement.y = 62;
    LastfixValueElement.str = "Last Fix";
};

void IncrementCoordinateSystem() {
    ++coordinateSystem;

    // TODO: once MGRS is implemented change check to CS_MAX instead of CS_MGRS
    if(coordinateSystem == CS_MGRS) {
        coordinateSystem = CS_LatLong;
    }
};

static void render_callback(Canvas* const canvas, void* context) {
    furi_assert(context);
    GpsUart* gps_uart = context;
    furi_mutex_acquire(gps_uart->mutex, FuriWaitForever);

    if(!gps_uart->changing_baudrate) {
        if(gps_uart->status.fix_quality <= 0) {
            DrawAcquiringFix(canvas, gps_uart);
        } else {
            DrawAcquiredFix(canvas);
        }
        DrawCoordinatesElements(canvas, gps_uart);
        DrawCourseElements(canvas, gps_uart);
        DrawSpeedElements(canvas, gps_uart);
        DrawAltitudeElements(canvas, gps_uart);
        DrawSatelliteElements(canvas, gps_uart);
        DrawLastFixElements(canvas, gps_uart);
    } else {
        char buffer[64];
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignBottom, "Baudrate set to:");

        snprintf(buffer, 64, "%ld baud", gps_uart->baudrate);
        canvas_draw_str_aligned(canvas, 64, 47, AlignCenter, AlignBottom, buffer);
    }

    furi_mutex_release(gps_uart->mutex);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

int32_t gps_app(void* p) {
    UNUSED(p);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    GpsUart* gps_uart = gps_uart_enable();

    gps_uart->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!gps_uart->mutex) {
        FURI_LOG_E("GPS", "cannot create mutex\r\n");
        free(gps_uart);
        return 255;
    }

    SetupCanvasElements();
    gps_uart->Icon = icon_animation_alloc(&A_Sub1ghz_14);

    // set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, gps_uart);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        furi_mutex_acquire(gps_uart->mutex, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypeShort) {
                    switch(event.input.key) {
                    case InputKeyUp:
                    case InputKeyDown:
                    case InputKeyRight:
                    case InputKeyLeft:
                    case InputKeyBack:
                        break;
                    case InputKeyOk:
                        if(!gps_uart->backlight_on) {
                            notification_message_block(
                                gps_uart->notifications, &sequence_display_backlight_enforce_on);
                            gps_uart->backlight_on = true;
                        } else {
                            notification_message_block(
                                gps_uart->notifications, &sequence_display_backlight_enforce_auto);
                            notification_message(
                                gps_uart->notifications, &sequence_display_backlight_off);
                            gps_uart->backlight_on = false;
                        }
                        break;
                    default:
                        break;
                    }
                } else if(event.input.type == InputTypeLong) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        gps_uart_deinit_thread(gps_uart);
                        const int baudrate_length =
                            sizeof(gps_baudrates) / sizeof(gps_baudrates[0]);
                        current_gps_baudrate++;
                        if(current_gps_baudrate >= baudrate_length) {
                            current_gps_baudrate = 0;
                        }
                        gps_uart->baudrate = gps_baudrates[current_gps_baudrate];

                        gps_uart_init_thread(gps_uart);
                        gps_uart->changing_baudrate = true;
                        view_port_update(view_port);
                        furi_mutex_release(gps_uart->mutex);
                        break;
                    case InputKeyRight:
                        if(gps_uart->speed_in_kms) {
                            gps_uart->speed_in_kms = false;
                        } else {
                            gps_uart->speed_in_kms = true;
                        }
                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    case InputKeyDown:
                        IncrementCoordinateSystem();
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        if(!gps_uart->changing_baudrate) {
            view_port_update(view_port);
            furi_mutex_release(gps_uart->mutex);
        } else {
            furi_delay_ms(1000);
            gps_uart->changing_baudrate = false;
        }
    }

    notification_message_block(gps_uart->notifications, &sequence_display_backlight_enforce_auto);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_mutex_free(gps_uart->mutex);
    gps_uart_disable(gps_uart);

    return 0;
}

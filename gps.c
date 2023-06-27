#include "gps_uart.h"
#include "gps.h"

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

static void draw_headers(Canvas* const canvas) {
    canvas_set_font(canvas, FontPrimary);

    // canvas_draw_str_aligned(
    //     canvas,
    //     latitude_header_coordinates.x,
    //     latitude_header_coordinates.y,
    //     AlignCenter,
    //     AlignBottom,
    //     latitude_header_coordinates.str);

    // canvas_draw_str_aligned(
    //     canvas,
    //     longitude_header_coordinates.x,
    //     longitude_header_coordinates.y,
    //     AlignCenter,
    //     AlignBottom,
    //     longitude_header_coordinates.str);

    canvas_draw_str(
        canvas,
        latitude_header_coordinates.x,
        latitude_header_coordinates.y,
        latitude_header_coordinates.str);
    canvas_draw_str(
        canvas,
        longitude_header_coordinates.x,
        longitude_header_coordinates.y,
        longitude_header_coordinates.str);

    canvas_draw_str_aligned(
        canvas,
        course_header_coordinates.x,
        course_header_coordinates.y,
        AlignCenter,
        AlignBottom,
        course_header_coordinates.str);

    canvas_draw_str_aligned(
        canvas,
        speed_header_coordinates.x,
        speed_header_coordinates.y,
        AlignCenter,
        AlignBottom,
        speed_header_coordinates.str);

    canvas_draw_str_aligned(
        canvas,
        altitude_header_coordinates.x,
        altitude_header_coordinates.y,
        AlignCenter,
        AlignBottom,
        altitude_header_coordinates.str);

    canvas_draw_str_aligned(
        canvas,
        satellites_header_coordinates.x,
        satellites_header_coordinates.y,
        AlignCenter,
        AlignBottom,
        satellites_header_coordinates.str);

    canvas_draw_str_aligned(
        canvas,
        lastFix_header_coordinates.x,
        lastFix_header_coordinates.y,
        AlignCenter,
        AlignBottom,
        lastFix_header_coordinates.str);
};

static void draw_values(Canvas* const canvas, GpsUart* gps_uart) {
    canvas_set_font(canvas, FontSecondary);

    char buffer[64];
    // latitude
    snprintf(buffer, 64, "%f", (double)gps_uart->status.latitude);
    latitude_value_coordinates.str = buffer;
    canvas_draw_str_aligned(
        canvas,
        latitude_value_coordinates.x,
        latitude_value_coordinates.y,
        AlignCenter,
        AlignBottom,
        latitude_value_coordinates.str);

    // longitude
    snprintf(buffer, 64, "%f", (double)gps_uart->status.longitude);
    longitude_value_coordinates.str = buffer;
    canvas_draw_str_aligned(
        canvas,
        longitude_value_coordinates.x,
        longitude_value_coordinates.y,
        AlignCenter,
        AlignBottom,
        longitude_value_coordinates.str);

    // course
    snprintf(buffer, 64, "%.1f", (double)gps_uart->status.course);
    course_value_coordinates.str = buffer;
    canvas_draw_str_aligned(
        canvas,
        course_value_coordinates.x,
        course_value_coordinates.y,
        AlignCenter,
        AlignBottom,
        course_value_coordinates.str);

    // speed
    if(!gps_uart->speed_in_kms) {
        snprintf(buffer, 64, "%.2f kn", (double)gps_uart->status.speed);
    } else {
        snprintf(buffer, 64, "%.2f km", (double)(gps_uart->status.speed * 1.852));
    }

    speed_value_coordinates.str = buffer;
    canvas_draw_str_aligned(
        canvas,
        speed_value_coordinates.x,
        speed_value_coordinates.y,
        AlignCenter,
        AlignBottom,
        speed_value_coordinates.str);

    // altitude
    snprintf(
        buffer,
        64,
        "%.1f %c",
        (double)gps_uart->status.altitude,
        tolower(gps_uart->status.altitude_units));

    altitude_value_coordinates.str = buffer;
    canvas_draw_str_aligned(
        canvas,
        altitude_value_coordinates.x,
        altitude_value_coordinates.y,
        AlignCenter,
        AlignBottom,
        altitude_value_coordinates.str);

    // satellites
    snprintf(buffer, 64, "%d", gps_uart->status.satellites_tracked);
    satellites_value_coordinates.str = buffer;
    canvas_draw_str_aligned(
        canvas,
        satellites_value_coordinates.x,
        satellites_value_coordinates.y,
        AlignCenter,
        AlignBottom,
        satellites_value_coordinates.str);

    // last fix
    snprintf(
        buffer,
        64,
        "%02d:%02d:%02d UTC",
        gps_uart->status.time_hours,
        gps_uart->status.time_minutes,
        gps_uart->status.time_seconds);

    lastfix_value_coordinates.str = buffer;
    canvas_draw_str_aligned(
        canvas,
        lastfix_value_coordinates.x,
        lastfix_value_coordinates.y,
        AlignCenter,
        AlignBottom,
        lastfix_value_coordinates.str);
};

static void render_callback(Canvas* const canvas, void* context) {
    furi_assert(context);
    GpsUart* gps_uart = context;
    furi_mutex_acquire(gps_uart->mutex, FuriWaitForever);

    if(!gps_uart->changing_baudrate) {
        draw_headers(canvas);
        draw_values(canvas, gps_uart);
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

    setup_canvas_coordinates();

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

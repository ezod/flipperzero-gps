

struct canvas_coordinates {
    int x;

    int y;

    const char* str;
} latitude_header_coordinates, longitude_header_coordinates, course_header_coordinates,
    speed_header_coordinates, altitude_header_coordinates, satellites_header_coordinates,
    lastFix_header_coordinates, latitude_value_coordinates, longitude_value_coordinates,
    course_value_coordinates, speed_value_coordinates, altitude_value_coordinates,
    satellites_value_coordinates, lastfix_value_coordinates;

static void setup_canvas_coordinates() {
    latitude_header_coordinates.x = 2;
    latitude_header_coordinates.y = 8;
    latitude_header_coordinates.str = "Latitude";

    // longitude_header_coordinates.x = 96;
    // longitude_header_coordinates.y = 8;
    longitude_header_coordinates.x = 2;
    longitude_header_coordinates.y = 18;
    longitude_header_coordinates.str = "Longitude:";

    course_header_coordinates.x = 2;
    course_header_coordinates.y = 30;
    course_header_coordinates.str = "Course";

    speed_header_coordinates.x = 64;
    speed_header_coordinates.y = 30;
    speed_header_coordinates.str = "Speed";

    altitude_header_coordinates.x = 107;
    altitude_header_coordinates.y = 30;
    altitude_header_coordinates.str = "Altitude";

    satellites_header_coordinates.x = 32;
    satellites_header_coordinates.y = 52;
    satellites_header_coordinates.str = "Satellites";

    lastFix_header_coordinates.x = 96;
    lastFix_header_coordinates.y = 52;
    lastFix_header_coordinates.str = "Last Fix";

    // latitude_value_coordinates.x = 32;
    // latitude_value_coordinates.y = 18;
    latitude_value_coordinates.x = 80;
    latitude_value_coordinates.y = 8;
    latitude_value_coordinates.str = "Latitude:";

    // longitude_value_coordinates.x = 96;
    // longitude_value_coordinates.y = 18;
    longitude_value_coordinates.x = 85;
    longitude_value_coordinates.y = 18;
    longitude_value_coordinates.str = "Longitude";

    course_value_coordinates.x = 21;
    course_value_coordinates.y = 40;
    course_value_coordinates.str = "Course";

    speed_value_coordinates.x = 64;
    speed_value_coordinates.y = 40;
    speed_value_coordinates.str = "Speed";

    altitude_value_coordinates.x = 107;
    altitude_value_coordinates.y = 40;
    altitude_value_coordinates.str = "Altitude";

    satellites_value_coordinates.x = 32;
    satellites_value_coordinates.y = 62;
    satellites_value_coordinates.str = "Satellites";

    lastfix_value_coordinates.x = 96;
    lastfix_value_coordinates.y = 62;
    lastfix_value_coordinates.str = "Last Fix";
};

// struct canvas_coordinates CanvasCoordinateArray[14] = {latitude_header_coordinates};
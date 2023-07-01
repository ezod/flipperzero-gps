#include <gui/gui.h>

enum CoordinateSystem { CS_LatLong = 0, CS_UTM = 1, CS_MGRS = 2, CS_MAX = 3 };

struct CanvasElement {
    int x;

    int y;

    char* str;
} LatitudeHeaderElement, LongitudeHeaderElement, CourseHeaderElement, SpeedHeaderElement,
    AltitudeHeaderElement, SatellitesHeaderElement, LastFixHeaderElement, LatitudeValueElement,
    LongitudeValueElement, CourseValueElement, SpeedValueElement, AltitudeValueElement,
    SatellitesValueElement, LastfixValueElement, UTMZoneHeaderElement, UTMNorthingHeaderElement,
    UTMEastingHeaderElement, UTMZoneValueElement, UTMNorthingValueElement, UTMEastingValueElement;

enum CoordinateSystem coordinateSystem = CS_LatLong;

void SetupCanvasElements();

void DrawCoordinatesElements(Canvas* const canvas, GpsUart* gps_uart);

void DrawLatLongCoordinatesElements(Canvas* const canvas, GpsUart* gps_uart);

void DrawUTMCoordinatesElements(Canvas* const canvas, GpsUart* gps_uart);

void DrawCourseElements(Canvas* const canvas, GpsUart* gps_uart);

void DrawSpeedElements(Canvas* const canvas, GpsUart* gps_uart);

void DrawAltitudeElements(Canvas* const canvas, GpsUart* gps_uart);

void DrawSatelliteElements(Canvas* const canvas, GpsUart* gps_uart);

void DrawLastFixElements(Canvas* const canvas, GpsUart* gps_uart);

void DrawAcquiringFix(Canvas* const canvas, GpsUart* gps_uart);

void DrawAcquiredFix(Canvas* const canvas);

void IncrementCoordinateSystem();

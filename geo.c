#include "geo.h"

#include <math.h>

double distance(double lat1, double lon1, double lat2, double lon2) {
    const double deg2rad = M_PI / 180.0;

    double phi1 = lat1 * deg2rad;
    double phi2 = lat2 * deg2rad;
    double delta_phi = (lat2 - lat1) * deg2rad;
    double delta_lambda = (lon2 - lon1) * deg2rad;

    double a = sin(delta_phi / 2.0l) * sin(delta_phi / 2.0l) + cos(phi1) * cos(phi2) *
               sin(delta_lambda / 2.0l) * sin(delta_lambda / 2.0l);
    double c = 2.0l * atan2(sqrt(a), sqrt(1.0l - a));

    return EARTH_RADIUS * c;
}

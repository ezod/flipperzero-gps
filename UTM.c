#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include "conversions.h"
#include "ellipsoid.h"
#include "UTM.h"

struct UTM LLtoUTM(int ReferenceEllipsoid, const double Latitude, const double Longitude) {
    struct Ellipsoid ellipsoid = ellipsoids[ReferenceEllipsoid];

    double a = ellipsoid.EquatorialRadius;
    double eccSquared = ellipsoid.EccentricitySquared;
    double k0 = 0.9996;

    double LongOrigin;
    double eccPrimeSquared;
    double N, T, C, A, M;

    //Make sure the longitude is between -180.00 .. 179.9
    double LongTemp =
        (Longitude + 180) - (((int)(Longitude) + 180) / 360) * 360 - 180; // -180.00 .. 179.9;

    double LatitudeRadians = DegreesToRadians(Latitude);
    double LongitudeRadians = DegreesToRadians(LongTemp);
    double LongOriginRad;
    int ZoneNumber;

    ZoneNumber = (((int)(LongTemp) + 180) / 6) + 1;

    if(Latitude >= (double)(56.0) && Latitude < (double)(64.0) && LongTemp >= (double)(3.0) &&
       LongTemp < (double)(12.0))
        ZoneNumber = 32;

    // Special zones for Svalbard
    if(Latitude >= (double)(72.0) && Latitude < (double)(84.0)) {
        if(LongTemp >= (double)(0.0) && LongTemp < (double)(9.0))
            ZoneNumber = 31;
        else if(LongTemp >= (double)(9.0) && LongTemp < (double)(21.0))
            ZoneNumber = 33;
        else if(LongTemp >= (double)(21.0) && LongTemp < (double)(33.0))
            ZoneNumber = 35;
        else if(LongTemp >= (double)(33.0) && LongTemp < (double)(42.0))
            ZoneNumber = 37;
    }
    LongOrigin = (ZoneNumber - 1) * 6 - 180 + 3; //+3 puts origin in middle of zone
    LongOriginRad = DegreesToRadians(LongOrigin);

    struct UTM utm;

    // compute the UTM Zone from the latitude and longitude
    char buffer[64];
    int bufferSize = sizeof buffer;
    snprintf(
        &utm.Zone, bufferSize, "%d%c", (int)(ZoneNumber), GetUTMZoneLetterDesignator(Latitude));

    eccPrimeSquared = (eccSquared) / (1 - eccSquared);

    N = a / sqrt(1 - eccSquared * sin(LatitudeRadians) * sin(LatitudeRadians));
    T = tan(LatitudeRadians) * tan(LatitudeRadians);
    C = eccPrimeSquared * cos(LatitudeRadians) * cos(LatitudeRadians);
    A = cos(LatitudeRadians) * (LongitudeRadians - LongOriginRad);

    M = a *
        ((1 - eccSquared / 4 - 3 * eccSquared * eccSquared / 64 -
          5 * eccSquared * eccSquared * eccSquared / 256) *
             LatitudeRadians -
         (3 * eccSquared / 8 + 3 * eccSquared * eccSquared / 32 +
          45 * eccSquared * eccSquared * eccSquared / 1024) *
             sin(2 * LatitudeRadians) +
         (15 * eccSquared * eccSquared / 256 + 45 * eccSquared * eccSquared * eccSquared / 1024) *
             sin(4 * LatitudeRadians) -
         (35 * eccSquared * eccSquared * eccSquared / 3072) * sin(6 * LatitudeRadians));

    utm.Easting = (double)(k0 * N *
                               (A + (1 - T + C) * A * A * A / 6 +
                                (5 - 18 * T + T * T + 72 * C - 58 * eccPrimeSquared) * A * A * A *
                                    A * A / 120) +
                           (double)(500000.0));

    utm.Northing =
        (double)(k0 * (M + N * tan(LatitudeRadians) *
                               (A * A / 2 + (5 - T + 9 * C + 4 * C * C) * A * A * A * A / 24 +
                                (61 - 58 * T + T * T + 600 * C - 330 * eccPrimeSquared) * A * A *
                                    A * A * A * A / 720)));
    if(Latitude < 0)
        utm.Northing += (double)(10000000.0); //10000000 meter offset for southern hemisphere

    return utm;
}

char GetUTMZoneLetterDesignator(double Latitude) {
    char LetterDesignator;

    if((84 >= Latitude) && (Latitude >= 72))
        LetterDesignator = 'X';
    else if((72 > Latitude) && (Latitude >= 64))
        LetterDesignator = 'W';
    else if((64 > Latitude) && (Latitude >= 56))
        LetterDesignator = 'V';
    else if((56 > Latitude) && (Latitude >= 48))
        LetterDesignator = 'U';
    else if((48 > Latitude) && (Latitude >= 40))
        LetterDesignator = 'T';
    else if((40 > Latitude) && (Latitude >= 32))
        LetterDesignator = 'S';
    else if((32 > Latitude) && (Latitude >= 24))
        LetterDesignator = 'R';
    else if((24 > Latitude) && (Latitude >= 16))
        LetterDesignator = 'Q';
    else if((16 > Latitude) && (Latitude >= 8))
        LetterDesignator = 'P';
    else if((8 > Latitude) && (Latitude >= 0))
        LetterDesignator = 'N';
    else if((0 > Latitude) && (Latitude >= -8))
        LetterDesignator = 'M';
    else if((-8 > Latitude) && (Latitude >= -16))
        LetterDesignator = 'L';
    else if((-16 > Latitude) && (Latitude >= -24))
        LetterDesignator = 'K';
    else if((-24 > Latitude) && (Latitude >= -32))
        LetterDesignator = 'J';
    else if((-32 > Latitude) && (Latitude >= -40))
        LetterDesignator = 'H';
    else if((-40 > Latitude) && (Latitude >= -48))
        LetterDesignator = 'G';
    else if((-48 > Latitude) && (Latitude >= -56))
        LetterDesignator = 'F';
    else if((-56 > Latitude) && (Latitude >= -64))
        LetterDesignator = 'E';
    else if((-64 > Latitude) && (Latitude >= -72))
        LetterDesignator = 'D';
    else if((-72 > Latitude) && (Latitude >= -80))
        LetterDesignator = 'C';
    else
        LetterDesignator = 'Z';

    return LetterDesignator;
}

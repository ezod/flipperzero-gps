//LatLong- UTM conversion..h
//definitions for lat/long to UTM and UTM to lat/lng conversions
#include <string.h>
#include <stdio.h>

#ifndef LATLONGCONV
#define LATLONGCONV

struct UTM {
    char Zone;

    double Northing;

    double Easting;
};

struct UTM LLtoUTM(int ReferenceEllipsoid, const double Lat, const double Long);

char UTMLetterDesignator(double Lat);

#endif

/**
 * @file UTM.h
 *
 * @brief This file contains the structure for the Universal Transverse Mercator (UTM) coordinate system
 * @details The code in UTM.h and UTM.c was converted from code that was written by Chuck Gantz (chuck.gantz@globalstar.com) in cpp which can be found at http://gpsy.com/gpsinfo/geotoutm/
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <string.h>
#include <stdio.h>

#ifndef LATLONGCONV
#define LATLONGCONV

/**
 * @brief Represents a UTM coordiante
 * 
 */
struct UTM {
    char Zone;

    double Northing;

    double Easting;
};

/**
 * @brief Converts a latitude and longitude coordinate into a UTM based coordinate
 *
 * @param ReferenceEllipsoid    Ellipsoid to convert the coordiante in
 * @param Latitude              Latitude of the coordinate
 * @param Longitude             Longitude of the coordiante
 *
 * @return Representation of a latitude and longitude coordinate in UTM
 */
struct UTM LLtoUTM(int ReferenceEllipsoid, const double Latitude, const double Longitude);

/**
 * @brief Determines the UTM letter designator for a given latitude
 * 
 * @param Latitude Latitude to retreive the UTM letter designator from
 *
 * @return UTM letter designator
 * @return "Z" to signify an error where the latitude is outside the limits of UTM
 */
char GetUTMZoneLetterDesignator(double Latitude);

#endif

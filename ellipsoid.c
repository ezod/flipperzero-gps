#include "ellipsoid.h"

static struct Ellipsoid new(int Id, char* name, double radius, double ecc) {
    return (struct Ellipsoid){
        .id = Id, .ellipsoidName = name, .EquatorialRadius = radius, .eccentricitySquared = ecc};
}
const struct EllipsoidClass Ellipsoid = {.new = &new};

struct Ellipsoid {
    int id;
    char* ellipsoidName;
    double EquatorialRadius;
    double eccentricitySquared;
};
extern const struct EllipsoidClass {
    struct Ellipsoid (*new)(int Id, char* name, double radius, double ecc);
} Ellipsoid;

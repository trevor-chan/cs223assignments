#include <math.h>
#include "distance.h"


#define NORTH_POLE_LATITUDE (PI / 2)
#define EARTH_RADIUS_KILOMETERS 6378.1

//call function with distance(&prev, &curr);
/*
typedef struct
{
  char name[4];
  double lat;
  double lon;
} city;
*/

double distance(const city *from, const city *to)
{
  // convert latitudes to radians and get colatitude
  double colat1 = colatitude(to_radians(from->lat));
  double colat2 = colatitude(to_radians(to->lat));

  // compute difference in longitudes in radians
  double delta_lon = to_radians(from->lon - to->lon);

  // use spherical law of cosines to compute angle
  double x = acos(cos(colat1) * cos(colat2) + sin(colat1) * sin(colat2) * cos(delta_lon));

  // convert radians to arc length by multiplying by radius
  return x * EARTH_RADIUS_KILOMETERS;
}

double colatitude(double lat)
{
  return NORTH_POLE_LATITUDE - lat;
}

double to_radians(double a)
{
  return a * RADIANS_PER_DEGREE; // more_math.h has the #define in it
}


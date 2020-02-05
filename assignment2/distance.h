#ifndef __DISTANCE_H__
#define __DISTANCE_H__
#define PI 3.1415926535897932384
#define RADIANS_PER_DEGREE (PI / 180.0)


typedef struct
{
  char name[4];
  double lat;
  double lon;
  int visited;
} city;

double distance(const city *from, const city *to);

double colatitude(double);

double to_radians(double a);

#endif
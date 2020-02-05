#include <float.h>
#include "farthest.h"

int farthest(int index, const city *carray, int clistlength)
{
	double longdist = 0;
	int longindex = index;
	double currdist = 0;

	for (int i = 0; i < clistlength; i++)
	{
		//if (i != index)
		//{
			//index through all the things and use distance to figure out how far
			currdist = distance(&carray[i], &carray[index]);
			if (currdist > longdist)
			{
				if (carray[i].visited == 0)
				{
					longdist = currdist;
					longindex = i;
				}
			}
		//}
	}
	return longindex;
}


/*
#include <float.h>
#include "nearest.h"

int nearest(const city from, const city *carray, int clistlength)
{
	double shortdist = DBL_MAX;
	int shortindex = -1;
	double currdist = 0;

	for (int i = 0; i < clistlength; i++)
	{
		//if (i != index)
		//{
			currdist = distance(&carray[i], &from);
			if (currdist < shortdist)
			{
				if (carray[i].visited == 0)
				{
					shortdist = currdist;
					shortindex = i;
				}
			}
		//}
	}
	return shortindex;
}
*/
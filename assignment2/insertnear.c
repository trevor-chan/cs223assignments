#include "insertnear.h"
//#include "distance.h"
#include "nearest.h"
#include <stdlib.h>
#include <float.h>
#include <stdio.h>

int beforeorafter(const city *c1, const city *c2, const city *insert);

city *insertnear(const city *carray, int clistlength)
{
	city *sorted = malloc(sizeof(city) * (clistlength+1));
	city remaining[clistlength];
	int addindex = 0;
	double leastdist = DBL_MAX;
	double currdist = 0;
	int tempindex;

	for (int i = 0; i < clistlength; i ++)
	{
		remaining[i] = carray[i];
	}
	sorted[0] = carray[0];
	sorted[clistlength] = carray[0];

	for (int i = 1; i < clistlength; i ++)//i = number of occupied indices in sorted
	{
		for (int j = 0; j < i; j++)//iterate through each element of sorted
		{
			tempindex = nearest(sorted[j],remaining,clistlength);
			//tempindex should find the nearest remaining city from sorted[j]
			//(one of the cities in the current loop)
			printf("i=%d, j=%d, tempindex=%d\n", i, j, tempindex);
			if(tempindex == -1)
			{
			//printf("ERROR, all locations visited");
				exit(3);
			}
			currdist = distance(&remaining[tempindex], &sorted[j]);
			printf("currdist=%lf\n", currdist);
			if (currdist < leastdist)
			{
				leastdist = currdist;
				addindex = tempindex;
				printf("least=curr, addindex=%d\n", addindex);
			}
			else
			{
				printf("unchanged least, addindex, \n");
			}
		}
		
		if(i > 1)
		{
			if (beforeorafter(&sorted[i-2], &sorted[i-1], &carray[addindex]) == 0)
			{
				printf("added carray[%d] in sorted[%d] BEFORE\n", addindex, (i-1);
				printf("%s -> in sorted[%d]\n\n", carray[addindex].name, i);
				sorted[i] = sorted[i-1];
				sorted[i-1] = carray[addindex];
			}
			else
			{
				printf("added carray[%d] in sorted[%d]\n", addindex, i);
				printf("%s -> in sorted[%d]\n\n", carray[addindex].name, i);
				sorted[i] = carray[addindex];//change this to before or after before or after before or after
				leastdist = DBL_MAX;
				remaining[addindex].visited = 1;
			}
		}
		else
		{
			printf("added carray[%d] in sorted[%d]\n", addindex, i);
			printf("%s -> in sorted[%d]\n\n", carray[addindex].name, i);
			sorted[i] = carray[addindex];//change this to before or after before or after before or after
			leastdist = DBL_MAX;
			remaining[addindex].visited = 1;
		}
		

		
	}
	return sorted;
}


int beforeorafter(const city *c1,const city *c2,const city *insert)
{
	int distanceafter;
	int distancemiddle;
	distanceafter = distance(c1, c2) + distance(c2, insert);
	distancemiddle = distance(c1, insert) + distance(insert, c2);
	if (distanceafter < distancemiddle)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/* simple near, except at each stage check nearest for all cities currently
in sorted and compare, find least distance and add that in the correct order:
add either before or after - check both options and compare distance

in the end, check reverse pathway so orientation is correct

//if the path is shorter by inserting before, return 0, else return 1
check before or after:
int beforeorafter(city* c1, city* c2, city* insert)
{
	int distanceafter;
	int distnacemiddle;
	//path1
	distanceafter = distance(c1,c2) + distance(c2, insert);
	distancemiddle = distance(c1,insert) + distance(insert,c2);
	if (distanceafter < distancemiddle)
	{
		return 1;
	}
	else{return 0;}
}

//insert a city into the array at a specific spot and return the new array
city *insertnear()
{
	
}


when adding 
*/
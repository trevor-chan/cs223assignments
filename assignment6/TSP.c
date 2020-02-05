#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <string.h>
//#include <stddef.h>
#include "location.h"
#include "lugraph.h"

typedef struct
{
  char name[4];
  location loc;
  int visited;
} city;

typedef struct
{
  city c1;
  int v1;
  city c2;
  int v2;
  double dist;
} pair;

city *greedy(const city *carray, int clistlength);

int comparepairs(const void *p1, const void *p2);

int nearest(const city from, const city *carray, int clistlength);

int farthest(const city from, const city *carray, int clistlength);

city *simplenear(const city *carray, int clistlength);

city *mininsert(city *currorder, int triplength, 
	int currlength, int nearestindex, const city insert);

city *insertnear(const city *carray, int clistlength);

city *insertfar(const city *carray, int clistlength);

double totaldistance(int clistlength, const city *carray);

city *rotatearray(city *sorted, city *input, int numcities);

int cityequals(const city *one, const city *two);

void printtrip(const city *carray, int triplength);


int main(int argc, char **argv)
{
	FILE * infile;
	if(argc < 2)
	{
		fprintf(stderr, "TSP: missing filename\n");
		exit(0);
	}
	else
	{

	infile = fopen(argv[1],"r");

	if (infile == NULL) {
        fprintf(stderr,"TSP: could not open %s\n",argv[1]);
        exit(0);
    }

    //read number of cities
	int numcities;
	fscanf(infile,"%d",&numcities);

	if (numcities < 2)
	{
		fprintf(stderr, "TSP: too few cities\n");
		exit(0);
	}

	city carray[numcities];

	//read in city names
	for (int i = 0; i < numcities; i++)
	{
		fscanf(infile,"%s", carray[i].name);
		carray[i].visited = 0;
	}

	//read in city locations
	for (int i = 0; i < numcities; i++)
	{
		fscanf(infile,"%lf", &carray[i].loc.lat);
		fscanf(infile,"%lf", &carray[i].loc.lon);
	}

	for (int i = 2; i < argc; i++)
	{
		if (strcmp(argv[i],"-nearest") == 0)
		{
			city *trip = simplenear(carray,numcities);
			printf("-nearest        :");
			printtrip(trip,numcities+1);
			free(trip);
		}
		else if (strcmp(argv[i],"-insert") == 0)
		{
			i++;
			if (i == argc)
			{
				fprintf(stderr,"TSP: missing criterion\n");
				exit(0);
			}
			else if (strcmp(argv[i],"nearest") == 0)
			{
				city *trip = insertnear(carray,numcities);
				trip = rotatearray(trip, carray, numcities);
				printf("-insert nearest :");
				printtrip(trip,numcities+1);
				free(trip);
			}
			else if (strcmp(argv[i],"farthest") == 0)
			{
				city *trip = insertfar(carray,numcities);
				trip = rotatearray(trip, carray, numcities);
				printf("-insert farthest:");
				printtrip(trip,numcities+1);
				free(trip);
			}
			else
			{
				fprintf(stderr, "TSP: invalid criterion %s\n", argv[i]);
				exit(0);
			}
		}
		else if (strcmp(argv[i],"-optimal") == 0)
		{
			printf("-optimal        :");
			city ogtrip[numcities+1];
			for (int j = 0; j < numcities; j++)
			{
				ogtrip[j] = carray[j];
			}
			ogtrip[numcities] = carray[0];
			printtrip(ogtrip, numcities+1);
		}
		else if (strcmp(argv[i],"-greedy") == 0) //***********************************************************
		{
			printf("-greedy         :");
			city *trip = greedy(carray,numcities);
			trip = rotatearray(trip, carray, numcities); //necessary?
			printtrip(trip,numcities+1);
			free(trip);
		}
		else
		{
			fprintf(stderr,"TSP: invalid method %s\n", argv[i]);
			exit(0);
		}
	}

	fclose(infile);
	return 0;
	}
	return 1;
}






/*
generate a list of all pairs of cities
sort that list in order of increasing distance, breaking ties arbitrarily
start with an empty partial tour
for each pair of cities, if they are not already connected by the partial tour 
and if each city in the pair is included in at most one other pair added 
previously, then add a segment between the two cities in the pair to the partial tour.
*/

city *greedy(const city *carray, int clistlength)
{
	//make list of all pairs of cities

	int numpairs = (clistlength*(clistlength-1))/2;
	pair **pairarray = malloc(sizeof(pair*) * numpairs);

	int itr = 0;

	for (int i = 0; i < clistlength; i++)
	{
		for (int j = i; j < clistlength; j++)
		{
			if (i != j)
			{
				pairarray[itr] = malloc(sizeof(pair));
				pairarray[itr]->c1 = carray[i];
				pairarray[itr]->v1 = i;
				pairarray[itr]->c2 = carray[j];
				pairarray[itr]->v2 = j;
				pairarray[itr]->dist = location_distance(&(pairarray[itr]->c1.loc), &(pairarray[itr]->c2.loc));
				
				//printf("%d: (%s,%d) (%s,%d)\n", itr, pairarray[itr]->c1.name, pairarray[itr]->v1,pairarray[itr]->c2.name, pairarray[itr]->v2);

				itr++;
			}
		}
	}

	qsort(pairarray, numpairs, sizeof(pair*), comparepairs);

	lugraph *g = lugraph_create(clistlength);

	for (int i = 0; i < numpairs; i++)
	{
		if (lugraph_has_edge(g, pairarray[i]->v1, pairarray[i]->v2) == false) //if there is no existing edge
		{
			//if neither is connected yet
			if (lugraph_degree(g, pairarray[i]->v1) == 0 && lugraph_degree(g, pairarray[i]->v2) == 0) 
			{
				lugraph_add_edge(g, pairarray[i]->v1, pairarray[i]->v2);
			}
			//if they are not connected and the degree is less than 2
			else if(lugraph_connected(g, pairarray[i]->v1, pairarray[i]->v2) == false && 
			lugraph_degree(g, pairarray[i]->v1) < 2 && lugraph_degree(g, pairarray[i]->v2) < 2)
			{
				lugraph_add_edge(g, pairarray[i]->v1, pairarray[i]->v2);
			}
		}
	}

	for (int i = 0; i < numpairs; i++)
	{
		free(pairarray[i]);
	}
	free(pairarray);

	int start;
	int end;
	itr = 0;
	for (int i = 0; i < clistlength; i++)
	{
		if (lugraph_degree(g, i) == 1)
		{
			if (itr == 0)
			{
				start = i;
				itr++;
			}
			else
			{
				end = i;
			}
		}
	}

	int len;
	lug_search *s = lugraph_bfs(g, start);
	int *path = lug_search_path(s, end, &len);
	lug_search_destroy(s);
	//lugraph_add_edge(g, start, end);

	city *sorted = malloc(sizeof(city) * (clistlength+1));

	for (int i = 0; i < clistlength; i++)
	{
		sorted[i] = carray[path[i]];
	}
	sorted[clistlength] = sorted[0];

	free(path);
	lugraph_destroy(g);

	return sorted;
}

int comparepairs(const void *p1, const void *p2)
{
	pair **pp1 = (pair **) p1;
	pair **pp2 = (pair **) p2;
	if ((*pp1)->dist < (*pp2)->dist) return -1;
	if ((*pp1)->dist > (*pp2)->dist) return 1;
	return 0;
}







double totaldistance(int clistlength, const city *carray)
{
	double total = 0;
	for (int i = 1; i < clistlength; i++)
	{
		total += location_distance(&(carray[i].loc), &(carray[i-1].loc));
	}
	return total;
}

int nearest(const city from, const city *carray, int clistlength)
{
	double shortdist = DBL_MAX;
	int shortindex = -1;
	double currdist = 0;

	for (int i = 0; i < clistlength; i++)
	{
		currdist = location_distance(&(carray[i].loc), &(from.loc));
		if (currdist < shortdist)
		{
			if (carray[i].visited == 0)
			{
				shortdist = currdist;
				shortindex = i;
			}
		}
	}
	return shortindex;
}

int farthest(const city from, const city *carray, int clistlength)
{
	double longdist = 0;
	int longindex = -1;
	double currdist = 0;

	for (int i = 0; i < clistlength; i++)
	{
		currdist = location_distance(&(carray[i].loc), &(from.loc));
		if (currdist > longdist)
		{
			if (carray[i].visited == 0)
			{
				longdist = currdist;
				longindex = i;
			}
		}
	}
	return longindex;
}

city *simplenear(const city *carray, int clistlength)
{
	city *sorted = malloc(sizeof(city) * (clistlength+1));
	city remaining[clistlength];
	int addindex = 0;

	for (int i = 0; i < clistlength; i ++)
	{
		remaining[i] = carray[i];
	}
	remaining[0].visited = 1;
	sorted[0] = carray[0];
	sorted[clistlength] = carray[0];

	for (int i = 1; i < clistlength; i ++)
	{
		addindex = nearest(sorted[i-1], remaining, clistlength);
		if(addindex == -1)
		{
			printf("ERROR, all locations visited");
			exit(3);
		}
		sorted[i] = carray[addindex];
		remaining[addindex].visited = 1;
	}

	return sorted;
}

city *mininsert(city *currorder,int triplength, 
	int currlength, int nearestindex, const city insert)
{
	//triplength is the number of total cities + 1 including last
	//currlength is the number of cities in sorted including last
	double currtotaldist = 0;
	double mintotaldist = DBL_MAX;
	city temparray[triplength];
	city *neworder = malloc(sizeof(city) * (triplength));

	for (int i = 1; i < currlength; i++)//insert new city at index i in temparray
	{

		for (int j = 0; j < i; j++)
		{
			temparray[j] = currorder[j];
		}
		temparray[i] = insert;
		for (int j = i+1; j < currlength+1; j++)
		{
			temparray[j] = currorder[j-1];
		}

		//check the distance of the array, if it is less than the current
		//mintotaldist, set mintotaldist to it and copy temparray to neworder
		currtotaldist = totaldistance(currlength+1, temparray);
		if (currtotaldist < mintotaldist)
		{
			mintotaldist = currtotaldist;
			for(int k = 0; k < currlength+1; k++)
			{
				neworder[k] = temparray[k];
			}
		}
	}

	free(currorder);
	return neworder;

}

city *insertnear(const city *carray, int clistlength)
{
	city *sorted = malloc(sizeof(city) * (clistlength+1));
	city remaining[clistlength];
	int addindex = 0;
	double leastdist = DBL_MAX;
	double currdist = 0;
	int tempindex;
	int indexinsort;

	for (int i = 0; i < clistlength; i ++)
	{
		remaining[i] = carray[i];
	}
	
	int firstcity = 0;
	for (int i = 0; i < clistlength; i ++)
	{
		for (int j = 0; j < clistlength; j ++)
		{
			if (i != j)
			{
				currdist = location_distance(&(carray[i].loc), &(carray[j].loc));
				if (currdist < leastdist)
				{
					firstcity = i;
					leastdist = currdist;
				}
			}
		}
	}
	leastdist = DBL_MAX;

	sorted[0] = carray[firstcity];
	remaining[firstcity].visited = 1;
	sorted[1] = carray[firstcity];

	for (int i = 1; i < clistlength; i ++)//i = number of occupied indices in sorted
	{
		for (int j = 0; j < i; j++)//iterate through each element of sorted
		{
			tempindex = nearest(sorted[j],remaining,clistlength);
			if(tempindex == -1)
			{
			printf("ERROR, all locations visited");
				exit(3);
			}
			currdist = location_distance(&(remaining[tempindex].loc), &(sorted[j].loc));
			if (currdist < leastdist)
			{
				leastdist = currdist;
				addindex = tempindex;
				indexinsort = j;
			}
		}
		
		sorted = mininsert(sorted, clistlength+1, i+1, indexinsort, remaining[addindex]);
		leastdist = DBL_MAX;
		remaining[addindex].visited = 1;
	}
	return sorted;
}

city *insertfar(const city *carray, int clistlength)
{
	city *sorted = malloc(sizeof(city) * (clistlength+1));
	city remaining[clistlength];
	int addindex = 0;
	double maxdist = 0;
	double mindist = DBL_MAX;
	double currdist = 0;
	int tempindex;
	int indexinsort;

	for (int i = 0; i < clistlength; i ++)
	{
		remaining[i] = carray[i];
	}
	
	int firstcity = 0;

	for (int i = 0; i < clistlength; i ++)
	{
		for (int j = 0; j < clistlength; j ++)
		{
			if (i != j)
			{
				currdist = location_distance(&(carray[i].loc), &(carray[j].loc));
				if (currdist > maxdist)
				{
					firstcity = i;
					maxdist = currdist;
				}
			}
		}
	}

	maxdist = 0;
	sorted[0] = carray[firstcity];
	remaining[firstcity].visited = 1;
	sorted[1] = carray[firstcity];

	for (int i = 1; i < clistlength; i ++)//i = number of occupied indices in sorted - final
	{
		for (int j = 0; j < i; j++)//iterate through each element of sorted
		{
			tempindex = farthest(sorted[j],remaining,clistlength);

			if(tempindex == -1)
			{
				printf("ERROR, all locations visited");
				exit(3);
			}
			currdist = location_distance(&(remaining[tempindex].loc), &(sorted[j].loc));
			if (currdist > maxdist)
			{
				maxdist = currdist;
				addindex = tempindex;
				//indexinsort = j;
			}
		}

		for (int j = 0; j < i; j++)//iterate through each element of sorted
		{
			currdist = location_distance(&(remaining[addindex].loc), &(sorted[j].loc));
			if (currdist < mindist)
			{
				mindist = currdist;
				indexinsort = j;
			}
		}

		sorted = mininsert(sorted, clistlength+1, i+1, indexinsort, remaining[addindex]);
		maxdist = 0;
		remaining[addindex].visited = 1;
		mindist = DBL_MAX;
		
	}
	return sorted;
}

city *rotatearray(city *sorted, city *input, int numcities)
{
	city tempcity;
	int count = 0;
	while(cityequals(&sorted[0], &input[0]) != 0 && count < numcities+2)
	{
		tempcity = sorted[0];
		for(int i = 0; i < numcities-1; i ++)
		{
			sorted[i] = sorted[i+1];
		}
		sorted[numcities-1] = tempcity;
		count++;
	}
	sorted[numcities] = sorted[0];


	//change the flip function: make it so that the second city in the sorted
	//comes earlier than the penultimate city in sorted, not the other way around*******
	city second = sorted[1];
	city penultimate = sorted[numcities-1];

	for (int i = 0; i < numcities; i++)
	{
		if (cityequals(&input[i],&second) == 0)
		{
			return sorted;
		}
		else if (cityequals(&input[i],&penultimate) == 0)
		{
			break;
		}
	}

	city *invertedcity = malloc(sizeof(city)*(numcities+1));
	for (int i = 0; i < numcities; i++)
	{
		invertedcity[i] = sorted[numcities-i];
	}
	invertedcity[numcities] = sorted[0];

	free(sorted);

	return invertedcity;
}

int cityequals(const city *one, const city *two)
{
	if (strcmp(one->name,two->name) == 0 && one->loc.lat == two->loc.lat
		&& one->loc.lon == two->loc.lon)// && one->visited == two->visited)
	{
		return 0;
	}
	return 1;
}

void printtrip(const city *carray, int triplength)
{
	printf("%10.2f", totaldistance(triplength,carray));
	for (int i = 0; i < triplength; i++)
	{
		printf(" %s", carray[i].name);
	}
	printf("\n");
}

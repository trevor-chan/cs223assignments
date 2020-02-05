#include "track.h"
#include "trackpoint.h"
#include "location.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char **argv)
{
	//printf("hello there\n");
	if(argc < 5)
	{
		printf("too few arguments\n");
		exit(0);
	}

	double cell_width;
	double cell_height;
	cell_width = atof(argv[1]);
	cell_height = atof(argv[2]);
	char *chararray = argv[3];
	//strcopy(chararray, argv[3]);
	int chararraylength = strlen(chararray);
	int charrange = atoi(argv[4]);

	//make track:
	//initialize track
	track *trk = track_create();

	//printf("track created\n");

	char ch = ' ';
	double templat, templon;
	long temptime;

	while((ch = getchar()) != EOF)
	{
		ungetc(ch, stdin);

		//printf("reading file and ch = '%c'\n", ch);
		
		while((ch = getchar()) != '\n')
		{
			ungetc(ch, stdin);

			//read trackpoint values
			scanf("%lf", &templat);
			//printf("templat = '%lf'\n", templat);
			scanf("%lf", &templon);
			//printf("templon = '%lf'\n", templon);
			scanf("%ld", &temptime);
			//printf("temptime = '%ld'\n", temptime);

			trackpoint *pt = trackpoint_create(templat, templon, temptime);
			if (pt != NULL)
			{
				if (track_add_point(trk, pt) == false)
				{
					//printf("ERROR: failed to add point\n");
					track_destroy(trk);
					trackpoint_destroy(pt);
					printf("didn't add the point\n");
					return 0;
				}
				trackpoint_destroy(pt);
				//printf("added 1 point\n");
			}
			else
			{
				printf("trackpoint_create didn't work\n");
				return 0;
			}
			
			ch = getchar();
			//printf("ch = '%c'\n", ch);
			if ((ch = getchar()) == EOF)
			{
				break;
			}
			ungetc(ch, stdin);
		}
		if (ch == EOF)
		{
			break;
		}
		track_start_segment(trk);
		ch = getchar();
		if (isdigit(ch) != 0) ungetc(ch, stdin);
		//printf("made it through one segment\n");
	}
	//printf("track is created!\n");

	//track_heatmap()
	int **map;
	int map_rows;
	int map_cols;
	track_heatmap(trk, cell_width, cell_height, &map, &map_rows, &map_cols);
	if (map == NULL)
    {
      printf("ERROR: couldn't make heatmap\n");
      track_destroy(trk);
      return 0;
    }

    /*printf("made it to here\n");
    printf("map_rows = %d  map_cols = %d\n", map_rows, map_cols);*/

    /*//print map
    for(int i = 0; i < map_rows; i++)
    {
    	for (int j = 0; j < map_cols; j++)
    	{
    		printf("%d ", map[i][j]);
    	}
    	printf("\n");
    }*/

    //print values
    int tempvalue;
    for(int i = 0; i < map_rows; i++)
    {
    	for (int j = 0; j < map_cols; j++)
    	{
    		tempvalue = (map[i][j]/charrange);
    		if (tempvalue > chararraylength-1)
    		{
    			printf("%c", chararray[chararraylength-1]);
    		}
    		else
    		{
    			printf("%c", chararray[tempvalue]);
    		}
    	}
    	printf("\n");
    }

    //free array
    for(int i = 0; i < map_rows; i++)
    {
    	free(map[i]);
    }
    free(map);
    track_destroy(trk);

    return 1;
}



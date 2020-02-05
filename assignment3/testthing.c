#include "track.h"
#include "trackpoint.h"
#include "location.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//#include <string.h>
//#include "trackpoint.h"
//#include "location.h"



struct track
{
	trackpoint ***segarray;
	int *seglengths;
	int segnumber;

	int *segptspace;
	int segspace;
	//int trkptnumber;
};

/*a track is a metadata struct that contains 
	-an array of pointers, each to a segment containing an array of trackpoints
	-an array of ints with the length of each segment
	-an integer with the number of segments
	-an integer with the number of trackpoints

	***segarray
	array of pointers to pointers to trackpoints containing the segments
	each segment is an array of pointers to trackpoints
	segarray is a pointer to an array of pointers to pointers
                                                                                     
 * Creates a track with one empty segment.                                              
 *                                                                                      
 * @return a pointer to the new track, or NULL if there was an allocation error         
 */
track *track_create()
{
	track *trk = malloc(sizeof(track));

	//segspace is the number of segments allocated in segarray
	trk->segspace = 1;
	//segptspace is the number of trackpoint* allocated in each segment
	trk->segptspace = malloc(sizeof(int));//needs to increase every time segspace goes up

	//for (int i = 0; i < 4; i ++)
	//{
	//trk->segptspace[0] = 1;
	//}

	trk->segarray = malloc(sizeof(trackpoint**));

	//trk->segarray[0] = malloc(sizeof(trackpoint*));

	trk->seglengths = malloc(sizeof(int));

	trk->seglengths[0] = 0;
	trk->segptspace[0] = 0;
	trk->segnumber = 1;
	//t->trkptnumber = 0;
	return trk;
}

/**                                                                                     
 * Destroys the given track, releasing all memory held by it.                           
 *                                                                                      
 * @param trk a pointer to a valid track                                                
 */
void track_destroy(track *trk)
{
	for (int i = 0; i < trk->segnumber; i++)
	{
		//destroy individual trackpoints
		for (int j = 0; j < trk->seglengths[i]; j++) // fix fix fix
		{
			trackpoint_destroy(trk->segarray[i][j]);
		}
		if (trk->seglengths[i] != 0 || trk->segptspace[i] > 0)
		{
			free(trk->segarray[i]);
		}
	}
	/*for (int i = 0; i < trk->segspace; i++)
	{
		if(i < trk->segnumber)
		{
			//destroy individual trackpoints
			for (int j = 0; j < trk->seglengths[i]; j++) // fix fix fix
			{
				trackpoint_destroy(trk->segarray[i][j]);
			}
		}
		
		//if (trk->seglengths[i] != 0)
		//{
		free(trk->segarray[i]);
			//free(trk->segptspace[i]);
		//}
	}*/
	free(trk->segarray);
	free(trk->seglengths);
	free(trk->segptspace);
	free(trk);
}

/**                                                                                     
 * Returns the number of segments in the given track.                                   
 *                                                                                      
 * @param trk a pointer to a valid track                                                
 */
int track_count_segments(const track *trk)
{
	return trk->segnumber;
}

/**                                                                                     
 * Returns the number of trackpoints in the given segment of the given                  
 * track.  The segment is specified by a 0-based index.  The return                     
 * value is 0 if the segment index is invalid.                                          
 *                                                                                      
 * @param trk a pointer to a valid track                                                
 * @param i a nonnegative integer less than the number of segments in trk               
 * @return the number of trackpoints in the corresponding segment                       
 */
int track_count_points(const track *trk, int i)
{
	if(i < 0 || i > (trk->segnumber-1))
	{
		return 0;
	}
	return trk->seglengths[i];
}

/**                                                                                     
 * Returns a copy of the given point in this track.  The segment is                     
 * specified as a 0-based index, and the point within the segment is                    
 * specified as a 0-based index into the corresponding segment.  The                    
 * return value is NULL if either index is invalid or if there is a memory              
 * allocation error.  It is the caller's responsibility to destroy the                  
 * returned trackpoint.                                                                 
 *                                                                                      
 * @param trk a pointer to a valid track                                                
 * @param i a nonnegative integer less than the number of segments in trk               
 * @param j a nonnegative integer less than the number of points in segment i           
 * of track trk                                                                         
 */
trackpoint *track_get_point(const track *trk, int i, int j)
{
	trackpoint *trkpt = trackpoint_copy(trk->segarray[i][j]);
	return trkpt;
}


/**                                                                                     
 * Returns an array containing the length of each segment in this track.                
 * The length of a segment is the sum of the distances between each point               
 * in it and the next point.  The length of a segment with fewer than two               
 * points is zero.  If there is a memory allocation error then the returned             
 * pointer is NULL.  It is the caller's responsibility to free the returned             
 * array.                                                                               
 *                                                                                      
 * @param trk a pointer to a valid track                                                
 */
double *track_get_lengths(const track *trk)
{
	double *arr = malloc(sizeof(int)*trk->segnumber);
	if (arr == NULL)
	{
		return NULL;
	}
	double templength = 0;
	location current;
	location next;
	for (int i = 0; i < trk->segnumber; i++)
	{
		if (trk->seglengths[i] < 2)
		{
			arr[i] = 0;
		}
		else
		{
			next = trackpoint_location(trk->segarray[i][0]);
			for (int j = 0; j < trk->seglengths[i]-2; j++)
			{
				current = next;
				next = trackpoint_location(trk->segarray[i][j+1]);
				templength = location_distance(&current, &next);
				arr[i] += templength;
			}
		}
	}
	return arr;
}

/**                                                                                     
 * Adds a copy of the given point to the last segment in this track.                    
 * The point is not added and there is no change to the track if there                  
 * is a last point in the track (the last point in the current segment                  
 * or the last point on the previous segment if the current segment                     
 * is empty) and the timestamp on the new point is                                      
 * not strictly after the timestamp on the last point.  There is no                     
 * effect if there is a memory allocation error.  The return value                      
 * indicates whether the point was added.  This function must execute                   
 * in amortized O(1) time (so a sequence of n consecutive operations must               
 * work in worst-case O(n) time).                                                       
 *                                                                                      
 * @param trk a pointer to a valid track                                                
 * @param pt a trackpoint with a timestamp strictly after the last trackpoint           
 * in the last segment in this track (if there is such a point)                         
 * @return true if and only if the point was added                                      
 */
bool track_add_point(track *trk, const trackpoint *pt)
{
	if (trk->segnumber == 1 && trk->seglengths[0] == 0)
	{
		trk->segarray[0] = malloc(sizeof(trackpoint*));
		trk->segptspace[0]++;
		trk->seglengths[0]++;

		location ptloc = trackpoint_location(pt);
		long pttime = trackpoint_time(pt);
		trk->segarray[0][0] = trackpoint_create(ptloc.lat, ptloc.lon, pttime);
		
		return true;
	}

	//calculate index of the previous point
	int lastsegindex = trk->segnumber-1;
	if (trk->seglengths[lastsegindex] == 0)
	{
		lastsegindex --;
	}
	int lastptindex = trk->seglengths[lastsegindex]-1;
	//printf("segnum = %d; last seglengths = %d\n", trk->segnumber, trk->seglengths[trk->segnumber-1]);
	
	//get timestamps of each
	trackpoint* temppt = trk->segarray[lastsegindex][lastptindex];
	long time2 = trackpoint_time(temppt);
	long time1 = trackpoint_time(pt);

	//compare timestamps for validity
	if (time2 >= time1)
	{
		return false;
	}
	
	//if necessary, allocate 4x memory to the last segment and increase the segptspace index appropriately
	if (trk->seglengths[trk->segnumber-1] >= trk->segptspace[trk->segnumber-1])
	{
		trk->segptspace[trk->segnumber-1] *= 4;
		trk->segarray[trk->segnumber-1] = realloc(trk->segarray[trk->segnumber-1], 
			sizeof(trackpoint*) * trk->segptspace[trk->segnumber-1]);
	}
	//trk->segarray[trk->segnumber-1] = realloc(trk->segarray[trk->segnumber-1], sizeof(trackpoint*) * (trk->seglengths[trk->segnumber-1]+1));  //something wrong here

	//add the point
	if (trk->segarray[trk->segnumber-1] != NULL)
	{
		trk->seglengths[trk->segnumber-1]++;

		location ptloc = trackpoint_location(pt);
		long pttime = trackpoint_time(pt);

		//printf("make new trackpoint\n");
		trk->segarray[trk->segnumber-1][trk->seglengths[trk->segnumber-1]-1] = 
			trackpoint_create(ptloc.lat, ptloc.lon, pttime); // make new trackpoint at last available index
		return true;
	}
	return false;
}

/**                                                                                     
 * Starts a new segment in the given track.  There is no effect on the track            
 * if the current segment is empty or if there is a memory allocation error.            
 *                                                                                      
 * @param trk a pointer to a valid track                                                
 */
void track_start_segment(track *trk)
{
	if (trk->seglengths[trk->segnumber-1] > 0)
	{
		if(trk->segnumber >= trk->segspace) //if more memory is needed
		{
			trk->segspace *= 4;
			trk->segarray = realloc(trk->segarray, sizeof(trackpoint **) * trk->segspace);
			trk->segptspace = realloc(trk->segptspace, sizeof(int) * trk->segspace);
			trk->seglengths = realloc(trk->seglengths, sizeof(int) * trk->segspace);
			//allocate 4x memory to segarray, segptspace, and seglengths arrays
			//trk->segptspace[trk->segnumber] = trk->segspace;
		}
		trk->segarray[trk->segnumber] = malloc(sizeof(trackpoint *));
		trk->seglengths[trk->segnumber] = 0;
		trk->segptspace[trk->segnumber] = 1;
		trk->segnumber ++;
	}
}

/**                                                                                     
 * Merges the given range of segments in this track into one.  The segments             
 * to merge are specified as the 0-based index of the first segment to                  
 * merge and one more than the index of the last segment to merge.                      
 * The resulting segment replaces the first merged one and later segments               
 * are moved up to replace the other merged segments.  If the range is                  
 * invalid then there is no effect.                                                     
 *                                                                                      
 * @param trk a pointer to a valid track                                                
 * @param start an integer greather than or equal to 0 and strictly less than           
 * the number if segments in trk                                                        
 * @param end an integer greater than or equal to start and less than or                
 * equal to the number of segments in trk                                               
 */
void track_merge_segments(track *trk, int start, int end)
{
	if (start >= end || start < 0 || end > trk->segnumber)
	{
		return;
	}

	int newlength = 0;
	for (int i = start; i < end; i++)
	{
		newlength += trk->seglengths[i];
	}
	trackpoint ** newsegment = malloc(sizeof(trackpoint*)*newlength);

	int segindex = 0;
	for (int i = start; i < end; i++)
	{
		for (int j = 0; j < trk->seglengths[i]; j++)
		{
			newsegment[segindex] = trackpoint_copy(trk->segarray[i][j]);
			trackpoint_destroy(trk->segarray[i][j]);
			segindex++;
		}
		free(trk->segarray[i]);
	}

	//figure out some memory hijinks here
	trk->segarray[start] = newsegment;
	trk->seglengths[start] = newlength;
	trk->segptspace[start] = newlength;

	//shift remaining segments up
	int numshifted = trk->segnumber - end;
	for (int i = 0; i < numshifted; i ++)
	{
		trk->segarray[i+start+1] = trk->segarray[i+end];
		trk->seglengths[i+start+1] = trk->seglengths[i+end];
		trk->segptspace[i+start+1] = trk->segptspace[i+end];
	}

	trk->segspace -= (end-start-1);
	trk->segarray = realloc(trk->segarray, sizeof(trackpoint **) * trk->segspace);
	trk->seglengths = realloc(trk->seglengths, sizeof(int) * trk->segspace);
	trk->segnumber -= (end-start-1);


}

/**                                                                                     
 * Creates a heapmap of the given track.  The heatmap will be a                         
 * rectangular 2-D array with each row separately allocated.  The last                  
 * three paramters are (simulated) reference parameters used to return                  
 * the heatmap and its dimensions.  Each element in the heatmap                         
 * represents an area bounded by two circles of latitude and two                        
 * meridians of longitude.  The circle of latitude bounding the top of                  
 * the top row is the northernmost (highest) latitude of any                            
 * trackpoint in the given track.  The meridian bounding the left of                    
 * the first column is the western edge of the smallest spherical                       
 * wedge bounded by two meridians the contains all the points in the                    
 * track (the "western edge" for a nontrivial wedge being the one                       
 * that, when you move east from it along the equator, you stay in the                  
 * wedge).  When there are multple such wedges, choose the one with        ****              
 * the lowest normalized (adjusted to the range -180 (inclusive) to                     
 * 180 (exclusive)) longitude.  The distance (in degrees) between the                   
 * bounds of adjacent rows and columns is given by the last two                            
 * parameters.  The heat map will have just enough rows and just                        
 * enough columns so that all points in the track fall into some cell.                  
 * The value in each entry in the heatmap is the number of trackpoints                  
 * located in the corresponding cell.  If a trackpoint is on the                        
 * border of two or more cells then it is counted in the bottommost                     
 * and rightmost cell it is on the border of, but do not add a row or                   
 * column just to place points on the south and east borders into                       
 * them and instead place the points on those borders by breaking ties                  
 * only between cells that already exist.                                               
 * If there are no trackpoints in the track then the function                           
 * creates a 1x1 heatmap with the single element having a value of 0.            ****  
 * If the cell size is invalid or if there is a memory allocation                       
 * error then the map is set to NULL and the rows and columns                           
 * parameters are unchanged.  It is the caller's responsibility to                      
 * free each row in the returned array and the array itself.                            
 *                                                                   
 * @param trk a pointer to a valid trackpoint                                           
 * @param cell_width a positive double less than or equal to 360.0                      
 * @param cell_height a positive double less than or equal to 180.0                     
 * @param map a pointer to a pointer to a 2-D array of ints                             
 * @param rows a pointer to an int                                                      
 * @param cols a pointer to an int                                                      
 */
void track_heatmap(const track *trk, double cell_width, double cell_height,
                    int ***map, int *rows, int *cols)
{
	//finding the maximum and minimum latitude and longitudinal bounds
	//determine whether wedge crosses dateline

	if (trk->seglengths[0]==0)
	{
		map[0] = malloc(sizeof(int*));
		map[0][0] = calloc(1, sizeof(int));
		*rows = 1;
		*cols = 1;
		return;
	}

	if (cell_height > 180 || cell_height <= 0)
	{
		map = NULL;
	}
	if (cell_width > 360 || cell_width <= 0)
	{
		map = NULL;
	}


	double longestgap = 0;
	double tempsmallest = 361;
	double tempgap = 0;
	double lon1, lon2;

	double maxlat = -90, maxlon = -180, minlat = 90, minlon = 180;
	double currlat, currlon, secondlon;
	for (int i = 0; i < trk->segnumber; i++)
	{
		for (int j = 0; j < trk->seglengths[i]; j++)
		{
			currlat = trackpoint_location(trk->segarray[i][j]).lat;
			currlon = trackpoint_location(trk->segarray[i][j]).lon;
			if (currlat > maxlat) maxlat = currlat;
			if (currlat < minlat) minlat = currlat;
			if (currlon > maxlon) maxlon = currlon;
			if (currlon < minlon) minlon = currlon;

			for (int m = 0; m < trk->segnumber; m ++)
			{
				for (int l = 0; l < trk->seglengths[m]; l++)
				{
					secondlon = trackpoint_location(trk->segarray[m][l]).lon;
					if (secondlon > currlon)
					{
						tempgap = secondlon - currlon;
						if (tempgap < tempsmallest)
						{
							tempsmallest = tempgap;
						}
					}	
				}
			}

			if(tempsmallest > longestgap && tempsmallest != 361) 
			{
				longestgap = tempsmallest;
				lon1 = currlon;//goes to maxlon
				lon2 = secondlon;//goes to minlon
			}
			tempsmallest = 361;
			//find length and borders of largest gap with no points
			//start with point of lowest lon, find point with next highest lon, 
			//record difference and lon pairs, keeping track for the longest
			//requires points ordered by lon
			//OR
			//for each point in trk,
				//iterate through and find the next highest lon point
				//this covers all points and should cover both directions without double counting
				//next highest lon point must be greater than lon1 but have the smallest difference
				//requires a smallesttemplon and a largestgap
		}
	}
	printf("longestgap = %lf\nlon1 = %lf lon2 = %lf\n", longestgap, lon1, lon2);
	int inverted = 0;
	if(longestgap > (360 + minlon - maxlon))
	{
		inverted = 1;
	}

	if (inverted == 0)
	{
		//determining array size
		//printf("maxlat = %lf minlat = %lf\n", maxlat, minlat);
		//printf("maxlon = %lf minlon = %lf\n", maxlon, minlon);
		int numrows = (maxlat - minlat)/cell_height + 1;
		int numcols = (maxlon - minlon)/cell_width + 1;

		if (minlat == (maxlat - (cell_height * (numrows-1)))) numrows--;
		if (maxlon == (minlon + (cell_width * (numcols-1)))) numcols--;

		*rows = numrows;
		*cols = numcols;

		//printf("numrows = %d numcols = %d\n", numrows, numcols);

		//memory allocation of 2D array -- may need to change as map is ***
		*map = malloc(sizeof(int*) * numrows);
		for (int i = 0; i < numrows; i ++)
		{
			map[0][i] = calloc(numcols, sizeof(int));
		}

		//printf("segnumber = %d\n", trk->segnumber);
		int rowindex;
		int colindex;
		for (int i = 0; i < trk->segnumber; i ++)
		{
			//printf("seglength = %d\n", trk->seglengths[i]);
			for (int j = 0; j < trk->seglengths[i]; j ++)
			{
				//calculate index in map to add to
				currlat = trackpoint_location(trk->segarray[i][j]).lat;
				currlon = trackpoint_location(trk->segarray[i][j]).lon;
				rowindex = (maxlat - currlat)/cell_height;
				colindex = (-minlon + currlon)/cell_width;
				
				//if on southern or eastern border
				if (currlat == (maxlat - (cell_height * numrows))) rowindex--;
				if (currlon == (minlon + (cell_width * numcols))) colindex--;
				
				map[0][rowindex][colindex] ++;
				//printf("lat = %lf lon = %lf\n", currlat, currlon);
				//printf("map[%d][%d] = %d\n", rowindex, colindex, map[0][rowindex][colindex]);
			}
		}
	}

	if (inverted == 1)
	{
		//code for inverted wedge map

		//find which is maxlon and which is minlon (minlon is larger)
		if (lon1 > lon2)
		{
			minlon = lon1;
			maxlon = lon2;
		}
		else
		{
			minlon = lon2;
			maxlon = lon1;
		}
		printf("minlon = %lf maxlon = %lf\n", minlon, maxlon);

		int numrows = (maxlat - minlat)/cell_height + 1;
		int numcols = ((maxlon+360) - minlon)/cell_width + 1;

		if (minlat == (maxlat - (cell_height * (numrows-1)))) numrows--;
		if ((maxlon+360) == (minlon + (cell_width * (numcols-1)))) numcols--;

		*rows = numrows;
		*cols = numcols;

		printf("numrows = %d numcols = %d\n", numrows, numcols);

		//memory allocation of 2D array -- may need to change as map is ***
		*map = malloc(sizeof(int*) * numrows);
		for (int i = 0; i < numrows; i ++)
		{
			map[0][i] = calloc(numcols, sizeof(int));
		}

		//printf("segnumber = %d\n", trk->segnumber);
		int rowindex;
		int colindex;
		for (int i = 0; i < trk->segnumber; i ++)
		{
			//printf("seglength = %d\n", trk->seglengths[i]);
			for (int j = 0; j < trk->seglengths[i]; j ++)
			{
				//calculate index in map to add to
				currlat = trackpoint_location(trk->segarray[i][j]).lat;
				currlon = trackpoint_location(trk->segarray[i][j]).lon;
				if(currlon <= maxlon)
				{
					currlon += 360;
				}
				rowindex = (maxlat - currlat)/cell_height;
				colindex = (-minlon + currlon)/cell_width;
				
				//if on southern or eastern border
				if (currlat == (maxlat - (cell_height * numrows))) rowindex--;
				if (currlon == (minlon + (cell_width * numcols))) colindex--;
				
				map[0][rowindex][colindex] ++;
				//printf("lat = %lf lon = %lf\n", currlat, currlon);
				//printf("map[%d][%d] = %d\n", rowindex, colindex, map[0][rowindex][colindex]);
			}
		}
	}
}
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <string.h>
#include <math.h>

#include "location.h"
#include "kdtree.h"

/**                                                                                                      
 * structure of a node in the kdtree 
 * contains information on the location (x and y coords)
 * int level from root (root level = 0)
 * pointers to parent, left and right subtrees                                                          
 */
typedef struct node
{
	location loc; //key and also value
  	int level; //level of node
  	int height; //size of subtree rooted at this node in number of levels
  	struct node *parent;
  	struct node *left;
  	struct node *right;
}node;

/**                                                                                                      
 * A set of geographic locations given by latitude and longitude                                         
 * implemented using a k-d tree, where k = 2.  For all functions,                                        
 * points are considered different based on the values of the                                            
 * coordinates that are passed in, even if the coordinates represent                                     
 * the same physical point, so, for example, longitude -180 is                                           
 * considered different than longitude 180.                                                              
 */
struct _kdtree
{
	node *root;
	int size;
};


/* Useful other function declarations*/
node *make_node(const location *loc, int level);
void print_tree(kdtree kdt);
int compare(const location *loc1, const location *loc2, int level);
void print_kdtree(const node *root);
void destroy_subtree(node *root);


bool subtree_add(node *root, const location *p);
bool subtree_contains(const node *root, const location *p);
void for_each_subtree(node *root, void (*f)(const location *, void *), void *arg);
bool loc_compare(const location loc1, const location loc2);
void merge(int n1, const location a1[], int n2, const location a2[], location out[], int xory);
void mergeSort(int n, const location a[], location out[], int xory);
void subtree_nearest_neighbor(node *root, const location *p, location *neighbor, double *d);
void subtree_create(node **root, location * xarray, location * yarray, int n, int level);

/*
#define MAX_LON 180.0;
#define MIN_LON -179.999999;
#define MAX_LAT 90.0;
#define MIN_LAT -89.999999;
*/

#define MAX_LON DBL_MAX;
#define MIN_LON DBL_MIN;
#define MAX_LAT DBL_MAX;
#define MIN_LAT DBL_MIN;




//typedef struct _kdtree kdtree;

/**                                                                                                      
 * Creates a balanced k-d tree containing copies of the points in the                                    
 * given array of locations.  If the array is NULL and n is 0 then the                                   
 * returned tree is empty.  If the tree could not be created then the                                    
 * returned value is NULL.                                                                               
 *                                                                                                       
 * @param pts an array of unique valid locations, non-NULL if n != 0                                     
 * @param n the number of points in that array, or 0 if pts is NULL                                      
 * @return a pointer to the newly created collection of points, or NULL                                  
 * if it could not be created                                                                            
 */
kdtree *kdtree_create(const location *pts, int n)
{
	kdtree *t = malloc (sizeof(kdtree));
	t->root = NULL;
	t->size = 0;


	if (n < 1)
	{
		return t;
	}

	//make root node

	location *xsort = malloc(sizeof(location) * n);
	location *ysort = malloc(sizeof(location) * n);

	mergeSort(n, pts, xsort, 0);
	mergeSort(n, pts, ysort, 1);

	//add root node to tree
	/*if (kdtree_add(t, &(xsort[n/2])) == false)
	{
		kdtree_destroy(t);
		exit(0);
	}*/

	subtree_create(&(t->root), xsort, ysort, n, 0);
	//subtree_create(t->root, t->root->right, xsort, ysort, n);

	free(xsort);
	free(ysort);

	return t;

}

//takes a null node, sorted x and y arrays, size, and level
void subtree_create(node **root, location * xarray, location * yarray, int n, int level)
{

	if (n == 0) // if no node passed, end subtree
	{
		return;
	}
	if (n == 1) // if only one node passed, add node and end subtree
	{
		//subtree_add(root, &(xarray[0]));
		node *newnode = make_node(&(xarray[0]), level);
		if (newnode == NULL) return;

		*root = newnode;
		newnode->parent = NULL;

		return;
	}

	int nleft, nright;
	if (n%2 == 0)
	{
		nleft = (n/2);
		nright = (n/2)-1;
	}
	else
	{
		nleft = (n/2);
		nright = (n/2);
	}

	location *xleft = malloc(sizeof(location) * nleft);
	location *yleft = malloc(sizeof(location) * nleft);
	location *xright = malloc(sizeof(location) * nright);
	location *yright = malloc(sizeof(location) * nright);

	if (level%2 == 0)//select by x, sort by y
	{
		//subtree_add(root, &(xarray[n/2]));
		node *newnode = make_node(&(xarray[n/2]), level);
		if (newnode == NULL) return;

		*root = newnode;
		newnode->parent = NULL;

		//initialize x and y arrays
		for (int i = 0; i < nleft; i++) 
		{
			xleft[i] = xarray[i];
		}
		for (int i = 0; i < nright; i++)
		{
			xright[i] = xarray[i+nleft+1];
		}

		int il = 0, ir = 0;

		for (int i = 0; i < n; i++)
		{
			if (loc_compare(yarray[i], xarray[n/2]) == false)
			{
				if (yarray[i].x < xarray[n/2].x)
				{
					yleft[il] = yarray[i];
					il++;
				}
				else
				{
					yright[ir] = yarray[i];
					ir++;
				}
			}
		}

	}
	else //select by y, sort by x
	{
		//subtree_add(root, &(yarray[n/2]));
		node *newnode = make_node(&(yarray[n/2]), level);
		if (newnode == NULL) return;

		*root = newnode;
		newnode->parent = NULL;

		//initialize x and y arrays
		for (int i = 0; i < nleft; i++) 
		{
			yleft[i] = yarray[i];
		}
		for (int i = 0; i < nright; i++)
		{
			yright[i] = yarray[i+nleft+1];
		}

		int il = 0, ir = 0;
		for (int i = 0; i < n; i++)
		{
			if (loc_compare(xarray[i], yarray[n/2]) == false)
			{
				if (xarray[i].y < yarray[n/2].y)
				{
					xleft[il] = xarray[i];
					il++;
				}
				else
				{
					xright[ir] = xarray[i];
					ir++;
				}
			}
		}
	}


	//subtree only added one node to one side, either root->left or root->right is null
	subtree_create(&((*root)->left), xleft, yleft, nleft, level+1);
	subtree_create(&((*root)->right), xright, yright, nright, level+1);
	
	if(level != 0)
	{	
		if ((*root)->left != NULL) (*root)->left->parent = (*root);
		if ((*root)->right != NULL) (*root)->right->parent = (*root);
	}

	free(xleft);
	free(xright);
	free(yleft);
	free(yright);

}

/**                                                                                                      
 * Adds a copy of the given point to the given k-d tree.  There is no                                    
 * effect if the point is already in the tree.  The tree need not be                                     
 * balanced after the add.  The return value is true if the point was                                    
 * added successfully and false otherwise (if the point was already in the                               
 * tree or if there was a memory allocation error when trying to add).                                   
 *                                                                                                       
 * @param t a pointer to a valid k-d tree, non-NULL                                                      
 * @param p a pointer to a valid location, non-NULL                                                      
 * @return true if and only if the point was successfully added                                          
 */
bool kdtree_add(kdtree *t, const location *p)
{
	if (t->root == NULL) //if tree is empty
	{
		node *newnode = make_node(p, 0);
		if (newnode == NULL) return false;

		t->root = newnode;
		newnode->parent = NULL;
		return true;
	}
	else //otherwise
	{
		node *temp;
		temp = t->root;
		return subtree_add(temp, p);
	}
}

bool subtree_add(node *root, const location *p)
{
	if (loc_compare(root->loc,*p) == true)
	{
		return false;
	}
	else
	{
		if (compare(p, &(root->loc), root->level) == -1)
		{
			if (root->left == NULL)
			{
				node *newnode = make_node(p, root->level+1);
				if (newnode == NULL) return false;

				root->left = newnode;
				newnode->parent = root;
				return true;
			}

			return subtree_add(root->left, p);
			if (root->height == root->left->height)
			{
				root->height ++;
			}
		}
		else
		{
			if (root->right == NULL)
			{
				node *newnode = make_node(p, root->level+1);
				if (newnode == NULL) return false;

				root->right = newnode; 
				newnode->parent = root;
				return true;
			}
			return subtree_add(root->right, p);
			if (root->height == root->right->height)
			{
				root->height ++;
			}
		}
	}
}



/**                                                                                                      
 * Determines if the given tree contains a point with the same coordinates                               
 * as the given point.                                                                                   
 *                                                                                                       
 * @param t a pointer to a valid k-d tree, non-NULL                                                      
 * @param p a pointer to a valid location, non-NULL                                                      
 * @return true if and only of the tree contains the location                                            
 */
bool kdtree_contains(const kdtree *t, const location *p)
{
	return subtree_contains(t->root, p);
}

bool subtree_contains(const node *root, const location *p)
{
	if (root == NULL)
	{
		return false;
	}

	if (loc_compare(root->loc, *p) == false)
	{
		if (compare(p, &(root->loc), root->level) == -1)//-1 if p is left of root, +1 otherwise
		{
			return subtree_contains(root->left, p);
		}
		else
		{
			return subtree_contains(root->right, p);
		}
	}
	else
	{
		return true;
	}
}

/**                                                                                                      
 * Copies the nearest neighbor to the given point among points in the                                    
 * given tree to the given location.  Ties are broken arbitrarily.                                       
 * There is no change to neighbor and distance is set to infinity if                                     
 * the tree is empty.  If p is equal to a point in the tree then p is                                    
 * copied into neighbor and distance is set to zero.                                                     
 *                                                                                                       
 * @param t a pointer to a valid k-d tree, non-NULL                                                      
 * @param p a pointer to a valid location, non-NULL                                                      
 * @param d a pointer to a double, non-NULL                                                              
 * @param neighbor a pointer to a valid location, non-NULL                                               
 */
void kdtree_nearest_neighbor(kdtree *t, const location *p, location *neighbor, double *d)
{
	if (t->root == NULL)
	{
		*d = INFINITY;
		return;
	}
	if (kdtree_contains(t, p))
	{
		*neighbor = *p;
		*d = 0.0;
		return;
	}

	*d = DBL_MAX;
	subtree_nearest_neighbor(t->root, p, neighbor, d);
	
	//search by following tree down towards the point (as if searching), 
	//keep track of minimum distance from each point and update distance (d) as
	//minimum is surpassed
	//After reaching the bottom and finding a minimum d;
	//search back up the tree: before calling each other subtree, check using
	//rectangle near function to see if the subtree can be skipped: it can
	//be skipped if the minimum distance d is less than the minimum possible distance
	//(this minimum possible distance must be calculated)gdb
}

void subtree_nearest_neighbor(node *root, const location *p, location *neighbor, double *d)
{
	if (root == NULL)
	{
		return;
	}

	//printf("level %d\n", root->level);

	//calculate distance from root to point; if less, update d, update neighbor
	double tempdist = location_distance(&(root->loc), p);
	if (tempdist < *d)
	{
		*d = tempdist;
		*neighbor = root->loc;
	}

	//calculate minimum and maximum bounds of the subtree: ***********************************************************
	double minx = MIN_LON;
	double miny = MIN_LAT;
	double maxx = MAX_LON;
	double maxy = MAX_LAT;
	//node *temp = root;

	//go to parent, if level is even (split by x) then set xmax or xmin to that based on
	//whether it is larger or smaller than the child x value
	//if level is odd (split by y) then set ymax or ymin based on that
	//if the grandparent of the node is null, set other bounds to max and min, it is the root
	//if the grandparent is non-null, again find bounds for the other parameter

	if (root->parent != NULL) // if not the root node
	{
		if (root->level %2 == 0) // parent sorts by y, grandparent sorts by x
		{
			if (root->parent->left == root) // if root is left child of parent
			{
				maxy = root->parent->loc.y;
			}
			else if (root->parent->right == root) // if root is right child of parent
			{
				miny = root->parent->loc.y;
			}
			if (root->parent->parent != NULL) // if root->parent is the tree root
			{
				if (root->parent->parent->left == root->parent) // if parent is left child of grandparent
				{
					maxx = root->parent->parent->loc.x;
				}
				else if (root->parent->parent->right == root->parent) // if root is right child of parent
				{
					minx = root->parent->parent->loc.x;
				}
			}
		}
		else // parent sorts by x, grandparent sorts by y
		{
			if (root->parent->left == root) // if root is left child of parent
			{
				maxx = root->parent->loc.x;
			}
			else if (root->parent->right == root) // if root is right child of parent
			{
				minx = root->parent->loc.x;
			}
			if (root->parent->parent != NULL) // if root->parent is the tree root
			{
				if (root->parent->parent->left == root->parent) // if parent is left child of grandparent
				{
					maxy = root->parent->parent->loc.y;
				}
				else if (root->parent->parent->right == root->parent) // if root is right child of parent
				{
					miny = root->parent->parent->loc.y;
				}
			}
		}

		location *tempminloc = malloc(sizeof(location));
		location *tempmaxloc = malloc(sizeof(location));
		tempminloc->x = minx;
		tempminloc->y = miny;
		tempmaxloc->x = maxx;
		tempmaxloc->y = maxy;

		if (location_distance_to_rectangle(p, tempminloc, tempmaxloc) > *d)
		{
			/*printf("eliminated subtree\n");
			printf("d = %lf, mind = %lf\n", *d, location_distance_to_rectangle(p, tempminloc, tempmaxloc));
			printf("min = (%lf,%lf) max = (%lf,%lf)\n", minx, miny, maxx, maxy);
			printf("root = (%lf,%lf)\n", root->loc.x, root->loc.y);
			printf("p = (%lf,%lf)\n\n", p->x, p->y);*/
			free(tempminloc);
			free(tempmaxloc);
			return;
		}
		free(tempminloc);
		free(tempmaxloc);
	}

	//call on the closer subtree
	if (compare(p, &(root->loc), root->level) == -1)//-1 if p is left of root, +1 otherwise
	{
		subtree_nearest_neighbor(root->left, p, neighbor, d);
		subtree_nearest_neighbor(root->right, p, neighbor, d);
	}
	else
	{
		subtree_nearest_neighbor(root->right, p, neighbor, d);
		subtree_nearest_neighbor(root->left, p, neighbor, d);
	}

}

/*
 finds minimum lat and lon of the subtree and returns a location containing these points
*/
location *minloc(node *root) 
{
	location *result = malloc(sizeof(location));
	result->x = root->loc.x;
	result->y = root->loc.y;
	return result;
}

/*
 finds maximum lat and lon of the subtree and returns a location containing these points
*/
location *maxloc(node *root)
{
	location *result = malloc(sizeof(location));
	result->x = root->loc.x;
	result->y = root->loc.y;
	return result;
}


/**                                                                                                      
 * Passes the points in the given tree to the given function                                             
 * in an arbitrary order.  The last argument to this function is also passed                             
 * to the given function along with each point.                                                          
 *                                                                                                       
 * @param t a pointer to a valid k-d tree, non-NULL                                                      
 * @param f a pointer to a function that takes a location and an extra                                   
 * argument, and does not modify t, non-NULL                                                             
 * @param arg a pointer                                                                                  
 */
void kdtree_for_each(const kdtree* r, void (*f)(const location *, void *), void *arg)
{
	for_each_subtree(r->root, f, arg);
}

void for_each_subtree(node *root, void (*f)(const location *, void *), void *arg)
{
	if (root != NULL)
	{
		for_each_subtree(root->left, f, arg);
		f(&(root->loc), arg);
		for_each_subtree(root->right, f, arg);
	}
}



/**                                                                                                      
 * Destroys the given k-d tree.  The tree is invalid after being destroyed.                              
 *                                                                                                       
 * @param t a pointer to a valid k-d tree, non-NULL                                                      
 */
void kdtree_destroy(kdtree *t)
{
	destroy_subtree(t->root);
	t->size = 0;
	t->root = NULL;
	free(t);
}

void destroy_subtree(node *root)
{
    if(root != NULL)
    {
        destroy_subtree(root->left);
        destroy_subtree(root->right);
        //free(root->loc);
        free(root);
    }
}




//Self defined functions (excluding subtree specific recursive functions)

//**need merge sort**

/**
 *allocates a new node under the parent with specified location
 *sets level to be 1 greater than parent's
 *sets children = null
 *does not set parent node to it (do that after calling)
 * alternative is to not take parent or child information,
 but will need to set parent and child pointers to it, as well
 as setting new node level (parent level +1) and size (child size +1)
*/
node *make_node(const location *loc, int level)
{
	node *newnode = malloc(sizeof(node));
	if (newnode == NULL)
	{
		return NULL;
	}

	/*newnode->loc = (location) malloc(sizeof(location));
	if (newnode->loc == NULL)
	{
		free(newnode);
		return NULL;
	}*/

	newnode->loc = *loc;
	newnode->level = level;
	newnode->height = 0;
	newnode->left = NULL;
	newnode->right = NULL;
	return newnode;
}


/**
 *given two locations and a level in the tree:
 * returns -1 if loc1 is left of loc2, +1 if loc1 is right of or equal to loc2
 */
int compare(const location *loc1, const location *loc2, int level)
{
	//if parent node level is even, compare by x
	//if parent node level is odd, compare by y
	if (level % 2 == 0)
	{
		if (loc1->x < loc2->x)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		if (loc1->y < loc2->y)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
}

void print_kdtree(const node *root)//redundant once for_each is finished
{
    if(root != NULL)
    {
        print_kdtree(root->left);

        for(int i = 0; i < (4*root->level); i++) {
            putchar(' ');
        }
        printf("x: %lf, y: %lf\n", root->loc.x, root->loc.y);

        print_kdtree(root->right);
    }
}

bool loc_compare(const location loc1, const location loc2)
{
	if (loc1.x == loc2.x && loc1.y == loc2.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}



void merge(int n1, const location a1[], int n2, const location a2[], location out[], int xory)
{
    int i1;
    int i2;
    int iout;

    i1 = i2 = iout = 0;
    if (xory == 0) // if 0, sort by x, if 1, sort by y
    {
        while(i1 < n1 || i2 < n2)
        {

            if(i2 >= n2 || ((i1 < n1) && (a1[i1].x < a2[i2].x))) 
            {
                /* a1[i1] exists and is smaller */
                out[iout++] = a1[i1++];
            }  
            else 
            {
                /* a1[i1] doesn't exist, or is bigger than a2[i2] */
                out[iout++] = a2[i2++];
            }
        }
        
    }
    else if (xory == 1) // if 0, sort by x, if 1, sort by y
    {
        while(i1 < n1 || i2 < n2)
        {
            if(i2 >= n2 || ((i1 < n1) && (a1[i1].y < a2[i2].y))) 
            {
                /* a1[i1] exists and is smaller */
                out[iout++] = a1[i1++];
            }  
            else 
            {
                /* a1[i1] doesn't exist, or is bigger than a2[i2] */
                out[iout++] = a2[i2++];
            }
        }
    }
}

/* sort a, putting result in out */
/* we call this mergeSort to avoid conflict with mergesort in libc */
/* xory is 0 if sort by x, 1 if sort by y */
void mergeSort(int n, const location a[], location out[], int xory)
{
    location *a1;
    location *a2;

    if(n < 2) {
        /* 0 or 1 elements is already sorted */
        memcpy(out, a, sizeof(location) * n);
    } else {
        /* sort into temp arrays */
        a1 = malloc(sizeof(location) * (n/2));
        a2 = malloc(sizeof(location) * (n - n/2));

        mergeSort(n/2, a, a1, xory);
        mergeSort(n - n/2, a + n/2, a2, xory);

        /* merge results */
        merge(n/2, a1, n - n/2, a2, out, xory);

        /* free the temp arrays */
        free(a1);
        free(a2);
    }
}

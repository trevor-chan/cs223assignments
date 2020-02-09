#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "location.h"



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
        memcpy(out, a, sizeof(int) * n);
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














void subtree_create(node *root, location * larray, int n, int level)
{
    if (n == 1) // if only one node passed, end subtree
    {
        subtree_add(root, &(xarray[0]));
        return;
    }

    if (level%2 == 0)//select by x, sort by y
    {
        location *xsorted = malloc(sizeof(location) * n);
        mergeSort(n, larray, xsorted, 0);

        if (subtree_add(root, &(xsorted[n/2])) == false)

        location *xleft = malloc(sizeof(location) * nleft);
        location *xright = malloc(sizeof(location) * nright);

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

        //initialize x arrays
        for (int i = 0; i < nleft; i++) 
        {
            xleft[i] = xsorted[i];
        }
        for (int i = 0; i < nright; i++)
        {
            xright[i] = xsorted[i+nleft+1];
        }

        //initialize y arrays
        int li = 0, ri = 0;
        for (int i = 0; i < n; i++)
        {
            if (xsorted[i].y)
        }

    }
    else //select by y, sort by x
    {
        if (kdtree_add(t, &(yarray[n/2])) == false)
        {
            kdtree_destroy(t);
            return;
        }

        location *yleft = malloc(sizeof(location) * nleft);
        location *yright = malloc(sizeof(location) * nright);
    }

    free(xsortleft)

}








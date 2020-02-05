#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "smap.h"

typedef struct
{
  char key[32];
  int *value;
  bool occupied;
  bool removed;
} entry;

struct smap
{
  int capacity;
  int size;
  int numremoved;
  entry *table; // INVARIANT: always at least one free slot in table
  int (*hash)(const char*);
};

#define smap_INITIAL_CAPACITY 1024

/**
 * Returns the index of the slot where the given key would be in a hash
 * table of the given capacity, assuming no collisions.
 *
 * @param key an integer
 * @param hash a hash function for integers, non-NULL
 * @param cap a positive integer
 */
int smap_compute_index(const char *key, int (*hash)(const char*), int cap);

/**
 * INCOMPLETE.  Intended to resize the hash table. ******************************************
 */
void smap_embiggen(smap *m);

/**
 * Adds the given (key, value) pair into the given hash table.  Since the
 * table and capacity of the table are given as parameters, this can be used
 * on both the main hash table and any temporary hash tables.
 *
 * @param table a pointer to the first slot in the hash table, non-NULL
 * @param key an integer
 * @param value a value
 * @param hash a hash function, non-NULL
 * @param cap a positive integer giving the number of slots in the hash table
 */
void smap_table_add(smap *m, const char *key, int *value, int (*hash)(const char*), int cap);

/**
 * Finds the given key in the given hash table.
 *
 * @param table a hash table with at least one unoccupied slot, non-NULL
 * @param key an integer
 * @param hash a hash function, non-NULL
 * @param cap a positive integer giving the number of slors in the hash table
 */
int smap_table_find_key(const entry *table, const char *key, int (*hash)(const char*), int capacity);



/**                                                                             
 * Returns a hash value for the given string.                                   
 *                                                                              
 * @param s a string, non-NULL                                                  
 * @return an int                                                               
 */
int smap_default_hash(const char *s)
{
    //Hash function obtained from http://www.cse.yorku.ca/~oz/hash.html
    unsigned long hash = 5381;
        int c;

        while((c = *s++))
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return (int) hash;
}

smap *smap_create(int (*h)(const char *s))
{
  smap *result = malloc(sizeof(smap));
  if (result != NULL)
    {
      result->size = 0;
      result->hash = h;
      result->numremoved = 0;
      result->table = malloc(sizeof(entry) * smap_INITIAL_CAPACITY);
      result->capacity = (result->table != NULL ? smap_INITIAL_CAPACITY : 0);
      
      // set all slots to unoccupied
      for (int i = 0; i < result->capacity; i++)
	{
	  result->table[i].occupied = false;
      result->table[i].removed = false;
	}
    }
    //printf("smap_created\n");
  return result;
}

int smap_size(const smap *m)
{
  return m->size;
}

/**
 * Returns the index where key is located in the given map, or the index
 * where it would go if it is not present.
 * 
 * @param table a table with at least one free slot
 * @param key a string, non-NULL
 * @param hash a hash function for strings
 * @param capacity the capacity of table
 * @return the index of key in table, or the index of the empty slot to put it in if it is not present
 */
int smap_table_find_key(const entry *table, const char *key, int (*hash)(const char *), int capacity)
{
    // compute starting location for search from hash function
    int i = smap_compute_index(key, hash, capacity);

    //print checks
    //table[i].occupied ? printf("table[%d].occupied = true\n", i) : 
    //printf("table[%d].occupied = false\n", i);
    //printf("key '%s' hashes to index %d\n", key, i);
    //end print checks

    while ((table[i].occupied && strcmp(table[i].key,key) != 0) || table[i].removed == true)
    {
        //printf("tried to increment table_find_key index\n");
        i = (i + 1) % capacity;
    }
    return i;
}

bool smap_put(smap *m, const char *key, void *value)
{
    int index = smap_table_find_key(m->table, key, m->hash, m->capacity);
    //printf("put index = %d\n", index);
    if (m->table[index].occupied)
    {
        //printf("key already present\n");
        // key already present
        m->table[index].value = value;
        return true;
    }
    else
    {
        if (m->size + 1 > m->capacity / 2)
	    {
	        // grow
            //printf("\n**tried to call embiggen**\n\n");
	        smap_embiggen(m);
            //printf("new capacity = %d\n", m->capacity);
            //printf("new size = %d\n", m->size);
	  
	        // add to table
	        if (m->size + 1 < m->capacity)
	        {
	            // need add here since the index of the free spot will
	            // have changed after rehashing
	            smap_table_add(m, key, value, m->hash, m->capacity);
	            m->size++;
	        }
            return true;
	    }
        else
	    {
	        // add to table at free spot we found
	        strcpy(m->table[index].key, key);
	        m->table[index].value = value;
	        m->table[index].occupied = true;
	        m->size++;
            //printf("added '%s' at index %d\n", key, index);
            return true;
	    }
    }
    //printf("put returned false\n");
    return false;
}

/**
 * Adds the given key and value into the given table.
 *
 * @param table a hash table with >= 2 unoccupied slots
 * @param key a string not present as a key in table, non-NULL
 * @param value a pointer to an integer, non-NULL
 * @param hash a hash function for strings
 * @param cap the number of slots in table
 */
void smap_table_add(smap *m, const char *key, int *value, int (*hash)(const char*), int cap)
{
  int i = smap_compute_index(key, hash, cap);
  while (m->table[i].occupied == true || m->table[i].removed == true)
    {
      i = (i + 1) % cap;
    }
  strcpy(m->table[i].key, key);
  m->table[i].value = value;
  m->table[i].occupied = true;
  if (m->table[i].removed == true)
  {
      m->table[i].removed = false;
      m->numremoved --;
  }
}

void smap_embiggen(smap *m)
{
    smap *copy;
    //struct entry *e;
    struct smap swap;
    int newcap;

    //calculate new size:
    double loadfactor = ((double)(m->size + m->numremoved))/m->capacity;
    //printf("loadfactor = %lf\n", loadfactor);
    if(loadfactor < 0.125)
    {
        newcap = m->capacity/2; 
    }
    else if (loadfactor >= .5)
    {
        newcap = m->capacity*2;
    }
    else
    {
        return;
    }

    //create with specified size
    copy = malloc(sizeof(smap));
    if (copy != NULL)
    {
        copy->size = 0;
        copy->hash = m->hash;
        copy->numremoved = 0;
        copy->table = malloc(sizeof(entry) * newcap);
        copy->capacity = (copy->table != NULL ? newcap : 0);
        for (int i = 0; i < copy->capacity; i++)
	    {
	        copy->table[i].occupied = false;
            copy->table[i].removed = false;
	    }
    }

    for(int i = 0; i < m->capacity; i++) 
    {
        if(m->table[i].occupied == true)
        {
            smap_table_add(copy, m->table[i].key, m->table[i].value, m->hash, newcap);
            copy->size ++;
        }
    }

    swap = *m;
    *m = *copy;
    *copy = swap;

    //destroy copy
    /*for(int i = 0; i < copy->capacity; i++)
    {
        free(copy->table[i].key);
        free(copy->table[i].value);
        free(m->table[i]);
    }*/
    free(copy->table);
    free(copy);

    //smap_destroy(copy);
}

int smap_compute_index(const char *key, int (*hash)(const char *), int cap)
{
  return (hash(key) % cap + cap) % cap;
}

bool smap_contains_key(const smap *m, const char *key)
{
  return m->table[smap_table_find_key(m->table, key, m->hash, m->capacity)].occupied;
}

void *smap_get(smap *m, const char *key)
{
    //printf("called smap_get\n");
    int index = smap_table_find_key(m->table, key, m->hash, m->capacity);
    //printf("get index = %d\n", index);
    if (m->table[index].occupied)
    {
        return m->table[index].value;
    }
    else
    {
        return NULL;
    }
}

/**                                                                             
 * Removes the given key and its associated value from the given map if         
 * the key is present.  The return value is NULL and there is no effect         
 * on the map if the key is not present.                                        
 *                                                                              
 * @param m a pointer to a map, non-NULL                                        
 * @param key a key, non-NULL                                                   
 * @return the value associated with the key                                    
 */
void *smap_remove(smap *m, const char *key)
{
    /*
        remove value
        change remove status true
        change occupied status false
        change map size -1
        change removed count +1
    */
    //int index = smap_compute_index(key, m->hash, m->capacity);

    int index = smap_table_find_key(m->table, key, m->hash, m->capacity);

    if(m->table[index].occupied == false)
    {
        //printf("tried to remove unoccupied index");
        return NULL;
    }
    
    m->table[index].occupied = false;
    m->table[index].removed = true;
    m->size --;
    m->numremoved ++;
    //printf("removed '%s' from index %d\n", key, index);
    return m->table[index].value;
}


void smap_for_each(smap *m, void (*f)(const char *, void *, void *), void *arg)
{
  for (int i = 0; i < m->capacity; i++)
    {
      if (m->table[i].occupied)
	{
	  f(m->table[i].key, m->table[i].value, arg);
	}
    }
}

/**                                                                             
 * Returns a dynamically array containing pointers to the keys in the           
 * given map.  It is the caller's responsibility to free the array,             
 * but the map retains ownership of the keys.  If there is a memory             
 * allocation error then the returned value is NULL.  If the map is empty       
 * then the returned value is NULL.                                             
 *                                                                              
 * @param m a pointer to a map, non NULL                                        
 * @return a pointer to a dynamically allocated array of pointer to the keys    
 */
const char **smap_keys(smap *m)
{
    char **keyarray = malloc (sizeof(char*) * m->size);
    //if (*keyarray == NULL) return NULL;

    int index = 0;
    for (int i = 0; i < m->capacity; i++)
    {
        if (m->table[i].occupied)
	    {
	        keyarray[index] = m->table[i].key;
            index ++;
	    }
    }
    return keyarray;
}

void smap_destroy(smap *m)
{
    // not our responsibility to free values
    //printf("call destroy\n");
    //free keys
    /*for(int i = 0; i < m->capacity; i++)
    {
        if(m->table[i].occupied == true)
        {
            free(m->table[i].key);
            //free(m->table[i].value);
            //free(m->table[i]);
        }
    }*/
    free(m->table);
    free(m);
}
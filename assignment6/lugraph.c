#define _GNU_SOURCE

#include <stdlib.h>
#include <stdbool.h>

#include "lugraph.h"

struct lugraph
{
  int n;          // the number of vertices                                                    
  int *list_size; // the size of each adjacency list                                           
  int *list_cap;  // the capacity of each adjacency list                                       
  int **adj;      // the adjacency lists                                                       
};

struct lug_search
{
  const lugraph *g;
  int from;
  int *dist;
  int *pred;
};

#define LUGRAPH_ADJ_LIST_INITIAL_CAPACITY 4

/**                                                                                            
 * Resizes the adjacency list for the given vertex in the given graph.                         
 *                                                                                             
 * @param g a pointer to a directed graph                                                      
 * @param from the index of a vertex in that graph                                             
 */
void lugraph_list_embiggen(lugraph *g, int from);

/**                                                                                            
 * Prepares a search result for the given graph starting from the given                        
 * vertex.  It is the responsibility of the caller to destroy the result.                      
 *                                                                                             
 * @param g a pointer to a directed graph                                                      
 * @param from the index of a vertex in that graph                                             
 * @return a pointer to a search result                                                        
 */
lug_search *lug_search_create(const lugraph *g, int from);

/**                                                                                            
 * Adds the to vertex to the from vertex's adjacency list.                                     
 *                                                                                             
 * @param g a pointer to a graph, non-NULL                                                     
 * @param from the index of a vertex in g                                                      
 * @param to the index of a vertex in g                                                        
 */
void lugraph_add_half_edge(lugraph *g, int from, int to);

lugraph *lugraph_create(int n)
{
  if (n < 1)
    {
      return NULL;
    }

  lugraph *g = malloc(sizeof(lugraph));
  if (g != NULL)
    {
      g->n = n;
      g->list_size = malloc(sizeof(int) * n);
      g->list_cap = malloc(sizeof(int) * n);
      g->adj = malloc(sizeof(int *) * n);

      if (g->list_size == NULL || g->list_cap == NULL || g->adj == NULL)
        {
          free(g->list_size);
          free(g->list_cap);
          free(g->adj);
          free(g);

          return NULL;
        }

      for (int i = 0; i < n; i++)
        {
          g->list_size[i] = 0;
          g->adj[i] = malloc(sizeof(int) * LUGRAPH_ADJ_LIST_INITIAL_CAPACITY);
          g->list_cap[i] = g->adj[i] != NULL ? LUGRAPH_ADJ_LIST_INITIAL_CAPACITY : 0;
        }
    }

  return g;
}

int lugraph_size(const lugraph *g)
{
  if (g != NULL)
    {
      return g->n;
    }
  else
    {
      return 0;
    }
}

void lugraph_list_embiggen(lugraph *g, int from)
{
  if (g->list_cap[from] != 0)
    {
      int *bigger = realloc(g->adj[from], sizeof(int) * g->list_cap[from] * 2);
      if (bigger != NULL)
        {
          g->adj[from] = bigger;
          g->list_cap[from] = g->list_cap[from] * 2;
        }
    }
}

void lugraph_add_edge(lugraph *g, int from, int to)
{
  if (g != NULL && from >= 0 && to >= 0 && from < g->n && to < g->n && from != to)
    {
      lugraph_add_half_edge(g, from, to);
      lugraph_add_half_edge(g, to, from);
    }
}

void lugraph_add_half_edge(lugraph *g, int from, int to)
{
  if (g->list_size[from] == g->list_cap[from])
    {
      lugraph_list_embiggen(g, from);
    }

  if (g->list_size[from] < g->list_cap[from])
    {
      g->adj[from][g->list_size[from]++] = to;
    }
}

bool lugraph_has_edge(const lugraph *g, int from, int to)
{
  if (g != NULL && from >= 0 && to >= 0 && from < g->n && to < g->n && from != to)
    {
      int i = 0;
      while (i < g->list_size[from] && g->adj[from][i] != to)
        {
          i++;
        }
      return i < g->list_size[from];
    }
  else
    {
      return false;
    }
}

int lugraph_degree(const lugraph *g, int v)
{
  if (g != NULL && v >= 0 && v < g->n)
    {
      return g->list_size[v];
    }
  else
    {
      return -1;
    }
}

/**                                                                                            
 * Returns the result of running breadth-first search on the given                             
 * graph starting with the given vertex.  When the search arrives                              
 * at a vertex, its neighbors are considered in the order the                                  
 * corresponding edges were added to the graph.  The result is returned                        
 * as a pointer to a lug_search that must subsequently be passed to                            
 * lugraph_search_destroy.                                                                     
 *                                                                                             
 * @param g a pointer to an undirected graph, non-NULL                                         
 * @param from the index of a vertex in the given graph                                        
 * @return a pointer to the result of the search                                               
 */
lug_search *lugraph_bfs(const lugraph *g, int from) //********************************************************
{
  return NULL;
}

/**                                                                                            
 * Determines if the two given vertices are connected by some path                             
 * in the given graph.                                                                         
 *                                                                                             
 * @param g a pointer to a directed graph, non-NULL                                            
 * @param v1 the index of a vertex in the given graph                                          
 * @param v2 the index of a vertex in the given graph                                          
 * @return true if the two vertices are connected in the graph, false otherwise                
 */
bool lugraph_connected(const lugraph *g, int from, int to) //********************************************************
{
  return false;
}

void lugraph_destroy(lugraph *g)
{
  if (g != NULL)
    {
      for (int i = 0; i < g->n; i++)
        {
          free(g->adj[i]);
        }
      free(g->adj);
      free(g->list_cap);
      free(g->list_size);
      free(g);
    }
}

lug_search *lug_search_create(const lugraph *g, int from)
{
  if (g != NULL && from >= 0 && from < g->n)
    {
      lug_search *s = malloc(sizeof(lug_search));

      if (s != NULL)
        {
          s->g = g;
          s->from = from;
          s->dist = malloc(sizeof(int) * g->n);
          s->pred = malloc(sizeof(int) * g->n);

          if (s->dist != NULL && s->pred != NULL)
            {
              for (int i = 0; i < g->n; i++)
                {
                  s->dist[i] = -1;
                  s->pred[i] = -1;
                }
            }
          else
            {
              free(s->pred);
              free(s->dist);
              free(s);
              return NULL;
            }
        }

      return s;
    }
  else
    {
      return NULL;
    }
}

/**                                                                                            
 * Returns the distance from the source to the given vertex after the                          
 * given search.  If the vertex was not reached then then return value is                      
 * -1.                                                                                         
 *                                                                                             
 * @param s a pointer to a search result, non-NULL                                             
 * @param to a vertex in the graph the search was performed in                                 
 * @return the distance from the source of the search to that vertex, or -1                    
 */
int lug_search_distance(const lug_search *s, int to) //********************************************************
{
  return -1;
}

/**                                                                                            
 * Returns the path from the staring point of the given search to the                          
 * given vertex.  The returned path starts with the origin of the                              
 * search and ends with the to vertex.  If no such path exists then                            
 * the return value is NULL and the length is -1.  The integer pointed                         
 * to by len is set to the length of (number of edges on) the path.                            
 * It is the caller's responsibility to free the returned array.                               
 *                                                                                             
 * @param s a pointer to a search result, non-NULL                                             
 * @param to a vertex in the graph the search was performed in                                 
 * @param len a pointer to an int, non-NULL                                                    
 * @return an array containing the vertices on the path, or NULL if there                      
 * is no such path                                                                             
 */
int *lug_search_path(const lug_search *s, int to, int *len) //********************************************************
{
  return NULL;
}

void lug_search_destroy(lug_search *s)
{
  if (s != NULL)
    {
      free(s->dist);
      free(s->pred);
      free(s);
    }
}
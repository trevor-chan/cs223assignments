#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "smap.h"

typedef struct
{
    int *distribution;
    double score;
    double wins;
    int matches;
}value;

typedef struct
{
    char *id;
    value v;
} entry;

typedef struct
{
    char p1[32];
    char p2[32];
} match;

typedef struct
{
    char *id;
    double pointscore;
} points;


typedef enum parse_state {ID, DISTRIBUTION} parse_state;

entry entry_read(FILE *in, int max_id, int battlefields);

void entry_destroy(entry *e);

int compare(const void *a, const void *b);


int main(int argc, char **argv)
{
    FILE * matchfile;
    matchfile = fopen(argv[1],"r");

    if (matchfile == NULL) {
        fprintf(stderr,"TSP: could not open %s\n",argv[1]);
        exit(0);
    }

    //read matchup file:
    match *marray = malloc(sizeof(match) * 4);//g
    int marraycapacity = 4;
    int nummatches = 0;
    char temp[32];

    while (fscanf(matchfile, "%31s", temp) != EOF)
	{
        //printf("%s ", temp);
        //check size of array
        if (nummatches == marraycapacity)
        {
            marray = realloc(marray, sizeof(match) * marraycapacity * 4);
            marraycapacity *= 4;
        }
        
        strcpy(marray[nummatches].p1, temp);
        fscanf(matchfile, "%31s", temp);
        //printf("%s\n", temp);
        strcpy(marray[nummatches].p2, temp);
        nummatches++;
	}

    //score or win
    int scoreorwin = 0;
    if (strcmp(argv[2],"score") == 0)
	{
        scoreorwin = 1;
	}
    else if (strcmp(argv[2],"win") == 0)
    {
        scoreorwin = 2;
    }
    else
    {
        printf("score or win invalid");
        exit(0);
    }

    //read battlefield values
    int numbattlefields = argc - 3;
    int bfvals [numbattlefields];
    for (int i = 0; i < numbattlefields; i ++)
    {
        bfvals[i] = atoi(argv[i+2])+1;
    }
    //printf("numbattlefields = %d\n\n", numbattlefields);
    
    //read standard input
    
    //make map of entries
    smap *entriesmap = smap_create(smap_default_hash);
    char ch;
    entry *tempentry = malloc(sizeof(entry));
    //value *tempvalue;
    while ((ch = getchar()) != EOF)
    {
        ungetc(ch, stdin);
        *tempentry = entry_read(stdin, 31, numbattlefields);

        if (tempentry->id == NULL || tempentry->v.distribution == NULL)
        {
            smap_destroy(entriesmap);
            free(tempentry);
            exit(1);
        }

        value *tempvalue = malloc(sizeof(value));

        tempvalue->matches = 0;
        tempvalue->wins = 0;
        tempvalue->score = 0;
        tempvalue->distribution = malloc(sizeof(int)*numbattlefields);
        for(int j = 0; j < numbattlefields; j++)
        {
            tempvalue->distribution[j] = tempentry->v.distribution[j];
        }

        smap_put(entriesmap, tempentry->id, tempvalue);
        //entry_destroy(tempentry);
        free(tempentry->id);
        free(tempentry->v.distribution);
    }
    free(tempentry);


    //insert
    /*
    const char **entriesarray = smap_keys(entriesmap);
    value *tempvalue;
    for (int i = 0; i < smap_size(entriesmap); i++)
    {
        tempvalue = (value *)(smap_get(entriesmap, entriesarray[i]));
        printf("%s ", entriesarray[i]);
        printf("score:%lf ",tempvalue->score);
        printf("wins:%lf ",tempvalue->wins);
        printf("matches:%d  ",tempvalue->matches);
        for (int j = 0; j < numbattlefields; j ++)
        {
            printf("%d ", tempvalue->distribution[j]);
        }
        printf("\n");
    }*/
    //end insert
    

    //for each match specified in file input
    entry *p1entry = malloc(sizeof(entry));
    p1entry->id = malloc(sizeof(char)*32);

    entry *p2entry = malloc(sizeof(entry));
    p2entry->id = malloc(sizeof(char)*32);

    value *newvalue;// = malloc(sizeof(value));


    for (int i = 0; i < nummatches; i++)
    {
        //read players of match

        if (smap_contains_key(entriesmap, marray[i].p1) == false || 
        smap_contains_key(entriesmap, marray[i].p2) == false)
        {
            exit(1);
        }
        
        strcpy(p1entry->id, marray[i].p1);
        strcpy(p2entry->id, marray[i].p2);

        p1entry->v = (*(value*) smap_get(entriesmap, p1entry->id));
        p2entry->v = (*(value*) smap_get(entriesmap, p2entry->id));

        double p1score = 0, p2score = 0;
        int p1val, p2val;


        //for each battlefield
        for (int j = 0; j < numbattlefields; j++)
        {

            p1val = p1entry->v.distribution[j];
            p2val = p2entry->v.distribution[j];
            //calculate scores and winners

            if(p1val == p2val)
            {
                p1score += bfvals[j]/2;
                p2score += bfvals[j]/2;
                if (bfvals[j]%2==1)
                {
                    p1score += 0.5;
                    p2score += 0.5;
                }
            }
            else if(p1val > p2val)
            {
                p1score += bfvals[j];
            }
            else if(p2val > p1val)
            {
                p2score += bfvals[j];
            }
            else
            {
                exit(1);
            }
        }

        p1entry->v.score += p1score;
        p2entry->v.score += p2score;

        if (p1score > p2score)
        {
            p1entry->v.wins += 1;
        }
        else if (p1score < p2score)
        {
            p2entry->v.wins += 1;
        }
        else if (p1score == p2score)
        {
            p1entry->v.wins += .5;
            p2entry->v.wins += .5;
        }
        

        p1entry->v.matches +=1;
        p2entry->v.matches +=1;

        //printf("matches %lf\n", p1entry->v.matches);
        //create new value struct and add put that in the map instead
        newvalue = malloc(sizeof(value));
        newvalue->matches = p1entry->v.matches;
        newvalue->wins = p1entry->v.wins;
        newvalue->score = p1entry->v.score;
        newvalue->distribution = malloc(sizeof(int)*numbattlefields);
        for(int j = 0; j < numbattlefields; j++)
        {
            newvalue->distribution[j] = p1entry->v.distribution[j];
        }
        //free(newvalue->distribution);

        free( (*(value*) smap_get(entriesmap,p1entry->id)).distribution);
        free( ((value*) smap_get(entriesmap,p1entry->id)));
        smap_put(entriesmap, p1entry->id, newvalue);

        newvalue = malloc(sizeof(value));
        newvalue->matches = p2entry->v.matches;
        newvalue->wins = p2entry->v.wins;
        newvalue->score = p2entry->v.score;
        newvalue->distribution = malloc(sizeof(int)*numbattlefields);
        for(int j = 0; j < numbattlefields; j++)
        {
            newvalue->distribution[j] = p2entry->v.distribution[j];
        }
        //free(newvalue->distribution);

        free( (*(value*) smap_get(entriesmap,p2entry->id)).distribution);
        free( ((value*) smap_get(entriesmap,p2entry->id)));
        smap_put(entriesmap, p2entry->id, newvalue);

    }

    //create array of entries and sort them
    const char **entriesarray = smap_keys(entriesmap);

    value *tempvalue;
    
    points *pointsarray = malloc(sizeof(points) * smap_size(entriesmap));//g

    if (scoreorwin == 2)
    {
        for (int i = 0; i < smap_size(entriesmap); i++)
        {
            pointsarray[i].id = entriesarray[i];
            p1entry->v = (*(value *)(smap_get(entriesmap, entriesarray[i])));
            pointsarray[i].pointscore = p1entry->v.wins/p1entry->v.matches;
        }
    }
    else if (scoreorwin == 1)
    {
        for (int i = 0; i < smap_size(entriesmap); i++)
        {
            pointsarray[i].id = entriesarray[i];
            p1entry->v = (*(value *)(smap_get(entriesmap, entriesarray[i])));            
            pointsarray[i].pointscore = p1entry->v.score/p1entry->v.matches;
        }
    }

    qsort(pointsarray, smap_size(entriesmap), sizeof(points), compare);

    for (int i = 0; i < smap_size(entriesmap); i++)
    {
        if (pointsarray[i].pointscore >= 1000)
        {
            printf("%lf %s\n", pointsarray[i].pointscore, pointsarray[i].id);
        }
        else if (pointsarray[i].pointscore >= 100)
        {
            printf("%7.3f %s\n", pointsarray[i].pointscore, pointsarray[i].id);
        }
        else if (pointsarray[i].pointscore >= 10)
        {
            printf("%7.3f %s\n", pointsarray[i].pointscore, pointsarray[i].id);
        }
        else if (pointsarray[i].pointscore > 0)
        {
            printf("%7.3f %s\n", pointsarray[i].pointscore, pointsarray[i].id);
        }
        else
        {
            printf("%7.3f %s\n", pointsarray[i].pointscore, pointsarray[i].id);
        }
    }

    free(p1entry->id);
    free(p2entry->id);
    free(p1entry);
    free(p2entry);


    //free values
    for(int i = 0; i < smap_size(entriesmap); i++)
    {
        free( (*(value*) smap_get(entriesmap,entriesarray[i])).distribution);
        free( ((value*) smap_get(entriesmap,entriesarray[i])));
    }

    //free ids
    /*for(int i = 0; i < smap_size(entriesmap); i++)
    {
        free(pointsarray[i].id);
    }*/
    free(pointsarray);
    
    fclose(matchfile);
    smap_destroy(entriesmap);
    free(entriesarray);
    free(marray);

}

int compare(const void *a, const void *b)
{
    const points *p1 = a;
    const points *p2 = b;
    //if (scoreorwin == 1)
    //{
        if (p1->pointscore < p2->pointscore)
        {
            return 1;
        }
        else if (p1->pointscore > p2->pointscore)
        {
            return -1;
        }
        else if (p1->pointscore == p2->pointscore)
        {
            if (strcmp(p1->id, p2->id) > 0)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }
        return 0;
}

//returns an allocated entry
//includes allocated id and distribution
entry entry_read(FILE *in, int max_id, int battlefields)
{
  entry result;

  if (in == NULL)
    {
      result.id = NULL;
      result.v.distribution = NULL;
      return result;
    }

  result.v.distribution = malloc(sizeof(int) * battlefields);
  result.id = malloc(sizeof(char) * (max_id + 1));

  if (result.v.distribution == NULL || result.id == NULL)
    {
      // allocation error; abort
      free(result.v.distribution);
      result.v.distribution = NULL;
      free(result.id);
      result.id = NULL;
    }
  else
    {
        
      // initialize FSM to read entry
      int ch;
      parse_state state = ID; // part of entry we're in (ID or DISTRIBUTION)    
      int id_len = 0; // chars in id read so far                                
      result.id[0] = '\0';
      int curr_bf = 0; // number of battlefields read so far                    
      int curr_int = 0; // value of digits in number of units read so far       
      
      //printf("start read of entry\n");
      
      while ((ch = fgetc(in)) != EOF && ch != '\n' && ch != '\r')
        {
          switch (state)
            {
            case ID:
              if (isspace(ch))
              {
                  ch = fgetc(in);
              }
              if (ch == ',')
                {
                  // start reading distribution of units                                     
                  state = DISTRIBUTION;
                }
              else if (id_len < max_id)
                {
                  // save character up to max (ignore characters beyond max)                 
                  result.id[id_len] = ch;
                  id_len++;
                  result.id[id_len] = '\0';
                }

              break;

            case DISTRIBUTION:
              if (ch == ',')
                {
                  // comma -- start reading next battlefield                                 
                  curr_bf++;
                  if (curr_bf >= battlefields)
                    {
                      // too many battlefields
                      entry_destroy(&result);
                      return result;
                    }

                  curr_int = 0;
                }
              else if (isdigit(ch)==0)
                {
                    //printf("not digit");
                  // non-comma in integer distribution                                       
                  entry_destroy(&result);
                  return result;
                }
              else if(isspace(ch) != 0)//if not enough battlefields
                {
                  entry_destroy(&result);
                  return result;
                }
              else
                {
                  // update value of integer we're currently reading                         
                  curr_int = curr_int * 10 + (ch - '0');
                  result.v.distribution[curr_bf] = curr_int;
                  //printf("read distribution %d\n", curr_bf);
                }
              break;
            }
        }

      // eat carriage-return before line-feed in case some DOS-mode files                    
      // sneak in                                                                            
      if (ch == '\r')
        {
          ch = fgetc(in);
        }

      // check that the number of battlefields was correct and if so,                        
      // whether the id is non-empty                                                         
      if ((id_len == 0 && curr_bf > 0)
          || (id_len > 0 && curr_bf != battlefields - 1))
        {
          entry_destroy(&result);
        }
      else if (id_len == 0)
        {
          // empty id and battlefields = 0 means end-of-input                                
          free(result.v.distribution);
          result.v.distribution = NULL;
        }
    }
    result.v.score = 0;
    result.v.wins = 0;
    result.v.matches = 0;
    //printf("return result\n\n");

  return result;
}

void entry_destroy(entry *e)
{
  if (e != NULL)
    {
      free(e->id);
      e->id = NULL;
      free(e->v.distribution);
      e->v.distribution = NULL;
      //free(e);
    }
}



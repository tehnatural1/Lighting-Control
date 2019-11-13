#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define HASHSIZE 101


struct dict
{
    struct dict *next;
    char *key;
    char *value;
};

static struct dict *hashtab[HASHSIZE];


unsigned hash(char *s)
{
    unsigned hashval;

    for (hashval = 0; *s != '\0'; s++)
    {
        hashval = *s + 31 * hashval;
    }

    return (hashval % HASHSIZE);
}


struct dict *search(char *s)
{
    struct dict *np;

    for (np = hashtab[hash(s)]; np != 0; np = np->next)
    {
        if (strcmp(s, np->key) == 0)
        {
            return np;
        }
    }

    return 0;
}


char *lookup(char *s)
{
    struct dict *d = search(s);
    return d->value;
}


struct dict *addentry(char *key, char *value)
{
    struct dict *np;
    unsigned hashval;


    if ((np = search(key)) == 0)
    {

        np = (struct dict *) malloc(sizeof(*np));
        if (np == 0 || (np->key = strdup(key)) == 0)
        {
            return 0;
        }

        hashval = hash(key);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    }
    else
    {
        printf("    Key exists! Removing old value: %s\n", np->value);

        free((void *) np->value);
    }


    if ((np->value = strdup(value)) == 0)
    {
       return 0;
    }


    return np;
}
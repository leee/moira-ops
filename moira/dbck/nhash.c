/* $Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/dbck/nhash.c,v 1.1 1996-10-24 21:35:05 danw Exp $
 *
 * Generic hash table routines.  Uses integer keys to store integer values.
 *
 *  (c) Copyright 1996 by the Massachusetts Institute of Technology.
 *  For copying and distribution information, please see the file
 *  <mit-copyright.h>.
 */

#include <mit-copyright.h>
#include <ctype.h>
/* #include <moira.h> */

struct int_bucket {
    struct int_bucket *next;
    int	key;
    int data;
};
struct int_hash {
    int	size;
    struct int_bucket **data;
};

extern char *malloc();

#define NULL 0
#define int_hash_func(h, key) (key >= 0 ? (key % h->size) : (-key % h->size))

/* Create an int_hash table.  The size is just a hint, not a maximum. */

struct int_hash *create_int_hash(size)
int size;
{
    struct int_hash *h;

    h = (struct int_hash *) malloc(sizeof(struct int_hash));
    if (h == (struct int_hash *) NULL)
      return((struct int_hash *) NULL);
    h->size = size;
    h->data = (struct int_bucket **) malloc(size * sizeof(char *));
    if (h->data == (struct int_bucket **) NULL) {
	free(h);
	return((struct int_hash *) NULL);
    }
    bzero(h->data, size * sizeof(char *));
    return(h);
}

/* Lookup an object in the int_hash table.  Returns the value associated with
 * the key, or NULL (thus NULL is not a very good value to store...)
 */

int int_hash_lookup(h, key)
struct int_hash *h;
register int key;
{
    register struct int_bucket *b;

    b = h->data[int_hash_func(h, key)];
    while (b && b->key != key)
      b = b->next;
    if (b && b->key == key)
      return(b->data);
    else
      return(0);
}


/* Update an existing object in the int_hash table.  Returns 1 if the object
 * existed, or 0 if not.
 */

int int_hash_update(h, key, value)
struct int_hash *h;
register int key;
int value;
{
    register struct int_bucket *b;

    b = h->data[int_hash_func(h, key)];
    while (b && b->key != key)
      b = b->next;
    if (b && b->key == key) {
	b->data = value;
	return(1);
    } else
      return(0);
}


/* Store an item in the int_hash table.  Returns 0 if the key was not previously
 * there, 1 if it was, or -1 if we ran out of memory.
 */

int int_hash_store(h, key, value)
struct int_hash *h;
register int key;
int value;
{
    register struct int_bucket *b, **p;

    p = &(h->data[int_hash_func(h, key)]);
    if (*p == NULL) {
	b = *p = (struct int_bucket *) malloc(sizeof(struct int_bucket));
	if (b == (struct int_bucket *) NULL)
	  return(-1);
	b->next = NULL;
	b->key = key;
	b->data = value;
	return(0);
    }

    for (b = *p; b && b->key != key; b = *p)
      p = (struct int_bucket **) *p;
    if (b && b->key == key) {
	b->data = value;
	return(1);
    }
    b = *p = (struct int_bucket *) malloc(sizeof(struct int_bucket));
    if (b == (struct int_bucket *) NULL)
      return(-1);
    b->next = NULL;
    b->key = key;
    b->data = value;
    return(0);
}


/* Search through the int_hash table for a given value.  For each piece of
 * data with that value, call the callback proc with the corresponding key.
 */

int_hash_search(h, value, callback)
struct int_hash *h;
register int value;
void (*callback)();
{
    register struct int_bucket *b, **p;

    for (p = &(h->data[h->size - 1]); p >= h->data; p--) {
	for (b = *p; b; b = b->next) {
	    if (b->data == value)
	      (*callback)(b->key);
	}
    }
}


/* Step through the int_hash table, calling the callback proc with each key.
 */

int_hash_step(h, callback, hint)
struct int_hash *h;
void (*callback)();
char *hint;
{
    register struct int_bucket *b, **p;

    for (p = &(h->data[h->size - 1]); p >= h->data; p--) {
	for (b = *p; b; b = b->next) {
	    (*callback)(b->key, b->data, hint);
	}
    }
}


/* Deallocate all of the memory associated with a table */

int_hash_destroy(h)
struct int_hash *h;
{
    register struct int_bucket *b, **p, *b1;

    for (p = &(h->data[h->size - 1]); p >= h->data; p--) {
	for (b = *p; b; b = b1) {
	    b1 = b->next;
	    free(b);
	}
    }
}
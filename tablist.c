//
//  tablist.c
//  prog-2022-11-20-DM
//
//  Created by Emmanuel Mera on 20/10/2022.
//

#define MAX_CAP 100

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int x, y;
    unsigned next, prev; // I used unsigned because we won't adress to negative values so we gain
                        // a bit of adress space
    
    // bool cant be smaller, even tho it would be possible to cheat with the fact that
    // the range of the pointers next and prev, because of the implementation wouldn't
    // a priori use en entire unsigned integer so we could use one bite to store this
    // the downside of this implementation would be that you would have to add one instruction (and)
    // to access to a memory cell.
    char is_free;
} elem_t;

typedef struct {
    unsigned    size;
    elem_t      tab[MAX_CAP];
    unsigned    first, last;
} tlist_t;

enum {
    IS_FREE,
    IS_USED
};

tlist_t* tlist_new(void) {
    tlist_t *t = calloc(sizeof(tlist_t), 1); // I use calloc to set all values to 0
    return t;
}

int tlist_free(tlist_t *l) {
    free(l);
    return sizeof(tlist_t);
}

int tlist_size(tlist_t *l) {
    if (l) return l->size;
    return 0;
}

int tlist_add(tlist_t *l, int x, int y) {
    if (!l) // we always remove the case of an empty pointer, just an idiot-proof case
        return 0;
    
    for (int i=0; i<MAX_CAP; i++) {
        if (l->tab[i].is_free == IS_FREE) {
            l->tab[i].is_free = IS_USED;
            l->tab[i].x = x;
            l->tab[i].y = y;
            l->tab[i].prev = l->last;
            l->tab[i].next = l->first;
            l->tab[l->first].prev = i;
            l->tab[l->last].next = i;
            if (l->size == 0) {
                l->first = i;
            }
            l->last = i;
            l->size++;
            return 1;
        }
    }
    return 0;
}

/// TODO: optimize the loop, i don't know right now to do that in a very optimized way, maybe changing the implementation...
int tlist_remove(tlist_t *l, int x, int y) {
    if (!l) return 0;
    if (l->size == 0) return 0;
    
    elem_t t = l->tab[l->first];
    for (int i=0; i<MAX_CAP; i++) {
        if (l->tab[i].x == x && l->tab[i].y == y) {
            l->tab[i].is_free = IS_FREE; // set it to free
            if (l->first == i) { // we want to make sure we maintain the head and the tail of the list
                l->first = l->tab[i].next;
            } else {
                l->tab[l->tab[i].prev].next = l->tab[i].next;
            }
            if (l->last == i) {
                l->last = l->tab[i].prev;
            }
            l->tab[i].next = l->tab[i].prev = 0;
            l->size--;
            return 1;
        }
    }
    return 0;
}

int tlist_pop(tlist_t *l) {
    if (!l) return 0;
    if (l->size == 0) return 0;
    if (l->size == 1) {
        l->tab[l->first].is_free = IS_FREE;
        l->size --;
        return 1;
    }
    l->tab[l->first].is_free = IS_FREE;
    l->first = l->tab[l->first].next;
    l->tab[l->first].prev = l->last;
    l->size--;
    return 1;
}

int tlist_top(tlist_t *l, int *x, int *y) {
    if (!l) return 0;
    if (l->size == 0) return 0;
    *x = l->tab[l->first].x;
    *y = l->tab[l->first].y;
    return 1;
}

// a simple push, we continue to maintain the fact that our list is circular
int tlist_push(tlist_t *l, int x, int y) {
    if (!l) return 0;
    
    for (int i=0; i<MAX_CAP; i++) {
        if (l->tab[i].is_free == IS_FREE) {
            l->tab[i].is_free = IS_USED;
            l->tab[i].x = x;
            l->tab[i].y = y;
            l->tab[i].prev = l->last;
            l->tab[i].next = l->first;
            l->tab[l->first].prev = i;
            l->tab[l->last].next = i;
            if (l->size == 0) {
                l->last = i;
            }
            l->first = i;
            l->size++;
            return 1;
        }
    }
    return 0;
}

// the swap don't actually swap the order of the cells but their value which became convenient in the furthur functions
int tlist_swap(tlist_t *l, int i, int j) {
    if (i >= MAX_CAP || j >= MAX_CAP) return 0;
    float tmpx = l->tab[i].x, tmpy = l->tab[i].y;
    l->tab[i].x = l->tab[j].x;
    l->tab[i].y = l->tab[j].y;
    l->tab[j].x = tmpx;
    l->tab[j].y = tmpy;

    return 1;
}

// we actually compute the square of the distance to (0, 0)
// we also cast the value to long unsigned so that the square won't overflow
long unsigned dist(elem_t point) {
    return (long unsigned)point.x*point.x + (long long unsigned)point.y*point.y;
}

// we divide our list in two based on the pivot point (the first point of the list)
int divide(tlist_t *l, int i, int j) {
    int d = i;
    int first = i;
    int p = l->tab[i].next;
    while (p != j) {
        if (dist(l->tab[p]) <= dist(l->tab[d])) {
            first = l->tab[first].next;
            tlist_swap(l, p, first);
        }
        p = l->tab[p].next;
    }
    if (dist(l->tab[p]) <= dist(l->tab[d])) {
        first = l->tab[first].next;
        tlist_swap(l, p, first);
    }
    tlist_swap(l, first, d);
    return first;
}

void sort(tlist_t *l, int i, int j) {
    if (i != j && ((j==l->last) ? 1 : i != l->tab[j].next)) {
        int div = divide(l, i, j);
        if (div != l->first) {
            sort(l, i, l->tab[div].prev);
        }
        if (div != l->last) {
            sort(l, l->tab[div].next, j);
        }
    }
}

// I'll do a quick sort even tho this is not optimal
int tlist_sort(tlist_t *l) {
    if (!l) {
        return 0;
    }
    sort(l, l->first, l->last);
    return 1;
}

int tlist_print(tlist_t *l) {
    if (!l) {
        return 0;
    }
    int p = l->first;
    while (p != l->last) {
        printf("(%d, %d) ", l->tab[p].x, l->tab[p].y);
        p = l->tab[p].next;
    }
    printf("(%d, %d)\n", l->tab[p].x, l->tab[p].y);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc % 2 != 1) {
        printf("Please enter an even number\n");
        return EXIT_FAILURE;
    }
    tlist_t *l = tlist_new();
    
    for (int i=1; i<argc-1; i+=2) {
        // just push the parameters 1 by 1
        tlist_push(l, atoi(argv[i]), atoi(argv[i+1]));
    }
    
    // sort and print
    tlist_sort(l);
    tlist_print(l);
    
    // clean up
    tlist_free(l);
    return EXIT_SUCCESS;
}

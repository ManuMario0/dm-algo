//
//  hlist.c
//  prog-2022-11-20-DM
//
//  Created by Emmanuel Mera on 13/11/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct hnode_t {
    struct hnode_t  *next, *prev, *up, *down;
    double          val; // we use float to don't have to bother too much about +inf and -inf
                         // and because of the alignment, we can be fancy and use a double type without any loose of performance (kinda)
} hnode_t;

typedef struct hlist_t {
    hnode_t     *head;
    unsigned    height;
} hlist_t;

hlist_t* hlist_new(void) {
    hlist_t *l = malloc(sizeof(hlist_t));
    l->height = 1;
    hnode_t *n = l->head = malloc(sizeof(hnode_t));
    n->val = -INFINITY;
    n->prev = n->up = n->down = NULL;
    n->next = malloc(sizeof(hnode_t));
    n->next->prev = n;
    n = n->next;
    n->val = INFINITY;
    n->next = n->up = n->down = NULL;
    return l;
}

int hlist_search (hlist_t *l, int v, hnode_t* path[]) {
    hnode_t *n = l->head;
    _Bool b = 0;
    path[0] = n;
    for (int i = 1; i<l->height; i++) {
        n = n->down;
        while (n->val <= (double)v) {
            n = n->next;
        }
        n = n->prev;
        if (n->val == (double)v) { // yup, this is possible only because one integer translate into the same float value
            b = 1;
        }
        path[i] = n;
    }
    return b;
}

int hlist_add(hlist_t *l, int v) {
    hnode_t **path = calloc(l->height, sizeof(hnode_t*));
    int i = 1;
    if (hlist_search(l, v, path)) {
        free(path);
        return 0;
    }
    srand(time(NULL));
    _Bool b = 1;
    while (b) {
        hnode_t *n = path[l->height-i];
        hnode_t *new = malloc(sizeof(hnode_t));
        // set the new pointer
        new->val = (double)v;
        new->next = n->next;
        new->prev = n;
        new->up = NULL;
        if (i != 1) {
            new->down = n->down->next;
            n->down->next->up = new;
        }
        // update surrounding pointers
        n->next->prev = new;
        n->next = new;
        
        if (l->height == i) { // if we are on top
            // -inf
            hnode_t *n = malloc(sizeof(hnode_t));
            l->head->up = n;
            n->down = l->head;
            l->head = n;
            n->val = -INFINITY;
            n->up = n->prev = NULL;
            // +inf
            n->next = malloc(sizeof(hnode_t));
            n->next->prev = n;
            n = n->next;
            n->val = INFINITY;
            n->next = n->up = NULL;
            // look up for the -inf of the line just underneath
            hnode_t *p = l->head->down;
            while (p->val != -INFINITY) {
                p = p->next;
            }
            p->up = n;
            
            // now we update just the value of the path we need
            path[1] = path[0];
            path[0] = l->head;
            
            l->height ++;
        }
        i++;
        b = rand() % 2;
    }
    
    free(path);
    return 1;
}

int hlist_remove(hlist_t *l, int v) {
    hnode_t **path = calloc(l->height, sizeof(hnode_t*));
    int i = 1;
    if (!hlist_search(l, v, path)) {
        free(path);
        return 0;
    }
    while (path[l->height-i]->val == (double)v) {
        path[l->height-i]->next->prev = path[l->height-i]->prev;
        path[l->height-i]->prev->next = path[l->height-i]->next;
        free(path[l->height-i]);
        i++;
    }
    free(path);
    return 1;
}

int main(int argc, char **argv) {
    hlist_t *l = hlist_new();
    for (int i=1; i<argc; i++) {
        hlist_add(l, atoi(argv[i]));
    }
    hnode_t *n = l->head;
    while (n->down != NULL) n = n->down;
    printf("-inf ");
    n = n->next;
    while (n->next != NULL) {
        printf("%d ", (int)n->val);
        n = n->next;
    }
    printf("inf\n");
    return 0;
}

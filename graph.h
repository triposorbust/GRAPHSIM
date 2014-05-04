#ifndef __GRAPH_H__
#define __GRAPH_H__

#ifndef __ASSERT_H_
#define __ASSERT_H__
#include <assert.h>
#endif

#ifndef __STDLIB_H__
#define __STDLIB_H__
#include <stdlib.h>
#endif

#ifndef __STDIO_H__
#define __STDIO_H__
#include <stdio.h>
#endif

#ifndef __STRING_H__
#define __STRING_H__
#include <string.h>
#endif

typedef void * GRAPH_P;

void init_graph(GRAPH_P *, int);
void add_edge(GRAPH_P, int, int, double);
double adjacent(GRAPH_P, int, int);
void neighbours(GRAPH_P, int, int *, double *, int);
int graph_size(GRAPH_P);
void print_graph(GRAPH_P);
void clean_graph(GRAPH_P *);

#endif

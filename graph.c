#ifndef __GRAPH_H__
#include "graph.h"
#endif

struct Edge {
  int dest;
  double weight;
  struct Edge *next;
};

struct Graph {
  struct Edge **vertices;
  int n;
};

void init_graph(GRAPH_P *ptr, int n)
{
  struct Graph *G = (struct Graph *) malloc(sizeof(struct Graph));
  G->n = n;
  G->vertices = (struct Edge **) malloc(sizeof(struct Edge *) * n);

  int i;
  for (i=0; i<n; ++i) {
    G->vertices[i] = NULL;
  }

  *ptr = (GRAPH_P) G;
}

static void _add_edge(struct Graph *G, int s, int t, double wt)
{
  struct Edge *old = G->vertices[s];

  struct Edge *new = (struct Edge *) malloc(sizeof(struct Edge));
  new->weight = wt;
  new->next = old;
  new->dest = t;

  G->vertices[s] = new;
}

void add_edge(GRAPH_P ptr, int v, int u, double wt)
{
  struct Graph *G = (struct Graph *) ptr;

  assert(v >= 0 && v < G->n);
  assert(u >= 0 && u < G->n);

  _add_edge(G, v, u, wt);
  _add_edge(G, u, v, wt);
}

double adjacent(GRAPH_P ptr, int v, int u)
{
  struct Graph *G = (struct Graph *) ptr;
  struct Edge *us = G->vertices[v];
  double ret = 0.0;

  while (NULL != us) {
    if (u == us->dest) {
      ret = us->weight;
      break;
    }
    us = us->next;
  }

  return ret;
}

void neighbours(GRAPH_P ptr, int v, int *ns, double *wts, int n)
{
  struct Graph *G = (struct Graph *) ptr;
  struct Edge *us = G->vertices[v];
  memset((void *) ns,  0xff, n * sizeof(int));
  memset((void *) wts, 0x00, n * sizeof(double));

  int i = 0;
  while (NULL != us && i < n) {
    wts[i] = us->weight;
    ns[i] = us->dest;
    us = us->next;
    ++i;
  }
}

int graph_size(GRAPH_P ptr)
{
  struct Graph *G = (struct Graph *) ptr;
  return G->n;
}

void clean_graph(GRAPH_P *ptr)
{
  struct Graph *G = (struct Graph *) *ptr;
  struct Edge *curr;

  int i;
  for (i=0; i<G->n; ++i) {
    while (G->vertices[i] != NULL) {
      curr = G->vertices[i];
      G->vertices[i] = curr->next;
      free(curr);
    }
  }

  free(G->vertices);
  free(G);
  *ptr = NULL;
}

void print_graph(GRAPH_P ptr)
{
  struct Graph *G = (struct Graph *) ptr;
  struct Edge *curr;
  int i;

  for (i=0; i<G->n; ++i) {
    printf("%d : ", i);
    curr = G->vertices[i];

    while (NULL != curr) {
      printf("%d [%.3lf] ", curr->dest, curr->weight);
      curr = curr->next;
    }
    printf("\n");
  }
}

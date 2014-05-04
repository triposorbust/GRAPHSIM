#ifndef __SIMULATE_H__
#include "simulate.h"
#endif

/* MAJOR HACK. */
#define BUFFER_SIZE 16


/*
 *
 * Initializing simulation parameters.
 *
 */

void init_spec(SPEC_P *ptr)
{
  double * m = (double *) malloc(sizeof(double) * N_STATES * N_STATES);
  memset((void *) m, 0x00, sizeof(double) * N_STATES * N_STATES);
  *ptr = (SPEC_P) m;
}

void set_spec(SPEC_P spec, State i, State j, double p)
{
  double * matrix = (double *) spec;
  matrix[N_STATES * i + j] = p;
}

void norm_spec(SPEC_P spec)
{
  int i,j;
  double row_sum, normed;
  double * matrix = (double *) spec;
  for (i=0; i<N_STATES; ++i) {
    row_sum = 0.0;
    for (j=0; j<N_STATES; ++j) {
      row_sum += matrix[N_STATES * i + j];
    }
    for (j=0; j<N_STATES; ++j) {
      normed = matrix[N_STATES * i + j] / row_sum;
      matrix[N_STATES * i + j] = normed;
    }
  }
}

void print_spec(SPEC_P spec)
{
  int i,j;
  double * matrix = (double *) spec;
  for (i=0; i<N_STATES; ++i) {
    for (j=0; j<N_STATES; ++j) {
      printf("%5.3lf  ", matrix[N_STATES * i + j]);
    }
    printf("\n");
  }
}

void clean_spec(SPEC_P *ptr)
{
  double * m = (double *) *ptr;
  free(m);
  *ptr = NULL;
}


/*
 *
 * Running simulation.
 *
 */

static double _prob(double *mat, State s, State t)
{
  return mat[N_STATES * s + t];
}

struct ThreadSpec {
  GRAPH_P graph;
  SPEC_P spec;

  State *prev;
  State *next;

  int start;
  int stop;
};

static State _trans(double *mat, State curr, double cont)
{
  State next = curr;
  double p = ((double) rand() / (double) RAND_MAX);

  double ps = _prob(mat, curr, STATE_SUSC);
  double pi = _prob(mat, curr, STATE_INF);
  double pr = _prob(mat, curr, STATE_REM);

  /* TODO: Will need to update these to accommodate other models. */
  if (STATE_SUSC == curr) {

    assert(0.0 == pr);
    if (p < pi * cont)
      next = STATE_INF;

  } 
  if (STATE_INF == curr) {

    assert(0.0 == ps);
    if (p < pr)
      next = STATE_REM;
  }
  if (STATE_REM == curr) {

    assert(1.0 == pr);

  }
  return next;
}

static void *_run(void *ptr)
{
  struct ThreadSpec *spec = (struct ThreadSpec *) ptr;
  double * matrix = (double *) spec->spec;

  int sz = graph_size(spec->graph);
  int *us = (int *) malloc(sizeof(int) * sz);
  double *ws = (double *) malloc(sizeof(double) * sz);

  int i,v,start,stop;
  start = spec->start;
  stop  = spec->stop;
  double cont = 0.0;
  for (v=start; v<=stop; ++v) {
    neighbours(spec->graph, v, us, ws, sz);
    for (i=0; i<sz; ++i) {
      if (-1 == us[i]) break;
      if (STATE_INF == spec->prev[us[i]]) {
        cont += ws[i];
      }
    }
    spec->next[v] = _trans(matrix, spec->prev[v], cont);
  }

  free(ws);
  free(us);
  pthread_exit(NULL);
}

static void set_specs(struct ThreadSpec *specs,
                      GRAPH_P graph,
                      SPEC_P spec,
                      int n)
{
  int graph_sz   = graph_size(graph);
  int per_thread = graph_sz / n;

  int i;
  for (i=0; i<n; ++i) {
    specs[i].graph = graph;
    specs[i].spec  = spec;

    specs[i].prev = NULL;
    specs[i].next = NULL;

    specs[i].start = i * per_thread;
    specs[i].stop  = specs[i].start + per_thread - 1;
  }
  specs[n-1].stop = graph_sz - 1;
}

static void push_state(struct Result **p_res, int sz)
{
  struct Result *old = *p_res;
  struct Result *new;
  new = *p_res = (struct Result *) malloc(sizeof(struct Result));

  new->next = old;
  new->index = (NULL == old) ? 0 : old->index + 1;
  new->state = (State *) malloc(sizeof(State) * sz);
  memset((void *) new->state, 0x00, sizeof(State) * sz);
}

static void set_state(struct Result **p_res, int sz, int *set)
{
  assert(NULL == *p_res);
  push_state(p_res, sz);

  int i,j;
  struct Result *res = *p_res;
  /* HACK! */
  for (i=0; i<BUFFER_SIZE; ++i) {
    j = set[i];
    if (-1 == j) break;
    res->state[j] = STATE_INF;
  }
  if (0 == i) {
    srand(time(NULL));
    j = rand() % sz;
    res->state[j] = STATE_INF;
  }
}

static int _delta(State *a, State *b, int n)
{
  int i,delta = 0;
  for (i=0; i<n; ++i) {
    if (a[i] != b[i]) {
      delta = 1;
      break;
    }
  }
  return delta;
}

static int _step(pthread_t *ts,
                 struct ThreadSpec *tss, 
                 struct Result **res,
                 int nt,
                 int sz)
{
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  push_state(res, sz);

  State *prev = (*res)->next->state;
  State *next = (*res)->state;

  int i,rc;
  for (i=0; i<nt; ++i) {
    tss[i].prev = prev;
    tss[i].next = next;

    rc = pthread_create(&ts[i], &attr, &_run, (void *) &tss[i]);
    if (rc) {
      printf("Error deploying thread.\n");
      exit(-1);
    }
  }

  pthread_attr_destroy(&attr);
  for (i=0; i<nt; ++i) {
    rc = pthread_join(ts[i], NULL);
    if (rc) {
      printf("Error joining thread.\n");
      exit(-1);
    }
  }

  return _delta(prev, next, sz);
}

void simulate(struct SimSpec *s_spec, struct Result **p_res)
{
  struct ThreadSpec *t_specs;
  pthread_t *threads;
  struct Result *results = NULL;
  int n_threads = s_spec->n_threads;
  int graph_sz = graph_size(s_spec->graph);

  if (-1 == s_spec->prng_seed) srand(time(NULL));
  else srand((unsigned int) s_spec->prng_seed);

  t_specs = (struct ThreadSpec *) malloc(sizeof(struct ThreadSpec)*n_threads);
  threads = (pthread_t *)  malloc(sizeof(pthread_t) * n_threads);
  set_specs(t_specs,s_spec->graph,s_spec->spec,n_threads);
  set_state(&results, graph_sz, s_spec->init_set);

  int n_steps  = 0, max_steps  = s_spec->max_steps;
  int n_static = 0, max_static = s_spec->term_cond;
  int delta;
  while (n_steps <= max_steps && n_static < max_static) {
    delta = _step(threads, t_specs, &results, n_threads, graph_sz);
    if (delta) n_static  = 0;
    else       n_static += 1;
    n_steps += 1;
  }

  free(t_specs);
  free(threads);
  *p_res = results;
}

void scrub_result(struct Result **p_res)
{
  struct Result *res = *p_res;
  struct Result *temp;
  while (res != NULL) {
    temp = res;
    res = temp->next;
    free(temp->state);
    free(temp);
  }
  *p_res = NULL;
}

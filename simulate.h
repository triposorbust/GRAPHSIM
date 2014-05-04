#ifndef __SIMULATE_H__
#define __SIMULATE_H__

#ifndef __ASSERT_H__
#define __ASSERT_H__
#include <assert.h>
#endif

#ifndef __STRING_H__
#define __STRING_H__
#include <string.h>
#endif

#ifndef __STDLIB_H__
#define __STDLIB_H__
#include <stdlib.h>
#endif

#ifndef __STDIO_H__
#define __STDIO_H__
#include <stdio.h>
#endif

#ifndef __PTHREAD_H__
#define __PTHREAD_H__
#include <pthread.h>
#endif

#ifndef __UNISTD_H__
#define __UNISTD_H__
#include <unistd.h>
#endif

#ifndef __GRAPH_H__
#include "graph.h"
#endif

#define N_STATES 3

typedef enum {
  STATE_SUSC=0,
  STATE_INF=1,
  STATE_REM=2
} State;

typedef void * SPEC_P;

void init_spec(SPEC_P *);
void set_spec(SPEC_P, State, State, double);
void norm_spec(SPEC_P);
void print_spec(SPEC_P);
void clean_spec(SPEC_P *);

struct Result {
  int index;
  State *state;
  struct Result *next;
};

struct SimSpec {
  GRAPH_P graph;
  SPEC_P spec;

  int *init_set;
  int n_threads;
  int prng_seed;
  int max_steps;
  int term_cond;
};

void simulate(struct SimSpec *, struct Result **);
void scrub_result(struct Result **);

#endif

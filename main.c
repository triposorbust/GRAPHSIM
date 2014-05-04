#ifndef __STDIO_H__
#define __STDIO_H__
#include <stdio.h>
#endif

#ifndef __STDLIB_H__
#define __STDLIB_H__
#include <stdlib.h>
#endif

#ifndef __GRAPH_H__
#include "graph.h"
#endif

#ifndef __SIMULATE_H__
#include "simulate.h"
#endif

#ifndef __PARSER_H__
#include "parser.h"
#endif

void dump(struct Result *res, char *fn, int sz)
{
  FILE *fd = stdout;
  if ('\0' != fn[0]) {
    fd = fopen(fn, "w");
    if (NULL == fd) {
      printf("Could not open file: \"%s\".\n", fn);
      exit(-1);
    }
  }

  int i;
  fprintf(fd,"N,");
  for (i=0; i<sz; ++i) {
    fprintf(fd, "%d", i);
    if (i != sz-1) fprintf(fd, ",");
    else fprintf(fd, "\n");
  }

  while (NULL != res) {
    fprintf(fd, "%d,", res->index);
    for (i=0; i<sz; ++i) {
      fprintf(fd, "%d", res->state[i]);
      if (i != sz-1) fprintf(fd, ",");
      else fprintf(fd, "\n");
    }
    res = res->next;
  }

  if (fd != stdout) {
    fclose(fd);
  }
}

void populate(struct SimSpec *ss, GRAPH_P graph, SPEC_P spec, void **opts)
{
  ss->graph = graph;
  ss->spec  = spec;

  ss->init_set  =  (int *) opts[OPTION_INIT];
  ss->n_threads = *(int *) opts[OPTION_THREADS];
  ss->prng_seed = *(int *) opts[OPTION_SEED];
  ss->max_steps = *(int *) opts[OPTION_STEPS];
  ss->term_cond = *(int *) opts[OPTION_TERM];
}

int main(int argc, char **argv)
{
  void **opts;
  init_options(&opts);

  GRAPH_P graph;
  SPEC_P spec;

  parse_options(&argc, argv, opts);
  parse_graph(&graph, argv[1]);
  parse_spec(&spec, argv[2]);

  struct Result *res;
  struct SimSpec *ss;
  ss = (struct SimSpec *) malloc(sizeof(struct SimSpec));
  populate(ss, graph, spec, opts);
  simulate(ss, &res);
  dump(res, (char *) opts[OPTION_OUTPUT], graph_size(graph));

  scrub_result(&res);
  clean_graph(&graph);
  clean_spec(&spec);

  free(ss);
  free_options(opts);
  return 0;
}

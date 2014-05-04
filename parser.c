#ifndef __PARSER_H__
#include "parser.h"
#endif

#define N_OPTIONS 7
#define STR_BUFFER_SIZE 256
#define INT_BUFFER_SIZE 16

static const char LETTERS[] = { 'h', 't', 's', 'i', 'n', 'x', 'o' };
static const char *DESCRIPTIONS[] = {
  "Prints help menu to standard output.",
  "Number of compute threads to use. (dflt: 2)",
  "Pseudo-random number generator seed. (dflt: ~time~)",
  "Adds node to vertex subset zero. (dflt: ~rand~)",
  "Maximum number of simulation steps. (dflt: 100)",
  "Fixed point termination condition. (dflt: 10)",
  "File to output results. (dflt: ~stdout~)"
};

static void print_help(void)
{
  int i;
  printf("\n");
  printf("GRAPHSIM: Computational epidemiology software to simulate\n");
  printf("          disease propagation on contact networks. Inputs\n");
  printf("          from files specifying graph structure & disease\n");
  printf("          epidemiological modeling parameters.\n\n");
  printf("Citation: In preparation.\n\n");
  printf("Usage:\ngraphsim [options] graph spec\n\n");
  printf("Options:\n");
  for (i=0; i<N_OPTIONS; i++) {
    printf("  -%c    %s\n",
           LETTERS[i],
           DESCRIPTIONS[i]);
  }
  printf("\nThis software is distributed under the MIT License.\n");
  printf("Copyright (c) 2014 Andy Chiang (Columbia University).\n\n");
  return;
}

void init_options(void ***pointer)
{
  *pointer = (void **) malloc(N_OPTIONS * sizeof(void *));
  void **options = *pointer;

  options[OPTION_HELP] = (int *) malloc(sizeof(int));
  * (int *) options[OPTION_HELP] = 0;

  options[OPTION_THREADS] = (int *) malloc(sizeof(int));
  * (int *) options[OPTION_THREADS] = 2;

  options[OPTION_SEED] = (int *) malloc(sizeof(int));
  * (int *) options[OPTION_SEED] = -1;

  options[OPTION_INIT] = (int *) malloc(INT_BUFFER_SIZE * sizeof(int));
  memset((void *) options[OPTION_INIT], 0xff, INT_BUFFER_SIZE * sizeof(int));

  options[OPTION_STEPS] = (int *) malloc(sizeof(int));
  * (int *) options[OPTION_STEPS] = 100;

  options[OPTION_TERM] = (int *) malloc(sizeof(int));
  * (int *) options[OPTION_TERM] = 10;

  options[OPTION_OUTPUT] = (char *) malloc(STR_BUFFER_SIZE * sizeof(char));
  bzero((void *) options[OPTION_OUTPUT], STR_BUFFER_SIZE * sizeof(char));
}

void free_options(void **options)
{
  int i;
  for (i=0; i<N_OPTIONS; ++i)
    if (NULL != options[i]) free(options[i]);
  if (NULL != options) free(options);
}

void parse_options(int *argc, char **argv, void *options[])
{
  int i,j;
  int n_args = 1;
  int n_init = 0;

  for (i=1; i<*argc; i++) {
    if (argv[i][0] == '-' && strlen(argv[i]) == 2) {
      i++;

      for (j=0; j<N_OPTIONS; j++) {
        if (argv[i-1][1] == LETTERS[j]) {
          switch (j) {
            case OPTION_HELP:
              * (int *) options[OPTION_HELP] = 1;
              break;
            case OPTION_THREADS:
              * (int *) options[OPTION_THREADS] = atoi(argv[i]);
              break;
            case OPTION_SEED:
              * (int *) options[OPTION_SEED] = atoi(argv[i]);
              break;
            case OPTION_INIT:
              ((int *) options[OPTION_INIT])[n_init] = atof(argv[i]);
              n_init++;
              break;
            case OPTION_STEPS:
              * (int *) options[OPTION_STEPS] = atoi(argv[i]);
              break;
            case OPTION_TERM:
              * (int *) options[OPTION_TERM] = atoi(argv[i]);
              break;
            case OPTION_OUTPUT:
              strcpy((char *) options[OPTION_OUTPUT], argv[i]);
              break;
            default:
              break;
          }
        }
      }
    } else {
      if (n_args < i) {
        strcpy(argv[n_args], argv[i]);
      }
      n_args++;
    }
  }

  *argc = n_args;
  if (* (int *) options[OPTION_HELP]) {
    print_help();
    exit(0);
  }
  if (n_args != 3) {
    printf("Usage:\ngraphsim [options] graph spec\n");
    exit(-1);
  }
}

static void _parse_spec(void **p_obj, FILE *fp)
{
  SPEC_P p_spec;
  int i,j;
  double wt;
  init_spec(&p_spec);

  for (i=0; i<N_STATES; ++i) {
    for (j=0; j<N_STATES; ++j) {
      if (N_STATES - 1 != j) fscanf(fp, "%lf,", &wt);
      else                   fscanf(fp, "%lf\n", &wt);
      set_spec(p_spec, i, j, wt);
    }
  }

  norm_spec(p_spec);
  *p_obj = (void *) p_spec;
}

static void _parse_graph(void **p_obj, FILE *fp)
{
  GRAPH_P p_graph;
  int n = 0;
  int i = fscanf(fp, "%d\n", &n);
  init_graph(&p_graph, n);

  double wt;
  int u,v;
  while (EOF != i) {
    i = fscanf(fp, "%d,%d,%lf\n", &u, &v, &wt);
    if (3 == i) add_edge(p_graph, u, v, wt);
  }

  *p_obj = (void *) p_graph;
}

static void parse_file(void (*callback)(void **, FILE *),
                       void **p_obj,
                       char *filename)
{
  if (NULL == p_obj || '\0' == filename[0]) {
    printf("Could not load data from \"%s\" to %p.\n", filename, p_obj);
    exit(-1);
  }

  FILE *fp;
  if ( (fp = fopen((const char *) filename, "r")) ) {
    callback(p_obj, fp);
    fclose(fp);
  } else {
    printf("Failed to open file: \"%s\".\n", filename);
    exit(-1);
  }
}

void parse_graph(GRAPH_P *p_graph, char *filename)
{
  parse_file(&_parse_graph, (void **) p_graph, filename);
}

void parse_spec(SPEC_P *p_spec, char *filename)
{
  parse_file(&_parse_spec, (void **) p_spec, filename);
}

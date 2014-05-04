#ifndef __PARSER_H__
#define __PARSER_H__

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

#ifndef __GRAPH_H__
#include "graph.h"
#endif

#ifndef __SIMULATE_H___
#include "simulate.h"
#endif

enum { OPTION_HELP,
       OPTION_THREADS,
       OPTION_SEED,
       OPTION_INIT,
       OPTION_STEPS,
       OPTION_TERM,
       OPTION_OUTPUT };

/*
 *               ARG    ARG     ARG
 *                ^      ^       ^
 *                |      |       |
 * [0x1234]--> [  1   ][ 2 ]...[ n ]
 *  0xabcd      0x1234
 */

void init_options(void ***pointer);
void parse_options(int *argc, char **argv, void **options);
void free_options(void **options);

void parse_graph(GRAPH_P *, char *);
void parse_spec(SPEC_P *, char *);

#endif

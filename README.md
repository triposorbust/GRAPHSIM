GRAPHSIM: Network Epidemiology
===

Simulating disease spread on contact networks
---


Graphs provide a powerful formalism for expressing connections between individuals and across which infectious disease can spread. This package `GRAPHSIM` provides software for simulating ID propagation on provided contact networks.


### Quickstart

Build the software by using the default `Makefile` target:

```
% make
```

Run the compiled `graphsim` binary:

```
% graphsim <graph-file> <parameter-file>
```

The first argument specifies `N` size of the graph in the first line, and `s,t,wt` in tuple notation the edges in the graph. The second argument file specifies in CSV format the compartment transitions.


### Background

Understanding disease propagation is an important step toward predicting and mitigating infectious disease outbreaks. Simulations, in particular, provide an _in silico_ way of both observing and perturbing "outbreak" to better inform medical and public health decisions.

This package provides a platform for running this type of simulation. For specifyied underlying contact network structure and parameters of disease propagation, program simulates disease spread through the population.


### Data Files

This package requires, as input, CSV files specifying a contact graph and a matrix of compartment transition rates.

Graph files should specify (i) The integer size of the graph (cardinality of `V`) and (ii) The triples `(s,t,wt)` corresponding to weighted undirected edges.

```
4
0,1,0.5
0,2,0.1
1,2,0.75
...
```

Parameter files provide a `3x3` matrix, comma-separated, that specify the rates of transition between susceptible, infected, and resistant populations. Note that zero-ing probabilities enables reduction to limiting cases.

```
0.8,0.2,0.0
0.0,0.8,0.2
0.0,0.0,1.0
```

In general, these files will determine the behaviour of the system.


### Output

Output is in CSV form. Each row corresponds to an abstracted "time step" in the simulation, and columns represent the state of individuals in the network. Right-hand-side columns, additionally, provide summary statistics of disease propagation.

```
N,1,2,3,...,TOT,FRAC
1,0,0,0,...,1,0.05
...
```

Further analysis options are provided in the associated analysis packages.


### Options

Use `-h` help flag to output a full help banner. Otherwise, options are summarized below:

```
  -h    Prints help menu to standard output.
  -t    Number of compute threads to use.
  -s    Pseudo-random number generator seed.
  -i    Adds node to vertex subset zero.
  -n    Maximum number of simulation steps.
  -x    Fixed point termination condition.
  -o    File to output results.
```

Most of the behaviour of the simulation is specified instead through the required argument files. For modification of apparent _behaviour_, more likely to obtain results by modifying the parameter files.

### Authors

 - Andy Chiang (Columbia University)
 - ...


### License

This software is distributed under the MIT License.
Copyright &copy; 2014 Andy Chiang (Columbia University).
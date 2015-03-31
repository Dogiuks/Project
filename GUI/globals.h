#ifdef __cplusplus
extern "C"
{
#endif __cplusplus

#ifndef GLOBALS_H
#define GLOBALS_H

static int NUM_COL = 1000;
static int NUM_ROW = 1;
static int NUM_NODES;
static int LEVELS_BACK = 100;
static int POPULATION_SIZE = 25;
static int PENALTY = 1;
static int NUM_GENERATIONS = 500;

static int MU = 2;
//all population is mutated and parents not preserved
static int PRESERVE_PARENTS = 1;
//ELITISM+TOURNAMENT+ROULETTE = MU
static int ELITISM = 1;
static int TOURNAMENT = 0;
static int ROULETTE = 0;

static int T_SIZE = 8;
static int MUTATION_RATE; //%

static int warehouse_size;
static int num_gates;
static int  num_files;
static outputtype *output_array;

static int warehouse_grid_x;
static int warehouse_grid_y;
static int *warehouse_grid;
static struct file *files;
static struct products* empty_input;
static struct delivery* deliveries;
//struct products output[100];
static struct location* warehouse;
static int first_run;
static node_pointer* input;

static int* old_fitness;

static int best_fitness;

#endif // GLOBALS_H


#ifdef __cplusplus
}
#endif __cplusplus


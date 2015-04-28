#ifdef __cplusplus
extern "C"
{
#endif __cplusplus

#ifndef GLOBALS_H
#define GLOBALS_H

static int NUM_COL = 1000;
static int NUM_ROW = 1;
int NUM_NODES;
static int LEVELS_BACK = 100;
static int POPULATION_SIZE = 25;
static double PENALTY = 1;
static int NUM_GENERATIONS = 500;

static int MU = 2;
//all population is mutated and parents not preserved
static int PRESERVE_PARENTS = 1;
//ELITISM+TOURNAMENT+ROULETTE = MU
static int ELITISM = 1;
static int TOURNAMENT = 0;
static int ROULETTE = 0;

static int T_SIZE = 8;
static float MUTATION_RATE = 0.1; //%

int warehouse_size;
int num_gates;
int  num_files;
outputtype *output_array;

int warehouse_grid_x;
int warehouse_grid_y;
int *warehouse_grid;
struct file *files;
struct products* empty_input;
struct delivery* deliveries;
//struct products output[100];
struct location* warehouse;
int first_run;
node_pointer* input;
struct node* best_ch;
int* old_fitness;

int best_fitness;

#endif // GLOBALS_H


#ifdef __cplusplus
}
#endif __cplusplus


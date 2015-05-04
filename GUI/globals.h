#ifdef __cplusplus
extern "C"
{
#endif __cplusplus

#ifndef GLOBALS_H
#define GLOBALS_H

static int NUM_COL = 1000;
static int NUM_ROW = 3;
int NUM_NODES;
static int LEVELS_BACK = 50;
static int POPULATION_SIZE = 25;
static double PENALTY = 1;
static int NUM_GENERATIONS = 5000;

static int MU = 2;
//all population is mutated and parents not preserved
static int PRESERVE_PARENTS = 1;
//ELITISM+TOURNAMENT+ROULETTE = MU
static int ELITISM = 1;
static int TOURNAMENT = 4;
static int ROULETTE = 0;

static int T_SIZE = 8;
static double MUTATION_RATE = 0.3; //%

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
product_pointer* input;
struct node* best_ch;
double* old_fitness;

double best_fitness;
int missing;
static int done =0;

#endif // GLOBALS_H


#ifdef __cplusplus
}
#endif __cplusplus


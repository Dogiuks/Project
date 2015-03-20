/* cgp.h
   Domas Druzas
*/

#ifdef __cplusplus
extern "C"
{
#endif __cplusplus

#include <stdio.h>
#include <time.h>
#include "cgp_warehouse.h"

#ifndef __CGP_H__

#define __CGP_H__


#define NUM_COL 10000
#define NUM_ROW 1
#define NUM_NODES NUM_ROW*NUM_COL
#define LEVELS_BACK 100
#define POPULATION_SIZE 25
#define PENALTY 1
#define NUM_GENERATIONS 50000

#define MU 2
//all population is mutated and parents not preserved
#define PRESERVE_PARENTS
//ELITISM+TOURNAMENT+ROULETTE = MU
#define ELITISM 1
#define TOURNAMENT 1
//#define ROULETTE 1

#ifdef TOURNAMENT
#define T_SIZE 8
#endif //TOURNAMENT
#define MUTATION_RATE 0.1 //%



enum Functions
{
    A,
    B,
    A_IF_MORE,
    A_IF_LESS,
    A_IF_TWICE,
    A_IF_HALF,
    A_IF_MORE_DEMAND,
    A_IF_LESS_DEMAND,
    A_IF_TWICE_DEMAND,
    A_IF_HALF_DEMAND,
    FUNC_COUNT,
};

struct node
{
    enum Functions func;
    int a;
    int b;
    struct prducts* out;
};

typedef struct node* node_pointer;

int first_run;
node_pointer* input;

int* old_fitness;

int best_fitness;

void make_output(void);
int find_worst_pick(int* pick_list, int list_size, int gate);
void run_EA(void);
void create_population(node_pointer* population);
void evaluate_population(node_pointer* population, int* evaluation);
struct node* create_chromosome(void);
int evaluate_output(product_pointer* output, struct products* product_count, int num_of_products);
struct node create_random_node(int collumn);
void create_input(struct products* product_count, int num_of_products);
void copy_chromosome(struct node* source, struct node* destination);
void select_parents(node_pointer* population, int* fitness);
void decode(struct node* ch, product_pointer* outputs);
void mutate(struct node* population);
void compute_node(int i, struct node* ch);
void print_product_list(struct node* ch, char output_file[MAX_FILENAME]);
struct products* compute_output(struct products* a, struct products* b, enum Functions func);
void clean_chromosome(struct node* ch);
int evaluate_position(product_pointer* output, struct delivery deliv);
int evaluate_output(product_pointer* output, struct products* product_count, int num_of_products);

#endif

#ifdef __cplusplus
}
#endif __cplusplus


/* cgp.h
   Domas Druzas
*/


#ifdef __cplusplus
extern "C"
{
#endif __cplusplus

#ifndef __CGP_H__

#define __CGP_H__

#include <stdio.h>
#include <time.h>
#include "cgp_warehouse.h"

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

void create_output(struct node* ch);
void coppy_output(product_pointer* source, product_pointer* destination);
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

product_pointer* return_selected_output(int i);
int get_elitism(void);
int get_levels_back(void);
int get_mutation_rate(void);
int get_num_row(void);
int get_num_col(void);
int get_num_generations(void);
double get_penalty(void);
int get_population_size(void);
int get_preserve_parents(void);
int get_roulete(void);
int get_tournament(void);
int get_t_size(void);
void set_elitism(int a);
void set_levels_back(int a);
void set_mutation_rate(float a);
void set_num_row(int a);
void set_num_col(int a);
void set_num_generations(int a);
void set_penalty(double a);
void set_population_size(int a);
void set_preserve_parents(int a);
void set_roulete(int a);
void set_tournament(int a);
void set_t_size(int a);

#endif

#ifdef __cplusplus
}
#endif __cplusplus


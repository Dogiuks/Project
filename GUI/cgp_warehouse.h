/* cgp_warehause.h
   Domas Druzas
*/


#ifdef __cplusplus
extern "C"
{
#endif __cplusplus
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define MAX_FILENAME 255

#ifndef __CGP_WAREHOUSE__

#define __CGP_WAREHOUSE__



int warehouse_size;
int num_gates;

struct location
{
    int  product_code;
    int*  dist;
};

struct products
{
    char name[50];
    int  qnt;
    int  code;
    int  demand;
};

struct delivery
{
    int  gate;
    int  list_size;
    struct products* load;
};

struct file
{
    int in;
    char name[MAX_FILENAME];
    struct delivery deliv;
};

typedef struct products* product_pointer;

int  num_files;

struct file *files;
struct products* empty_input;
struct delivery* deliveries;
struct products output[100];
struct location* warehouse;

/* function prototypes */


void create_empty_input(void);
void define_warehouse(char warehousefile[MAX_FILENAME]);
struct delivery read_deliveries(char productsfile[MAX_FILENAME]);
struct delivery read_input(char productsfile[MAX_FILENAME]);
void read_files(char file_list[MAX_FILENAME]);
void make_deliveries(void);
void print_warehouse(void);
void create_dummy_output(void);
#endif
#ifdef __cplusplus
}
#endif __cplusplus




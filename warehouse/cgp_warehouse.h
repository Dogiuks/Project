/* cgp_warehause.h
   Domas Druzas
*/


#include <stdio.h>
#include <time.h>
#include <stdbool.h>
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
}product;

struct delivery
{
    int  gate;
    int  list_size;
    struct products* load;
};

struct file
{
    bool in;
    char name[MAX_FILENAME];
    struct delivery deliv;
};

typedef struct products* product_pointer;

int  num_files;

struct file *files;
struct products* empty_input;
struct delivery* deliveries;
//struct products* product_count;
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

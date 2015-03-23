/* cgp_functions_warehouse.c
   Domas Druzas 2015
*/

#include "cgp_warehouse.h"
#include "cgp.h"

#ifdef __cplusplus
extern "C"
{
#endif __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


void define_warehouse(char warehousefile[MAX_FILENAME], char gridfile[MAX_FILENAME])
{
    FILE*	fp;
    char dummy[50];
    int i,j,no;

    fp=fopen(warehousefile,"r");
    if (!fp)
	{
		printf("Missing file: %s\n",warehousefile);
		exit(1);
	}
	fscanf(fp,"%d %s", &warehouse_size,dummy);
	printf("Warehouse contains %d locations\n", warehouse_size);
	fscanf(fp,"%d %s", &num_gates,dummy);
	printf("Warehouse contains %d loading gates\n", num_gates);

	warehouse = (struct location *)malloc(sizeof(struct location)*warehouse_size);
	for(i=0; i<warehouse_size; i++)
    {
        fscanf(fp,"%d", &no);
        warehouse[i].dist = (int *)malloc(sizeof(int)*num_gates);
        warehouse[i].product_code = 0;
        if(no != i+1)
        {
            printf("File is invalid on line %d\n",no);
            exit(1);
        }
        for(j=0; j<num_gates; j++)
        {
            fscanf(fp,"%d", &warehouse[i].dist[j]);
        }
    }
    printf("test");
    fclose(fp);
    fp=fopen(gridfile,"r");
    if (!fp)
    {
        printf("Missing file: %s\n",gridfile);
        exit(1);
    }
    printf("test");
    fscanf(fp,"%s %d %s %d", dummy, &warehouse_grid_x, dummy, &warehouse_grid_y);
    warehouse_grid = (int*) malloc(sizeof(int)*warehouse_grid_x*warehouse_grid_y);
    for(i=0;i<warehouse_grid_y;i++)
    {
        for(j=0;j<warehouse_grid_x;j++)
            fscanf(fp, "%d", &warehouse_grid[i*warehouse_grid_x + j]);
    }
    fclose(fp);
}

struct delivery read_deliveries(char productsfile[MAX_FILENAME])
{
    FILE*	fp;
    char dummy[50];
    int gate, code, list_size, qnt;
    int j;
    struct delivery deliveries;

    fp=fopen(productsfile,"r");
    if (!fp)
	{
		printf("Missing file: %s\n",productsfile);
		exit(1);
	}
    fscanf(fp,"%s", dummy);
    fscanf(fp,"%d", &list_size);
    fscanf(fp,"%s %d", dummy, &gate);
    deliveries.load = (struct products *)malloc(sizeof(struct products)*list_size);
    deliveries.gate = gate;
    deliveries.list_size = list_size;
    for (j=0;j<list_size;j++)
    {
        fscanf(fp,"%d %d", &code, &qnt);
        deliveries.load[j].code = code;
        deliveries.load[j].qnt = qnt;
        //input.qnt = qnt;
    }
    return deliveries;
}

struct delivery read_input(char productsfile[MAX_FILENAME])
{
    FILE*	fp;
    char name[50], dummy[50];
    int qnt, code, demand, i;
    int num_of_products;
    struct products *product_count;
    struct delivery deliv;
    num_of_products = 0;
    i=0;

    fp=fopen(productsfile,"r");
    if (!fp)
	{
		printf("Missing file: %s\n",productsfile);
		exit(1);
	}
	fscanf(fp,"%s %s %s %s", dummy,dummy,dummy,dummy);
    do{
        fscanf(fp,"%s", name);
        if(0==strcmp(name, "EOF"))
        {
            break;
        }
        num_of_products++;
        fscanf(fp,"%d %d %d", &qnt, &code, &demand);
	}while(1);
    product_count = (struct products*) malloc(sizeof(struct products)*num_of_products);
    rewind(fp);
    fscanf(fp,"%s %s %s %s", dummy,dummy,dummy,dummy);
	do{
        fscanf(fp,"%s", name);
        if(0==strcmp(name, "EOF"))
        {
            break;
        }
        fscanf(fp,"%d %d %d", &qnt, &code, &demand);
        // Create array of products for reference
        product_count[i].code = code;
        product_count[i].qnt = qnt;
        product_count[i].demand = demand;
        strcpy(product_count[i].name, name);
        i++;
	}while(1);
	deliv.load = product_count;
	deliv.list_size = num_of_products;
	return deliv;
}

void read_list(char file_list[MAX_FILENAME])
{
    FILE *fp;
    char dummy[250];
    char type[5];int i;
    char name[MAX_FILENAME];

    num_files=0;
    fp=fopen(file_list,"r");
    if (!fp)
	{
		printf("Missing file: %s\n",file_list);
		exit(1);
	}
	fscanf(fp, "%s %s %s %s", dummy, dummy, dummy, dummy);
	fscanf(fp, "%s %s %s %s", type, dummy, dummy, name);
	while(0!=strcmp(type, "EOF"))
    {
        num_files++;
        fscanf(fp, "%s %s %s %s", type, dummy, dummy, name);
    }
    files = (struct file*) malloc(sizeof(struct file)*num_files);
    rewind(fp);
	fscanf(fp, "%s %s %s %s", dummy, dummy, dummy, dummy);

	for(i=0;i<num_files;i++)
    {
        fscanf(fp, "%s %s %s %s", type, dummy, dummy, name);
        if(0==strcmp(type, "In"))
        {
            files[i].in = 1;
            files[i].deliv = read_input(name);
        }
        else
        {
            files[i].in = 0;
            files[i].deliv = read_deliveries(name);
        }
        strcpy(files[i].name, name);
    }
    fclose(fp);
    calculate_product_qnt(num_files);
    printf("list reading finished");
}

void print_warehouse(void)
{
    int i,j;
    for(i=0; i<warehouse_size; i++)
    {
        printf("\n%d\t",i);
        printf("%d\t", warehouse[i].product_code);
        for(j=0; j<num_gates; j++)
        {
            printf("\t%d", warehouse[i].dist[j]);
        }
    }
}

void create_empty_input(void)
{
    empty_input = (struct products*) malloc(sizeof(struct products));
    empty_input->code = 0;
    strcpy((*empty_input).name,"Empty");
    empty_input->demand = 0;
}

void make_deliveries(void)
{
    struct location crate;
    int i, j, k, l;
    int best_location = 0;
    int delivery_time = 0;

    crate.dist = (int *)malloc(sizeof(int)*num_gates);

    for(i=0;i<num_gates;i++)
    {
        for(j=0;j<deliveries[i].list_size;j++)
        {
            for(k=0;k<deliveries[i].load[j].qnt;k++)
            {
                crate.dist[i] = 256;
                crate.product_code = deliveries[i].load[j].code;

                for(l=0;l<warehouse_size;l++)
                {
                    if((deliveries[i].load[j].code==warehouse[l].product_code)
                       &&(crate.dist[i]>warehouse[l].dist[i]))
                    {
                        crate = warehouse[l];
                        best_location = l;
                    }
                }
                warehouse[best_location].product_code = 0;
                printf("code %d, location %d, dist %d\n", deliveries[i].load[j].code, best_location, warehouse[best_location].dist[i]);
                delivery_time = delivery_time+warehouse[best_location].dist[i];
            }
        }
        printf("delivery time %d",delivery_time);
        delivery_time = 0;
    }
}

#ifdef __cplusplus
}
#endif __cplusplus

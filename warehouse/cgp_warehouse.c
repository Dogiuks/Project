/* cgp_warehouse.c
   Domas Druzas 2015
*/

#include <stdio.h>
#include "cgp_warehouse.h"
#include "cgp.h"


int main(int argc,char* argv[])
{
//    struct location shelf;
//    struct location *bunch_of_shelfs;
//    int i;
//
//    bunch_of_shelfs = (struct location*)malloc(sizeof(struct location)*1);
//
//
//    shelf.dist = (int*)malloc(sizeof(int)*5);
//    for(i=0;i<5;i++)
//        shelf.dist[i]=i;
//    shelf.product_code = 1;
//
//    bunch_of_shelfs[0]= shelf;
//    bunch_of_shelfs = (struct location*)realloc(bunch_of_shelfs, sizeof(struct location)*2);
//    printf("%d", bunch_of_shelfs[0].dist[3]);
//    shelf.dist[3]=5;
//    printf("%d", bunch_of_shelfs[0].dist[3]);
//    return 0;

    first_run = true;
    create_empty_input();
    generate_test_files();
    define_warehouse("Testfiles/warehouse.txt");
    read_list("files.txt");

   run_EA();

    return 0;
}

void generate_test_files(void)
{
    FILE *fp;
    FILE *listfile;
    char name[50];
    int i, j;
    int* counter;
    //deliveries variables
    int gates = 5;
    int max_list_size = 20;
    int max_code = 20;
    int max_qnt = 100;
    int list_size, qnt, code;
    //products variables
    int max_demand = 20;
    //warehouse variables
    int min_size =1;
    int max_dist;
    int num_locations;


    counter = (int*) malloc(sizeof(int)*max_code);
    for(i=0;i<max_code;i++)
        counter[i]=0;
    listfile = fopen("files.txt", "w+");
    if (!listfile)
    {
        printf("Missing file\n");
        exit(1);
    }
    printf("creating deliveries\n");

    for(i=0;i<gates;i++)
    {

        sprintf(name, "Testfiles/deliveries%d.txt", i+1);
        fp = fopen(name, "w+");
        if (!fp)
        {
            printf("Missing file\n");
            exit(1);
        }
        list_size = 1+rand()%max_list_size;
        fprintf(fp,"delivery %d\n", list_size);
        fprintf(fp,"\tgate %d\n", i);
        for(j=0;j<list_size;j++)
        {
            code = rand()%max_code+1;
            qnt = rand()%max_qnt;
            min_size +=qnt;
            counter[code]+=qnt;
            fprintf(fp,"\t%d %d\n", code, qnt);
        }
        fprintf(fp,"\n");
        fprintf(fp,"EOF");
        fclose(fp);
        if(i==0)
        {
            fp = fopen("Testfiles/products.txt", "w+");
            if (!fp)
            {
                printf("Missing file\n");
                exit(1);
            }
            fprintf(fp, "Name\tQnt\tcode\tdemand\n");
            for(j=1;j<max_code;j++)
            {
                qnt = rand()%20;
                fprintf(fp,"morka\t%d\t%d\t%d\n", counter[j]+qnt, j, rand()%max_demand);
                counter[j] = qnt;
            }
            fprintf(fp, "EOF");
            fclose(fp);
        }
        else
        {
            sprintf(name, "Testfiles/in%d.txt", i);
            fp = fopen(name, "w+");
            if (!fp)
            {
                printf("Missing file\n");
                exit(1);
            }
            fprintf(fp, "Name\tQnt\tcode\tdemand\n");
            for(j=1;j<max_code;j++)
            {
                qnt = rand()%5;
                fprintf(fp,"morka\t%d\t%d\t%d\n", counter[j]+qnt, j, rand()%max_demand);
                counter[j] = qnt;
            }
            fprintf(fp, "EOF");
            fclose(fp);
        }

    }

    printf("creating warehouse\n");
    fp = fopen("Testfiles/warehouse.txt", "w+");
    if (!fp)
	{
		printf("Missing file\n");
		exit(1);
	}
	num_locations = min_size+rand()%min_size;
	max_dist = num_locations/5;
    fprintf(fp,"%d locations %d inouts", num_locations, gates);
    for(i=1;i<=num_locations;i++)
    {
        fprintf(fp, "\n%d", i);
        for(j=0;j<gates;j++)
        {
            fprintf(fp,"\t%d", rand()%max_dist);
        }
    }
    fclose(fp);
    listfile = fopen("files.txt", "w+");
    if (!listfile)
    {
        printf("Missing file\n");
        exit(1);
    }
    fprintf(listfile, "Type\t\tDate\t\tTime\t\tFile\n");
    fprintf(listfile, "In\t\tDate\t\tTime\t\tTestfiles/products.txt\n",i);
    for(i=1;i<5;i++)
    {
        fprintf(listfile, "Out\t\tDate\t\tTime\t\tTestfiles/deliveries%d.txt\n",i);
        fprintf(listfile, "In\t\tDate\t\tTime\t\tTestfiles/in%d.txt\n",i);
    }
    fprintf(listfile, "In\t\tDate\t\tTime\t\tTestfiles/deliveries5.txt\nEOF");
    fclose(listfile);
    //free(counter);
}


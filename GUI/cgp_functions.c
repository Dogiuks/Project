/* cgp_functions.c
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


void run_EA(void)
{
    node_pointer* population;
    int i, j;
    int* fitness;
    int fittest = 0;

    fitness = (int*) malloc(sizeof(int)*POPULATION_SIZE);
    population = (node_pointer*)malloc(sizeof(node_pointer)*POPULATION_SIZE);
    create_population(population);
    for(i=0;i<NUM_GENERATIONS;i++)
    {
        evaluate_population(population, fitness);
        select_parents(population, fitness);
        #ifndef PRESERVE_PARENTS
        for (j=0; j<MU; j++)
        {
            mutate(population[j]);
        }
        #endif // PRESERVE_PARENTS
        for (j=MU; j<POPULATION_SIZE; j++)
        {
            mutate(population[j]);
        }
        if(i%10==0)
        {
            printf("For generaton %d best fitness is: %d\n", i, best_fitness);
        }
    }
    evaluate_population(population, fitness);
    for(i=0;i<POPULATION_SIZE;i++)
    {
        if(fitness[i]<fitness[fittest])
            fittest = i;
    }
    create_output(population[fittest]);
}

void write_output(node_pointer* population, int* fitness)
{
    int best = 0, i;
    for(i=0;i<POPULATION_SIZE;i++)
    {
        if(fitness[i]<fitness[best])
            best = i;
    }
    best_fitness = fitness[best];
    printf("CGP finished with best fitness %d\n", best_fitness);
    print_product_list(population[best], "Testfiles/out.txt");
}

void print_product_list(struct node* ch, char output_file[MAX_FILENAME])
{
    FILE *fp;
    int i;
    struct products* out;
    struct products* temp;
    out = (struct products*) malloc(sizeof(struct products)*warehouse_size);

    for (i=0; i<warehouse_size; i++)
    {
        temp = ch[NUM_NODES+i-warehouse_size].out;
        out[i] = *temp;
    }

    fp = fopen(output_file, "w+");
    if (!fp)
	{
		printf("Missing file\n");
		exit(1);
	}
	fprintf(fp, "Location\tCode\n");
    for (i=1; i<=warehouse_size; i++)
    {
        fprintf(fp, "%d\t%d\n", i, out[i].code);
        //printf("warehouse location %d contains product %d\n", i, out[i-1].code);
    }
    fclose(fp);
    free(out);
}

void select_parents(node_pointer* population, int* fitness)
{
    int best, i, j, n=0;
    int random;
    node_pointer new_population[MU];
    #ifdef ROULETTE
    int total;
    #endif // ROULETTE

    for(i=0;i<MU;i++)
    {
        new_population[i] = (struct node*) malloc(sizeof(struct node)*NUM_NODES);
    }

    best = 0;
    for(i=0;i<POPULATION_SIZE;i++)
    {
        if(fitness[i]<fitness[best])
            best = i;
    }
    best_fitness = fitness[best];
    #ifdef ELITISM
    for(j=0;j<ELITISM;j++)
    {
        copy_chromosome(population[best], new_population[n]);
        n++;
    }
    #endif
    #ifdef TOURNAMENT
    for(j=0;j<TOURNAMENT;j++)
    {
        best = rand()%POPULATION_SIZE;
        for(i=1;i<T_SIZE;i++)
        {
            random = rand()%POPULATION_SIZE;
            if(fitness[random]<fitness[best])
                best = random;
        }
        copy_chromosome(population[best], new_population[n]);
        n++;
    }
    #endif
    #ifdef ROULETTE
    for(j=0;j<ROULETTE;j++)
    {
        best = 0;
        for(i=0;i<POPULATION_SIZE;i++)
        {
            total+=fitness[i];
        }
        for(i=0;i<POPULATION_SIZE;i++)
        {
            best+=total/fitness[i];
        }
        random = rand()%best;
        i=0;
        do
        {
            random-=total/fitness[i];
            if(random<=0)
                break;
            i++;
        }while(1);
        copy_chromosome(population[i], new_population[n]);
        n++;
    }
    #endif
    for(i=0;i<MU;i++)
    {
        clean_chromosome(new_population[i]);
    }
    for(i=0;i<POPULATION_SIZE;i++)
    {
        copy_chromosome(new_population[i%MU], population[i]);
    }
    for(i=0;i<MU;i++)
    {
        free(new_population[i]);
    }
}

void mutate(struct node* ch)
{
    int random, i;

    for(i=0;i<MUTATION_RATE*NUM_NODES/100;i++)
    {
        random = rand() % (NUM_NODES);
        ch[random] = create_random_node(random/NUM_ROW);
    }
}

void evaluate_population(node_pointer* population, int* evaluation)
{
    int i,j;
    product_pointer* output;
    struct products* product_count;
    int num_of_products;
    //int evaluation;

    output = (product_pointer*)malloc(sizeof(product_pointer)*warehouse_size);
    //evaluation = (int*) malloc(sizeof(int)*POPULATION_SIZE);

    for(i=0;i<POPULATION_SIZE;i++)
    {
        evaluation[i] = 0;
        for(j=0;j<num_files;j++)
        {
            if(files[j].in)
            {
                product_count = files[j].deliv.load;
                num_of_products = files[j].deliv.list_size;
                create_input(product_count, num_of_products);
                decode(population[i], output);
                evaluation[i] += (evaluate_output(output, product_count,
                                                  num_of_products)/(j+1));
            }
            else
            {
                //decode(population[i], output);
                evaluation[i] += evaluate_position(output, files[j].deliv);
            }
        }
    }
    free(output);
}

void create_output(struct node* ch)
{
    product_pointer* output;
    struct products* product_count;
    int num_of_products;
    int j;

    output = (product_pointer*)malloc(sizeof(product_pointer)*warehouse_size);
    output_array = (outputtype*)realloc(output_array, sizeof(outputtype)*num_files);

    for(j=0;j<num_files;j++)
    {
        if(files[j].in)
        {
            product_count = files[j].deliv.load;
            num_of_products = files[j].deliv.list_size;
            create_input(product_count, num_of_products);
            decode(ch, output);
        }
        else
        {
            evaluate_position(output, files[j].deliv);
        }
        coppy_output(output, output_array[j]);
    }
}

void coppy_output(product_pointer* source, product_pointer* destination)
{
    int i;
    destination = (product_pointer*)realloc(destination, sizeof(product_pointer)*warehouse_size);
    for(i=0;i<warehouse_size;i++)
        destination[i] = source[i];
}

int evaluate_output(product_pointer* output, struct products* product_count, int num_of_products)
{
    int i,j;
    struct products* counter;
    int code, fitness=0;

    //make product_count copy
    counter = (struct products*)malloc(sizeof(struct products)*num_of_products);
    for (i=0;i<num_of_products;i++)
    {
        counter[i]=product_count[i];
    }
    //check that all products represented
    for (j=0;j<warehouse_size;j++)
    {
        code = output[j]->code;
        for (i=0;i<num_of_products;i++)
        {
            if (counter[i].code == code)
            {
                counter[i].qnt--;
                break;
            }
        }
    }
    //calculate penalty for lost products
    for (i=0;i<num_of_products;i++)
    {
        fitness+= abs(counter[i].qnt);
    }
    fitness = fitness*PENALTY*warehouse_size;
    free(counter);
    return fitness;
}

int evaluate_position(product_pointer* output, struct delivery deliv)
{
    int i,j,k;
    int code, fitness=0, best_pick, gate;
    int* pickup_list;

    //deliver all products
    for (j=0; j<deliv.list_size; j++)
    {
        pickup_list = (int*)malloc(sizeof(int)*deliv.load[j].qnt);
        code = deliv.load[j].code;
        gate = deliv.gate;
        i = 0;
        k = 0;
        //Find all products
        do
        {
            if(output[k]->code == code)
            {
                pickup_list[i]=k;
                i++;
            }
            k++;
        }while((k<warehouse_size) && (i < deliv.load[j].qnt));
        //look for products closer to the gate
        best_pick = find_best_pick(pickup_list, i, gate);
        for (; k<warehouse_size;k++)
        {
            if((output[k]->code == code)
                && (warehouse[k].dist[gate]<warehouse[best_pick].dist[gate]))
            {
                pickup_list[best_pick]=k;
                best_pick = find_best_pick(pickup_list, deliv.load[j].qnt, gate);
            }
        }
        //add penalty to fitness and remove products from list
        for(k=0;k<i;k++)
        {
            fitness+=warehouse[pickup_list[k]].dist[gate];
            output[pickup_list[k]] = empty_input;
        }
        free(pickup_list);
    }
    return fitness;
}

int find_best_pick(int* pick_list, int list_size, int gate)
{
    int best_pick,  min_dist = 0;
    int i;

    for(i=0;i<list_size;i++)
    {
        if(warehouse[pick_list[i]].dist[gate]<min_dist)
        {
            min_dist = warehouse[pick_list[i]].dist[gate];
            best_pick = i;
        }
    }
    return best_pick;
}

void create_population(node_pointer* population)
{
    int i;
    if (1)
    {
        for (i=0; i<POPULATION_SIZE; i++)
        {
            population[i]= create_chromosome();
        }
        first_run = 0;
    }
    else
    {
        for (i=MU; i<POPULATION_SIZE; i++)
        {
            //make copy and mutate
        }
    }

}

struct node* create_chromosome(void)
{
    struct node* ch;
    int i;

    ch = (struct node *)malloc(sizeof(struct node)*NUM_NODES);
    for (i=0; i<NUM_NODES; i++)
    {
        ch[i] = create_random_node(i);
    }

    return ch;
}

struct node create_random_node(int collumn)
{
    struct node n;

    n.func = rand()%FUNC_COUNT;
    n.out = NULL;
    if (collumn<LEVELS_BACK)
    {
        n.a = rand()%(collumn+warehouse_size-1);
        n.b = rand()%(collumn+warehouse_size-1);
    }
    else
    {
        n.a = rand()%(LEVELS_BACK+warehouse_size-1);
        if (n.a>warehouse_size-1)
        {
            n.a=n.a+(collumn-LEVELS_BACK);
        }

        n.b = rand()%(LEVELS_BACK+warehouse_size-1);
        if (n.b>warehouse_size-1)
        {
            n.b=n.b+(collumn-LEVELS_BACK);
        }
    }

    return n;
}

void copy_chromosome(struct node* source, struct node* destination)
{
    int i;
    for (i=0;i<NUM_NODES; i++)
    {
        destination[i].func = source[i].func;
        destination[i].a = source[i].a;
        destination[i].b = source[i].b;
        destination[i].out = source[i].out;
    }
}

void decode(struct node* ch, product_pointer* outputs)
{
    int i;

    for (i=0; i<warehouse_size; i++)
    {
        compute_node(NUM_NODES+i, ch);
        outputs[i] = (product_pointer) ch[NUM_NODES+i-warehouse_size].out;
    }
}

void clean_chromosome(struct node* ch)
{
    int i;

    for (i=0; i<NUM_NODES; i++)
    {
        ch[i].out=NULL;
    }
}

void compute_node(int i, struct node* ch)
{
    struct products *a, *b;

    i = i-warehouse_size;
    if (ch[i].out ==0)
    {
        if (ch[i].a < warehouse_size)
        {
            a = input[ch[i].a];
        }
        else
        {
            compute_node(ch[i].a, ch);
            a = ch[ch[i].a - warehouse_size].out;
        }

        if (ch[i].b < warehouse_size)
        {
            b = input[ch[i].b];
        }
        else
        {
            compute_node(ch[i].b, ch);
            b = ch[ch[i].b - warehouse_size].out;
        }
        ch[i].out = compute_output(a, b, ch[i].func);
    }
}

struct products* compute_output(struct products* a, struct products* b, enum Functions func)
{
    struct products* out;

    switch (func)
    {
        case A:
            out = a;
            break;
        case B:
            out = b;
            break;
        case A_IF_MORE:
            if (a->qnt>b->qnt)
                out = a;
            else
                out = b;
            break;
        case A_IF_LESS:
            if (a->qnt<b->qnt)
                out = a;
            else
                out = b;
            break;
        case A_IF_TWICE:
            if (a->qnt>(b->qnt)*2)
                out = a;
            else
                out = b;
            break;
        case A_IF_HALF:
            if ((a->qnt)*2<b->qnt)
                out = a;
            else
                out = b;
            break;
        case A_IF_MORE_DEMAND:
            if (a->demand>b->demand)
                out = a;
            else
                out = b;
            break;
        case A_IF_LESS_DEMAND:
            if (a->demand<b->demand)
                out = a;
            else
                out = b;
            break;
        case A_IF_TWICE_DEMAND:
            if (a->demand>(b->demand)*2)
                out = a;
            else
                out = b;
            break;
        case A_IF_HALF_DEMAND:
            if ((a->demand)*2<b->demand)
                out = a;
            else
                out = b;
            break;
        default:
            out = a;
            break;
    }
    return out;
}

void create_input(struct products* product_count, int num_of_products)
{
    int i,j,n=0;
    //int num_of_products;

    input = (product_pointer*)realloc(input, sizeof(product_pointer)*warehouse_size);
    for (i=0; i<num_of_products; i++)
    {
        if(n+product_count[i].qnt>warehouse_size)
        {
            printf("You need a biger warehouse for all this stuff");
            exit(0);
        }
        for (j=0;j<product_count[i].qnt;j++)
        {
            input[n+j] = product_count+i;
        }
        n+=product_count[i].qnt;
    }

    //Make empty locations
    for (;n<warehouse_size;n++)
    {
        input[n]=empty_input;
    }
}

void calculate_product_qnt(int step)
{
    struct products* product_count;
    int i, j, k;
    int array_size;

    array_size = files[0].deliv.list_size;

    product_count = (struct products*) malloc(sizeof(struct products)*array_size);

    for(i=0;i<array_size;i++)
    {
        product_count[i] = files[0].deliv.load[i];

    }
    for(i=1;i<step;i++)
    {
        if(files[i].in)
        {
            for(j=0;j<files[i].deliv.list_size;j++)
            {
                for(k=0;k<array_size;k++)
                {
                    if(files[i].deliv.load[j].code == product_count[k].code)
                    {
                        product_count[k].qnt +=files[i].deliv.load[j].qnt;
                        break;
                    }
                }
                if(k==array_size)
                {
                    array_size++;
                    product_count = (struct products*) realloc(product_count,
                                            array_size*sizeof(struct products));
                    product_count[k] = files[i].deliv.load[j];
                }

            }
            files[i].deliv.list_size = array_size;
            files[i].deliv.load = (struct products*) realloc(files[i].deliv.load, array_size*sizeof(struct products));
            for(k=0;k<array_size; k++)
            {
                files[i].deliv.load[k] = product_count[k];
            }
        }
        else
        {
            for(j=0;j<files[i].deliv.list_size;j++)
            {
                for(k=0;k<array_size;k++)
                {
                    if(files[i].deliv.load[j].code == product_count[k].code)
                    {
                        product_count[k].qnt -=files[i].deliv.load[j].qnt;
                        break;
                    }
                }
                if((k==array_size)||(product_count[k].qnt <0))
                {
                    printf("\nProduct doesnt exist!\n");
                    exit(0);
                }
                else if(product_count[k].qnt ==0)
                {
                    for (; k< array_size ; k++ )
                        product_count[k] = product_count[k+1];
                    array_size--;
                    product_count = (struct products*)realloc(product_count, array_size*sizeof(struct products));
                }
            }
        }

    }
    free(product_count);
}

#ifdef __cplusplus
}
#endif __cplusplus

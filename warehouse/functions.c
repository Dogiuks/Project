/* cgp-functions_tsp.c
   Julian F. Miller (c) 2012
   Solves TSP problems

   The approach is based on my function optimization
   program cgp_fo.

   A number of outputs are used that is equal
   to the number of cities.

   The permutation is obtained by sorting the outputs
   into numerical oder and noting which output came from where.
   e.g. consider a five city problem for which CGP has generated
   the outputs

  0      1     2    3   4
  0.125, 0.7, 0.3, 0.9,0.48

  sort

  0      3     4    1   3
  0.125 0.3  0.48  0.7 0.9

  That is the tour.

  Notes on 25/10/12.
  The one problem with approach is that one has to sort
  a string of numbers. Good sorting methods can do this in
  O(nlog(n)) (i.e. quicksort). However, this is a big overhead
  as every chromosome has to be sorted. Radix sort may be able
  to reduce this overhead to O(n).

  Standard evolutionary methods would use a chromosome that
  already was a permutation. However, they need to use
  repair operators on chromosomes which probably are O(n).

  The Fisher-Yates shuffle was an attempt to
  obtain a valid permutation froma  string of numbers
  in O(n) however, I suspect it tends to randomize the
  permutation thus making the problem harder.

  An obvious direct method to evolve permutation would be
  to not use crossover and use mutation operators that
  carry out valid re-arrangements of permutations such as
  swap two cities, reverse permutation, etc...
  I am not sure whether CGP could be used to generate
  a sequence of operations to apply to a permutation that
  would always result in a permutation. I am just thinking
  out loud now, but suppose the data being fed through the CGP program
  was a permutation and all the functions were valid permutation
  operations (like swap, re-arrange, and any others that are recommended
  in the literature)? No sort would be necessary...I like this idea
  but it is a whole new program...
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "cgp_tsp.h"


/* validate cgp program command line and create file strings for .par and data files */
void validate_command_line(int argc, char* argv[], char parfile[], char tspfile[])
{
	puts("");
	puts("*********    WELCOME TO CARTESIAN GENETIC PROGRAMMING      *********");
	puts("*********      cgp_tsp uses CGP to find solve TSP problems *********");
	puts("********* Validating command line arguments to cgp program *********");

	if (argc!=3)
	{
		puts("INCORRECT NUMBER OF ARGUMENTS");
		puts("Type cgp <file.par> <tspfile.tsp> then return");
		exit(1);
	}
	if (strlen(argv[1])>(MAX_NUM_LETTERS-1))
	{
   		puts("filename for parameter file is too long");
		printf("It should be less than %d characters\n",MAX_NUM_LETTERS);
		exit(2);
	}
	if (strlen(argv[2])>(MAX_NUM_LETTERS-1))
	{
   		puts("filename for tsp file is too long");
		printf("It should be less than %d characters\n",MAX_NUM_LETTERS);
		exit(2);
	}

	strcpy(parfile,argv[1]);
	strcpy(tspfile,argv[2]);
}

/* read from the parameter file all the global parameters */
void get_parameters(char parfile[MAX_NUM_LETTERS],
					char tspfile[MAX_NUM_LETTERS])
{
	int		i;
	int     arity, max_arity;
	char	dummy[50];
	FILE*	fp;

	printf("\n********* Reading parameters defined in %s *********\n",parfile);
	fp=fopen(parfile,"r");
	if (!fp)
	{
		printf("Missing file: %s\n",parfile);
		exit(1);
	}
	fscanf(fp,"%d %s", &population_size,dummy);
	fscanf(fp,"%lf %s",&per_cent_connection_mutate,dummy);
	fscanf(fp,"%lf %s",&per_cent_function_mutate,dummy);
	fscanf(fp,"%lf %s",&per_cent_output_mutate,dummy);
	fscanf(fp,"%d %s", &num_generations,dummy);
	fscanf(fp,"%d %s", &num_runs_total,dummy);
	fscanf(fp,"%d %s", &num_rows,dummy);
	fscanf(fp,"%d %s", &num_cols,dummy);
	fscanf(fp,"%d %s", &levels_back,dummy);
	fscanf(fp,"%d %s", &progress_report,dummy);
	fscanf(fp,"%d %s", &report_interval,dummy);
	fscanf(fp,"%u %s", &global_seed,dummy);
	fscanf(fp,"%d %s", &save_chrom,dummy);
	fscanf(fp,"%d %s", &run_from_chrom,dummy);
	fscanf(fp,"%d %s", &output_copy_mutation,dummy);
	fscanf(fp,"%lf %s",&output_mutate_copy_prob,dummy);
	fscanf(fp,"%d %s", &mutate_by_prob,dummy);
	fscanf(fp,"%d %s", &num_constant_inputs,dummy);
	fscanf(fp,"%lf %s",&per_cent_output_crossover,dummy);
	fscanf(fp,"%d %s", &quick_sort,dummy);

	num_nodes = num_rows * num_cols;
	num_functions=0;
	max_arity =0;

	for (i = 0; i < MAX_NUM_FUNCTIONS; i++)
	{
		/* number[] holds whether the function is used or not */
		fscanf(fp,"%d%d%s",&number[i],&arity,node_types[i]);
		if (number[i])
		{
			allowed_functions[num_functions][0]=i;
			allowed_functions[num_functions][1]=arity;
			num_functions++;

			if (arity > max_arity)
				max_arity = arity;
		}
	}
	fclose(fp);

	if (num_constant_inputs > MAX_NUM_CONSTANT_INPUTS)
	{
		printf("Too many constants. Maximum is %d\n",MAX_NUM_CONSTANT_INPUTS);
		exit(0);
	}

	generate_constant_inputs();

	/* reads in city data and gets num_outputs (number of cities ) */
	read_city_data(tspfile);

	/* create array space for city distance matrix */
	intercity_distance = create_2d_space(num_outputs, num_outputs);

    /* this calculates all the intercity distances
    and stores them in a matrix */
	create_distance_matrix();

	/*
	print_distance_matrix();
	*/

	/* source is used in Fisher-yates shuffle and is initially
	the identity permutation */
	for (i = 0; i < num_outputs; i++)
		source[i] = i;

	/* each node is assigned max_arity connection genes */
	num_genes_per_node = max_arity + 1;


	num_genes = num_genes_per_node*num_nodes + num_outputs;

	num_inputs = num_constant_inputs;

	num_connection_genes_per_node = max_arity;
	num_connection_genes = num_nodes*num_connection_genes_per_node;

    get_number_of_mutations();

	num_crossover_genes = get_number_of_crossover_genes(num_outputs, per_cent_output_crossover);

	if (population_size > MAX_NUM_CHROMOSOMES)
	{
		printf("Too large a population size (<= %d)\n",MAX_NUM_CHROMOSOMES);
		exit(0);
	}

	if (num_genes > MAX_NUM_GENES)
	{
		printf("Too many genes selected (<= %d)\n",MAX_NUM_GENES);
		exit(0);
	}

	if (levels_back > num_cols)
	{
		printf("ERROR. levels-back exceeeds the number of columns\n");
		exit(0);

	}

	if (num_runs_total < 1)
	{
		puts("Number of runs of EA must be at least 1");
		exit(0);
	}
	else if (num_runs_total > MAX_NUM_RUNS)
	{
		printf("Number of runs of EA must be less than %d\n", MAX_NUM_RUNS);
		exit(0);
	}

	if (num_genes < 10)
	{
		puts("Number of genes/bits must be at least 10");
		exit(0);
	}


	srand(global_seed);


	puts("********* Beginning execution *********");
}

/* This generates the inputs to cgp programs.
   they are all constants. Here theye are at equal intervals
   between 0 and 1- 1/num_constants */
void generate_constant_inputs(void)
{
	int i;

	for (i = 0 ; i < num_constant_inputs; i++)
			constant_inputs[i]= (double) i/(double) num_constant_inputs;
}

/* write out parameter values in results file */
void write_cgp_info(char command[])
{
	int		i;
	FILE*	fp;

	fp=fopen("cgp_tsp.txt","w");
	fprintf(fp,"The program is					%s\n",command);
	fprintf(fp,"population_size is				%d\n",population_size);
	fprintf(fp,"connection mutation rate is		%6.4lf\n",per_cent_connection_mutate);
	fprintf(fp,"function mutation rate is		%6.4lf\n",per_cent_function_mutate);
	fprintf(fp,"output mutation rate is			%6.4lf\n",per_cent_output_mutate);
	fprintf(fp,"num_generations is				%d\n",num_generations);
	fprintf(fp,"num_runs is						%d\n",num_runs_total);
	fprintf(fp,"num_rows is						%d\n",num_rows);
	fprintf(fp,"num_cols is						%d\n",num_cols);
	fprintf(fp,"levels_back is					%d\n",levels_back);
	fprintf(fp,"progress report is				%d\n",progress_report);
	fprintf(fp,"report interval is				%d\n",report_interval);
	fprintf(fp,"global_seed is					%u\n",global_seed);
	fprintf(fp,"run_from_chrom is				%d\n",run_from_chrom);
	fprintf(fp,"save_chrom is		    		%d\n",save_chrom);
	fprintf(fp,"output_copy_mutation is			%d\n",output_copy_mutation);
	fprintf(fp,"output_mutate_copy_prob is		%6.4lf\n",output_mutate_copy_prob);
	fprintf(fp,"mutate_by_prob is				%d\n",mutate_by_prob);
	fprintf(fp,"num_constant_inputs is			%d\n",num_constant_inputs);
	fprintf(fp,"per_cent_output_crossover is	%6.4lf\n",per_cent_output_crossover);
	fprintf(fp,"quick_sort is		            %d\n",quick_sort);

	for (i=0;i<MAX_NUM_FUNCTIONS;i++)
	{
		fprintf(fp,"%d %s\n",number[i],node_types[i]);
	}
	fprintf(fp,"\nHere are the Results\n");
	fclose(fp);
}

/*  returns a random integer between 0 and range-1 */
int newrand(int range)
{
    int temp;

    temp=rand() % range;
    return(temp);
}

/* returns a double x where 0.0<= x < 1.0.
   rand is a function defined in stdlib.h
   that returns an interger from 0 to RAND_MAX.
   RAND_MAX is defined in stdlib.
*/
double drand(void)
{
	double value;

	value = (double)rand()/(double)(RAND_MAX-1);

	return value;
}


/* read the tsp specification file in JFM format (easier)
   and stores city data and calculates num_outputs */
void read_city_data(char tspfile[MAX_NUM_LETTERS])
{
	int		i;
	char	dummy[MAX_NUM_LETTERS];

	FILE* fp;

	fp = fopen(tspfile,"r");
	if (!fp)
	{
		printf("Missing tsp file: %s\n",tspfile);
		exit(1);
	}

	fscanf(fp,"%d %s", &num_outputs,dummy);

	if (num_outputs > MAX_NUM_CITIES)
	{
		printf("\nERROR. Cannot have more than %d cities\n", MAX_NUM_CITIES);
		exit(0);
	}

	for (i = 0; i < num_outputs; i++)
	{
		fscanf(fp, "%s%d%d", dummy, &cities[i][0],&cities[i][1]);
	}

	fclose(fp);
}

/* prints a chromosome to a file
   when append is 1, the function appends the information to the file
   when append is 0, the function creates a new file
*/
void fprint_a_chromosome(int* chromosome, char name[], int append)
{
   int		i, node_label;
   int      write_bracket=1;
   FILE*	fp;

   if (append)
	   fp=fopen(name,"a");
   else
	   fp=fopen(name,"w");

   node_label = num_inputs-1;

   for (i=0;i<num_nodes*num_genes_per_node;i++)
   {
      if ((i+1)%num_genes_per_node == 0)
	  {
		 node_label++;
		 fprintf(fp,"[%d]:%d)\t",chromosome[i],node_label);
		 write_bracket = 1;
	  }
      else
	  {
		 if (write_bracket)
			fprintf(fp,"(");

         fprintf(fp,"%d,",chromosome[i]);
		 write_bracket = 0;
	  }
   }
   fprintf(fp,"\t\t");
   for (i=0;i<num_outputs;i++)
      fprintf(fp," %d",chromosome[num_nodes*num_genes_per_node+i]);
   fprintf(fp,"\n\n");
   fclose(fp);
}

/* prints a chromosome to the screen */
void print_a_chromosome(int* chromosome)
{
   int i;

   for (i=0;i<num_nodes*num_genes_per_node;i++)
   {
      if ((i+1)%num_genes_per_node == 0)
		printf(" %d\t",chromosome[i]);
      else
         printf(" %d",chromosome[i]);
   }
   printf("\t\t");
   for (i=0;i<num_outputs;i++)
      printf(" %d",chromosome[num_nodes*num_genes_per_node+i]);
   printf("\n");
}

/* prints a chromosome to file in raw format,
  so that it can be read by the program
*/
void fprint_a_raw_chromosome(int* chromosome,char name[], int append)
{
   int i;
   FILE* fp;

   if (append)
	   fp = fopen(name, "a");
   else
	   fp = fopen(name, "w");

   for (i = 0; i < num_nodes*num_genes_per_node;i++)
   {
      if ((i+1)%num_genes_per_node == 0)
		fprintf(fp," %d\t",chromosome[i]);
      else
         fprintf(fp," %d",chromosome[i]);
   }
   fprintf(fp, "\t\t");

   for (i=0;i<num_outputs;i++)
      fprintf(fp, " %d",chromosome[num_nodes*num_genes_per_node+i]);

   printf("\n");

   fclose(fp);
}

/* prints a tour and coordinates of cities and distance
*/
void fprint_a_tour(int tour[MAX_NUM_CITIES],char name[], int append)
{
   int i;
   int xd, yd, dist;
   FILE* fp;

   if (append)
	   fp = fopen(name, "a");
   else
	   fp = fopen(name, "w");

   fprintf(fp,"  start\t   stop\t xstart\t  xstop\t  xdiff\t ystart\t  ystop\t  ydiff\t   dist\n");
   dist = 0;
   for (i = 0; i < num_outputs-1;i++)
   {
	   	xd = cities[tour[i+1]][0]-cities[tour[i]][0];
		yd = cities[tour[i+1]][1]-cities[tour[i]][1];
		dist = dist + (int)ceil(sqrt(xd*xd+yd*yd));

		fprintf(fp,"%7d\t%7d\t%7d\t%7d\t%7d\t%7d\t%7d\t%7d\t%7d\n",tour[i]+1, tour[i+1]+1, cities[tour[i]][0],cities[tour[i+1]][0],xd,cities[tour[i]][1],cities[tour[i+1]][1], yd, dist);
   }
   /* and back to beginning */
   xd = cities[tour[num_outputs-1]][0]-cities[tour[0]][0];
   yd = cities[tour[num_outputs-1]][1]-cities[tour[0]][1];
   dist = dist + (int)ceil(sqrt(xd*xd+yd*yd));
   fprintf(fp,"%7d\t%7d\t%7d\t%7d\t%7d\t%7d\t%7d\t%7d\t%7d\n",tour[num_outputs-1]+1, tour[0]+1, cities[tour[num_outputs-1]][0],cities[tour[0]][0],xd,cities[tour[num_outputs-1]][1],cities[tour[0]][1], yd, dist);



   fclose(fp);
}




/* prints out array to a file */
void fprint_node_used(int size, int array[MAX_NUM_NODES_PLUS_INPUTS], char name[], int append)
{
	int i;
	FILE* fp;

	if (append)
		fp = fopen(name, "a");
	else
		fp = fopen(name, "w");

	fprintf(fp, "\nnode_used is now\n");
	fprintf(fp, "  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20\n");
	for (i = 0; i < size; i++)
		fprintf(fp,"%3d", array[i]);

	fclose(fp);
}


/* determines from the function gene what its arity is.
   It uses the global arrays allowed_functions */
int get_arity(int function_gene)
{
	int i, arity = 0;

	for (i = 0; i < num_functions; i++)
		if (function_gene == allowed_functions[i][0])
			arity = allowed_functions[i][1];

	return arity;
}


/* calculates the addresses of nodes used
   and stores them in node_to_process
   returns the number of nodes used
*/
int get_nodes_to_process(int* chromosome, int nodes_to_process[MAX_NUM_NODES])
{
	int		i,j,index;
	int		num_nodes_to_process;
	int		node_genes[MAX_NUM_GENES_PER_NODE];
	int     node_used[MAX_NUM_NODES_PLUS_INPUTS];
	int		max_size_node_used;

	max_size_node_used = num_nodes + num_inputs;

	/* say all nodes not used */
	for (i = 0; i < max_size_node_used; i++)
		node_used[i]=0;

	/* all the nodes whose output is given by the output genes are active */
	/* last num_outputs genes are all output genes */
	for (i = num_genes - num_outputs; i < num_genes; i++)
		node_used[chromosome[i]]=1;

	for (i = max_size_node_used - 1; i >= num_inputs; i--)
	{
		if (node_used[i])
		{
			/* get input addresses and type of this gate */
			index = num_genes_per_node*(i-num_inputs);

			/* write genes for node into array node_genes */
			for (j = 0; j < num_genes_per_node;j++)
				node_genes[j] = chromosome[index+j];

			/* each function has an arity stored in
			   allowed_functions[][1].
			   Find the nodes whose data is used
			*/

			for (j = 0; j < get_arity(node_genes[num_genes_per_node-1]); j++)
				node_used[node_genes[j]]=1;

		}
	}
	/* find number of used nodes */
	num_nodes_to_process = 0;
	for (i = num_inputs; i < max_size_node_used; i++)
	{
		if (node_used[i])
		{
			nodes_to_process[num_nodes_to_process] = i;
			num_nodes_to_process++;
		}
	}
	return num_nodes_to_process;
}



/**** print chromosome to file and indicate inactive genes with -1 */
void fprint_active_genes(int* chromosome, char name[30])
{
	int		i,j,index;
	int		write_bracket;
	int		node_label;
	int		num_unused_nodes, num_nodes_active;
	int		node_genes[MAX_NUM_GENES_PER_NODE];
	int     node_used[MAX_NUM_NODES_PLUS_INPUTS];
	int		max_size_node_used;

	int*	active_chromosome = NULL;
	FILE*	fp;


	active_chromosome = create_1d_space(num_genes);

	max_size_node_used = num_nodes + num_inputs;

	fp=fopen(name,"a");
	for (i=0;i<num_genes;i++)
		active_chromosome[i]=-1;
	for (i=num_genes-num_outputs;i<num_genes;i++)
		active_chromosome[i]=chromosome[i];

	/* say all nodes not used */
	for (i = 0; i < max_size_node_used; i++)
		node_used[i]=0;

	/* all the nodes whose output is given by the output genes are active */
	/* last num_outputs genes are all output genes */
	for (i = num_genes - num_outputs; i < num_genes; i++)
		node_used[chromosome[i]]=1;

	for (i = max_size_node_used - 1; i >= num_inputs; i--)
	{
		if (node_used[i])
		{
			/* get input addresses and type of this gate */
			index = num_genes_per_node*(i-num_inputs);

			/* write genes for node into array node_genes */
			for (j = 0; j < num_genes_per_node;j++)
			{
				node_genes[j] = chromosome[index+j];
				active_chromosome[index+j]=node_genes[j];
			}

			/* each function has an arity stored in
			   allowed_functions[][1].
			   Find the nodes whose data is used
			*/
			for (j = 0; j < get_arity(node_genes[num_genes_per_node-1]); j++)
				node_used[node_genes[j]]=1;

		}
	}

	node_label = num_inputs-1;
	write_bracket = 1;

	for (i = 0;i < num_nodes*num_genes_per_node; i++)
	{
		if ((i+1)%num_genes_per_node == 0)
		{
			node_label++;
			if (active_chromosome[i]<0)
				fprintf(fp,"[*]:%d)\t",node_label);
			else
				fprintf(fp,"[%d]:%d)\t",active_chromosome[i],node_label);
			write_bracket = 1;
		}
		else
		{
			if (write_bracket == 1)
				fprintf(fp,"(");

			if (active_chromosome[i]<0)
				fprintf(fp,"*,");
			else
				fprintf(fp,"%d,",active_chromosome[i]);

			write_bracket = 0;
		}
	}
	fprintf(fp,"\t\t");
	for (i = 0; i < num_outputs;i++)
      fprintf(fp," %d",active_chromosome[num_nodes*num_genes_per_node+i]);

	num_unused_nodes = 0;
	for (i = num_inputs; i < num_inputs+num_nodes;i++)
		if (!node_used[i])
		   num_unused_nodes++;

   num_nodes_active = num_nodes - num_unused_nodes;

   fprintf(fp,"\nnumber of active nodes is %d\n\n",num_nodes_active);
   fclose(fp);

   free(active_chromosome);
}



/* generate a starting population
   from a chromosome read from a file (cgp.chr)
*/
void read_from_chrom(int** chromosomes)
{
	int	  i,j;
	FILE* fp;

    fp=fopen("cgp.chr","r");
    if (!fp)
    {
		puts("Missing file cgp.chr (contains a chromosome)");
        exit(1);
    }
    else
	{
        /* make starting population copies of loaded chromosome */
		for (j=0;j<population_size;j++)
        {
			if (j==0)
            {
				i=0;
				do
				{
					fscanf(fp,"%d",&chromosomes[j][i]);
					i++;
				}
				while (!feof(fp));

				if (i!=num_genes)
				{
					puts("ERROR. Number of genes in cgp.chr does not match the expected number");
					printf("\nnum_genes required is %d, num_genes read is %d",num_genes, i);
					puts("Check the number of genes in the .par file");
					exit(0);
				}
            }
            else
            {
				for (i=0;i<num_genes;i++)
					chromosomes[j][i]=chromosomes[0][i];
            }
        }
		fclose(fp);
	}
}




/* this decodes the cgp chromosome.
   It is given data_inputs corresponding to a single
   test case and it calculates what the cgp genotype gives
   for the data outputs (cgp_outputs)
   It only processes nodes that are used (nodes_to_process)
   It also save an array tour which is sorted the same way as
   cgp_outputs so will hold the permutation.
*/
void decode_cgp(int* chromosome,
				double cgp_outputs[MAX_NUM_OUTPUTS],
				int num_nodes_to_process,
				int nodes_to_process[MAX_NUM_NODES],
				int tour[MAX_NUM_OUTPUTS])
{
	int			i, j;
	int			first_node_gene, function_type;
	int			node_index;
	double	    in[MAX_NUM_GENES_PER_NODE];
	double	    output[MAX_OUTPUT_SIZE];


	/* load user defined constants into output array */
	for (j = 0; j < num_constant_inputs; j++)
		output[j] = constant_inputs[j];

	/* only process nodes that are used */
    for (j = 0; j< num_nodes_to_process; j++)
    {
		/* get address of node */
		node_index = nodes_to_process[j]-num_inputs;

		/* get address of first used gene in node */
		first_node_gene = num_genes_per_node*node_index;

		for (i = 0; i < num_genes_per_node-1; i++)						/* get input data to node */
			in[i]=output[chromosome[first_node_gene+i]];

        function_type=chromosome[first_node_gene+num_genes_per_node-1];	/* get node function */

        output[node_index+num_inputs]=node_type(in,function_type);		/* compute output of node and store */

	}

	/* process outputs */
    for (j = 0; j < num_outputs; j++)
	{
		 cgp_outputs[j] = output[chromosome[num_genes-num_outputs+j]];
		 tour[j] = j;
	}
}

/* integer radix sort */
void radixsort(unsigned  a[MAX_NUM_OUTPUTS][2],int tour[MAX_NUM_OUTPUTS],
               int num_items)
{
	int i, exp = 1;
	unsigned m=0;
	int x;
	unsigned b[MAX_NUM_OUTPUTS][2];

	/* get largest int in sort list */
	for (i = 0; i < num_items; i++)
	{
		if( a[i][0] > m)
			m = a[i][0];
	}

	while( m/exp > 0)
	{
		int bucket[10] = {0};


		for ( i = 0; i < num_items; i++)
		{
			x = (a[i][0]/exp) % 10;

			bucket[x] = bucket[x] + 1 ;
		}

		for (i = 1; i < 10; i++)
		{
			bucket[i] = bucket[i]+bucket[i-1];
		}


		for(i = num_items - 1; i >= 0; i--)
		{
			x = (a[i][0]/exp) % 10;

			bucket[x] = bucket[x]-1;

			b[bucket[x]][0]=a[i][0];
			b[bucket[x]][1]=a[i][1];

		}

		for(i = 0; i < num_items; i++)
		{
			a[i][0] = b[i][0];
			a[i][1] = b[i][1];
			tour[i] = a[i][1];
		}

		exp = exp*10;

		/*
		printf("\nPASS   : ");
		print_array( a, num_items);
		*/
	}
}

/* quicksort swap */
void swap(double* x,double* y,
		  int* r, int* s)
{
   int    t;
   double temp;

   temp = *x;
   *x = *y;
   *y = temp;

   t = *r;
   *r = *s;
   *s = t;

}

/* pivot for quicksort - should this be random, for faster quicksort? */
int choose_pivot(int i,int j )
{
   return((i+j) /2);

}


/* quicksort list but move ilist with it */
void quicksort(double list[],int ilist[], int m,int n)
{

   int i,j,k;
   double key;

   if( m < n)

   {

      k = choose_pivot(m,n);

      swap(&list[m],&list[k],&ilist[m],&ilist[k]);

      key = list[m];

      i = m+1;

      j = n;

      while(i <= j)

      {

         while((i <= n) && (list[i] <= key))

                i++;

         while((j >= m) && (list[j] > key))

                j--;

         if( i < j)

                swap(&list[i],&list[j],&ilist[i],&ilist[j]);

      }

      /* swap two elements */

      swap(&list[m],&list[j],&ilist[m],&ilist[j]);

      /* recursively sort the lesser lists */

      quicksort(list,ilist,m,j-1);

      quicksort(list,ilist,j+1,n);

   }

}

/* converts cgp_outputs to unsigned ints
   and then carries out radix sort to determine
   the tour. Not finished...may not need radix sort
*/
void get_tour_radix(int tour[MAX_NUM_CITIES],
			        double cgp_outputs[MAX_NUM_OUTPUTS])
{
	int i;
	unsigned working[MAX_NUM_OUTPUTS][2];
	double temp;

	/* convert cgp_outputs to 16 digit ints */

	for (i = 0; i < num_outputs; i++)
	{
		temp = 1e9*cgp_outputs[i];
		/* convert to unsigned */
		working[i][0] = (unsigned)temp;
		working[i][1] = i;
	}

	/* radix sort of working and getting the tour */
	radixsort(working, tour, num_outputs);

}

/* to find the tour just sort the cgp_outputs and
   move the city addreses (in tour) as you do the sort */
void get_tour(int tour[MAX_NUM_CITIES],
			  double cgp_outputs[MAX_NUM_OUTPUTS])
{

	/* quicksort cgp_outputs to get the tour */
	if (quick_sort)
		quicksort(cgp_outputs, tour, 0, num_outputs-1);
	else
		Fisher_Yates_shuffle(tour, cgp_outputs);

}


/*
The Fisher–Yates shuffle, in its original form, was described in 1938 by Ronald A. Fisher and Frank Yates in their book Statistical tables for
biological, agricultural and medical research (see wikipedia).
Here we use the "inside-out" version of Richard Durstenfeld. He proposed
his method in 1964 in Communications of the ACM volume 7, issue 7, as
"Algorithm 235: Random permutation"*/
void Fisher_Yates_shuffle(int tour[MAX_NUM_CITIES], double cgp_outputs[MAX_NUM_OUTPUTS])
{
	int i, j;

	/* source is initialised in get_parameters */

	tour[0] = source[0];

	for (i = 1; i < num_outputs; i++)
	{
		j = (int) ((double)i * cgp_outputs[i]);
		tour[i] = tour[j];
		tour[j] = source[i];

	}

}

/* calculates the distances between cities
   and puts them in a 2-d array. Note
   ceil(x) returns the nearest double with no fractional part
   greater than x. So (int)ceil(x) is the nearest integer greater
   than the double x. This is the recommended way of calculating
   distances in the TSPlib benchmark suite
   http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/
*/
void create_distance_matrix(void)
{
	int i, j;
	int xd, yd;

	for (i = 0 ; i <num_outputs-1; i++)
	{
		for (j = i+1 ; j < num_outputs; j++)
		{
			xd = cities[i][0]-cities[j][0];
			yd = cities[i][1]-cities[j][1];

			intercity_distance[i][j] = (int)ceil(sqrt(xd*xd+yd*yd));
			intercity_distance[j][i] = intercity_distance[i][j];
		}
		intercity_distance[i][i] = 0;
	}
	intercity_distance[num_outputs-1][num_outputs-1] = 0;
}

void print_distance_matrix(void)
{
	int i, j;
	FILE *fp;

	fp = fopen("city_distance.txt", "w");
	for (i = 0 ; i <num_outputs; i++)
	{
		for (j = 0; j < num_outputs; j++)
		{

			fprintf(fp,"%6d",intercity_distance[i][j]);
		}
		fprintf(fp,"\n");
	}
	fclose(fp);
}

/* returns the length of the tour
*/
int get_distance_from_matrix(int tour[MAX_NUM_CITIES], double previous_best_fit)
{
	int i, dist = 0;

	for (i = 0 ; i < num_outputs-1; i++)
	{

		dist = dist + intercity_distance[tour[i]][tour[i+1]];

		/* this tour is already worse.
		   So return with a worse value immediately */
		/* */
		if (dist < previous_best_fit)
			return (int) previous_best_fit - 10;
		/* */

	}
	/* and back to beginning */
    dist = dist + intercity_distance[tour[num_outputs-1]][tour[0]];

	return dist;
}

/* this calculates the exact distance and does not use
   ceil function which turns each distance into an integer.
*/
double get_fit_distance(int tour[MAX_NUM_CITIES], double previous_best_fit)
{
	int i;
	double dist = 0.0;
	double xd, yd;

	for (i = 0 ; i < num_outputs-1; i++)
	{
		xd = cities[tour[i+1]][0]-cities[tour[i]][0];
		yd = cities[tour[i+1]][1]-cities[tour[i]][1];

		dist = dist + sqrt(xd*xd+yd*yd);

		/* this tour is already worse.
		   So return with a worse value immediately */
		if (dist < previous_best_fit)
			return previous_best_fit - 10.0;
	}
	/* and back to beginning */
	xd = cities[tour[num_outputs-1]][0]-cities[tour[0]][0];
    yd = cities[tour[num_outputs-1]][1]-cities[tour[0]][1];
    dist = dist + sqrt(xd*xd+yd*yd);


	return dist;
}

/* this is the EA fitness function
*/
double fitness(int* chromosome, double previous_best_fit)
{
   int			num_nodes_to_process;
   int			nodes_to_process[MAX_NUM_NODES];
   double       fit;
   double   	cgp_outputs[MAX_NUM_OUTPUTS];
   int          tour[MAX_NUM_CITIES];

   /* find out how many nodes there are in the phenotype */
   num_nodes_to_process = get_nodes_to_process(chromosome, nodes_to_process);

   decode_cgp(chromosome, cgp_outputs,
			  num_nodes_to_process, nodes_to_process, tour);

   /* determine tour */
	get_tour(tour, cgp_outputs);

   /* minus sign to turn minimization in maximization */
    fit = -get_distance_from_matrix(tour, previous_best_fit);

   return fit;
}


/* returns the tour of cities */
void return_tour(int* chromosome,
				 int tour[MAX_NUM_OUTPUTS])
{
   int			num_nodes_to_process;
   int			nodes_to_process[MAX_NUM_NODES];
   double		cgp_outputs[MAX_NUM_OUTPUTS];

   /* find out how many nodes there are in the phenotype */
   num_nodes_to_process = get_nodes_to_process(chromosome, nodes_to_process);

   decode_cgp(chromosome, cgp_outputs,
			  num_nodes_to_process, nodes_to_process, tour);

   get_tour(tour, cgp_outputs);
}


/* This calculates the limits that are used in the claculation
   of allowed gene values (alleles) */
void get_gene_limits(int column, int* limit_min, int* limit)
{
	int limit_max;

	limit_max = num_inputs+column*num_rows;

	if (column < levels_back)
		*limit_min=0;
	else
		*limit_min = num_inputs + (column-levels_back)*num_rows;

	*limit= limit_max-(*limit_min);
}

/* returns a random valid connection allele that
   obeys the constraints imposed by levels_back.
   Also allows program inputs to disobey levels_back */
int get_connection_allele(int limit_min, int limit)
{
	int limit_plus, rand_num;
	int connection_allele;

	if (limit_min==0)
		connection_allele = newrand(limit);
	else /* allows inputs to disobey levels_back */
	{
		limit_plus = limit+num_inputs;
		rand_num = newrand(limit_plus);
		if (rand_num<limit)
			connection_allele = rand_num+limit_min;
		else
			connection_allele = rand_num-limit;
	}

	return connection_allele;
}

/* returns a random valid function allele */
int get_function_allele(void)
{
	return allowed_functions[newrand(num_functions)][0];
}

/* returns a random valid output allele */
int get_output_allele(void)
{
	int limit_min,limit;
	int output_allele;

	limit_min=num_inputs+(num_cols-levels_back)*num_rows;
	limit=levels_back*num_rows;

    output_allele = newrand(limit)+limit_min;

	return output_allele;
}


/* calculates how many mutations to do per chromosome */
int get_num_mutant(int num_genes, double per_cent_mutate)
{
	return (int)(num_genes*per_cent_mutate/100.0);
}


/* calculates how many genes to crossover */
int get_number_of_crossover_genes(int num_outputs, double per_cent_output_crossover)
{
	return (int)(num_outputs*per_cent_output_crossover/100.0);
}

/* determines the number of mutations required
   for functions, connections and arguments
*/
void get_number_of_mutations(void)
{
	num_connection_mutations = get_num_mutant(num_connection_genes,per_cent_connection_mutate);
	num_function_mutations   = get_num_mutant(num_nodes,per_cent_function_mutate);
	num_output_mutations     = get_num_mutant(num_outputs,per_cent_output_mutate);
}


/* get a random connection gene */
int get_valid_connection_gene(int num_connection_genes)
{
	int new_gene;

	new_gene = newrand(num_connection_genes);

	return new_gene;
}

/* get a random function gene */
int get_valid_function_gene(int num_functions)
{
	int new_gene;

	new_gene = newrand(num_functions);

	return new_gene;
}

/* get a random output gene */
int get_valid_output_gene(int num_outputs)
{
	int new_gene;

	new_gene = newrand(num_outputs);

	return new_gene;
}


/* mutate a connection gene in a chromosome */
void mutate_a_connection_gene(int*  chromosome)
{
	int which_connection, which_connection_gene;
	int which_node, which_col, which_gene;
	int limit_min, limit;

	/* get a randomly chosen connection gene */
	which_connection_gene = get_valid_connection_gene(num_connection_genes);

	/* find out what column it is in */
	which_col = which_connection_gene/(num_connection_genes_per_node*num_rows);

	/* calculate the gene limits obeying levels back */
	get_gene_limits(which_col,&limit_min,&limit);

	/* identify which node we landed on */
	which_node = which_connection_gene/num_connection_genes_per_node;

	/* identify which connection */
	which_connection = which_connection_gene % num_connection_genes_per_node;

	/* calculate which gene should be changed in the chromosome */
	which_gene = which_node*num_genes_per_node + which_connection;

	chromosome[which_gene]=get_connection_allele(limit_min,limit);

}

/* mutate a specific connection gene in a chromosome */
void mutate_a_specific_connection_gene(int*  chromosome, int which_connection_gene)
{
	int which_connection;
	int which_node, which_col, which_gene;
	int limit_min, limit;

	/* find out what column it is in */
	which_col = which_connection_gene/(num_connection_genes_per_node*num_rows);

	/* calculate the gene limits obeying levels back */
	get_gene_limits(which_col,&limit_min,&limit);

	/* identify which node we landed on */
	which_node = which_connection_gene/num_connection_genes_per_node;

	/* identify which connection */
	which_connection = which_connection_gene % num_connection_genes_per_node;

	/* calculate which gene should be changed in the chromosome */
	which_gene = which_node*num_genes_per_node + which_connection;

	chromosome[which_gene]=get_connection_allele(limit_min,limit);

}

/* mutate a function gene in a chromosome */
void mutate_a_function_gene(int*  chromosome)
{
	int which_node, which_gene;

	which_node = get_valid_function_gene(num_nodes);

	which_gene = (which_node+1)*num_genes_per_node-1;

	chromosome[which_gene] = get_function_allele();
}

/* mutate a specific function gene in a chromosome */
void mutate_a_specific_function_gene(int*  chromosome, int which_node)
{
	int which_gene;

	which_gene = (which_node+1)*num_genes_per_node-1;

	chromosome[which_gene] = get_function_allele();

}

/* mutate an output gene in a chromosome */
void mutate_an_output_gene(int*  chromosome)
{
	int which_output, which_gene, copy_allele;
	int another_output, another_gene;

	which_output = get_valid_output_gene(num_outputs);
	another_output = get_valid_output_gene(num_outputs);

	which_gene = num_nodes*num_genes_per_node + which_output;
	another_gene = num_nodes*num_genes_per_node + another_output;

	if (output_copy_mutation)
	{	/* swap one output gene with another */
		if (drand() < output_mutate_copy_prob)
		{
			copy_allele = chromosome[which_gene];
			chromosome[which_gene] = chromosome[another_gene];
			chromosome[another_gene] = copy_allele;
		}
		else /* random mutation of output_genes */
			chromosome[which_gene] = get_output_allele();
	}
	else /* just do random output mutation */
		chromosome[which_gene] = get_output_allele();

}

/* mutate a specific output gene in a chromosome */
void mutate_a_specific_output_gene(int*  chromosome, int which_output)
{
	int which_gene;
	int another_output, another_gene, copy_allele;

	another_output = get_valid_output_gene(num_outputs);

	which_gene = num_nodes*num_genes_per_node + which_output;
	another_gene = num_nodes*num_genes_per_node + another_output;

	if (output_copy_mutation)
	{	/* swap one output gene with another */
		if (drand() < output_mutate_copy_prob)
		{
			copy_allele = chromosome[which_gene];
			chromosome[which_gene] = chromosome[another_gene];
			chromosome[another_gene] = copy_allele;
		}
		else /* random mutation of output_genes */
			chromosome[which_gene] = get_output_allele();
	}
	else /* just do random output mutation */
		chromosome[which_gene] = get_output_allele();

}




/* carry out num_mutations mutations on the chromosome */
void mutate_a_chromosome(int* chromosome,
						 int num_connection_mutations,
						 int num_function_mutations,
						 int num_output_mutations)
{
	int i;

	if (mutate_by_prob) /* interpret per_cent mutates as probabilities */
	{
		for (i = 0; i < num_connection_genes; i++)
			if (drand() < per_cent_connection_mutate/100.0)
				mutate_a_specific_connection_gene(chromosome,i);

		for (i = 0; i < num_nodes; i++)
			if (drand() < per_cent_function_mutate/100.0)
				mutate_a_specific_function_gene(chromosome,i);

		for (i = 0; i < num_outputs; i++)
			if (drand() < per_cent_output_mutate/100.0)
				mutate_a_specific_output_gene(chromosome,i);
	}
	else /* mutate by rate */
	{
		for (i = 0; i < num_connection_mutations; i++)
			mutate_a_connection_gene(chromosome);

		for (i = 0; i < num_function_mutations; i++)
			mutate_a_function_gene(chromosome);

		for (i = 0; i < num_output_mutations; i++)
			mutate_an_output_gene(chromosome);
	}

}



/* generates a random chromosome. Used by initialise */
void generate_a_random_chromosome(int* chromosome)
{
	int i, count = 0;
	int row, col, limit, limit_min;

	for (col = 0; col < num_cols; col++)
	{
		get_gene_limits(col,&limit_min,&limit);

		for (row=0;row<num_rows;row++)
		{
			/* get random connection genes */
			for (i = 0; i < num_genes_per_node-1; i++)
				chromosome[count+i]=get_connection_allele(limit_min,limit);

			/* get random function gene */
			chromosome[count+num_genes_per_node-1]=get_function_allele();
			count=count+num_genes_per_node;
        }
	}
	/* get random function genes */
	for ( i = 0; i < num_outputs; i++)
		chromosome[count+i]=get_output_allele();
}


/* creates initial population of chromosomes
   either having been generated from a single
   chromosome from a file or by generating
   an entire random population
*/
void initialise(int** chromosomes)
{
    int  i;

    if (run_from_chrom)
		read_from_chrom(chromosomes);
    else  /* generate random population */
		for (i = 0; i < population_size; i++)
			generate_a_random_chromosome(chromosomes[i]);
}

/* calculate best population fitness and the best chromosome */
double  get_best_chromosome(int**  chromosomes,
                            int*   best_chromosome,
						    double previous_best_fitness,
						    int    gen)
{
	int		i;

    double	fitness_max= -1.0e200, fit;
    int		best_member = population_size -1;

    for (i = 0; i < population_size; i++)
    {

		if ((i == population_size -1) && (gen > 1))
			fit = previous_best_fitness;
		else
			fit = fitness(chromosomes[i], previous_best_fitness);

		if (fit > fitness_max)
		{
			fitness_max = fit;
			best_member = i;
		}
	}

	/* store the best chromosome if it is new */
	if (best_member != population_size)
		for (i=0;i<num_genes;i++)
			best_chromosome[i]=chromosomes[best_member][i];


	return fitness_max;
}

/* create output chromosomes that are a mixture of genes
  from one parent or the other
*/
void crossover_outputs(int* chromosome1, int* chromosome2,
					   int child1[MAX_NUM_OUTPUTS], int child2[MAX_NUM_OUTPUTS])
{
	int i;
	int place_to_write;
	int num_crossover_sites;
	int crossover_mask[MAX_NUM_OUTPUTS] = {0};

	/* generate a crossover mask with exactly
	   num_crossover_genes ones where crossover of
	   genes will take place
	*/
	num_crossover_sites = 0;
	do
	{
		place_to_write = newrand(num_outputs);
		if (crossover_mask[place_to_write] == 0)
		{
			crossover_mask[place_to_write] = 1;
			num_crossover_sites++;
		}
	}
	while (num_crossover_sites < num_crossover_genes);

	/* create child1 and child 2 output chromosomes
	   by swapping genes over from pair of population
	   members according to mask
	*/
	for (i = 0; i < num_outputs; i++)
	{
		if (crossover_mask[i])
		{
			child1[i] = chromosome1[num_genes-num_outputs+i];
			child2[i] = chromosome2[num_genes-num_outputs+i];
		}
		else
		{
			child1[i] = chromosome2[num_genes-num_outputs+i];
			child2[i] = chromosome1[num_genes-num_outputs+i];
		}

	}

	/* write back to chromosome */
	for (i = 0; i < num_outputs; i++)
	{
		chromosome1[num_genes-num_outputs+i] = child1[i];
		chromosome2[num_genes-num_outputs+i] = child2[i];

	}
}

/* (1+lambda evolutionary strategy where lamda = population size -1 */
void generate_new_pop_es(int** chromosomes,
						 int*  best_chromosome)
{
	int		j , k;
	int     child1[MAX_NUM_OUTPUTS];
	int		child2[MAX_NUM_OUTPUTS];


     /* copy best_chromosome into last member of chromosome array */
    for (j = 0; j < num_genes;j ++)
       chromosomes[population_size-1][j]=best_chromosome[j];

    /* generate new population by mutating all but last */
    for (k = 0; k < population_size-1; k++)
    {
		for (j = 0; j < num_genes; j++) /* copy best chromosome */
          chromosomes[k][j]=best_chromosome[j];

		/* mutate the chromosome */
		mutate_a_chromosome(chromosomes[k],
		                   num_connection_mutations,
			               num_function_mutations,
			               num_output_mutations);
	}
	/* take pairs of chromosomes excluding best and crossover outputs */
	if (num_crossover_genes != 0)
	{

		for (k = 0; k <= (population_size-1)/2; k = k + 2)
		{

			crossover_outputs(chromosomes[k], chromosomes[k+1],
				              child1, child2);
		}
	}
}

/* allocate space for 2 dimensional array
   e.g. a population of chromosomes */
int** create_2d_space(int num_horizontals, int num_verticals)
{
	int i;
	int **array2d = NULL;

	/* create space for pointers to int pointers */
	array2d = (int** ) calloc(num_verticals, sizeof(int*));
	if (array2d == NULL)
	{
       printf("ERROR.Can not allocate space for %d many int pointers\n",num_verticals);
       exit(0);
	}

	/* create array of pointers to ints  */
	for (i = 0;i < num_verticals;i++)
	{
	  array2d[i] = create_1d_space(num_horizontals);
      if (array2d[i] == NULL)
      {
         printf("ERROR.Not enough memory for int pointer arrays of length %d\n",num_horizontals);
         exit(0);
      }
   }

	return array2d;
}

/* allocate space for a 1d array of with size items */
int* create_1d_space(int size)
{
	int* array1d = NULL;

	array1d = (int*) calloc(size,sizeof(int));

    if (array1d == NULL)
    {
		printf("ERROR.Not enough memory for a 1d array of length %d\n", size);
        exit(0);
    }
	return array1d;
}


/* release memory */
/* if this is for chromosomes then num_verticals is population_size */
void free_array2d(int num_verticals, int** array2d)
{
	int i;

	/* free 1darray of pointers  */
	for (i = 0; i < num_verticals; i++)
		free(array2d[i]);

	free(array2d);
}

void write_generation_to_screen(int gen_index)
{
	if (gen_index % report_interval==0)
		printf("\nGENERATION is %d",gen_index);
}

/* writes generation and fitness of best in population */
void write_progress_info_to_screen(int generation, double fit)
{
	printf("\nGENERATION is %d Best fit is now %20.1lf",generation,fit);
}


/* writes out chromosome to file defined by string prog */
void write_progress_info_to_file(char prog[MAX_NUM_LETTERS],
								 int gen, double best_fit,
						         int* best_chromosome)
{
	FILE* fp;

    fp=fopen(prog,"a");
    fprintf(fp,"\nGENERATION is %u     Best fitness is now %20.1lf",gen,best_fit);
    fprintf(fp,"\nThe chromosome is\n");
    fclose(fp);
    fprint_a_chromosome(best_chromosome,prog,1);
	fprint_active_genes(best_chromosome,prog);
}

/* checks to see if the best fitness in the population has improved.
   writes the generation, the new best fitness and the improved chromosome
   to the progress report (if progress report is set to 1)
*/
void check_if_improvement(double best_fit, double* previous_best_fit, int* best_gen, int gen,
						  char prog[MAX_NUM_LETTERS],
						  int* best_chromosome)
{
	if ((best_fit - *previous_best_fit) > FITNESS_THRESHOLD) /* we have an improvement */
	{
		if (progress_report)
		{
			write_progress_info_to_screen(gen,best_fit);
			if (progress_report ==1)
				write_progress_info_to_file(prog, gen, best_fit, best_chromosome);
		}
		*best_gen = gen;				/* update the best generation */
		*previous_best_fit = best_fit;	/* update previous best fitness */
	}
}


/* report on results of evolutionary run in cgp_tsp.txt */
void write_result_of_EA_to_file(int run, int bgen, double best_fit,
								int tour[MAX_NUM_CITIES],
								int* best_chromosome)
{
	int i;
	FILE* fp;

	fp=fopen("cgp_tsp.txt","a");

	fprintf(fp,"\nRun %d and gen %d achieved fitness %20.1lf\n",run,bgen,best_fit);
    fprintf(fp,"And achieved true fitness %20d\n",-get_distance_from_matrix(tour,best_fit));

	fprintf(fp,"The tour is:\n");
	for (i = 0; i < num_outputs; i++)
		fprintf(fp,"%d \n",tour[i]+1);
	fprintf(fp,"\nHere is the chromosome\n");
	fclose(fp);

	fprint_a_chromosome(best_chromosome,"cgp_tsp.txt",1);
	fprint_active_genes(best_chromosome,"cgp_tsp.txt");
}

/* Do a run of the EA */
double  EA(int *gen_of_best, int* num_nodes_active, int run,
		char prog[MAX_NUM_LETTERS])
{
	/* int     i, j; */
	int		gen, best_gen;
	int		nodes_to_process[MAX_NUM_NODES];
	int**	chromosomes;
	int*	best_chromosome;
	char    runstring[MAX_NUM_LETTERS];
	char    chromstring[MAX_NUM_LETTERS];
	char    tourstring[MAX_NUM_LETTERS];
	int     tour[MAX_NUM_CITIES];
	double	best_fit=1.0e200, previous_best_fit = -1.0e200;
	/* FILE*   fpx; */

	/*
    population_size = MAX_NUM_CHROMOSOMES;
    */

	chromosomes = create_2d_space(num_genes, population_size);
	best_chromosome = create_1d_space(num_genes);

	initialise(chromosomes);

	for (gen = 1 ; gen <= num_generations; gen++)
	{
		write_generation_to_screen(gen);

		best_fit = get_best_chromosome(chromosomes,
									   best_chromosome,
									   previous_best_fit, gen);

		check_if_improvement(best_fit, &previous_best_fit, &best_gen, gen, prog,
							 best_chromosome);

		/* create a new population */
		generate_new_pop_es(chromosomes,best_chromosome);
	}

	*num_nodes_active = get_nodes_to_process(best_chromosome, nodes_to_process);

	return_tour(best_chromosome, tour);

	write_result_of_EA_to_file(run, best_gen, best_fit, tour, best_chromosome);

	*gen_of_best = best_gen;

	strcpy(chromstring, "cgp");
	sprintf(runstring,"%d",run); /* store run as characters */
	strcat(chromstring,runstring);
	strcpy(tourstring,chromstring);
	strcat(tourstring,".tor");
	strcat(chromstring,".chr");

	/* write the raw best chromosome to cgp#.chr */
	/* uncomment fprintf_a_tour to make
	   the program print out a detailed breakdown
	   of why the tour has a particular length.
	   This is written to cgp#.tor */
	if (save_chrom)
	{
		fprint_a_raw_chromosome(best_chromosome,chromstring,0);
		/* fprint_a_tour(tour,tourstring,0); */
	}


	free_array2d(population_size, chromosomes);
	free(best_chromosome);

	return best_fit;
}

void setup_report_files(int run, char prog[MAX_NUM_LETTERS])
{
	char runstring[MAX_NUM_LETTERS];
	FILE* fp;

	sprintf(runstring,"%d",run); /* store run as characters */
	if (progress_report > 0)
	{
		strcpy(prog,"cgp_tsp");
		strcat(prog,runstring);
		strcat(prog,".prg"); /* create .prg file name */
		fp=fopen(prog,"w");  /* create empty .prg file */
		fclose(fp);
	}
}

void report_final_results(double av_fitness, double st_dev,
						  double av_num_nodes_active,
						  double best_of_best_fit,
						  double worst_of_best_fit,
						  double av_best_gen,
						  double stdev_best_gen,
						  double av_num_evals,
						  double stdev_num_evals,
						  clock_t start)
{

	FILE* best;
	best=fopen("cgp_tsp.txt","a");

	fprintf (best, "Execution time is %f\n", ( (double)clock() - start ) / CLOCKS_PER_SEC );


	fprintf(best,"\naverage fitness  %20.10lf\n",av_fitness);
	fprintf(best,"\nstd dev          %20.10lf\n\n",st_dev);

	fprintf(best,"\naverage number of generations  %10.6lf\n",av_best_gen);
	fprintf(best,"\nstd dev number of generations  %10.6lf\n\n",stdev_best_gen);

	fprintf(best,"\naverage number of evaluations  %10.6lf\n",av_num_evals);
	fprintf(best,"\nstd dev number of evaluations  %10.6lf\n\n",stdev_num_evals);

	fprintf(best,"\naverage number of active nodes is  %6.4lf\n",av_num_nodes_active);
	fprintf(best,"\nThe best fitnes of all runs  is  %20.1lf\n",best_of_best_fit);
	fprintf(best,"\nThe worst fitness of all runs is  %20.1lf\n",worst_of_best_fit);
	fclose(best);
}

double average(int num_items, double items[MAX_NUM_RUNS])
{
	int		i;
	double	av;

	av = 0.0;
	for (i = 0; i < num_items; i++)
		av = av + items[i];

	av = av/num_items;

	return av;
}

double get_standard_deviation(int num_items, double average,
							  double items[MAX_NUM_RUNS])
{
	int i;
	double temp, st_dev;

	st_dev = 0.0;
	for (i = 0; i < num_items;i++)
	{
		temp=(items[i]-average);
		temp=temp*temp;
		st_dev=st_dev+temp;
	}

	st_dev=st_dev/(double)num_items;
	st_dev=sqrt(st_dev);

	return st_dev;
}

/* do mutiple runs of EA and write out results */
void run_EA(int num_runs_total)
{
	int		best_gen,run;
	int		num_nodes_active;
	double  active_nodes_in_run[MAX_NUM_RUNS];
	double	generations_for_best[MAX_NUM_RUNS];
	double  evals_for_best[MAX_NUM_RUNS];
	char	prog[MAX_NUM_LETTERS];
	double  worst_of_best_fit=1.0e200, best_of_best_fit = -1.0e200;
	double  fitness_final;
	double	st_dev, av_fitness;
	double  av_best_gen, stdev_best_gen;
	double  av_num_evals, stdev_num_evals;
	double  av_num_nodes_active;
	double	fitnesses[MAX_NUM_RUNS];

	clock_t start = clock();

	for (run = 0; run < num_runs_total; run++)
	{
		printf("\n\nRUN %d\n",run);

		setup_report_files(run, prog);

		fitness_final =  EA(&best_gen, &num_nodes_active, run, prog);

		if (run == 0)
			worst_of_best_fit = fitness_final;

		fitnesses[run] = fitness_final;
		active_nodes_in_run[run] = num_nodes_active;
		generations_for_best[run] = best_gen;
		evals_for_best[run]=(population_size-1)*best_gen+1;
        if (fitness_final < worst_of_best_fit)
			worst_of_best_fit=fitness_final;

		if (fitness_final > best_of_best_fit)
			best_of_best_fit=fitness_final;


	}
	av_best_gen = average(num_runs_total, generations_for_best);
	stdev_best_gen = get_standard_deviation(num_runs_total, av_best_gen, generations_for_best);
	av_num_evals = (population_size-1)*av_best_gen+1.0;
	stdev_num_evals = get_standard_deviation(num_runs_total, av_num_evals, evals_for_best);

	av_num_nodes_active = average(num_runs_total, active_nodes_in_run);
	av_fitness = average(num_runs_total, fitnesses);
	st_dev = get_standard_deviation(num_runs_total, av_fitness, fitnesses);
	report_final_results(av_fitness, st_dev, av_num_nodes_active,
						 best_of_best_fit, worst_of_best_fit,
						 av_best_gen, stdev_best_gen, av_num_evals, stdev_num_evals,start);

	printf ( "Execution time is %f\n", ( (double)clock() - start ) / CLOCKS_PER_SEC );
}

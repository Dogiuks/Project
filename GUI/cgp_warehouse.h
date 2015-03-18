#ifndef CGP_WAREHOUSE_H
#define CGP_WAREHOUSE_H


struct products
{
    char name[50];
    int  qnt;
    int  code;
    int  demand;
}product;


//typedef struct products* product_pointer;

products output[100];

#endif // CGP_WAREHOUSE_H


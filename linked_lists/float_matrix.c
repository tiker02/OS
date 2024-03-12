#include <stdlib.h>
#include "float_matrix.h" 


void FloatMatrixInit(MatrixHead* head)
{
    List_init( head);
    for(int i = 0; i < head->rows; i++)
    {
        MatrixRow* row_it = malloc(sizeof(MatrixRow));
        row_it->list.prev = NULL;
        row_it->list.next = NULL;
        row_it->row = malloc(sizeof(ListHead));
        List_insert( head, head->list.last,  row_it);
        List_init(row_it->row);
        
        for(int j = 0; j < head->cols; j++)
        {
            FloatListItem* new_it = malloc(sizeof(FloatListItem));
            new_it->info = (float)(i + j*0.1);
            new_it->list.prev = NULL; 
            new_it->list.next = NULL;
            List_insert(row_it->row, row_it->row->last,  new_it);
        }
        
        //FloatListItem* second_float_it = (FloatListItem*) second_in_list;
        //printf("%0.2f\n", second_float_it->info);
    }
}

void FloatMatrixDelete(MatrixHead* matrix)
{
    MatrixRow* row_it = matrix->list.first;
    while(row_it)
    {
        while(row_it->row->first)
        {
            free(List_detach(row_it->row, row_it->row->first));
        }
        free(row_it->row);
        free(List_detach( matrix,  row_it));

        row_it = (MatrixRow*)  matrix->list.first;
    }
    free(matrix);
}

void FloatList_print(ListHead* list) 
{
    FloatListItem* f_it = list->first;
    while(f_it)
    {
        printf("%0.2f ", f_it->info);
        f_it = f_it->list.next;
    }
    printf("\n");
}

void FloatMatrix_print(MatrixHead* matrix)
{
    MatrixRow* row = matrix->list.first;
    while(row)
    {
        FloatList_print(row->row);
        row = row->list.next;
    }
}

ListHead* perRowSum(MatrixHead* matrix)
{
    ListHead* SumList = malloc(sizeof(ListHead));
    List_init(SumList);

    MatrixRow* row = matrix->list.first;
    while(row)
    {
        float sum = 0;
        FloatListItem* f_it = row->row->first;
        while(f_it)
        {
            sum += f_it->info;
            f_it = f_it->list.next;
        }

        FloatListItem* sum_it = malloc(sizeof(FloatListItem));
        sum_it->list.next = NULL;
        sum_it->list.prev = NULL;
        sum_it->info = sum;
        List_insert(SumList, SumList->last, sum_it);

        row = row->list.next; 
    }

    return SumList;
}


int main(int  argc, char* argv[])
{
    MatrixHead* m = malloc(sizeof(MatrixHead));
    m->rows = 4;
    m->cols = 5;
    FloatMatrixInit(m);
    FloatMatrix_print(m);
    FloatList_print(perRowSum(m));
    FloatMatrixDelete(m);
}

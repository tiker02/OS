#include "float_matrix.h" 


void FloatMatrixInit(MatrixHead* head)
{
    head->list = malloc(sizeof(ListHead));
    List_init(head->list);
    ListItem* prev = head->list->last;
    for(int i = 0; i < head->rows; i++)
    {
        MatrixRow* row = malloc(sizeof(MatrixRow));
        row->row = malloc(sizeof(ListHead));
        List_insert(head, prev, row);
        prev = prev = head->list->last;

        for(int j = 0; j < head->cols; j++)
        {
            List_init(row->row);

            FloatListItem* new_it = malloc(sizeof(FloatListItem));
            new_it->info = (float)(i + j/10);
            List_insert(row->row, row->row->last, new_it); 
        }
    }
}

void FloatMatrixDelete(MatrixHead* matrix)
{
    MatrixRow* row = matrix->list->first;
    while(row)
    {
        while(row->row->first)
        {
            free(List_detach(row->row, row->row->first));
        }
        free(row->row);

        free(List_detach(matrix->list, row));
    }
    free(matrix->list);
    free(matrix);
}

void FloatList_print(ListHead* list) 
{
    FloatListItem* f_it = list->first;
    while(f_it)
    {
        printf("%0.2f ", f_it->info);
        f_it = f_it->list->next;
    }
    printf("\n");
}

void FloatMatrix_print(MatrixHead* matrix)
{
    MatrixRow* row = matrix->list->first;
    while(row)
    {
        FloatList_print(row->row);
        row = row->list->next;
    }
}

ListHead* perRowSum(MatrixHead* matrix)
{
    ListHead* SumList = malloc(sizeof(ListHead));
    List_init(SumList);

    MatrixRow* row = matrix->list->first;
    while(row)
    {
        float sum = 0;
        FloatListItem* f_it = row->row->first;
        while(f_it)
        {
            sum += f_it->info;
            f_it = f_it->list->next;
        }

        FloatListItem* sum_it = malloc(sizeof(FloatListItem));
        sum_it->info = sum;
        List_insert(SumList, SumList->last, sum_it);

        row = row->list->next; 
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
    FloatMatrix_print(perRowSum(m));
    FloatMatrixDelete(m);
}

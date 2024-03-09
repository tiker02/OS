#include "float_matrix.h" 

void FloatMatrixInit(MatrixHead* head)
{
    ListItem* prev = head->list->last;
    for(int i = 0; i < head->rows; i++)
    {
        MatrixRow* row = malloc(sizeof(MatrixRow));
        row->list = malloc(sizeof(ListHead));
        List_insert(head, prev, row);
        prev = prev = head->list->last;

        //ADD ACTUAL ROW LIST
    }
}
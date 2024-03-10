#include "linked_list.h"

typedef struct FloatListItem{
    struct ListItem* list;
    float info;
} FloatListItem;

typedef struct MatrixRow{
    struct ListItem* list;
    struct ListHead* row;
} MatrixRow;

typedef struct MatrixHead{
    struct ListHead* list;
    int rows;
    int cols;
} MatrixHead;

void FloatMatrixInit(MatrixHead* head);

void FloatList_print(ListHead* list);

void FloatMatrix_print(MatrixHead* matrix);

void FloatMatrixDelete(MatrixHead* matrix);

ListHead* perRowSum(MatrixHead* matrix);
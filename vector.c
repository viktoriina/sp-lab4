#include <stdlib.h>

#include "vector.h"


IntVector *create_vector()
{
    static const int initial_vector_size = 10;
    IntVector *tmp_vector = malloc(sizeof(IntVector));
    tmp_vector->real_size = initial_vector_size;
    tmp_vector->size = 0;
    tmp_vector->data = malloc(sizeof(int) * initial_vector_size);

    return tmp_vector;
}

void clear_vector(IntVector *vector) {
    vector->size = 0;
}

void double_vector(IntVector *vector) {
    const int new_size = vector->real_size*2;
    int *new_data = malloc(sizeof(int) * new_size);
    for (int i = 0; i < vector->size; i++)
    {
        new_data[i] = vector->data[i];
    }
    
    int *temp = vector->data;
    vector->data = new_data;
    vector->real_size = new_size;
    free(temp);
}

void add_to_vector(IntVector* vector, int element) {
    if(vector->size == vector->real_size) {
        double_vector(vector);
    }

    vector->data[vector->size] = element;
    vector->size++;
}

void free_vector(IntVector *vector) {
    free(vector->data);
    free(vector);
}
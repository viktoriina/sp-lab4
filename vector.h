typedef struct
{
    int real_size;
    int size;
    int *data;
} IntVector;

void clear_vector(IntVector *vector);
void add_to_vector(IntVector* vector, int element);
IntVector *create_vector();
void free_vector(IntVector *vector);
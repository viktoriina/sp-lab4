#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#include "vector.h"

void print_error_and_exit(const char *message)
{
    printf("Termination, reason: %s\n", message);
    exit(0);
}

typedef struct 
{
    size_t alphabet_size;
    int* alpahbet;
    size_t states_size;
    int initial_state;
    int* final_states;
    size_t final_states_size;
    int** transitions;
} Automat;

typedef struct {
    int* first_word;
    int* second_word;
} InputData;

Automat* read_automat_data(const char* file_path);
InputData* read_input_data(int argc, char *argv[]);

char* words_are_acceptable(Automat* automat, InputData* words);

int main(int argc, char *argv[]) {
    Automat* automat;
    InputData* input_data;

    automat = read_automat_data("./test_data");

    if(automat == NULL) {
        print_error_and_exit("Failed to upload automat data");
    }

    input_data = read_input_data(argc, argv);
    if(input_data == NULL) {
        print_error_and_exit("Failed to read input data");
    }

    char* acceptable = words_are_acceptable(automat, input_data);

    if(acceptable != NULL) {
        printf("Words are acceptable\n");
        printf("Word beetween: %s\n", acceptable);
    } else {
        printf("Words are not acceptable\n");
    }
}

int read_number(FILE* document);
int read_letter(FILE* document);

Automat* read_automat_data(const char* file_path) {
    const char only_read_mode[] = "r";
    FILE *document = fopen(file_path, only_read_mode);

    Automat* automat = malloc(sizeof(Automat));
    automat->alphabet_size = read_number(document);
    automat->alpahbet = malloc(sizeof(int) * automat->alphabet_size);
    for(int i = 0; i < automat->alphabet_size; i++) {
        char c = read_letter(document);
        automat->alpahbet[i] = c;
    }
    automat->states_size = read_number(document);
    automat->initial_state = read_number(document);
    automat->final_states_size = read_number(document);

    automat->final_states = malloc(sizeof(int) * automat->final_states_size);

    int symb;
    int counter = 0;

    while (counter < automat->final_states_size)
    {
        automat->final_states[counter] = read_number(document);
        counter++;
    }

    IntVector* transitions = create_vector();

    while (1)
    {
        symb = read_number(document);
        int c = read_letter(document);
        int symb2 = read_number(document);
        if(symb == -1) {
            break;
        }
        add_to_vector(transitions, symb);
        add_to_vector(transitions, c);
        add_to_vector(transitions, symb2);
    }

    automat->transitions = malloc(sizeof(int*) * automat->states_size);
    for(int i = 0; i < automat->states_size; i++) {
        automat->transitions[i] = malloc(sizeof(char) * 256);
    }

    for(int i = 0; i < automat->states_size; i++) {
        for(int j = 0; j < automat->alphabet_size; j++) {
            automat->transitions[i][automat->alpahbet[j]] = i;
        }
    }
    
    for(int i = 0; i < transitions->size/3; i++) {
        int p = i*3;
        automat->transitions[transitions->data[p]][transitions->data[p+1]] = transitions->data[p+2];
    }

    free_vector(transitions);
    
    return automat;
}

int* extract_word(const char* str);

InputData* read_input_data(int argc, char *argv[]) {
    InputData* input_data = malloc(sizeof(InputData));
    
    input_data->first_word = extract_word(argv[1]);
    input_data->second_word = extract_word(argv[2]);

    return input_data;
}


int apply_word_to_state(Automat* automat, int initial_state, int* word);
int check_if_states_are_reachable(Automat* automat, int initial_state, int final_sate);
int letter_in_array(int letter, int* array, int size);
char* get_word_between_states(Automat* automat, int state1, int state2);

char* words_are_acceptable(Automat* automat, InputData* words) {
    int state_after_word1 = apply_word_to_state(automat, automat->initial_state, words->first_word);
    int* states_map_word2 = malloc(sizeof(int) * automat->states_size);

    for(int i = 0; i < automat->states_size; i++) {
        states_map_word2[i] = apply_word_to_state(automat, i, words->second_word);
    }

    for(int i = 0; i < automat->states_size; i++) {
        if(letter_in_array(states_map_word2[i], automat->final_states, automat->final_states_size)) {
            if(check_if_states_are_reachable(automat, state_after_word1, i)) {
                return get_word_between_states(automat, state_after_word1, i);
            }
        }
    }

    return NULL;
}

char* get_word_between_states(Automat* automat, int state1, int state2) {
    if (state1 == state2) {
        return "";
    }
    IntVector* remaining_states = create_vector();
    for(int i = 0; i < automat->states_size; i++) {
        if(i != state1 ) {
            add_to_vector(remaining_states, i);
        }
    }
    int index = 0;
    IntVector* result = create_vector();

    while(state1 != state2) {
        int current_testable = remaining_states->data[index];
        index++;
        if(check_if_states_are_reachable(automat, state1,current_testable) && check_if_states_are_reachable(automat, current_testable, state2)) {
            for(int i = 0; i < automat->alphabet_size; i++) {
                if(automat->transitions[state1][automat->alpahbet[i]] == current_testable) {
                    add_to_vector(result, automat->alpahbet[i]);
                    state1 = current_testable;
                    break;
                }
            }
        }
    }
    char* res = malloc(sizeof(char) * (result->size + 1));
    res[result->size] = '\0';
    for(int i = 0; i < result->size; i++) {
        res[i] = result->data[i];
    }

    return res;
}


int apply_word_to_state(Automat* automat, int initial_state, int* word) {
    int counter = 0;
    while(word[counter] != -1) {
        int i = 0;
        initial_state = automat->transitions[initial_state][word[counter]];
        counter++;
    }

    return initial_state;
}

int check_if_states_are_reachable(Automat* automat, int initial_state, int final_sate) {
    if(initial_state == final_sate) {
        return 1;
    }
    IntVector* states_to_check = create_vector();
    int counter = 0;
    char* visited_states = malloc(sizeof(char) * automat->states_size);
    for(int i = 0; i < automat->states_size; i++) {
        visited_states[i] = 0;
    }
    visited_states[initial_state] = 1;
    add_to_vector(states_to_check, initial_state);

    while (states_to_check->size != counter)
    {
        int current_state = states_to_check->data[counter];
        counter++;
        if(current_state == final_sate) {
            return 1;
        }
        for(int i = 0; i < automat->alphabet_size; i++) {
            int to_state = automat->transitions[current_state][automat->alpahbet[i]];

            if(!visited_states[to_state]) {
                visited_states[to_state] = 1;
                add_to_vector(states_to_check, to_state);
            }
        }
    }
    
    return 0;
}

int letter_in_array(int letter, int* array, int size) {
    for(int i = 0; i < size; i++) {
        if(array[i] == letter) {
            return 1;
        }
    }

    return 0;
}

int extract_next_letter(int* pointer, const char* str);


int* extract_word(const char* str) {
    int pointer = 0;
    IntVector* numbers = create_vector();
    int num = extract_next_letter(&pointer, str);
    while(num != -1) {
        add_to_vector(numbers, num);
        num = extract_next_letter(&pointer, str);
    }
    int* result = malloc(sizeof(int) * (numbers->size + 1));

    for(int i = 0; i < numbers->size; i++) {
        result[i] = numbers->data[i];
    }
    result[numbers->size] = -1;
    free_vector(numbers);

    return result;
}


int extract_next_letter(int* pointer, const char* str) {
    int num = str[*pointer];
    if(num == '\0') {
        return -1;
    }

    (*pointer) = *pointer + 1;
    num = str[*pointer];
    if(num == '\0') {
        return -1;
    }

    return num;
}

int read_number(FILE* document) {
    int num = getc(document);
    if(num == EOF) {
        return -1;
    }

    while(!isdigit(num)) {
        num = getc(document);
        if(num == EOF) {
            return -1;
        }
    }

    char* number = malloc(sizeof(char) * 40);
    int counter = 0;
    while(isdigit(num)) {
        number[counter] = num;
        counter++;
        num = getc(document);
    }
    number[counter] = '\0';
    

    int result = atoi(number);
    free(number);
    return result;
}

int read_letter(FILE* document) {
    int c = getc(document);
    if(c == EOF) {
        return -1;
    }

    while(!isalpha(c)) {
        c = getc(document);
        if(c == EOF) {
            return -1;
        }
    }

    return c;
}

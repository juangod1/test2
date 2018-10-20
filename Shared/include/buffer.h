#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer_t * buffer_p;

struct buffer_t{
    size_t size;
    size_t count;
    char * data_start;
    char * data_ptr;
};

int buffer_initialize(buffer_p * buffer,size_t size);
int buffer_finalize(buffer_p buffer);

int buffer_is_empty(buffer_p buffer);
int buffer_read(int file_descriptor, buffer_p buffer);
int buffer_write(int file_descriptor, buffer_p buffer);
int buffer_write_until_substring(int file_descriptor, buffer_p buffer, char * substring);

int find_substring(char * buffer, int size, char * substring);

#endif
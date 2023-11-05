//
// Created by varsem on 05.11.23.
//
#define eps 1e-16

#include <cstdio>
#include <iostream>

using namespace std;

int average(char *filename, double *min_, double *max_);
void *process_average(void *args);

int local_answer(char* filename, double average);
void *process_local_answer(void* args);

enum class io_status
{
    success,
    error_open,
    error_read,
    none
};

class ARGS
{
public:
    char *filename = nullptr;
    int m = 0;
    int p = 0;

    double local_min = 0;
    double local_max = 0;
    double average = 0;

    int local_result = 0;
    io_status status = io_status::none;
};
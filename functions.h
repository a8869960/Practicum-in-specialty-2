//
// Created by varsem on 05.11.23.
//
#include <cstdio>
#include <iostream>
#include <pthread.h>

using namespace std;

int average(char *filename, double *min_, double *max_);
int local_answer(char* filename, double average);

void *process_main(void *args);

void reduce_sum(int p, double* a = nullptr, int n = 0);

enum class io_status
{
    success,
    error_open,
    error_read,
    none
};

class RES
{
public:
    io_status status = io_status::none;

    double local_min = 0;
    double local_max = 0;
};

class ARGS
{
public:
    char *filename = nullptr;
    int m = 0;
    int p = 0;

    int local_result = 0;
    bool is_empty = false;
    
    RES *res = nullptr;
};

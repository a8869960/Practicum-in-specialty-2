//
// Created by varsem on 05.11.23.
//
#include "functions.h"

int average(char *filename, double *min_, double *max_)
{
    FILE *f;

    f = fopen(filename, "r");
    if(!f)
    {
        cout << "Can't open the file " << filename << endl;
        return -1;
    }

    double min, max, current;

    if(fscanf(f, "%lf", &current) == -1)
    {
        fclose(f);
        return 0;
    }

    max = current;
    min = current;

    while(fscanf(f, "%lf", &current) == 1)
    {
        if(current - min < eps)
            min = current;

        if(max - current < eps)
            max = current;

    }

    if(fscanf(f, "%lf", &current) != EOF)
    {
        cout << "Wrong input data in " << filename << endl;
        fclose(f);
        return -2;
    }

    fclose(f);

    *min_ = min;
    *max_ = max;

    return 0;
}

void *process_average(void *args)
{
    ARGS *arg = (ARGS*) args;

    int st = average(arg->filename, &(arg->local_min), &(arg->local_max));

    switch(st)
    {
        case -1:
            arg->status = io_status::error_open;
            break;

        case -2:
            arg->status = io_status::error_read;
            break;

        case 0:
            arg->status = io_status::success;
            break;
    }
    return 0;
}

int local_answer(char* filename, double average)
{
    int count = 0;

    FILE *f;

    f = fopen(filename, "r");
    if(!f)
    {
        cout << "Can't open the file " << filename << endl;
        return -1;
    }

    double current;

    while(fscanf(f, "%lf", &current) == 1)
    {
        if(current > average)
            count ++;
    }

    if(fscanf(f, "%lf", &current) != EOF)
    {
        cout << "Wrong input data in " << filename << endl;
        fclose(f);
        return -2;
    }

    fclose(f);

    return count;
}

void *process_local_answer(void* args)
{
    ARGS *arg = (ARGS*) args;

    int s = local_answer(arg->filename, arg->average);

    if(s == -1)
        arg->status = io_status::error_open;
    else if(s == -2)
        arg->status = io_status::error_read;
    else
    {
        arg->local_result = s;
        arg->status = io_status::success;
    }

    return 0;
}
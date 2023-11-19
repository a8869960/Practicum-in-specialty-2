//
// Created by varsem on 05.11.23.
//что с пустыми?
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
        return 1;
    }

    max = current;
    min = current;

    while(fscanf(f, "%lf", &current) == 1)
    {
        if(current < min)
            min = current;

        if(max < current)
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

void *process_main(void *args)
{
    ARGS *arg = (ARGS*) args;
    int k = arg->m, p = arg->p;
    
    int st = average(arg->filename, &(arg->res[k].local_min), &(arg->res[k].local_max));

    switch(st)
    {
        case -1:
            arg->res[k].status = io_status::error_open;
            break;

        case -2:
            arg->res[k].status = io_status::error_read;
            break;

        case 0:
            arg->res[k].status = io_status::success;
            break;

        case 1:
            arg->res[k].status = io_status::success;
            arg->is_empty = true;
            break;
    }
    reduce_sum(p);

    if(arg->is_empty)
        return 0;
    
    for(int i = 0; i < arg->p; i++)
            if(arg->res[i].status != io_status::success)
            {
                return 0;
            }
            
    double min = 0, max = 0;
    int count = 0;

    for(int i = -k; i < arg->p - k; i++)
    {
        if((arg + i)->is_empty)
            count++;
        else
        {
            min = arg->res[i + k].local_min;
            max = arg->res[i + k].local_max;
        }
    }

    if(count == arg->p)
        return 0;

    for(int i = 0; i < p; i++)
    {
        if(arg->res[i].local_max > max and (arg - k + i)->is_empty == false)
            max = arg->res[i].local_max;
        if(arg->res[i].local_min < min and (arg - k + i)->is_empty == false)
            min = arg->res[i].local_min;
    }

    double average = (min + max) / 2;
//    cout << min << " " << max << " " << average << endl;
    
    int s = local_answer(arg->filename, average);
    
    if(s == -1)
        arg->res[k].status = io_status::error_open;
    else if(s == -2)
        arg->res[k].status = io_status::error_read;
    else
    {
        arg->local_result = s;
        arg->res[k].status = io_status::success;
    }

    return 0;
}

void reduce_sum(int p, double* a, int n)
{
    static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
    static int t_in = 0;
    static int t_out = 0;
    static double *r = nullptr;
    int i;
    
    if(p <= 1)
        return;
    pthread_mutex_lock(&m);

    if(r == nullptr)
        r = a;
    else
        for(i = 0; i < n; i++) r[i] += a[i];

    t_in++;
    if(t_in >= p)
    {
        t_out = 0;
        pthread_cond_broadcast(&c_in);
    }
    else
        while(t_in < p)
            pthread_cond_wait(&c_in, &m);

    if(r != a)
        for(i = 0; i < n; i++) a[i] = r[i];

    t_out++;
    if(t_out >= p)
    {
        t_in = 0;
        r = nullptr;
        pthread_cond_broadcast(&c_out);
    }
    else
        while(t_out < p)
            pthread_cond_wait(&c_out, &m);

    pthread_mutex_unlock(&m);
}

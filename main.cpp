//
// Created by varsem on 05.11.23.
//
#include "functions.h"

int main(int ac, char *av[])
{
    try
    {
        int p = ac - 1, i;
        pthread_t *threads = new pthread_t[p];

        ARGS *args = new ARGS[p];

        //Заполняем аргументы
        for(i = 0; i < p; i++)
        {
            args[i].filename = av[i + 1];
            args[i].m = i;
            args[i].p = p;
        }

        for(i = 0; i < p; i++)
        {
            if(pthread_create(threads + i, 0, process_average, args + i))
            {
                cout << "Cannot create thread " << i << endl;
                delete[] threads;
                delete[] args;
                return -4;
            }
        }

        for(int i = 0; i < p; i++)
        {
            if(pthread_join(threads[i], 0))
                cout << "Cannot wait thread " << i << endl;
        }

        for(int i = 0; i < p; i++)
            if(args[i].status != io_status::success)
            {
                delete[] args;
                delete[] threads;
                return -5;
            }

        //Глобальный максимум и минимум + average
        double min = args[0].local_min, max = args[0].local_max;
        for(int i = 1; i < p; i++)
        {
            if(args[i].local_max > max)
                max = args[i].local_max;
            if(args[i].local_min < min)
                min = args[i].local_min;
        }

        double average_ = (min + max) / 2;
        for(int i = 0; i < p; i++)
            args[i].average = average_;

        for(i = 0; i < p; i++)
        {
            if(pthread_create(threads + i, 0, process_local_answer, args + i))
            {
                cout << "Cannot create thread " << i << endl;
                delete[] threads;
                delete[] args;
                return -4;
            }
        }

        for(int i = 0; i < p; i++)
        {
            if(pthread_join(threads[i], 0))
                cout << "Cannot wait thread " << i << endl;
        }

        int global_result = 0;
        for(int i = 0; i < p; i++)
        {
            if(args[i].status == io_status::success)
                global_result += args[i].local_result;
            else
            {
                delete[] threads;
                delete[] args;
                return -5;
            }
        }

        cout << "ANSWER: " << global_result << endl;

        delete[] args;
        delete[] threads;

        return 0;
    } catch (const bad_alloc& e)
    {
        cout << "Bad alloc" << endl;
        return -1;
    }
}
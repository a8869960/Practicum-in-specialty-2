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
        RES *ress = new RES[p];

        //Заполняем аргументы
        for(i = 0; i < p; i++)
        {
            args[i].filename = av[i + 1];
            args[i].m = i;
            args[i].p = p;
            args[i].res = ress;
        }

        // Создаем потоки
        for(i = 0; i < p; i++)
        {
            if(pthread_create(threads + i, 0, process_main, args + i))
            {
                cout << "Cannot create thread " << i << endl;
                delete[] threads;
                delete[] args;
                return -4;
            }
        }

        //Ждем потоки
        for(int i = 0; i < p; i++)
        {
            if(pthread_join(threads[i], 0))
                cout << "Cannot wait thread " << i << endl;
        }

        int global_result = 0;
        for(int i = 0; i < p; i++)
        {
            if(args[i].res[i].status == io_status::success)
                global_result += args[i].local_result;
            else
            {
                delete[] threads;
                delete[] args;
                delete[] ress;
                return -5;
            }
        }

        cout << "RESULT: " << global_result << endl;

        delete[] args;
        delete[] threads;
        delete[] ress;
        

        return 0;
    } catch (const bad_alloc& e)
    {
        cout << "Bad alloc" << endl;
        return -1;
    }
}

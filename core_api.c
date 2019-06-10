/* 046267 Computer Architecture - Spring 2019 - HW #4 */

#include "core_api.h"
#include "sim_api.h"

#include <stdio.h>

#define NUM_REGS 8

int num_insts = 0;
int num_cycles_blocked = 0;
int num_cycles_finegrained = 0;
tcontext* finegrained_thread_regs;
tcontext* blocked_thread_regs;

Status Core_blocked_Multithreading(){
	return Success;
}
Status init (tcontext* contexts, int* pcs, bool* thread_finished)
{
    int threads_num = Get_thread_number();
    int* pcs = (int*)malloc(threads_num * sizeof(int));
    if (pcs == NULL)
        return Failure;
    v = (tcontext*)malloc(threads_num * sizeof(tcontext));
    if (contexts == NULL)
        return Failure;
    thread_finished = (bool*)malloc(threads_num * sizeof(bool));
    for(int i = 0; i < threads_num; i++)
    {
        thread_finished[i] = false;
        pcs[i] = 0;
        for (int j = 0; j < NUM_REGS; j++)
            contexts[i][j] = 0;
    }
    return Success;
}

Status Core_fineGrained_Multithreading()
{
    int threads_num = Get_thread_number();
    int* pcs;
    bool* thread_finished;
    if (init(blocked_thread_regs, pcs, thread_finished) == Failure)
        return Failure;


    Instuction inst;
    int threads_done = 0;
    while(threads_done < threads_num)
    {









        threads_done = 0;
        for(int i = 0; i < threads_num; i++){
            threads_done += thread_finished[i];
        }
    }
	return Success;
}


double Core_finegrained_CPI(){
	return 0;
}
double Core_blocked_CPI(){
	return 0;
}

Status Core_blocked_context(tcontext* bcontext,int threadid){
	return Success;
}

Status Core_finegrained_context(tcontext* finegrained_context,int threadid){
	return Success;
}

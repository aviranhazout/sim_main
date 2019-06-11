/* 046267 Computer Architecture - Spring 2019 - HW #4 */

#include "core_api.h"
#include "sim_api.h"

#include <stdio.h>

#define NUM_REGS 8

int num_insts = 0;

int num_cycles_blocked = 0;
tcontext* blocked_thread_regs;
int* blocked_until; //should be always 0 except when we wait and then we should use update_latencies

int num_cycles_finegrained = 0;
tcontext* finegrained_thread_regs;
int* finegrained_until; //same as blocked_until


Status init (tcontext* contexts, int* pcs, bool* thread_finished, int* wait_to_memory)
{
    int threads_num = Get_thread_number();
    int* pcs = (int*)malloc(threads_num * sizeof(int));
    if (pcs == NULL)
        return Failure;
    v = (tcontext*)malloc(threads_num * sizeof(tcontext));
    if (contexts == NULL)
        return Failure;
    int* wait_to_memory = (int*)malloc(threads_num * sizeof(int));
    if (wait_to_memory == NULL)
        return Failure;
    thread_finished = (bool*)malloc(threads_num * sizeof(bool));
    for(int i = 0; i < threads_num; i++)
    {
        thread_finished[i] = false;
        wait_to_memory[i] = 0;
        pcs[i] = 0;
        for (int j = 0; j < NUM_REGS; j++)
            contexts[i][j] = 0;
    }
    return Success;
}

bool is_done(bool* thread_finished)
{
    for(int i = 0; i < Get_thread_number(); i++)
    {
        if (!thread_finished[i])
            return false;
    }
    return true;
}

int next_thread(int current_thread, bool* thread_finished)
{
    int next_thread = current_thread + 1;
    while (thread_finished[next_thread])
    {//next thread finished
        next_thread++;
        if (next_thread == Get_thread_number())
            next_thread = 0;
        if (next_thread == current_thread)
            return -1;  //shouldn't happen
    }
    return next_thread;
}

void update_latencies(int* wait_to_memory)
{
    for(int i = 0; i < Get_thread_number(); i++)
    {
        if (wait_to_memory[i] > 0)
            wait_to_memory[i]--;
    }
}




Status Core_blocked_Multithreading()
{
    int threads_num = Get_thread_number();
    if (threads_num == 0)
    {
        return Failure;
    }
    int* pcs;
    bool* thread_finished;
    if (init(blocked_thread_regs, pcs, thread_finished, blocked_until) == Failure)
        return Failure;

    int thread_idx = 0;
    Instuction inst;
    while(!is_done(thread_finished))
    {
        SIM_MemInstRead(pcs[thread_idx], &inst, thread_idx)
        switch (inst.opcode)
        {
            case CMD_NOP :
            case CMD_ADD :
            case CMD_SUB :
            case CMD_ADDI :
            case CMD_SUBI :
            case CMD_LOAD :
            case CMD_STORE :
            case CMD_HALT :
        }

    }
    return Success;
}

Status Core_fineGrained_Multithreading()
{
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

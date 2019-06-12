/* 046267 Computer Architecture - Spring 2019 - HW #4 */

#include "core_api.h"
#include "sim_api.h"

#include <stdio.h>

#define NUM_REGS 8

int num_insts = 0;

int num_cycles_blocked = 0;
tcontext* blocked_regs;

int num_cycles_finegrained = 0;
tcontext* finegrained_regs;


Status init (tcontext* contexts, int* pcs, bool* thread_finished, int* wait_to_memory, int* load_latency, int* store_latency)
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
    int latencies[2];
    Mem_latency(latencies);
    *load_latency = latencies[0];
    *store_latency = latencies[1];
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
            return current_thread;  //the last one
    }
    return next_thread;
}

//for blocked, for finegrained use next_thread
int context_switch (int current_thread, bool* thread_finished)
{
    int next = next_thread(current_thread, thread_finished);
    if (next != current_thread)
    {
        num_cycles_blocked += Get_switch_cycles();
        return next;
    }
    else    //no context switch
        return current_thread;
}

void update_latencies(int* wait_to_memory)
{
    for(int i = 0; i < Get_thread_number(); i++)
    {
        if (wait_to_memory[i] == 1)
            wait_to_memory[i] = -1;
        else if (wait_to_memory[i] > 0)
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
    int* wait_to_memory;
    int load_latency;
    int store_latency;
    if (init(blocked_regs, pcs, thread_finished, wait_to_memory, &load_latency, &store_latency) == Failure)
        return Failure;

    tcontext* regs;
    int thread_idx = 0;
    Instuction inst;
    while(!is_done(thread_finished))
    {
        SIM_MemInstRead(pcs[thread_idx], &inst, thread_idx)
        regs = blocked_regs[thread_idx];
        switch (inst.opcode)
        {
            case CMD_NOP :
                //TODO what???
                break;
            case CMD_ADD :
                regs[inst.dst_index] = regs[inst.src1_index] + regs[inst.src2_index_imm];
                pcs[thread_idx]++;
                break;
            case CMD_SUB :
                regs[inst.dst_index] = regs[inst.src1_index] - regs[inst.src2_index_imm];
                pcs[thread_idx]++;
                break;
            case CMD_ADDI :
                regs[inst.dst_index] = regs[inst.src1_index] + inst.src2_index_imm;
                pcs[thread_idx]++;
                break;
            case CMD_SUBI :
                regs[inst.dst_index] = regs[inst.src1_index] - inst.src2_index_imm;
                pcs[thread_idx]++;
                break;
            case CMD_LOAD :
                if (wait_to_memory == 0)
                {//start the instruction
                    int location = 0; //TODO calculate
                    SIM_MemDataRead(location, &(blocked_regs[thread_idx][inst.dst_index]));
                    wait_to_memory[thread_idx] = load_latency; //TODO maybe +1
                    context_switch(thread_idx, thread_finished);
                }
                else if (wait_to_memory == -1)
                {//end the instruction
                    pcs[thread_idx]++;
                }
                else
                {//still waiting
                    context_switch(thread_idx, thread_finished);
                }
                break;
            case CMD_STORE :
                //TODO
                break;
            case CMD_HALT :
                thread_finished[thread_idx] = true;
                context_switch(thread_idx, thread_finished);
                break;
        }
        update_latencies(wait_to_memory);
        num_cycles_blocked++;
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

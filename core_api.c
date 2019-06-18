/* 046267 Computer Architecture - Spring 2019 - HW #4 */

#include "core_api.h"
#include "sim_api.h"

#include <stdio.h>

#define NUM_REGS 8

enum multi_thread_type{FINE_GRAINED, BLOCKED};


//TODO ********FOR TESTING ***********
int* copyReg[3][NUM_REGS];

int num_insts_blocked = 0;
int num_cycles_blocked = 0;
tcontext* blocked_regs;

int num_insts_finegrained = 0;
int num_cycles_finegrained = 0;
tcontext* finegrained_regs;

//*********FOR TESTING*************
void copy_reg(tcontext copyReg[], int reg[], int Threads){
    for (int i = 0; i < NUM_REGS; ++i) {
        copyReg[Threads].reg[i] = reg[i];
    }
}




Status init (tcontext** contexts, int** pcs, bool** thread_finished, int** wait_to_memory, int* load_latency, int* store_latency)
{
    int threads_num = Get_thread_number();
    *pcs = (int*)malloc(threads_num * sizeof(int));
    if (*pcs == NULL)
        return Failure;
    *contexts = (tcontext*)malloc(threads_num * sizeof(tcontext));
    if (*contexts == NULL)
        return Failure;
    *wait_to_memory = (int*)malloc(threads_num * sizeof(int));
    if (*wait_to_memory == NULL)
        return Failure;
    *thread_finished = (bool*)malloc(threads_num * sizeof(bool));
    if (*thread_finished == NULL)
        return Failure;
    int latencies[2];
    Mem_latency(latencies);
    *load_latency = latencies[0];
    *store_latency = latencies[1];
    for(int i = 0; i < threads_num; i++)
    {
        (*thread_finished)[i] = false;
        (*wait_to_memory)[i] = 0;
        (*pcs)[i] = 0;
        for (int j = 0; j < NUM_REGS; j++)
            (*contexts)[i].reg[j] = 0;
    }
    return Success;
}

bool is_done(bool* thread_finished)
{
    for(int i = 0; i < Get_thread_number(); i++)
    {
        if (!(thread_finished[i]))
            return false;
    }
    return true;
}

int next_thread(int current_thread, bool* thread_finished)
{
    int next_thread = current_thread + 1;
    //Checks for last thread running
    if (next_thread >= Get_thread_number())
        next_thread -= Get_thread_number();
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

void debuggg (Instuction inst, int* pcs, int id, int curr_id)
{
    int threads_num = Get_thread_number();
    printf("\n\n\ninstruction for thread %d\n", curr_id);
    switch (inst.opcode)
    {
        case CMD_NOP :
            printf("NOP");
            break;
        case CMD_ADD :
            printf("ADD");
            break;
        case CMD_SUB :
            printf("SUB");
            break;
        case CMD_ADDI :
            printf("ADDI");
            break;
        case CMD_SUBI:
            printf("SUBI");
            break;
        case CMD_LOAD :
            printf("LOAD");
            break;
        case CMD_STORE :
            printf("STORE");
            break;
        case CMD_HALT :
            printf("HALT");
            break;
    }
    printf(" %d <- %d, %d  bool=%d\n",inst.dst_index, inst.src1_index, inst.src2_index_imm, inst.isSrc2Imm);

    for (int i = 0; i < threads_num; i++)
    {
        printf("\n---THREAD - %d---\n\n",i);

        for (int j = 0; j < NUM_REGS; j++)
        {
            printf("reg %d = %d, ", j, blocked_regs[i].reg[j]);
        }
        printf("\npc = %d\n",pcs[i]);

    }
    printf("\ninstructions = %d, cycles = %d, next thread = %d\n",num_insts_blocked, num_cycles_blocked, id);
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
    if (init(&blocked_regs, &pcs, &thread_finished, &wait_to_memory, &load_latency, &store_latency) == Failure)
        return Failure;
    int debuggg_thread;
    tcontext* regs;
    int thread_idx = 0;
    Instuction inst;
    while(!is_done(thread_finished))
    {
        //-------------------------
        debuggg_thread = thread_idx;
        if (num_cycles_blocked == 12)
            printf("test");
        //-------------------------
        SIM_MemInstRead(pcs[thread_idx], &inst, thread_idx);
        regs = &blocked_regs[thread_idx];
        switch (inst.opcode)
        {
            case CMD_NOP :
                //TODO what???
                break;
            case CMD_ADD :
                regs->reg[inst.dst_index] = regs->reg[inst.src1_index] + regs->reg[inst.src2_index_imm];
                pcs[thread_idx]++;
                num_insts_blocked++;
                break;
            case CMD_SUB :
                regs->reg[inst.dst_index] = regs->reg[inst.src1_index] - regs->reg[inst.src2_index_imm];
                pcs[thread_idx]++;
                num_insts_blocked++;
                break;
            case CMD_ADDI :
                regs->reg[inst.dst_index] = regs->reg[inst.src1_index] + inst.src2_index_imm;
                pcs[thread_idx]++;
                num_insts_blocked++;
                break;
            case CMD_SUBI :
                regs->reg[inst.dst_index] = regs->reg[inst.src1_index] - inst.src2_index_imm;
                pcs[thread_idx]++;
                num_insts_blocked++;
                break;
            case CMD_LOAD :
                if (wait_to_memory[thread_idx] == 0)
                {//start the instruction
                    int mem_address;
                    if (inst.isSrc2Imm)
                        mem_address = regs->reg[inst.src1_index] + inst.src2_index_imm;
                    else
                        mem_address = regs->reg[inst.src1_index] + regs->reg[inst.src2_index_imm];
                    SIM_MemDataRead(mem_address, &(regs->reg[inst.dst_index]));
                    wait_to_memory[thread_idx] = load_latency; //TODO maybe +1
                    thread_idx = context_switch(thread_idx, thread_finished);
                }
                else if (wait_to_memory[thread_idx] == -1)
                {//end the instruction
                    wait_to_memory[thread_idx] = 0;
                    pcs[thread_idx]++;
                    num_insts_blocked++;
                }
                else
                {//still waiting
                    thread_idx = context_switch(thread_idx, thread_finished);
                }
                break;
            case CMD_STORE :
                if (wait_to_memory[thread_idx] == 0)
                {//start the instruction
                    int mem_address;
                    if (inst.isSrc2Imm)
                        mem_address = regs->reg[inst.dst_index] + inst.src2_index_imm;
                    else
                        mem_address = regs->reg[inst.dst_index] + regs->reg[inst.src2_index_imm];
                    SIM_MemDataWrite(mem_address, regs->reg[inst.src1_index]);
                    wait_to_memory[thread_idx] = store_latency; //TODO maybe +1
                    thread_idx = context_switch(thread_idx, thread_finished);
                }
                else if (wait_to_memory[thread_idx] == -1)
                {//end the instruction
                    wait_to_memory[thread_idx] = 0;
                    pcs[thread_idx]++;
                    num_insts_blocked++;
                }
                else
                {//still waiting
                    thread_idx = context_switch(thread_idx, thread_finished);
                }
                break;
            case CMD_HALT :
                thread_finished[thread_idx] = true;
                pcs[thread_idx]++; //for debugg
                thread_idx = context_switch(thread_idx, thread_finished);
                num_insts_blocked++;
                break;
        }
        update_latencies(wait_to_memory);
        num_cycles_blocked++;
        //debuggg (inst, pcs, thread_idx, debuggg_thread);
    }
    return Success;
}

/*
Status _digest_opcode(enum multi_thread_type multi_type, int* thread_idx, int pcs[], bool thread_finished[],
        int *wait_to_memory, int load_latency, int store_latency){
    Instuction inst;

    SIM_MemInstRead(pcs[*thread_idx], &inst, *thread_idx);
    switch (inst.opcode) {
        case CMD_NOP :
            //bubble - i think just increase cycle count and if finegrained then switch thread
            if (multi_type == FINE_GRAINED)
                num_insts_finegrained++;
            else
                num_insts_blocked++;

            break;
        case CMD_ADD :
            finegrained_regs[*thread_idx].reg[inst.dst_index] = finegrained_regs[*thread_idx].reg[inst.src1_index]
                                                               + finegrained_regs[*thread_idx].reg[inst.src2_index_imm];

            if (multi_type == FINE_GRAINED)
                num_insts_finegrained++;
            else
                num_insts_blocked++;

            pcs[*thread_idx]++;
            break;
        case CMD_SUB :
            finegrained_regs[*thread_idx].reg[inst.dst_index] = finegrained_regs[*thread_idx].reg[inst.src1_index]
                                                               - finegrained_regs[*thread_idx].reg[inst.src2_index_imm];
            if (multi_type == FINE_GRAINED)
                num_insts_finegrained++;
            else
                num_insts_blocked++;

            pcs[*thread_idx]++;
            break;
        case CMD_ADDI :
            finegrained_regs[*thread_idx].reg[inst.dst_index] = finegrained_regs[*thread_idx].reg[inst.src1_index]
                                                               + inst.src2_index_imm;
            if (multi_type == FINE_GRAINED)
                num_insts_finegrained++;
            else
                num_insts_blocked++;

            pcs[*thread_idx]++;
            break;
        case CMD_SUBI :
            finegrained_regs[*thread_idx].reg[inst.dst_index] = finegrained_regs[*thread_idx].reg[inst.src1_index]
                                                               - inst.src2_index_imm;
            if (multi_type == FINE_GRAINED)
                num_insts_finegrained++;
            else
                num_insts_blocked++;

            pcs[*thread_idx]++;
            break;
        case CMD_LOAD :
            if (wait_to_memory[*thread_idx] == 0) {//start the instruction
                if (multi_type == FINE_GRAINED)
                    num_insts_finegrained++;
                else
                    num_insts_blocked++;

                int mem_address;
                if (inst.isSrc2Imm)
                    mem_address = finegrained_regs[*thread_idx].reg[inst.src1_index] + inst.src2_index_imm;
                else
                    mem_address = finegrained_regs[*thread_idx].reg[inst.src1_index] +
                                  finegrained_regs[*thread_idx].reg[inst.src2_index_imm];
                SIM_MemDataRead(mem_address, &(finegrained_regs[*thread_idx].reg[inst.dst_index]));
                wait_to_memory[*thread_idx] = load_latency; //TODO maybe +1
                if(multi_type == BLOCKED)
                    context_switch(*thread_idx, thread_finished);
            } else if (wait_to_memory[*thread_idx] == -1) {//end the instruction
                wait_to_memory[*thread_idx] = 0;
                pcs[*thread_idx]++;
            }
            //still waiting or completed
            break;
        case CMD_STORE :
            if (wait_to_memory[*thread_idx] == 0) {//start the instruction
                if (multi_type == FINE_GRAINED)
                    num_insts_finegrained++;
                else
                    num_insts_blocked++;

                int mem_address;
                if (inst.isSrc2Imm)
                    mem_address = finegrained_regs[*thread_idx].reg[inst.dst_index] + inst.src2_index_imm;
                else
                    mem_address = finegrained_regs[*thread_idx].reg[inst.dst_index] +
                                  finegrained_regs[*thread_idx].reg[inst.src2_index_imm];
                SIM_MemDataWrite(mem_address, finegrained_regs[*thread_idx].reg[inst.src1_index]);
                wait_to_memory[*thread_idx] = store_latency; //TODO maybe +1
                if(multi_type == BLOCKED)
                    context_switch(*thread_idx, thread_finished);
            } else if (wait_to_memory[*thread_idx] == -1) {//end the instruction
                wait_to_memory[*thread_idx] = 0;
                pcs[*thread_idx]++;
            }

            break;
        case CMD_HALT :
            thread_finished[*thread_idx] = true;
            if(multi_type == BLOCKED)
                context_switch(*thread_idx, thread_finished);

            if (multi_type == FINE_GRAINED)
                num_insts_finegrained++;
            else
                num_insts_blocked++;

            break;
    }
    // continue to next thread after command
    if(multi_type == FINE_GRAINED)
        *thread_idx = next_thread(*thread_idx, thread_finished);
    update_latencies(wait_to_memory);
    return Success;
}
*/

Status Core_fineGrained_Multithreading() {
    int threads_num = Get_thread_number();
    if (threads_num == 0) {
        return Failure;
    }
    int *pcs;
    bool *thread_finished;
    int *wait_to_memory;
    int load_latency;
    int store_latency;




    //init
    pcs = (int*)malloc(threads_num * sizeof(int));
    if (pcs == NULL)
        return Failure;
    finegrained_regs = (tcontext*)malloc(threads_num * sizeof(tcontext));
    if (finegrained_regs == NULL)
        return Failure;
    wait_to_memory = (int*)malloc(threads_num * sizeof(int));
    if (wait_to_memory == NULL)
        return Failure;
    thread_finished = (bool*)malloc(threads_num * sizeof(bool));
    int latencies[2];
    Mem_latency(latencies);
    load_latency = latencies[0];
    store_latency = latencies[1];
    for(int i = 0; i < threads_num; i++)
    {
        thread_finished[i] = false;
        wait_to_memory[i] = 0;
        pcs[i] = 0;
        for (int j = 0; j < NUM_REGS; j++)
            finegrained_regs[i].reg[j] = 0;
    }

    //TODO *********FOR TESTING ********
    for (int k = 0; k < 3; ++k) {
        for (int i = 0; i < NUM_REGS; ++i) {
            copyReg[k][i] = &finegrained_regs[k].reg[i];
        }
    }
    //if (init(finegrained_regs, pcs, thread_finished, wait_to_memory, &load_latency, &store_latency) == Failure)
     //   return Failure;
    int thread_idx = 0;
    while (!is_done(thread_finished)) {
        Instuction inst;

        SIM_MemInstRead(pcs[thread_idx], &inst, thread_idx);
        num_cycles_finegrained++;
        switch (inst.opcode) {
            case CMD_NOP :
                //bubble - i think just increase cycle count and if finegrained then switch thread
                break;

            case CMD_ADD :
                finegrained_regs[thread_idx].reg[inst.dst_index] = finegrained_regs[thread_idx].reg[inst.src1_index]
                                                                + finegrained_regs[thread_idx].reg[inst.src2_index_imm];
                pcs[thread_idx]++;
                break;

            case CMD_SUB :
                finegrained_regs[thread_idx].reg[inst.dst_index] = finegrained_regs[thread_idx].reg[inst.src1_index]
                                                                - finegrained_regs[thread_idx].reg[inst.src2_index_imm];
                pcs[thread_idx]++;
                break;

            case CMD_ADDI :
                finegrained_regs[thread_idx].reg[inst.dst_index] = finegrained_regs[thread_idx].reg[inst.src1_index]
                                                                    + inst.src2_index_imm;
                pcs[thread_idx]++;
                break;

            case CMD_SUBI :
                finegrained_regs[thread_idx].reg[inst.dst_index] = finegrained_regs[thread_idx].reg[inst.src1_index]
                                                                    - inst.src2_index_imm;
                pcs[thread_idx]++;
                break;

            case CMD_LOAD :
                if (wait_to_memory[thread_idx] == 0)
                {//start the instruction
                    num_insts_finegrained--;
                    int mem_address;
                    if (inst.isSrc2Imm)
                        mem_address = finegrained_regs[thread_idx].reg[inst.src1_index] + inst.src2_index_imm;
                    else
                        mem_address = finegrained_regs[thread_idx].reg[inst.src1_index] + finegrained_regs[thread_idx].reg[inst.src2_index_imm];
                    SIM_MemDataRead(mem_address, &(finegrained_regs[thread_idx].reg[inst.dst_index]));
                    wait_to_memory[thread_idx] = load_latency; //TODO maybe +1
                }
                else if (wait_to_memory[thread_idx] == -1)
                {//end the instruction
                    wait_to_memory[thread_idx] = 0;
                    pcs[thread_idx]++;
                    num_insts_blocked++;
                }
                else
                {//still waiting
                    num_insts_finegrained--;
                }
                break;
            case CMD_STORE :
                if (wait_to_memory[thread_idx] == 0)
                {//start the instruction
                    num_insts_finegrained--;
                    int mem_address;
                    if (inst.isSrc2Imm)
                        mem_address = finegrained_regs[thread_idx].reg[inst.dst_index] + inst.src2_index_imm;
                    else
                        mem_address = finegrained_regs[thread_idx].reg[inst.dst_index] + finegrained_regs[thread_idx].reg[inst.src2_index_imm];
                    SIM_MemDataWrite(mem_address, finegrained_regs[thread_idx].reg[inst.src1_index]);
                    wait_to_memory[thread_idx] = store_latency; //TODO maybe +1
                }
                else if (wait_to_memory[thread_idx] == -1)
                {//end the instruction
                    wait_to_memory[thread_idx] = 0;
                    pcs[thread_idx]++;
                    num_insts_blocked++;
                }
                else
                {//still waiting
                    num_insts_finegrained--;
                }
                break;

            case CMD_HALT :
                thread_finished[thread_idx] = true;
                break;
        }
        // continue to next thread after command
        num_insts_finegrained++;
        thread_idx = next_thread(thread_idx, thread_finished);
        update_latencies(wait_to_memory);
    }
    return Success;
}

double Core_finegrained_CPI(){
    if (num_cycles_finegrained == 0)
        return 0;

    return ((double)num_cycles_finegrained / (double)num_insts_finegrained);
}
double Core_blocked_CPI(){
    if (num_cycles_blocked == 0)
	    return 0;

    return ((double)num_cycles_blocked / (double)num_insts_blocked);
}

Status Core_blocked_context(tcontext* bcontext,int threadid)
{
    for (int i = 0; i < NUM_REGS; i++)
        bcontext[threadid].reg[i] = blocked_regs[threadid].reg[i];
	return Success;
}

Status Core_finegrained_context(tcontext* finegrained_context,int threadid){
    for (int i = 0; i < NUM_REGS; i++)
        finegrained_context[threadid].reg[i] = finegrained_regs[threadid].reg[i];
	return Success;
}


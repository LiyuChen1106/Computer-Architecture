#include <limits.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "loader.h"
#include "syscall.h"
#include "dlite.h"
#include "options.h"
#include "stats.h"
#include "sim.h"
#include "decode.def"

#include "instr.h"

/* PARAMETERS OF THE TOMASULO'S ALGORITHM */

#define INSTR_QUEUE_SIZE         16

#define RESERV_INT_SIZE    5
#define RESERV_FP_SIZE     3
#define FU_INT_SIZE        3
#define FU_FP_SIZE         1

#define FU_INT_LATENCY     5
#define FU_FP_LATENCY      7

/* IDENTIFYING INSTRUCTIONS */

//unconditional branch, jump or call
#define IS_UNCOND_CTRL(op) (MD_OP_FLAGS(op) & F_CALL || \
                         MD_OP_FLAGS(op) & F_UNCOND)

//conditional branch instruction
#define IS_COND_CTRL(op) (MD_OP_FLAGS(op) & F_COND)

//floating-point computation
#define IS_FCOMP(op) (MD_OP_FLAGS(op) & F_FCOMP)

//integer computation
#define IS_ICOMP(op) (MD_OP_FLAGS(op) & F_ICOMP)

//load instruction
#define IS_LOAD(op)  (MD_OP_FLAGS(op) & F_LOAD)

//store instruction
#define IS_STORE(op) (MD_OP_FLAGS(op) & F_STORE)

//trap instruction
#define IS_TRAP(op) (MD_OP_FLAGS(op) & F_TRAP) 

#define USES_INT_FU(op) (IS_ICOMP(op) || IS_LOAD(op) || IS_STORE(op))
#define USES_FP_FU(op) (IS_FCOMP(op))

#define WRITES_CDB(op) (IS_ICOMP(op) || IS_LOAD(op) || IS_FCOMP(op))

/* FOR DEBUGGING */

//prints info about an instruction
#define PRINT_INST(out,instr,str,cycle) \
  myfprintf(out, "%d: %s", cycle, str);  \
  md_print_insn(instr->inst, instr->pc, out); \
  myfprintf(stdout, "(%d)\n",instr->index);

#define PRINT_REG(out,reg,str,instr) \
  myfprintf(out, "reg#%d %s ", reg, str); \
  md_print_insn(instr->inst, instr->pc, out); \
  myfprintf(stdout, "(%d)\n",instr->index);

/* VARIABLES */

//instruction queue for tomasulo
static instruction_t* instr_queue[INSTR_QUEUE_SIZE];
//number of instructions in the instruction queue
static int instr_queue_size = 0;

//reservation stations (each reservation station entry contains a pointer to an instruction)
static instruction_t* reservINT[RESERV_INT_SIZE];
static instruction_t* reservFP[RESERV_FP_SIZE];

//functional units
static instruction_t* fuINT[FU_INT_SIZE];
static instruction_t* fuFP[FU_FP_SIZE];

//common data bus
static instruction_t* commonDataBus = NULL;

//The map table keeps track of which instruction produces the value for each register
static instruction_t* map_table[MD_TOTAL_REGS];

//the index of the last instruction fetched
static int fetch_index = 0;



/*ECE552 Assignment 3 -BEGIN CODE*/
static int instr_queue_position = 0;
static int instr_queue_issue = 0;
/*ECE552 Assignment 3 -END CODE*/

/* FUNCTIONAL UNITS */


/* RESERVATION STATIONS */

/* 
 * Description: 
 * 	Checks if simulation is done by finishing the very last instruction
 *      Remember that simulation is done only if the entire pipeline is empty
 * Inputs:
 * 	sim_insn: the total number of instructions simulated
 * Returns:
 * 	True: if simulation is finished
 */
static bool is_simulation_done(counter_t sim_insn) {


    /* ECE552: YOUR CODE GOES HERE */
    if (fetch_index >= sim_insn && instr_queue_size == 0 && commonDataBus == NULL && reservINT[0]==NULL && reservINT[1]==NULL && reservINT[2]==NULL &&reservINT[3]==NULL &&reservINT[4]==NULL&& reservINT[5]==NULL&& reservFP[0]==NULL &&reservFP[1]==NULL&&reservFP[2]==NULL && fuINT[0]==NULL&&fuINT[1]==NULL&&fuINT[2]==NULL&&fuFP[0]==NULL){
       
        return true;}
    else
        return false;

    return true; //ECE552: you can change this as needed; we've added this so the code provided to you compiles
}

/* 
 * Description: 
 * 	Retires the instruction from writing to the Common Data Bus
 * Inputs:
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void CDB_To_retire(int current_cycle) {

    /* ECE552: YOUR CODE GOES HERE */
    if (commonDataBus != NULL) {
        
        //update q to v
        for (int i = 0; i < RESERV_INT_SIZE; i++) {
            if (reservINT[i] != NULL) {
                for (int j = 0; j < 3; j++) {
                    if (reservINT[i]->Q[j] == commonDataBus) {
                        reservINT[i]->Q[j] = NULL;
                    }
                }
            }
        }

        for (int i = 0; i < RESERV_FP_SIZE; i++) {
            if (reservFP[i] != NULL) {
                for (int j = 0; j < 3; j++) {
                    if (reservFP[i]->Q[j] == commonDataBus) {
                        reservFP[i]->Q[j] = NULL;
                    }
                }
            }
        }
        

    }
    commonDataBus = NULL;
}

/* 
 * Description: 
 * 	Moves an instruction from the execution stage to common data bus (if possible)
 * Inputs:
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void execute_To_CDB(int current_cycle) {

    /* ECE552: YOUR CODE GOES HERE */
    instruction_t * CDBinst = NULL;
    for (int i = 0; i < FU_INT_SIZE; i++) {
        if (fuINT[i] != NULL) {
            if (fuINT[i]->tom_execute_cycle + FU_INT_LATENCY <= current_cycle) {
                if(IS_STORE(fuINT[i]->op)){
                    for(int j=0;j<RESERV_INT_SIZE;j++){
                        if(reservINT[j]==fuINT[i])
                            reservINT[j]=NULL;
                    }
                    fuINT[i]=NULL;
                    continue;}
                
                if (CDBinst == NULL) {
                    CDBinst = fuINT[i];
                } else if (CDBinst->index > fuINT[i]->index) {
                    CDBinst = fuINT[i];
                }
            }
        }
    }

    for (int i = 0; i < FU_FP_SIZE; i++) {
        if (fuFP[i] != NULL) {
            if (fuFP[i]->tom_execute_cycle + FU_FP_LATENCY <= current_cycle) {
                if (CDBinst == NULL) {
                    CDBinst = fuFP[i];
                } else if (CDBinst->index > fuFP[i]->index) {
                    CDBinst = fuFP[i];
                }
            }
        }
    }

    if (CDBinst != NULL && commonDataBus == NULL) {
        commonDataBus = CDBinst;
        commonDataBus->tom_cdb_cycle = current_cycle;
        
        //flush function units and reservation station first
        for (int i = 0; i < FU_INT_SIZE; i++) {
            if (fuINT[i] == commonDataBus)
                fuINT[i] = NULL;
        }
        for (int i = 0; i < FU_FP_SIZE; i++) {
            if (fuFP[i] == commonDataBus)
                fuFP[i] = NULL;
        }
        for (int i = 0; i < RESERV_FP_SIZE; i++) {
            if (reservFP[i] == commonDataBus)
                reservFP[i] = NULL;
        }
        for (int i = 0; i < RESERV_INT_SIZE; i++) {
            if (reservINT[i] == commonDataBus)
                reservINT[i] = NULL;
        }
        for (int i = 0; i < MD_TOTAL_REGS; i++) {
            if (map_table[i] == commonDataBus) {
                map_table[i] = NULL;
            }
        }
    }
}

/* 
 * Description: 
 * 	Moves instruction(s) from the issue to the execute stage (if possible). We prioritize old instructions
 *      (in program order) over new ones, if they both contend for the same functional unit.
 *      All RAW dependences need to have been resolved with stalls before an instruction enters execute.
 * Inputs:
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void issue_To_execute(int current_cycle) {

    /* ECE552: YOUR CODE GOES HERE */

    //check whether all rp status are ready or not
    //find the min seq available instr
    //check cdb to update the map table ????

    int intstatus[RESERV_INT_SIZE];
    int fpstatus[RESERV_FP_SIZE];
    for (int i = 0; i < RESERV_INT_SIZE; i++) {
        intstatus[i] = 0;
    }
    for (int i = 0; i < RESERV_FP_SIZE; i++) {
        fpstatus[i] = 0;
    }

    for (int i = 0; i < RESERV_INT_SIZE; i++) {
        if (reservINT[i] != NULL) {
            if (reservINT[i]->Q[0] == NULL && reservINT[i]->Q[1] == NULL && reservINT[i]->Q[2] == NULL) {
                intstatus[i] = 1;
            }
        }
    }
    for (int i = 0; i < RESERV_FP_SIZE; i++) {
        if (reservFP[i] != NULL) {
            if (reservFP[i]->Q[0] == NULL && reservFP[i]->Q[1] == NULL && reservFP[i]->Q[2] == NULL) {
                fpstatus[i] = 1;
            }
        }
    }

    //check for fu int units

    for (int i = 0; i < FU_INT_SIZE; i++) {
        int target = 0;
        if (fuINT[i] == NULL) {
            uint check = 300000000;
            for (int j = 0; j < RESERV_INT_SIZE; j++) {
                if (intstatus[j] == 1 ) {
                    if (reservINT[j]->index < check && reservINT[j]->tom_execute_cycle==0) {
                        check = reservINT[j]->index;
                        target = j;
                    }
                }
            }
            
            if (check != 300000000) {
                fuINT[i] = reservINT[target];
                intstatus[target] = 0;
                fuINT[i]->tom_execute_cycle = current_cycle;

            }
        }
    }

    //check for fu fp units
    for (int i = 0; i < FU_FP_SIZE; i++) {
        int target = 0;
        if (fuFP[i] == NULL) {
            uint check = 300000000;
            for (int j = 0; j < RESERV_FP_SIZE; j++) {
                if (fpstatus[j] == 1 ) {
                    if (reservFP[j]->index < check && reservFP[j]->tom_execute_cycle==0) {
                        check = reservFP[j]->index;
                        target = j;
                    }
                }
            }
            if (check != 300000000) {
                fuFP[i] = reservFP[target];
                fpstatus[target] = 0;
                fuFP[i]->tom_execute_cycle = current_cycle;

            }
        }
    }
}

/* 
 * Description: 
 * 	Moves instruction(s) from the dispatch stage to the issue stage
 * Inputs:
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void dispatch_To_issue(int current_cycle) {
    instruction_t* instr;
    if (instr_queue_size > 0) {
        instr = instr_queue[instr_queue_issue];

         

    } else
        return;

    //check branch op
    if (IS_COND_CTRL(instr->op) || IS_UNCOND_CTRL(instr->op)) {
        instr_queue[instr_queue_issue] = NULL;
        instr_queue_issue = (instr_queue_issue + 1) % INSTR_QUEUE_SIZE;
        instr_queue_size--;

    } else {
        //check are there any reseveration integer or float
        bool check = FALSE;
        if (USES_INT_FU(instr->op)) {
            int avail = 0;
            for (avail = 0; avail < RESERV_INT_SIZE; avail++) {
                if (reservINT[avail] == NULL)
                    break;
            }
            
            if (avail < RESERV_INT_SIZE) {
                
                instr ->tom_issue_cycle = current_cycle;

                reservINT[avail] = instr;
                instr_queue[instr_queue_issue] = NULL;
                if(instr_queue_position != instr_queue_issue)
                	instr_queue_issue = (instr_queue_issue + 1) % INSTR_QUEUE_SIZE;
                instr_queue_size--;

                check = TRUE;
            }


        } else if (USES_FP_FU(instr->op)) {
            int avail = 0;
            for (avail = 0; avail < RESERV_FP_SIZE; avail++) {
                if (reservFP[avail] == NULL)
                    break;
            }
            if (avail < RESERV_FP_SIZE) {
                instr ->tom_issue_cycle = current_cycle;

                reservFP[avail] = instr;
                instr_queue[instr_queue_issue] = NULL;
                if(instr_queue_position != instr_queue_issue)
                	instr_queue_issue = (instr_queue_issue + 1) % INSTR_QUEUE_SIZE;
                instr_queue_size--;

                check = TRUE;
            }

        }
        if (check) {

            for (int i = 0; i < 3; i++) {
                if (instr->r_in[i] != DNA && map_table[instr->r_in[i]] != NULL) {
                    instr->Q[i] = map_table[instr->r_in[i]];
                }
            }

            for (int i = 0; i < 2; i++) {
                if (instr->r_out[i] != DNA) {
                    map_table[instr->r_out[i]] = instr;
                }
            }
        }
    }
    /* ECE552: YOUR CODE GOES HERE */


}

/* 
 * Description: 
 * 	Grabs an instruction from the instruction trace (if possible)
 * Inputs:
 *      trace: instruction trace with all the instructions executed
 * Returns:
 * 	None
 */
void fetch(instruction_trace_t* trace) {

    if (instr_queue_size < INSTR_QUEUE_SIZE && fetch_index < sim_num_insn) {
        while ( IS_TRAP(get_instr(trace, fetch_index+1)->op)) {
            fetch_index++;
        }
    }
   
    if (instr_queue_size < INSTR_QUEUE_SIZE && fetch_index < sim_num_insn) {
        instruction_t* instruction = get_instr(trace, fetch_index+1);
        fetch_index=instruction->index;

		if (instr_queue_size != 0)
			instr_queue_position = (instr_queue_position + 1) % INSTR_QUEUE_SIZE;
			
        instr_queue[instr_queue_position] = instruction;
        
        instr_queue_size++;

    }
    /* ECE552: YOUR CODE GOES HERE */
}

/* 
 * Description: 
 * 	Calls fetch and dispatches an instruction at the same cycle (if possible)
 * Inputs:
 *      trace: instruction trace with all the instructions executed
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void fetch_To_dispatch(instruction_trace_t* trace, int current_cycle) {

    fetch(trace);

    instruction_t* instr = instr_queue[instr_queue_position];
    if (instr != NULL && instr->tom_dispatch_cycle == 0) {
        instr->tom_dispatch_cycle = current_cycle;

    }

    /* ECE552: YOUR CODE GOES HERE */
}

/* 
 * Description: 
 * 	Performs a cycle-by-cycle simulation of the 4-stage pipeline
 * Inputs:
 *      trace: instruction trace with all the instructions executed
 * Returns:
 * 	The total number of cycles it takes to execute the instructions.
 * Extra Notes:
 * 	sim_num_insn: the number of instructions in the trace
 */
counter_t runTomasulo(instruction_trace_t* trace) {
    //initialize instruction queue
    int i;
    for (i = 0; i < INSTR_QUEUE_SIZE; i++) {
        instr_queue[i] = NULL;
    }

    //initialize reservation stations
    for (i = 0; i < RESERV_INT_SIZE; i++) {
        reservINT[i] = NULL;
    }

    for (i = 0; i < RESERV_FP_SIZE; i++) {
        reservFP[i] = NULL;
    }

    //initialize functional units
    for (i = 0; i < FU_INT_SIZE; i++) {
        fuINT[i] = NULL;
    }

    for (i = 0; i < FU_FP_SIZE; i++) {
        fuFP[i] = NULL;
    }

    //initialize map_table to no producers
    int reg;
    for (reg = 0; reg < MD_TOTAL_REGS; reg++) {
        map_table[reg] = NULL;
    }

    int cycle = 1;
    while (true) {

        /* ECE552: YOUR CODE GOES HERE */
        CDB_To_retire(cycle);
        execute_To_CDB(cycle);
        issue_To_execute(cycle);
        dispatch_To_issue(cycle);
        fetch_To_dispatch(trace,cycle);

		print_all_instr(trace, sim_num_insn);
        

        if (is_simulation_done(sim_num_insn))
            break;
        cycle++;
    }

    return cycle;
}

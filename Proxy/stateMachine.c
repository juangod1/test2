//
// Created by juangod on 13/10/18.
//
#include <malloc.h>
#include "include/stateMachine.h"
#include "include/stateSelector.h"
#include "include/state.h"

state new_state(state_code id, execution_state (*on_arrive)(), execution_state (*on_resume)(), state_code (*on_leave)()){
    state new = malloc(sizeof(state));
    new->id = id;
    new->error = 0;
    new->on_arrive = on_arrive;
    new->on_resume = on_resume;
    new->on_leave = on_leave;
    new->exec_state = NOT_WAITING;
    new->internal_state=0;
    return new;
}

void free_state(state st){
    free(st);
}

void free_machine(state_machine * machine){
    int i;
    free_list(machine->states);
    free(machine);
}

state_machine * new_machine(){
    state_machine * new = malloc(sizeof(state_machine));
    return new;
}

void run_state(state_machine * sm)
{
    state previous = get_state_by_fd(sm->next_state,sm);

    if(previous->error){
        // error state has fd -1
        state err = get(sm->states,-1);
        err->on_arrive();
        err->on_resume();
    }

    file_descriptor next = select_state();
    state st = get_state_by_fd(next,sm);
    printf("State %d was chosen.",st->id);

    switch(st->exec_state)
    {
        case NOT_WAITING:
            switch(st->on_arrive()){
                case NOT_WAITING:
                    st->on_leave();
                    break;
                case WAITING_READ:
                    set_waiting_read(st->wait_read_fd, st);
                    break;
                case WAITING_WRITE:
                    set_waiting_write(st->wait_write_fd, st);
                    break;
            }
            break;
        case WAITING_READ:
            switch(st->on_resume()){
                case NOT_WAITING:
                    st->on_leave();
                    break;
                case WAITING_READ:
                    set_waiting_read(st->wait_read_fd, st);
                    break;
                case WAITING_WRITE:
                    set_waiting_write(st->wait_write_fd, st);
                    break;
            }
            break;
        case WAITING_WRITE:
            switch(st->on_resume()){
                case NOT_WAITING:
                    st->on_leave();
                    break;
                case WAITING_READ:
                    set_waiting_read(st->wait_read_fd, st);
                    break;
                case WAITING_WRITE:
                    set_waiting_write(st->wait_write_fd, st);
                    break;
            }
            break;
        default:
            break;
    }
}

state get_state_by_code(state_code code, state_machine * sm){
    int i;
    for(i=0;i<sm->states_amount;i++){
        if(sm->states[i]->id==code)
            return (sm->states[i]);
    }
    return NULL;
}
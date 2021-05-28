#ifndef __INSTRUCTION_ENVELOPE_H__
#define __INSTRUCTION_ENVELOPE_H__

#include "activebsp_instructions.h"

#include <string>
#include <vector>

#include "vector_distribution.h"

namespace activebsp
{

#define ENVELOPE_OFFSET(ENVELOPE,OFFSET_NAME) &ENVELOPE->data[ENVELOPE->OFFSET_NAME]

typedef struct
{
    instruction_t instruction;
    char data[];

} instruction_envelope_t;

typedef struct
{
    int name_offset;
    int pids_offset;
    int pids_length;
    char data[];

} create_actor_envelope_t;

typedef struct
{
    char ask_answer;

} stop_actor_envelope_t;

typedef struct
{
    int name_offset;
    int future_key;
    int args_offset;
    int args_size;
    char data[];

} call_actor_envelope_t;

typedef struct
{
    int future_key;
    int data_offset;
    int data_size;
    char data[];

} register_future_data_envelope_t;

typedef struct
{
    int resid;
    size_t data_size;
    bool is_addr;
    char data[];

} store_result_envelope_t;

typedef struct
{
    int future_key;

} get_future_data_envelope_t;



typedef struct
{
    int future_key;

} release_future_envelope_t;

typedef struct
{
    int resid;

} release_result_part_envelope_t;

typedef enum  { DISTR_BLOCK = 1 } distr_type_t;

typedef struct
{
    distr_type_t distr;
    char data[];

} distribute_vector_envelope_t;

typedef struct
{
    size_t n;
    size_t elem_size;
} block_distr_args_t;

} // namespace activebsp

#endif // __INSTRUCTION_ENVELOPE_H__

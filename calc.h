#ifndef CALC_H
#define CALC_H

#define OP_NUMBER   9
#define MAX_ERROR_LEN 512

enum ERROR {
    OK,
    ERR_UNKNOWN,
    ERR_BRACKET_OPEN,
    ERR_BRACKET_CLOSE,
    ERR_TERM_EMPTY,
    ERR_SHORT_TERM,
    ERR_UNKNOWN_VARIABLE
};

uint16_t calc_term(char *term);
uint16_t term_disassemble(char *term, uint16_t p, int64_t *result);
bool find_operation(char *term, char *Op, uint16_t *pPos);

#endif
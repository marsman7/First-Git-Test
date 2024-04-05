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

uint16_t calc_term(const char *term);
int64_t get_calc_result();

#endif
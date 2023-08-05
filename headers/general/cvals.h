#pragma once

char *_exclamationmark = (char*)"!";
char *_at = (char*)"@";
char *_dollar = (char*)"$";
char *_percent = (char*)"%";
char *_carat = (char*)"^";
char *_ampersand = (char*)"&";
char *_vertline = (char*)"|";
char *_star = (char*)"*";
char *_minus = (char*)"-";
char *_plus = (char*)"+";
char *_lesser = (char*)"<";
char *_greater = (char*)">";
char *_dot = (char*)".";
char *_fslash = (char*)"/";
char *_bslash = (char*)"\\";
char *_comment = (char*)"//";
char *_comma = (char*)",";
char *_question = (char*)"?";
char *_lrb = (char*)"(";
char *_rrb = (char*)")";
char *_lsb = (char*)"[";
char *_rsb = (char*)"]";
char *_lcb = (char*)"{";
char *_rcb = (char*)"}";
char *_semicolon = (char*)";";
char *_quote = (char*)"\"";
char *_singlequote = (char*)"'";
char *_colon = (char*)":";
char *_equal = (char*)"=";
char *_tilde = (char*)"~";
char *_newline = (char*)"\n";
char *_space = (char*)" ";
char *_tab = (char*)"\t";


char *_fatarrow = (char*)"=>";
char *_pipe = (char*)"|>";
char *_arrow = (char*)"->";
char *_gtet = (char*)">=";
char *_ltet = (char*)"<=";
char *_equalto = (char*)"==";
char *_pluset = (char*)"+=";
char *_minuset = (char*)"-=";
char *_staret = (char*)"*=";
char *_slashet = (char*)"/=";
char *_and = (char*)"&&";
char *_or = (char*)"||";
char *_net = (char*)"!=";
char *_if = (char*)"if";



char *_fun = (char*)"fun";
char *_return = (char*)"return";
char *_clone = (char*)"clone";
char *_elif = (char*)"elif";
char *_else = (char*)"else";
char *_while = (char*)"while";
char *_for = (char*)"for";
char *_let = (char*)"let";
char *_const = (char*)"const";
char *_ptr = (char*)"ptr";
char *_break = (char*)"break";

char *_stl = (char*)
"#define ui8 unsigned char\n"
"#define ui16 unsigned short\n"
"#define ui32 unsigned int\n"
"#define ui64 unsigned long int\n"
"#define ui128 unsigned long long int\n"
"#define i8 signed char\n"
"#define i16 signed short\n"
"#define i32 signed int\n"
"#define i64 signed long int\n"
"#define i128 signed long long int\n"
"#define f32 float\n"
"#define f64 double\n"
"#define f80 long double\n"
"#define str char*\n"
"#define size size_t\n"
"#define print printf\n"
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"#include <stdbool.h>\n\n\n";





//primitive types
#define __define_type(type) char*_##type = (char*)#type;
__define_type(true)
__define_type(false)
__define_type(bool)
__define_type(void)

__define_type(str)

__define_type(ui8)
__define_type(ui16)
__define_type(ui32)
__define_type(ui64)
__define_type(ui128)

__define_type(i8)
__define_type(i16)
__define_type(i32)
__define_type(i64)
__define_type(i128)

__define_type(f32)
__define_type(f64)
__define_type(f80)

const size_t _primitive_sizes[] = {8, 16, 32, 64, 80, 128};

#define __size_def(type, index) const size_t *_##type##_size = _primitive_sizes + index;
__size_def(bool, 0)

__size_def(ui8, 0)
__size_def(ui16, 1)
__size_def(ui32, 2)
__size_def(ui64, 3)
__size_def(ui128, 5)

__size_def(i8, 0)
__size_def(i16, 1)
__size_def(i32, 2)
__size_def(i64, 3)
__size_def(i128, 5)

__size_def(f32, 2)
__size_def(f64, 3)
__size_def(f80, 4)
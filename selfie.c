/*
Copyright (c) 2015-2018, the Selfie Project authors. All rights reserved.
Please see the AUTHORS file for details. Use of this source code is
governed by a BSD license that can be found in the LICENSE file.

Selfie is a project of the Computational Systems Group at the
Department of Computer Sciences of the University of Salzburg
in Austria. For further information and code please refer to:

http://selfie.cs.uni-salzburg.at

The Selfie Project provides an educational platform for teaching
undergraduate and graduate students the design and implementation
of programming languages and runtime systems. The focus is on the
construction of compilers, libraries, operating systems, and even
virtual machine monitors. The common theme is to identify and
resolve self-reference in systems code which is seen as the key
challenge when teaching systems engineering, hence the name.

Selfie is a self-contained 32-bit, 10-KLOC C implementation of:

1. a self-compiling compiler called starc that compiles
   a tiny but still fast subset of C called C Star (C*) to
   a tiny and easy-to-teach subset of RISC-V called RISC-U,
2. a self-executing emulator called mipster that executes
   RISC-U code including itself when compiled with starc,
3. a self-hosting hypervisor called hypster that provides
   RISC-U virtual machines that can host all of selfie,
   that is, starc, mipster, and hypster itself,
4. a prototypical symbolic execution engine called monster
   that executes RISC-U code symbolically,
5. a simple SAT solver that reads CNF DIMACS files, and
6. a tiny C* library called libcstar utilized by selfie.

Selfie is implemented in a single (!) file and kept minimal for simplicity.
There is also a simple in-memory linker, a RISC-U disassembler, a profiler,
and a debugger with replay as well as minimal operating system support in
the form of RISC-V system calls built into the emulator.

C* is a tiny Turing-complete subset of C that includes dereferencing
(the * operator) but excludes composite data types, bitwise and Boolean
operators, and many other features. There are only unsigned 32-bit
integers and 32-bit pointers as well as character and string literals.
This choice turns out to be helpful for students to understand the
true role of composite data types such as arrays and records.
Bitwise operations are implemented in libcstar using unsigned integer
arithmetics helping students better understand arithmetic operators.
C* is supposed to be close to the minimum necessary for implementing
a self-compiling, single-pass, recursive-descent compiler. C* can be
taught in one to two weeks of classes depending on student background.

The compiler can readily be extended to compile features missing in C*
and to improve performance of the generated code. The compiler generates
RISC-U executables in ELF format that are compatible with the official
RISC-V toolchain. The mipster emulator can execute RISC-U executables
loaded from file but also from memory immediately after code generation
without going through the file system.

RISC-U is a tiny Turing-complete subset of the RISC-V instruction set.
It only features unsigned 32-bit integer arithmetic, word memory,
and simple control-flow instructions but neither bitwise nor byte- and
word-level instructions. RISC-U can be taught in one week of classes.

The emulator implements minimal operating system support that is meant
to be extended by students, first as part of the emulator, and then
ported to run on top of it, similar to an actual operating system or
virtual machine monitor. The fact that the emulator can execute itself
helps exposing the self-referential nature of that challenge. In fact,
selfie goes one step further by implementing microkernel functionality
as part of the emulator and a hypervisor that can run as part of the
emulator as well as on top of it, all with the same code.

Selfie is the result of many years of teaching systems engineering.
The design of the compiler is inspired by the Oberon compiler of
Professor Niklaus Wirth from ETH Zurich. RISC-U is inspired by the
RISC-V community around Professor David Patterson from UC Berkeley.
The design of the hypervisor is inspired by microkernels of
Professor Jochen Liedtke from University of Karlsruhe.
*/

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     L I B R A R Y     ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ----------------------- BUILTIN PROCEDURES ----------------------
// -----------------------------------------------------------------

void      exit(uint32_t code);
uint32_t  read(uint32_t fd, uint32_t* buffer, uint32_t bytes_to_read);
uint32_t  write(uint32_t fd, uint32_t* buffer, uint32_t bytes_to_write);
uint32_t  open(uint32_t* filename, uint32_t flags, uint32_t mode);
uint32_t* malloc(uint32_t size);

// -----------------------------------------------------------------
// ----------------------- LIBRARY PROCEDURES ----------------------
// -----------------------------------------------------------------

void init_library();
void reset_library();

uint32_t two_to_the_power_of(uint32_t p);
uint32_t ten_to_the_power_of(uint32_t p);

uint32_t left_shift(uint32_t n, uint32_t b);
uint32_t right_shift(uint32_t n, uint32_t b);

uint32_t get_bits(uint32_t n, uint32_t i, uint32_t b);

// 32-bit port note: Renamed to avoid a conflict with C stdlib's abs(),
// which makes native (GCC) and emulated (mipster) version behave differently
uint32_t selfie_abs(uint32_t n);

uint32_t signed_less_than(uint32_t a, uint32_t b);
uint32_t signed_division(uint32_t a, uint32_t b);

uint32_t is_signed_integer(uint32_t n, uint32_t b);
uint32_t sign_extend(uint32_t n, uint32_t b);
uint32_t sign_shrink(uint32_t n, uint32_t b);

uint32_t  load_character(uint32_t* s, uint32_t i);
uint32_t* store_character(uint32_t* s, uint32_t i, uint32_t c);

uint32_t  string_length(uint32_t* s);
uint32_t* string_copy(uint32_t* s);
void      string_reverse(uint32_t* s);
uint32_t  string_compare(uint32_t* s, uint32_t* t);

uint32_t  atoi(uint32_t* s);
uint32_t* itoa(uint32_t n, uint32_t* s, uint32_t b, uint32_t a);

uint32_t fixed_point_ratio(uint32_t a, uint32_t b, uint32_t f);
uint32_t fixed_point_percentage(uint32_t r, uint32_t f);

void put_character(uint32_t c);

void print(uint32_t* s);
void println();

void print_character(uint32_t c);
void print_string(uint32_t* s);
void print_integer(uint32_t n);
void unprint_integer(uint32_t n);
void print_hexadecimal(uint32_t n, uint32_t a);
void print_octal(uint32_t n, uint32_t a);
void print_binary(uint32_t n, uint32_t a);

uint32_t print_format0(uint32_t* s, uint32_t i);
uint32_t print_format1(uint32_t* s, uint32_t i, uint32_t* a);

void printf1(uint32_t* s, uint32_t* a1);
void printf2(uint32_t* s, uint32_t* a1, uint32_t* a2);
void printf3(uint32_t* s, uint32_t* a1, uint32_t* a2, uint32_t* a3);
void printf4(uint32_t* s, uint32_t* a1, uint32_t* a2, uint32_t* a3, uint32_t* a4);
void printf5(uint32_t* s, uint32_t* a1, uint32_t* a2, uint32_t* a3, uint32_t* a4, uint32_t* a5);
void printf6(uint32_t* s, uint32_t* a1, uint32_t* a2, uint32_t* a3, uint32_t* a4, uint32_t* a5, uint32_t* a6);

uint32_t round_up(uint32_t n, uint32_t m);

uint32_t* smalloc(uint32_t size);
uint32_t* zalloc(uint32_t size);

// ------------------------ GLOBAL CONSTANTS -----------------------

uint32_t CHAR_EOF          =  -1; // end of file
uint32_t CHAR_BACKSPACE    =   8; // ASCII code 8  = backspace
uint32_t CHAR_TAB          =   9; // ASCII code 9  = tabulator
uint32_t CHAR_LF           =  10; // ASCII code 10 = line feed
uint32_t CHAR_CR           =  13; // ASCII code 13 = carriage return
uint32_t CHAR_SPACE        = ' ';
uint32_t CHAR_SEMICOLON    = ';';
uint32_t CHAR_PLUS         = '+';
uint32_t CHAR_DASH         = '-';
uint32_t CHAR_ASTERISK     = '*';
uint32_t CHAR_SLASH        = '/';
uint32_t CHAR_UNDERSCORE   = '_';
uint32_t CHAR_EQUAL        = '=';
uint32_t CHAR_LPARENTHESIS = '(';
uint32_t CHAR_RPARENTHESIS = ')';
uint32_t CHAR_LBRACE       = '{';
uint32_t CHAR_RBRACE       = '}';
uint32_t CHAR_COMMA        = ',';
uint32_t CHAR_LT           = '<';
uint32_t CHAR_GT           = '>';
uint32_t CHAR_EXCLAMATION  = '!';
uint32_t CHAR_PERCENTAGE   = '%';
uint32_t CHAR_SINGLEQUOTE  =  39; // ASCII code 39 = '
uint32_t CHAR_DOUBLEQUOTE  = '"';
uint32_t CHAR_BACKSLASH    =  92; // ASCII code 92 = backslash

uint32_t CPUBITWIDTH = 32;

uint32_t SIZEOFUINT32     = 4; // must be the same as REGISTERSIZE
uint32_t SIZEOFUINT32STAR = 4; // must be the same as REGISTERSIZE

uint32_t* power_of_two_table;

uint32_t INT32_MAX; // maximum numerical value of a signed 32-bit integer
uint32_t INT32_MIN; // minimum numerical value of a signed 32-bit integer

uint32_t UINT32_MAX; // maximum numerical value of an unsigned 32-bit integer

uint32_t MAX_FILENAME_LENGTH = 128;

uint32_t* character_buffer; // buffer for reading and writing characters
uint32_t* integer_buffer;   // buffer for printing integers
uint32_t* filename_buffer;  // buffer for opening files
uint32_t* binary_buffer;    // buffer for binary I/O

// flags for opening read-only files
// LINUX:       0 = 0x0000 = O_RDONLY (0x0000)
// MAC:         0 = 0x0000 = O_RDONLY (0x0000)
// WINDOWS: 32768 = 0x8000 = _O_BINARY (0x8000) | _O_RDONLY (0x0000)
// since LINUX/MAC do not seem to mind about _O_BINARY set
// we use the WINDOWS flags as default
uint32_t O_RDONLY = 32768;

// flags for opening write-only files
// MAC: 1537 = 0x0601 = O_CREAT (0x0200) | O_TRUNC (0x0400) | O_WRONLY (0x0001)
uint32_t MAC_O_CREAT_TRUNC_WRONLY = 1537;

// LINUX: 577 = 0x0241 = O_CREAT (0x0040) | O_TRUNC (0x0200) | O_WRONLY (0x0001)
uint32_t LINUX_O_CREAT_TRUNC_WRONLY = 577;

// WINDOWS: 33537 = 0x8301 = _O_BINARY (0x8000) | _O_CREAT (0x0100) | _O_TRUNC (0x0200) | _O_WRONLY (0x0001)
uint32_t WINDOWS_O_BINARY_CREAT_TRUNC_WRONLY = 33537;

// flags for rw-r--r-- file permissions
// 420 = 00644 = S_IRUSR (00400) | S_IWUSR (00200) | S_IRGRP (00040) | S_IROTH (00004)
// these flags seem to be working for LINUX, MAC, and WINDOWS
uint32_t S_IRUSR_IWUSR_IRGRP_IROTH = 420;

// ------------------------ GLOBAL VARIABLES -----------------------

uint32_t number_of_written_characters = 0;

uint32_t* output_name = (uint32_t*) 0;
uint32_t  output_fd   = 1; // 1 is file descriptor of standard output

// ------------------------- INITIALIZATION ------------------------

void init_library() {
  uint32_t i;

  // powers of two table with CPUBITWIDTH entries for 2^0 to 2^(CPUBITWIDTH - 1)
  power_of_two_table = smalloc(CPUBITWIDTH * SIZEOFUINT32);

  *power_of_two_table = 1; // 2^0 == 1

  i = 1;

  while (i < CPUBITWIDTH) {
    // compute powers of two incrementally using this recurrence relation
    *(power_of_two_table + i) = *(power_of_two_table + (i - 1)) * 2;

    i = i + 1;
  }

  // compute 32-bit unsigned integer range using signed integer arithmetic
  UINT32_MAX = -1;

  // compute 32-bit signed integer range using unsigned integer arithmetic
  INT32_MAX = two_to_the_power_of(CPUBITWIDTH - 1) - 1;
  INT32_MIN = INT32_MAX + 1;

  // allocate and touch to make sure memory is mapped for read calls
  character_buffer  = smalloc(SIZEOFUINT32);
  *character_buffer = 0;

  // accommodate at least CPUBITWIDTH numbers for itoa, no mapping needed
  integer_buffer = smalloc(CPUBITWIDTH + 1);

  // does not need to be mapped
  filename_buffer = smalloc(MAX_FILENAME_LENGTH);

  // allocate and touch to make sure memory is mapped for read calls
  binary_buffer  = smalloc(SIZEOFUINT32);
  *binary_buffer = 0;
}

void reset_library() {
  number_of_written_characters = 0;
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------    C O M P I L E R    ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- SCANNER ----------------------------
// -----------------------------------------------------------------

void init_scanner();
void reset_scanner();

void print_symbol(uint32_t symbol);
void print_line_number(uint32_t* message, uint32_t line);

void syntax_error_message(uint32_t* message);
void syntax_error_character(uint32_t character);
void syntax_error_identifier(uint32_t* expected);

void get_character();

uint32_t is_character_new_line();
uint32_t is_character_whitespace();

uint32_t find_next_character();

uint32_t is_character_letter();
uint32_t is_character_digit();
uint32_t is_character_letter_or_digit_or_underscore();
uint32_t is_character_not_double_quote_or_new_line_or_eof();

uint32_t identifier_string_match(uint32_t string_index);
uint32_t identifier_or_keyword();

void get_symbol();

void handle_escape_sequence();

// ------------------------ GLOBAL CONSTANTS -----------------------

uint32_t SYM_EOF          = -1; // end of file
uint32_t SYM_IDENTIFIER   = 0;  // identifier
uint32_t SYM_INTEGER      = 1;  // integer
uint32_t SYM_VOID         = 2;  // void
uint32_t SYM_UINT32       = 3;  // uint32_t
uint32_t SYM_SEMICOLON    = 4;  // ;
uint32_t SYM_IF           = 5;  // if
uint32_t SYM_ELSE         = 6;  // else
uint32_t SYM_PLUS         = 7;  // +
uint32_t SYM_MINUS        = 8;  // -
uint32_t SYM_ASTERISK     = 9;  // *
uint32_t SYM_DIV          = 10; // /
uint32_t SYM_EQUALITY     = 11; // ==
uint32_t SYM_ASSIGN       = 12; // =
uint32_t SYM_LPARENTHESIS = 13; // (
uint32_t SYM_RPARENTHESIS = 14; // )
uint32_t SYM_LBRACE       = 15; // {
uint32_t SYM_RBRACE       = 16; // }
uint32_t SYM_WHILE        = 17; // while
uint32_t SYM_RETURN       = 18; // return
uint32_t SYM_COMMA        = 19; // ,
uint32_t SYM_LT           = 20; // <
uint32_t SYM_LEQ          = 21; // <=
uint32_t SYM_GT           = 22; // >
uint32_t SYM_GEQ          = 23; // >=
uint32_t SYM_NOTEQ        = 24; // !=
uint32_t SYM_MOD          = 25; // %
uint32_t SYM_CHARACTER    = 26; // character
uint32_t SYM_STRING       = 27; // string

uint32_t* SYMBOLS; // strings representing symbols

uint32_t MAX_IDENTIFIER_LENGTH = 64;  // maximum number of characters in an identifier
uint32_t MAX_INTEGER_LENGTH    = 20;  // maximum number of characters in an unsigned integer
uint32_t MAX_STRING_LENGTH     = 128; // maximum number of characters in a string

// ------------------------ GLOBAL VARIABLES -----------------------

uint32_t line_number = 1; // current line number for error reporting

uint32_t* identifier = (uint32_t*) 0; // stores scanned identifier as string
uint32_t* integer    = (uint32_t*) 0; // stores scanned integer as string
uint32_t* string     = (uint32_t*) 0; // stores scanned string

uint32_t literal = 0; // stores numerical value of scanned integer or character

uint32_t integer_is_signed = 0; // enforce INT32_MIN limit if '-' was scanned before

uint32_t character; // most recently read character

uint32_t number_of_read_characters = 0;

uint32_t symbol; // most recently recognized symbol

uint32_t number_of_ignored_characters = 0;
uint32_t number_of_comments           = 0;
uint32_t number_of_scanned_symbols    = 0;

uint32_t* source_name = (uint32_t*) 0; // name of source file
uint32_t  source_fd   = 0;             // file descriptor of open source file

// ------------------------- INITIALIZATION ------------------------

void init_scanner () {
  SYMBOLS = smalloc((SYM_STRING + 1) * SIZEOFUINT32STAR);

  *(SYMBOLS + SYM_IDENTIFIER)   = (uint32_t) "identifier";
  *(SYMBOLS + SYM_INTEGER)      = (uint32_t) "integer";
  *(SYMBOLS + SYM_VOID)         = (uint32_t) "void";
  *(SYMBOLS + SYM_UINT32)       = (uint32_t) "uint32_t";
  *(SYMBOLS + SYM_SEMICOLON)    = (uint32_t) ";";
  *(SYMBOLS + SYM_IF)           = (uint32_t) "if";
  *(SYMBOLS + SYM_ELSE)         = (uint32_t) "else";
  *(SYMBOLS + SYM_PLUS)         = (uint32_t) "+";
  *(SYMBOLS + SYM_MINUS)        = (uint32_t) "-";
  *(SYMBOLS + SYM_ASTERISK)     = (uint32_t) "*";
  *(SYMBOLS + SYM_DIV)          = (uint32_t) "/";
  *(SYMBOLS + SYM_EQUALITY)     = (uint32_t) "==";
  *(SYMBOLS + SYM_ASSIGN)       = (uint32_t) "=";
  *(SYMBOLS + SYM_LPARENTHESIS) = (uint32_t) "(";
  *(SYMBOLS + SYM_RPARENTHESIS) = (uint32_t) ")";
  *(SYMBOLS + SYM_LBRACE)       = (uint32_t) "{";
  *(SYMBOLS + SYM_RBRACE)       = (uint32_t) "}";
  *(SYMBOLS + SYM_WHILE)        = (uint32_t) "while";
  *(SYMBOLS + SYM_RETURN)       = (uint32_t) "return";
  *(SYMBOLS + SYM_COMMA)        = (uint32_t) ",";
  *(SYMBOLS + SYM_LT)           = (uint32_t) "<";
  *(SYMBOLS + SYM_LEQ)          = (uint32_t) "<=";
  *(SYMBOLS + SYM_GT)           = (uint32_t) ">";
  *(SYMBOLS + SYM_GEQ)          = (uint32_t) ">=";
  *(SYMBOLS + SYM_NOTEQ)        = (uint32_t) "!=";
  *(SYMBOLS + SYM_MOD)          = (uint32_t) "%";
  *(SYMBOLS + SYM_CHARACTER)    = (uint32_t) "character";
  *(SYMBOLS + SYM_STRING)       = (uint32_t) "string";

  character = CHAR_EOF;
  symbol    = SYM_EOF;
}

void reset_scanner() {
  line_number = 1;

  number_of_read_characters = 0;

  get_character();

  number_of_ignored_characters = 0;
  number_of_comments           = 0;
  number_of_scanned_symbols    = 0;
}

// -----------------------------------------------------------------
// ------------------------- SYMBOL TABLE --------------------------
// -----------------------------------------------------------------

void reset_symbol_tables();

uint32_t hash(uint32_t* key);

void create_symbol_table_entry(uint32_t which, uint32_t* string, uint32_t line, uint32_t class, uint32_t type, uint32_t value, uint32_t address);

uint32_t* search_symbol_table(uint32_t* entry, uint32_t* string, uint32_t class);
uint32_t* search_global_symbol_table(uint32_t* string, uint32_t class);
uint32_t* get_scoped_symbol_table_entry(uint32_t* string, uint32_t class);

uint32_t is_undefined_procedure(uint32_t* entry);
uint32_t report_undefined_procedures();

// symbol table entry:
// +----+---------+
// |  0 | next    | pointer to next entry
// |  1 | string  | identifier string, big integer as string, string literal
// |  2 | line#   | source line number
// |  3 | class   | VARIABLE, BIGINT, STRING, PROCEDURE
// |  4 | type    | UINT32_T, UINT32STAR_T, VOID_T
// |  5 | value   | VARIABLE: initial value
// |  6 | address | VARIABLE, BIGINT, STRING: offset, PROCEDURE: address
// |  7 | scope   | REG_GP, REG_FP
// +----+---------+

uint32_t* get_next_entry(uint32_t* entry)  { return (uint32_t*) *entry; }
uint32_t* get_string(uint32_t* entry)      { return (uint32_t*) *(entry + 1); }
uint32_t  get_line_number(uint32_t* entry) { return             *(entry + 2); }
uint32_t  get_class(uint32_t* entry)       { return             *(entry + 3); }
uint32_t  get_type(uint32_t* entry)        { return             *(entry + 4); }
uint32_t  get_value(uint32_t* entry)       { return             *(entry + 5); }
uint32_t  get_address(uint32_t* entry)     { return             *(entry + 6); }
uint32_t  get_scope(uint32_t* entry)       { return             *(entry + 7); }

void set_next_entry(uint32_t* entry, uint32_t* next)   { *entry       = (uint32_t) next; }
void set_string(uint32_t* entry, uint32_t* identifier) { *(entry + 1) = (uint32_t) identifier; }
void set_line_number(uint32_t* entry, uint32_t line)   { *(entry + 2) = line; }
void set_class(uint32_t* entry, uint32_t class)        { *(entry + 3) = class; }
void set_type(uint32_t* entry, uint32_t type)          { *(entry + 4) = type; }
void set_value(uint32_t* entry, uint32_t value)        { *(entry + 5) = value; }
void set_address(uint32_t* entry, uint32_t address)    { *(entry + 6) = address; }
void set_scope(uint32_t* entry, uint32_t scope)        { *(entry + 7) = scope; }

// ------------------------ GLOBAL CONSTANTS -----------------------

// classes
uint32_t VARIABLE  = 1;
uint32_t BIGINT    = 2;
uint32_t STRING    = 3;
uint32_t PROCEDURE = 4;

// types
uint32_t UINT32_T     = 1;
uint32_t UINT32STAR_T = 2;
uint32_t VOID_T       = 3;

// symbol tables
uint32_t GLOBAL_TABLE  = 1;
uint32_t LOCAL_TABLE   = 2;
uint32_t LIBRARY_TABLE = 3;

// hash table size for global symbol table
uint32_t HASH_TABLE_SIZE = 1024;

// ------------------------ GLOBAL VARIABLES -----------------------

// table pointers
uint32_t* global_symbol_table  = (uint32_t*) 0;
uint32_t* local_symbol_table   = (uint32_t*) 0;
uint32_t* library_symbol_table = (uint32_t*) 0;

uint32_t number_of_global_variables = 0;
uint32_t number_of_procedures       = 0;
uint32_t number_of_strings          = 0;

uint32_t number_of_searches = 0;
uint32_t total_search_time  = 0;

// ------------------------- INITIALIZATION ------------------------

void reset_symbol_tables() {
  global_symbol_table  = (uint32_t*) zalloc(HASH_TABLE_SIZE * SIZEOFUINT32STAR);
  local_symbol_table   = (uint32_t*) 0;
  library_symbol_table = (uint32_t*) 0;

  number_of_global_variables = 0;
  number_of_procedures       = 0;
  number_of_strings          = 0;

  number_of_searches = 0;
  total_search_time  = 0;
}

// -----------------------------------------------------------------
// ---------------------------- PARSER -----------------------------
// -----------------------------------------------------------------

void reset_parser();

uint32_t is_not_rbrace_or_eof();
uint32_t is_expression();
uint32_t is_literal();
uint32_t is_star_or_div_or_modulo();
uint32_t is_plus_or_minus();
uint32_t is_comparison();

uint32_t look_for_factor();
uint32_t look_for_statement();
uint32_t look_for_type();

void save_temporaries();
void restore_temporaries(uint32_t number_of_temporaries);

void syntax_error_symbol(uint32_t expected);
void syntax_error_unexpected();
void print_type(uint32_t type);
void type_warning(uint32_t expected, uint32_t found);

uint32_t* get_variable_or_big_int(uint32_t* variable, uint32_t class);
void      load_upper_base_address(uint32_t* entry);
uint32_t  load_variable_or_big_int(uint32_t* variable, uint32_t class);
void      load_integer(uint32_t value);
void      load_string(uint32_t* string);

uint32_t help_call_codegen(uint32_t* entry, uint32_t* procedure);
void     help_procedure_prologue(uint32_t number_of_local_variable_bytes);
void     help_procedure_epilogue(uint32_t number_of_parameter_bytes);

uint32_t compile_call(uint32_t* procedure);
uint32_t compile_factor();
uint32_t compile_term();
uint32_t compile_simple_expression();
uint32_t compile_expression();
void     compile_while();
void     compile_if();
void     compile_return();
void     compile_statement();
uint32_t compile_type();
void     compile_variable(uint32_t offset);
uint32_t compile_initialization(uint32_t type);
void     compile_procedure(uint32_t* procedure, uint32_t type);
void     compile_cstar();

// ------------------------ GLOBAL VARIABLES -----------------------

uint32_t allocated_temporaries = 0; // number of allocated temporaries

uint32_t allocated_memory = 0; // number of bytes for global variables and strings

uint32_t return_branches = 0; // fixup chain for return statements

uint32_t return_type = 0; // return type of currently parsed procedure

uint32_t number_of_calls       = 0;
uint32_t number_of_assignments = 0;
uint32_t number_of_while       = 0;
uint32_t number_of_if          = 0;
uint32_t number_of_return      = 0;

// ------------------------- INITIALIZATION ------------------------

void reset_parser() {
  number_of_calls       = 0;
  number_of_assignments = 0;
  number_of_while       = 0;
  number_of_if          = 0;
  number_of_return      = 0;

  get_symbol();
}

// -----------------------------------------------------------------
// ---------------------- MACHINE CODE LIBRARY ---------------------
// -----------------------------------------------------------------

void emit_round_up(uint32_t reg, uint32_t m);
void emit_left_shift_by(uint32_t reg, uint32_t b);
void emit_program_entry();
void emit_bootstrapping();

// -----------------------------------------------------------------
// --------------------------- COMPILER ----------------------------
// -----------------------------------------------------------------

void selfie_compile();

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// -------------------     I N T E R F A C E     -------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- REGISTER ---------------------------
// -----------------------------------------------------------------

void init_register();

uint32_t* get_register_name(uint32_t reg);
void      print_register_name(uint32_t reg);

// ------------------------ GLOBAL CONSTANTS -----------------------

uint32_t NUMBEROFREGISTERS   = 32;
uint32_t NUMBEROFTEMPORARIES = 7;

uint32_t REG_ZR  = 0;
uint32_t REG_RA  = 1;
uint32_t REG_SP  = 2;
uint32_t REG_GP  = 3;
uint32_t REG_TP  = 4;
uint32_t REG_T0  = 5;
uint32_t REG_T1  = 6;
uint32_t REG_T2  = 7;
uint32_t REG_FP  = 8;
uint32_t REG_S1  = 9;
uint32_t REG_A0  = 10;
uint32_t REG_A1  = 11;
uint32_t REG_A2  = 12;
uint32_t REG_A3  = 13;
uint32_t REG_A4  = 14;
uint32_t REG_A5  = 15;
uint32_t REG_A6  = 16;
uint32_t REG_A7  = 17;
uint32_t REG_S2  = 18;
uint32_t REG_S3  = 19;
uint32_t REG_S4  = 20;
uint32_t REG_S5  = 21;
uint32_t REG_S6  = 22;
uint32_t REG_S7  = 23;
uint32_t REG_S8  = 24;
uint32_t REG_S9  = 25;
uint32_t REG_S10 = 26;
uint32_t REG_S11 = 27;
uint32_t REG_T3  = 28;
uint32_t REG_T4  = 29;
uint32_t REG_T5  = 30;
uint32_t REG_T6  = 31;

uint32_t* REGISTERS; // strings representing registers

// ------------------------- INITIALIZATION ------------------------

void init_register() {
  REGISTERS = smalloc(NUMBEROFREGISTERS * SIZEOFUINT32STAR);

  *(REGISTERS + REG_ZR)  = (uint32_t) "$zero";
  *(REGISTERS + REG_RA)  = (uint32_t) "$ra";
  *(REGISTERS + REG_SP)  = (uint32_t) "$sp";
  *(REGISTERS + REG_GP)  = (uint32_t) "$gp";
  *(REGISTERS + REG_TP)  = (uint32_t) "$tp";
  *(REGISTERS + REG_T0)  = (uint32_t) "$t0";
  *(REGISTERS + REG_T1)  = (uint32_t) "$t1";
  *(REGISTERS + REG_T2)  = (uint32_t) "$t2";
  *(REGISTERS + REG_FP)  = (uint32_t) "$fp";
  *(REGISTERS + REG_S1)  = (uint32_t) "$s1";
  *(REGISTERS + REG_A0)  = (uint32_t) "$a0";
  *(REGISTERS + REG_A1)  = (uint32_t) "$a1";
  *(REGISTERS + REG_A2)  = (uint32_t) "$a2";
  *(REGISTERS + REG_A3)  = (uint32_t) "$a3";
  *(REGISTERS + REG_A4)  = (uint32_t) "$a4";
  *(REGISTERS + REG_A5)  = (uint32_t) "$a5";
  *(REGISTERS + REG_A6)  = (uint32_t) "$a6";
  *(REGISTERS + REG_A7)  = (uint32_t) "$a7";
  *(REGISTERS + REG_S2)  = (uint32_t) "$s2";
  *(REGISTERS + REG_S3)  = (uint32_t) "$s3";
  *(REGISTERS + REG_S4)  = (uint32_t) "$s4";
  *(REGISTERS + REG_S5)  = (uint32_t) "$s5";
  *(REGISTERS + REG_S6)  = (uint32_t) "$s6";
  *(REGISTERS + REG_S7)  = (uint32_t) "$s7";
  *(REGISTERS + REG_S8)  = (uint32_t) "$s8";
  *(REGISTERS + REG_S9)  = (uint32_t) "$s9";
  *(REGISTERS + REG_S10) = (uint32_t) "$s10";
  *(REGISTERS + REG_S11) = (uint32_t) "$s11";
  *(REGISTERS + REG_T3)  = (uint32_t) "$t3";
  *(REGISTERS + REG_T4)  = (uint32_t) "$t4";
  *(REGISTERS + REG_T5)  = (uint32_t) "$t5";
  *(REGISTERS + REG_T6)  = (uint32_t) "$t6";
}

// -----------------------------------------------------------------
// ------------------------ ENCODER/DECODER ------------------------
// -----------------------------------------------------------------

void check_immediate_range(uint32_t found, uint32_t bits);

uint32_t encode_r_format(uint32_t funct7, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t rd, uint32_t opcode);
uint32_t get_funct7(uint32_t instruction);
uint32_t get_rs2(uint32_t instruction);
uint32_t get_rs1(uint32_t instruction);
uint32_t get_funct3(uint32_t instruction);
uint32_t get_rd(uint32_t instruction);
uint32_t get_opcode(uint32_t instruction);
void     decode_r_format();

uint32_t encode_i_format(uint32_t immediate, uint32_t rs1, uint32_t funct3, uint32_t rd, uint32_t opcode);
uint32_t get_immediate_i_format(uint32_t instruction);
void     decode_i_format();

uint32_t encode_s_format(uint32_t immediate, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t opcode);
uint32_t get_immediate_s_format(uint32_t instruction);
void     decode_s_format();

uint32_t encode_b_format(uint32_t immediate, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t opcode);
uint32_t get_immediate_b_format(uint32_t instruction);
void     decode_b_format();

uint32_t encode_j_format(uint32_t immediate, uint32_t rd, uint32_t opcode);
uint32_t get_immediate_j_format(uint32_t instruction);
void     decode_j_format();

uint32_t encode_u_format(uint32_t immediate, uint32_t rd, uint32_t opcode);
uint32_t get_immediate_u_format(uint32_t instruction);
void     decode_u_format();

// ------------------------ GLOBAL CONSTANTS -----------------------

// opcodes
uint32_t OP_LW     = 3;   // 0000011, I format (LW)
uint32_t OP_IMM    = 19;  // 0010011, I format (ADDI, NOP)
uint32_t OP_SW     = 35;  // 0100011, S format (SW)
uint32_t OP_OP     = 51;  // 0110011, R format (ADD, SUB, MUL, DIVU, REMU, SLTU)
uint32_t OP_LUI    = 55;  // 0110111, U format (LUI)
uint32_t OP_BRANCH = 99;  // 1100011, B format (BEQ)
uint32_t OP_JALR   = 103; // 1100111, I format (JALR)
uint32_t OP_JAL    = 111; // 1101111, J format (JAL)
uint32_t OP_SYSTEM = 115; // 1110011, I format (ECALL)

// f3-codes
uint32_t F3_NOP   = 0; // 000
uint32_t F3_ADDI  = 0; // 000
uint32_t F3_ADD   = 0; // 000
uint32_t F3_SUB   = 0; // 000
uint32_t F3_MUL   = 0; // 000
uint32_t F3_DIVU  = 5; // 101
uint32_t F3_REMU  = 7; // 111
uint32_t F3_SLTU  = 3; // 011
uint32_t F3_LW    = 2; // 010
uint32_t F3_SW    = 2; // 010
uint32_t F3_BEQ   = 0; // 000
uint32_t F3_JALR  = 0; // 000
uint32_t F3_ECALL = 0; // 000

// f7-codes
uint32_t F7_ADD  = 0;  // 0000000
uint32_t F7_MUL  = 1;  // 0000001
uint32_t F7_SUB  = 32; // 0100000
uint32_t F7_DIVU = 1;  // 0000001
uint32_t F7_REMU = 1;  // 0000001
uint32_t F7_SLTU = 0;  // 0000000

// f12-codes (immediates)
uint32_t F12_ECALL = 0; // 000000000000

// ------------------------ GLOBAL VARIABLES -----------------------

uint32_t opcode = 0;
uint32_t rs1    = 0;
uint32_t rs2    = 0;
uint32_t rd     = 0;
uint32_t imm    = 0;
uint32_t funct3 = 0;
uint32_t funct7 = 0;

// -----------------------------------------------------------------
// ---------------------------- BINARY -----------------------------
// -----------------------------------------------------------------

void reset_instruction_counters();

uint32_t get_total_number_of_instructions();

void print_instruction_counter(uint32_t total, uint32_t counter, uint32_t* mnemonics);
void print_instruction_counters();

uint32_t load_instruction(uint32_t baddr);
void     store_instruction(uint32_t baddr, uint32_t instruction);

uint32_t load_data(uint32_t baddr);
void     store_data(uint32_t baddr, uint32_t data);

void emit_instruction(uint32_t instruction);

void emit_nop();

void emit_lui(uint32_t rd, uint32_t immediate);
void emit_addi(uint32_t rd, uint32_t rs1, uint32_t immediate);

void emit_add(uint32_t rd, uint32_t rs1, uint32_t rs2);
void emit_sub(uint32_t rd, uint32_t rs1, uint32_t rs2);
void emit_mul(uint32_t rd, uint32_t rs1, uint32_t rs2);
void emit_divu(uint32_t rd, uint32_t rs1, uint32_t rs2);
void emit_remu(uint32_t rd, uint32_t rs1, uint32_t rs2);
void emit_sltu(uint32_t rd, uint32_t rs1, uint32_t rs2);

void emit_lw(uint32_t rd, uint32_t rs1, uint32_t immediate);
void emit_sw(uint32_t rs1, uint32_t immediate, uint32_t rs2);

void emit_beq(uint32_t rs1, uint32_t rs2, uint32_t immediate);

void emit_jal(uint32_t rd, uint32_t immediate);
void emit_jalr(uint32_t rd, uint32_t rs1, uint32_t immediate);

void emit_ecall();

void fixup_relative_BFormat(uint32_t from_address);
void fixup_relative_JFormat(uint32_t from_address, uint32_t to_address);
void fixlink_relative(uint32_t from_address, uint32_t to_address);

void emit_data_word(uint32_t data, uint32_t offset, uint32_t source_line_number);
void emit_string_data(uint32_t* entry);

void emit_data_segment();

uint32_t* create_elf_header(uint32_t binary_length);
uint32_t  validate_elf_header(uint32_t* header);

uint32_t open_write_only(uint32_t* name);

void selfie_output();

uint32_t* touch(uint32_t* memory, uint32_t length);

void selfie_load();

// ------------------------ GLOBAL CONSTANTS -----------------------

uint32_t MAX_BINARY_LENGTH = 262144; // 256KB = MAX_CODE_LENGTH + MAX_DATA_LENGTH

uint32_t MAX_CODE_LENGTH = 245760; // 240KB
uint32_t MAX_DATA_LENGTH = 16384; // 16KB

uint32_t ELF_HEADER_LEN = 84; // = 52 + 32 bytes (file + program header)

// according to RISC-V pk
uint32_t ELF_ENTRY_POINT = 65536; // = 0x10000 (address of beginning of code)

// ------------------------ GLOBAL VARIABLES -----------------------

// instruction counters

uint32_t ic_lui   = 0;
uint32_t ic_addi  = 0;
uint32_t ic_add   = 0;
uint32_t ic_sub   = 0;
uint32_t ic_mul   = 0;
uint32_t ic_divu  = 0;
uint32_t ic_remu  = 0;
uint32_t ic_sltu  = 0;
uint32_t ic_lw    = 0;
uint32_t ic_sw    = 0;
uint32_t ic_beq   = 0;
uint32_t ic_jal   = 0;
uint32_t ic_jalr  = 0;
uint32_t ic_ecall = 0;

uint32_t* binary        = (uint32_t*) 0; // binary of code and data segments
uint32_t  binary_length = 0; // length of binary in bytes including data segment
uint32_t* binary_name   = (uint32_t*) 0; // file name of binary

uint32_t code_length = 0; // length of code segment in binary in bytes
uint32_t entry_point = 0; // beginning of code segment in virtual address space

uint32_t* code_line_number = (uint32_t*) 0; // code line number per emitted instruction
uint32_t* data_line_number = (uint32_t*) 0; // data line number per emitted data

uint32_t* assembly_name = (uint32_t*) 0; // name of assembly file
uint32_t  assembly_fd   = 0; // file descriptor of open assembly file

uint32_t* ELF_header = (uint32_t*) 0;

// -----------------------------------------------------------------
// ----------------------- MIPSTER SYSCALLS ------------------------
// -----------------------------------------------------------------

void emit_exit();
void implement_exit(uint32_t* context);

void emit_read();
void implement_read(uint32_t* context);

void emit_write();
void implement_write(uint32_t* context);

void     emit_open();
uint32_t down_load_string(uint32_t* table, uint32_t vstring, uint32_t* s);
void     implement_open(uint32_t* context);

void emit_malloc();
void implement_brk(uint32_t* context);

// ------------------------ GLOBAL CONSTANTS -----------------------

uint32_t debug_read  = 0;
uint32_t debug_write = 0;
uint32_t debug_open  = 0;
uint32_t debug_brk   = 0;

uint32_t SYSCALL_EXIT  = 93;
uint32_t SYSCALL_READ  = 63;
uint32_t SYSCALL_WRITE = 64;
uint32_t SYSCALL_OPEN  = 1024;
uint32_t SYSCALL_BRK   = 214;

// -----------------------------------------------------------------
// ----------------------- HYPSTER SYSCALLS ------------------------
// -----------------------------------------------------------------

void      emit_switch();
void      do_switch(uint32_t* to_context, uint32_t timeout);
void      implement_switch();
uint32_t* mipster_switch(uint32_t* to_context, uint32_t timeout);

// ------------------------ GLOBAL CONSTANTS -----------------------

// TODO: fix this syscall for spike
uint32_t SYSCALL_SWITCH = 401;

uint32_t debug_switch = 0;

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ----------------------    R U N T I M E    ----------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- MEMORY -----------------------------
// -----------------------------------------------------------------

void init_memory(uint32_t megabytes);

uint32_t load_physical_memory(uint32_t* paddr);
void     store_physical_memory(uint32_t* paddr, uint32_t data);

uint32_t frame_for_page(uint32_t* table, uint32_t page);
uint32_t get_frame_for_page(uint32_t* table, uint32_t page);
uint32_t is_page_mapped(uint32_t* table, uint32_t page);

uint32_t is_valid_virtual_address(uint32_t vaddr);
uint32_t get_page_of_virtual_address(uint32_t vaddr);
uint32_t is_virtual_address_mapped(uint32_t* table, uint32_t vaddr);

uint32_t* tlb(uint32_t* table, uint32_t vaddr);

uint32_t load_virtual_memory(uint32_t* table, uint32_t vaddr);
void     store_virtual_memory(uint32_t* table, uint32_t vaddr, uint32_t data);

// ------------------------ GLOBAL CONSTANTS -----------------------

uint32_t debug_tlb = 0;

uint32_t MEGABYTE = 1048576; // 1MB

uint32_t VIRTUALMEMORYSIZE = 2147483648; // 2GB of virtual memory

uint32_t WORDSIZE       = 4; // in bytes
uint32_t WORDSIZEINBITS = 32;

uint32_t INSTRUCTIONSIZE = 4; // must be the same as WORDSIZE
uint32_t REGISTERSIZE    = 4; // must be the same as WORDSIZE

uint32_t PAGESIZE = 4096; // we use standard 4KB pages

// ------------------------ GLOBAL VARIABLES -----------------------

uint32_t page_frame_memory = 0; // size of memory for frames

// ------------------------- INITIALIZATION ------------------------

void init_memory(uint32_t megabytes) {
  if (megabytes > 4096)
    megabytes = 4096;

  page_frame_memory = megabytes * MEGABYTE;
}

// -----------------------------------------------------------------
// ------------------------- INSTRUCTIONS --------------------------
// -----------------------------------------------------------------

void print_code_line_number_for_instruction(uint32_t a);
void print_code_context_for_instruction(uint32_t a);

void print_lui();
void print_lui_before();
void print_lui_after();
void record_lui_addi_add_sub_mul_sltu_jal_jalr();
void do_lui();
void undo_lui_addi_add_sub_mul_divu_remu_sltu_lw_jal_jalr();
void constrain_lui();

void print_addi();
void print_addi_before();
void print_addi_add_sub_mul_divu_remu_sltu_after();
void do_addi();
void constrain_addi();

void print_add_sub_mul_divu_remu_sltu(uint32_t *mnemonics);
void print_add_sub_mul_divu_remu_sltu_before();

void do_add();
void constrain_add();

void do_sub();
void constrain_sub();

void do_mul();
void constrain_mul();

void record_divu_remu();
void do_divu();
void constrain_divu();

void do_remu();
void constrain_remu();

void do_sltu();
void constrain_sltu();
void backtrack_sltu();

void     print_lw();
void     print_lw_before();
void     print_lw_after(uint32_t vaddr);
void     record_lw();
uint32_t do_lw();
uint32_t constrain_lw();

void     print_sw();
void     print_sw_before();
void     print_sw_after(uint32_t vaddr);
void     record_sw();
uint32_t do_sw();
void     undo_sw();
uint32_t constrain_sw();
void     backtrack_sw();

void print_beq();
void print_beq_before();
void print_beq_after();
void record_beq();
void do_beq();

void print_jal();
void print_jal_before();
void print_jal_jalr_after();
void do_jal();
void constrain_jal_jalr();

void print_jalr();
void print_jalr_before();
void do_jalr();

void print_ecall();
void record_ecall();
void do_ecall();
void undo_ecall();
void backtrack_ecall();

void print_data_line_number();
void print_data_context(uint32_t data);
void print_data(uint32_t data);

// -----------------------------------------------------------------
// -------------------------- REPLAY ENGINE ------------------------
// -----------------------------------------------------------------

void init_replay_engine();

void record_state(uint32_t value);

void replay_trace();

// ------------------------ GLOBAL CONSTANTS -----------------------

uint32_t MAX_REPLAY_LENGTH = 100;

// trace

uint32_t tc = 0; // trace counter

uint32_t* pcs    = (uint32_t*) 0; // trace of program counter values
uint32_t* values = (uint32_t*) 0; // trace of values

// ------------------------- INITIALIZATION ------------------------

void init_replay_engine() {
  pcs    = zalloc(MAX_REPLAY_LENGTH * SIZEOFUINT32);
  values = zalloc(MAX_REPLAY_LENGTH * SIZEOFUINT32);
}

// -----------------------------------------------------------------
// ------------------- SYMBOLIC EXECUTION ENGINE -------------------
// -----------------------------------------------------------------

void init_symbolic_engine();

void print_symbolic_memory(uint32_t svc);

uint32_t cardinality(uint32_t lo, uint32_t up);
uint32_t combined_cardinality(uint32_t lo1, uint32_t up1, uint32_t lo2, uint32_t up2);

uint32_t is_symbolic_value(uint32_t type, uint32_t lo, uint32_t up);
uint32_t is_safe_address(uint32_t vaddr, uint32_t reg);
uint32_t load_symbolic_memory(uint32_t* pt, uint32_t vaddr);

uint32_t is_trace_space_available();

void ealloc();
void efree();

void store_symbolic_memory(uint32_t* pt, uint32_t vaddr, uint32_t value, uint32_t type, uint32_t lo, uint32_t up, uint32_t trb);

void store_constrained_memory(uint32_t vaddr, uint32_t lo, uint32_t up, uint32_t trb);
void store_register_memory(uint32_t reg, uint32_t value);

void constrain_memory(uint32_t reg, uint32_t lo, uint32_t up, uint32_t trb);

void set_constraint(uint32_t reg, uint32_t hasco, uint32_t vaddr, uint32_t hasmn, uint32_t colos, uint32_t coups);

void take_branch(uint32_t b, uint32_t how_many_more);
void create_constraints(uint32_t lo1, uint32_t up1, uint32_t lo2, uint32_t up2, uint32_t trb, uint32_t how_many_more);

uint32_t fuzz_lo(uint32_t value);
uint32_t fuzz_up(uint32_t value);

// ------------------------ GLOBAL CONSTANTS -----------------------

uint32_t MAX_TRACE_LENGTH = 100000;

uint32_t debug_symbolic = 0;

// ------------------------ GLOBAL VARIABLES -----------------------

// trace

uint32_t* tcs = (uint32_t*) 0; // trace of trace counters to previous values

uint32_t* types = (uint32_t*) 0; // memory range or integer interval

uint32_t* los = (uint32_t*) 0; // trace of lower bounds on values
uint32_t* ups = (uint32_t*) 0; // trace of upper bounds on values

uint32_t* vaddrs = (uint32_t*) 0; // trace of virtual addresses

// read history

uint32_t rc = 0; // read counter

uint32_t* read_values = (uint32_t*) 0;

uint32_t* read_los = (uint32_t*) 0;
uint32_t* read_ups = (uint32_t*) 0;

// registers

uint32_t* reg_typ = (uint32_t*) 0; // memory range or integer interval
uint32_t* reg_los = (uint32_t*) 0; // lower bound on register value
uint32_t* reg_ups = (uint32_t*) 0; // upper bound on register value

// register constraints on memory

uint32_t* reg_hasco = (uint32_t*) 0; // register has constraint
uint32_t* reg_vaddr = (uint32_t*) 0; // vaddr of constrained memory
uint32_t* reg_hasmn = (uint32_t*) 0; // constraint has minuend
uint32_t* reg_colos = (uint32_t*) 0; // offset on lower bound
uint32_t* reg_coups = (uint32_t*) 0; // offset on upper bound

// trace counter of most recent constraint

uint32_t mrcc = 0;

// fuzzing

uint32_t fuzz = 0; // power-of-two fuzzing factor for read calls

// ------------------------- INITIALIZATION ------------------------

void init_symbolic_engine() {
  pcs    = zalloc(MAX_TRACE_LENGTH * SIZEOFUINT32);
  tcs    = zalloc(MAX_TRACE_LENGTH * SIZEOFUINT32);
  values = zalloc(MAX_TRACE_LENGTH * SIZEOFUINT32);
  types  = zalloc(MAX_TRACE_LENGTH * SIZEOFUINT32);
  los    = zalloc(MAX_TRACE_LENGTH * SIZEOFUINT32);
  ups    = zalloc(MAX_TRACE_LENGTH * SIZEOFUINT32);
  vaddrs = zalloc(MAX_TRACE_LENGTH * SIZEOFUINT32);

  read_values = zalloc(MAX_TRACE_LENGTH * SIZEOFUINT32);
  read_los    = zalloc(MAX_TRACE_LENGTH * SIZEOFUINT32);
  read_ups    = zalloc(MAX_TRACE_LENGTH * SIZEOFUINT32);

  reg_typ = zalloc(NUMBEROFREGISTERS * REGISTERSIZE);
  reg_los = zalloc(NUMBEROFREGISTERS * REGISTERSIZE);
  reg_ups = zalloc(NUMBEROFREGISTERS * REGISTERSIZE);

  reg_hasco = zalloc(NUMBEROFREGISTERS * REGISTERSIZE);
  reg_vaddr = zalloc(NUMBEROFREGISTERS * REGISTERSIZE);
  reg_hasmn = zalloc(NUMBEROFREGISTERS * REGISTERSIZE);
  reg_colos = zalloc(NUMBEROFREGISTERS * REGISTERSIZE);
  reg_coups = zalloc(NUMBEROFREGISTERS * REGISTERSIZE);
}

// -----------------------------------------------------------------
// -------------------------- INTERPRETER --------------------------
// -----------------------------------------------------------------

void init_interpreter();
void reset_interpreter();

void     print_register_hexadecimal(uint32_t reg);
void     print_register_octal(uint32_t reg);
uint32_t is_system_register(uint32_t reg);
void     print_register_value(uint32_t reg);

void print_exception(uint32_t exception, uint32_t faulting_page);
void throw_exception(uint32_t exception, uint32_t faulting_page);

void fetch();
void decode_execute();
void interrupt();

uint32_t* run_until_exception();

uint32_t instruction_with_max_counter(uint32_t* counters, uint32_t max);
uint32_t print_per_instruction_counter(uint32_t total, uint32_t* counters, uint32_t max);
void     print_per_instruction_profile(uint32_t* message, uint32_t total, uint32_t* counters);

void print_profile();

void selfie_disassemble(uint32_t verbose);

// ------------------------ GLOBAL CONSTANTS -----------------------

uint32_t EXCEPTION_NOEXCEPTION        = 0;
uint32_t EXCEPTION_PAGEFAULT          = 1;
uint32_t EXCEPTION_SYSCALL            = 2;
uint32_t EXCEPTION_TIMER              = 3;
uint32_t EXCEPTION_INVALIDADDRESS     = 4;
uint32_t EXCEPTION_DIVISIONBYZERO     = 5;
uint32_t EXCEPTION_UNKNOWNINSTRUCTION = 6;
uint32_t EXCEPTION_MAXTRACE           = 7;

uint32_t* EXCEPTIONS; // strings representing exceptions

uint32_t debug_exception = 0;

// enables recording, disassembling, debugging, and symbolically executing code
uint32_t debug = 0;

uint32_t execute     = 0; // flag for executing code
uint32_t record      = 0; // flag for recording code execution
uint32_t undo        = 0; // flag for undoing code execution
uint32_t redo        = 0; // flag for redoing code execution
uint32_t disassemble = 0; // flag for disassembling code
uint32_t symbolic    = 0; // flag for symbolically executing code
uint32_t backtrack   = 0; // flag for backtracking symbolic execution

uint32_t disassemble_verbose = 0; // flag for disassembling code in more detail

// number of instructions from context switch to timer interrupt
// CAUTION: avoid interrupting any kernel activities, keep TIMESLICE large
// TODO: implement proper interrupt controller to turn interrupts on and off
uint32_t TIMESLICE = 10000000;

uint32_t TIMEROFF = 0;

// ------------------------ GLOBAL VARIABLES -----------------------

// hardware thread state

uint32_t pc = 0; // program counter
uint32_t ir = 0; // instruction register

uint32_t* registers = (uint32_t*) 0; // general-purpose registers

uint32_t* pt = (uint32_t*) 0; // page table

// core state

uint32_t timer = 0; // counter for timer interrupt
uint32_t trap  = 0; // flag for creating a trap

// profile

uint32_t  calls               = 0;             // total number of executed procedure calls
uint32_t* calls_per_procedure = (uint32_t*) 0; // number of executed calls of each procedure

uint32_t  iterations          = 0;             // total number of executed loop iterations
uint32_t* iterations_per_loop = (uint32_t*) 0; // number of executed iterations of each loop

uint32_t* loads_per_instruction  = (uint32_t*) 0; // number of executed loads per load instruction
uint32_t* stores_per_instruction = (uint32_t*) 0; // number of executed stores per store instruction

// ------------------------- INITIALIZATION ------------------------

void init_interpreter() {
  EXCEPTIONS = smalloc((EXCEPTION_MAXTRACE + 1) * SIZEOFUINT32STAR);

  *(EXCEPTIONS + EXCEPTION_NOEXCEPTION)        = (uint32_t) "no exception";
  *(EXCEPTIONS + EXCEPTION_PAGEFAULT)          = (uint32_t) "page fault";
  *(EXCEPTIONS + EXCEPTION_SYSCALL)            = (uint32_t) "syscall";
  *(EXCEPTIONS + EXCEPTION_TIMER)              = (uint32_t) "timer interrupt";
  *(EXCEPTIONS + EXCEPTION_INVALIDADDRESS)     = (uint32_t) "invalid address";
  *(EXCEPTIONS + EXCEPTION_DIVISIONBYZERO)     = (uint32_t) "division by zero";
  *(EXCEPTIONS + EXCEPTION_UNKNOWNINSTRUCTION) = (uint32_t) "unknown instruction";
  *(EXCEPTIONS + EXCEPTION_MAXTRACE)           = (uint32_t) "trace length exceeded";
}

void reset_interpreter() {
  pc = 0;
  ir = 0;

  registers = (uint32_t*) 0;

  pt = (uint32_t*) 0;

  trap = 0;

  timer = TIMEROFF;

  if (execute) {
    reset_instruction_counters();

    calls               = 0;
    calls_per_procedure = zalloc(MAX_CODE_LENGTH / INSTRUCTIONSIZE * SIZEOFUINT32);

    iterations          = 0;
    iterations_per_loop = zalloc(MAX_CODE_LENGTH / INSTRUCTIONSIZE * SIZEOFUINT32);

    loads_per_instruction  = zalloc(MAX_CODE_LENGTH / INSTRUCTIONSIZE * SIZEOFUINT32);
    stores_per_instruction = zalloc(MAX_CODE_LENGTH / INSTRUCTIONSIZE * SIZEOFUINT32);
  }
}

// -----------------------------------------------------------------
// ---------------------------- CONTEXTS ---------------------------
// -----------------------------------------------------------------

uint32_t* allocate_context(uint32_t* parent, uint32_t* vctxt, uint32_t* in);

uint32_t* find_context(uint32_t* parent, uint32_t* vctxt, uint32_t* in);

void      free_context(uint32_t* context);
uint32_t* delete_context(uint32_t* context, uint32_t* from);

// context struct:
// +----+----------------+
// |  0 | next context    | pointer to next context
// |  1 | prev context    | pointer to previous context
// |  2 | program counter | program counter
// |  3 | regs            | pointer to general purpose registers
// |  4 | page table      | pointer to page table
// |  5 | lo page         | lowest low unmapped page
// |  6 | me page         | highest low unmapped page
// |  7 | hi page         | highest high unmapped page
// |  8 | original break  | original end of data segment
// |  9 | program break   | end of data segment
// | 10 | exception       | exception ID
// | 11 | faulting page   | faulting page
// | 12 | exit code       | exit code
// | 13 | parent          | context that created this context
// | 14 | virtual context | virtual context address
// | 15 | name            | binary name loaded into context
// +----+-----------------+

uint32_t next_context(uint32_t* context)    { return (uint32_t) context; }
uint32_t prev_context(uint32_t* context)    { return (uint32_t) (context + 1); }
uint32_t program_counter(uint32_t* context) { return (uint32_t) (context + 2); }
uint32_t regs(uint32_t* context)            { return (uint32_t) (context + 3); }
uint32_t page_table(uint32_t* context)      { return (uint32_t) (context + 4); }
uint32_t lo_page(uint32_t* context)         { return (uint32_t) (context + 5); }
uint32_t me_page(uint32_t* context)         { return (uint32_t) (context + 6); }
uint32_t hi_page(uint32_t* context)         { return (uint32_t) (context + 7); }
uint32_t original_break(uint32_t* context)  { return (uint32_t) (context + 8); }
uint32_t program_break(uint32_t* context)   { return (uint32_t) (context + 9); }
uint32_t exception(uint32_t* context)       { return (uint32_t) (context + 10); }
uint32_t faulting_page(uint32_t* context)   { return (uint32_t) (context + 11); }
uint32_t exit_code(uint32_t* context)       { return (uint32_t) (context + 12); }
uint32_t parent(uint32_t* context)          { return (uint32_t) (context + 13); }
uint32_t virtual_context(uint32_t* context) { return (uint32_t) (context + 14); }
uint32_t name(uint32_t* context)            { return (uint32_t) (context + 15); }

uint32_t* get_next_context(uint32_t* context)    { return (uint32_t*) *context; }
uint32_t* get_prev_context(uint32_t* context)    { return (uint32_t*) *(context + 1); }
uint32_t  get_pc(uint32_t* context)              { return             *(context + 2); }
uint32_t* get_regs(uint32_t* context)            { return (uint32_t*) *(context + 3); }
uint32_t* get_pt(uint32_t* context)              { return (uint32_t*) *(context + 4); }
uint32_t  get_lo_page(uint32_t* context)         { return             *(context + 5); }
uint32_t  get_me_page(uint32_t* context)         { return             *(context + 6); }
uint32_t  get_hi_page(uint32_t* context)         { return             *(context + 7); }
uint32_t  get_original_break(uint32_t* context)  { return             *(context + 8); }
uint32_t  get_program_break(uint32_t* context)   { return             *(context + 9); }
uint32_t  get_exception(uint32_t* context)       { return             *(context + 10); }
uint32_t  get_faulting_page(uint32_t* context)   { return             *(context + 11); }
uint32_t  get_exit_code(uint32_t* context)       { return             *(context + 12); }
uint32_t* get_parent(uint32_t* context)          { return (uint32_t*) *(context + 13); }
uint32_t* get_virtual_context(uint32_t* context) { return (uint32_t*) *(context + 14); }
uint32_t* get_name(uint32_t* context)            { return (uint32_t*) *(context + 15); }

void set_next_context(uint32_t* context, uint32_t* next)     { *context        = (uint32_t) next; }
void set_prev_context(uint32_t* context, uint32_t* prev)     { *(context + 1)  = (uint32_t) prev; }
void set_pc(uint32_t* context, uint32_t pc)                  { *(context + 2)  = pc; }
void set_regs(uint32_t* context, uint32_t* regs)             { *(context + 3)  = (uint32_t) regs; }
void set_pt(uint32_t* context, uint32_t* pt)                 { *(context + 4)  = (uint32_t) pt; }
void set_lo_page(uint32_t* context, uint32_t lo_page)        { *(context + 5)  = lo_page; }
void set_me_page(uint32_t* context, uint32_t me_page)        { *(context + 6)  = me_page; }
void set_hi_page(uint32_t* context, uint32_t hi_page)        { *(context + 7)  = hi_page; }
void set_original_break(uint32_t* context, uint32_t brk)     { *(context + 8)  = brk; }
void set_program_break(uint32_t* context, uint32_t brk)      { *(context + 9)  = brk; }
void set_exception(uint32_t* context, uint32_t exception)    { *(context + 10) = exception; }
void set_faulting_page(uint32_t* context, uint32_t page)     { *(context + 11) = page; }
void set_exit_code(uint32_t* context, uint32_t code)         { *(context + 12) = code; }
void set_parent(uint32_t* context, uint32_t* parent)         { *(context + 13) = (uint32_t) parent; }
void set_virtual_context(uint32_t* context, uint32_t* vctxt) { *(context + 14) = (uint32_t) vctxt; }
void set_name(uint32_t* context, uint32_t* name)             { *(context + 15) = (uint32_t) name; }

// -----------------------------------------------------------------
// -------------------------- MICROKERNEL --------------------------
// -----------------------------------------------------------------

void reset_microkernel();

uint32_t* create_context(uint32_t* parent, uint32_t* vctxt);

uint32_t* cache_context(uint32_t* vctxt);

void save_context(uint32_t* context);

void map_page(uint32_t* context, uint32_t page, uint32_t frame);

void restore_context(uint32_t* context);

// ------------------------ GLOBAL CONSTANTS -----------------------

uint32_t debug_create = 0;
uint32_t debug_map    = 0;

// ------------------------ GLOBAL VARIABLES -----------------------

uint32_t* current_context = (uint32_t*) 0; // context currently running

uint32_t* used_contexts = (uint32_t*) 0; // doubly-linked list of used contexts
uint32_t* free_contexts = (uint32_t*) 0; // singly-linked list of free contexts

// ------------------------- INITIALIZATION ------------------------

void reset_microkernel() {
  current_context = (uint32_t*) 0;

  while (used_contexts != (uint32_t*) 0)
    used_contexts = delete_context(used_contexts, used_contexts);
}

// -----------------------------------------------------------------
// ---------------------------- KERNEL -----------------------------
// -----------------------------------------------------------------

uint32_t pavailable();
uint32_t pexcess();
uint32_t pused();

uint32_t* palloc();
void      pfree(uint32_t* frame);

void map_and_store(uint32_t* context, uint32_t vaddr, uint32_t data);

void up_load_binary(uint32_t* context);

uint32_t up_load_string(uint32_t* context, uint32_t* s, uint32_t SP);
void     up_load_arguments(uint32_t* context, uint32_t argc, uint32_t* argv);

uint32_t handle_system_call(uint32_t* context);
uint32_t handle_page_fault(uint32_t* context);
uint32_t handle_division_by_zero(uint32_t* context);
uint32_t handle_max_trace(uint32_t* context);
uint32_t handle_timer(uint32_t* context);

uint32_t handle_exception(uint32_t* context);

uint32_t mipster(uint32_t* to_context);
uint32_t hypster(uint32_t* to_context);

uint32_t mixter(uint32_t* to_context, uint32_t mix);

uint32_t minmob(uint32_t* to_context);
void     map_unmapped_pages(uint32_t* context);
uint32_t minster(uint32_t* to_context);
uint32_t mobster(uint32_t* to_context);

void     backtrack_trace(uint32_t* context);
uint32_t monster(uint32_t* to_context);

uint32_t is_boot_level_zero();

uint32_t selfie_run(uint32_t machine);

// ------------------------ GLOBAL CONSTANTS -----------------------

uint32_t* MY_CONTEXT = (uint32_t*) 0;

uint32_t DONOTEXIT = 0;
uint32_t EXIT      = 1;

uint32_t EXITCODE_NOERROR                = 0;
uint32_t EXITCODE_BADARGUMENTS           = 1;
uint32_t EXITCODE_IOERROR                = 2;
uint32_t EXITCODE_SCANNERERROR           = 3;
uint32_t EXITCODE_PARSERERROR            = 4;
uint32_t EXITCODE_COMPILERERROR          = 5;
uint32_t EXITCODE_OUTOFVIRTUALMEMORY     = 6;
uint32_t EXITCODE_OUTOFPHYSICALMEMORY    = 7;
uint32_t EXITCODE_DIVISIONBYZERO         = 8;
uint32_t EXITCODE_UNKNOWNINSTRUCTION     = 9;
uint32_t EXITCODE_UNKNOWNSYSCALL         = 10;
uint32_t EXITCODE_MULTIPLEEXCEPTIONERROR = 11;
uint32_t EXITCODE_SYMBOLICEXECUTIONERROR = 12;
uint32_t EXITCODE_OUTOFTRACEMEMORY       = 13;
uint32_t EXITCODE_UNCAUGHTEXCEPTION      = 14;

uint32_t MIPSTER = 1;
uint32_t DIPSTER = 2;
uint32_t RIPSTER = 3;

uint32_t MONSTER = 4;

uint32_t MINSTER = 5;
uint32_t MOBSTER = 6;

uint32_t HYPSTER = 7;

// ------------------------ GLOBAL VARIABLES -----------------------

uint32_t next_page_frame = 0;

uint32_t allocated_page_frame_memory = 0;
uint32_t free_page_frame_memory      = 0;

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ----------------   T H E O R E M  P R O V E R    ----------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// -------------------------- SAT Solver ---------------------------
// -----------------------------------------------------------------

uint32_t clause_may_be_true(uint32_t* clause_address, uint32_t depth);
uint32_t instance_may_be_true(uint32_t depth);

uint32_t babysat(uint32_t depth);

// ------------------------ GLOBAL CONSTANTS -----------------------

uint32_t FALSE = 0;
uint32_t TRUE  = 1;

uint32_t UNSAT = 0;
uint32_t SAT   = 1;

// ------------------------ GLOBAL VARIABLES -----------------------

uint32_t* dimacs_name = (uint32_t*) 0;

uint32_t number_of_sat_variables = 0;

// number_of_sat_variables
uint32_t* sat_assignment = (uint32_t*) 0;

uint32_t number_of_sat_clauses = 0;

// number_of_sat_clauses * 2 * number_of_sat_variables
uint32_t* sat_instance = (uint32_t*) 0;

// -----------------------------------------------------------------
// ----------------------- DIMACS CNF PARSER -----------------------
// -----------------------------------------------------------------

void selfie_print_dimacs();

void     dimacs_find_next_character(uint32_t new_line);
void     dimacs_get_symbol();
void     dimacs_word(uint32_t* word);
uint32_t dimacs_number();
void     dimacs_get_clause(uint32_t clause);
void     dimacs_get_instance();

void selfie_load_dimacs();

void selfie_sat();

// -----------------------------------------------------------------
// ----------------------------- MAIN ------------------------------
// -----------------------------------------------------------------

void init_selfie(uint32_t argc, uint32_t* argv);

uint32_t  number_of_remaining_arguments();
uint32_t* remaining_arguments();

uint32_t* peek_argument();
uint32_t* get_argument();
void      set_argument(uint32_t* argv);

void print_usage();

// ------------------------ GLOBAL VARIABLES -----------------------

uint32_t  selfie_argc = 0;
uint32_t* selfie_argv = (uint32_t*) 0;

uint32_t* selfie_name = (uint32_t*) 0;

// ------------------------- INITIALIZATION ------------------------

void init_selfie(uint32_t argc, uint32_t* argv) {
  selfie_argc = argc;
  selfie_argv = argv;

  selfie_name = get_argument();
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     L I B R A R Y     ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ----------------------- LIBRARY PROCEDURES ----------------------
// -----------------------------------------------------------------

uint32_t two_to_the_power_of(uint32_t p) {
  // assert: 0 <= p < CPUBITWIDTH
  return *(power_of_two_table + p);
}

uint32_t ten_to_the_power_of(uint32_t p) {
  // use recursion for simplicity and educational value
  // for p close to 0 performance is not relevant
  if (p == 0)
    return 1;
  else
    return ten_to_the_power_of(p - 1) * 10;
}

uint32_t left_shift(uint32_t n, uint32_t b) {
  // assert: 0 <= b < CPUBITWIDTH
  return n * two_to_the_power_of(b);
}

uint32_t right_shift(uint32_t n, uint32_t b) {
  // assert: 0 <= b < CPUBITWIDTH
  return n / two_to_the_power_of(b);
}

uint32_t get_bits(uint32_t n, uint32_t i, uint32_t b) {
  // assert: 0 < b <= i + b < CPUBITWIDTH
  if (i == 0)
    return n % two_to_the_power_of(b);
  else
    // shift to-be-loaded bits all the way to the left
    // to reset all bits to the left of them, then
    // shift to-be-loaded bits all the way to the right and return
    return right_shift(left_shift(n, CPUBITWIDTH - (i + b)), CPUBITWIDTH - b);
}

uint32_t selfie_abs(uint32_t n) {
  if (signed_less_than(n, 0))
    return -n;
  else
    return n;
}

uint32_t signed_less_than(uint32_t a, uint32_t b) {
  // INT32_MIN <= n <= INT32_MAX iff
  // INT32_MIN + INT32_MIN <= n + INT32_MIN <= INT32_MAX + INT32_MIN iff
  // -2^32 <= n + INT32_MIN <= 2^32 - 1 (sign-extended to 33 bits) iff
  // 0 <= n + INT32_MIN <= UINT32_MAX
  return a + INT32_MIN < b + INT32_MIN;
}

uint32_t signed_division(uint32_t a, uint32_t b) {
  // assert: b != 0
  // assert: a == INT32_MIN -> b != -1
  if (a == INT32_MIN)
    if (b == INT32_MIN)
      return 1;
    else if (signed_less_than(b, 0))
      return INT32_MIN / selfie_abs(b);
    else
      return -(INT32_MIN / b);
  else if (b == INT32_MIN)
    return 0;
  else if (signed_less_than(a, 0))
    if (signed_less_than(b, 0))
      return selfie_abs(a) / selfie_abs(b);
    else
      return -(selfie_abs(a) / b);
  else if (signed_less_than(b, 0))
    return -(a / selfie_abs(b));
  else
    return a / b;
}

uint32_t is_signed_integer(uint32_t n, uint32_t b) {
  // assert: 0 < b <= CPUBITWIDTH
  if (n < two_to_the_power_of(b - 1))
    // assert: 0 <= n < 2^(b - 1)
    return 1;
  else if (n >= -two_to_the_power_of(b - 1))
    // assert: -2^(b - 1) <= n < 2^32
    return 1;
  else
    return 0;
}

uint32_t sign_extend(uint32_t n, uint32_t b) {
  // assert: 0 <= n <= 2^b
  // assert: 0 < b < CPUBITWIDTH
  if (n < two_to_the_power_of(b - 1))
    return n;
  else
    return n - two_to_the_power_of(b);
}

uint32_t sign_shrink(uint32_t n, uint32_t b) {
  // assert: -2^(b - 1) <= n < 2^(b - 1)
  // assert: 0 < b < CPUBITWIDTH
  return get_bits(n, 0, b);
}

uint32_t load_character(uint32_t* s, uint32_t i) {
  // assert: i >= 0
  uint32_t a;

  // a is the index of the word where
  // the to-be-loaded i-th character in s is
  a = i / SIZEOFUINT32;

  // return i-th 8-bit character in s
  return get_bits(*(s + a), (i % SIZEOFUINT32) * 8, 8);
}

uint32_t* store_character(uint32_t* s, uint32_t i, uint32_t c) {
  // assert: i >= 0, 0 <= c < 2^8 (all characters are 8-bit)
  uint32_t a;

  // a is the index of the word where
  // the with c to-be-overwritten i-th character in s is
  a = i / SIZEOFUINT32;

  // subtract the to-be-overwritten character to reset its bits in s
  // then add c to set its bits at the i-th position in s
  *(s + a) = (*(s + a) - left_shift(load_character(s, i), (i % SIZEOFUINT32) * 8)) + left_shift(c, (i % SIZEOFUINT32) * 8);

  return s;
}

uint32_t string_length(uint32_t* s) {
  uint32_t i;

  i = 0;

  while (load_character(s, i) != 0)
    i = i + 1;

  return i;
}

uint32_t* string_copy(uint32_t* s) {
  uint32_t l;
  uint32_t* t;
  uint32_t i;

  l = string_length(s);

  t = zalloc(l + 1);

  i = 0;

  while (i <= l) {
    store_character(t, i, load_character(s, i));

    i = i + 1;
  }

  return t;
}

void string_reverse(uint32_t* s) {
  uint32_t i;
  uint32_t j;
  uint32_t tmp;

  i = 0;
  j = string_length(s) - 1;

  while (i < j) {
    tmp = load_character(s, i);

    store_character(s, i, load_character(s, j));
    store_character(s, j, tmp);

    i = i + 1;
    j = j - 1;
  }
}

uint32_t string_compare(uint32_t* s, uint32_t* t) {
  uint32_t i;

  i = 0;

  while (1)
    if (load_character(s, i) == 0)
      if (load_character(t, i) == 0)
        return 1;
      else
        return 0;
    else if (load_character(s, i) == load_character(t, i))
      i = i + 1;
    else
      return 0;
}

uint32_t atoi(uint32_t* s) {
  uint32_t i;
  uint32_t n;
  uint32_t c;

  // the conversion of the ASCII string in s to its
  // numerical value n begins with the leftmost digit in s
  i = 0;

  // and the numerical value 0 for n
  n = 0;

  // load character (one byte) at index i in s from memory requires
  // bit shifting since memory access can only be done in words
  c = load_character(s, i);

  // loop until s is terminated
  while (c != 0) {
    // the numerical value of ASCII-encoded decimal digits
    // is offset by the ASCII code of '0' (which is 48)
    c = c - '0';

    if (c > 9) {
      printf2((uint32_t*) "%s: cannot convert non-decimal number %s\n", selfie_name, s);

      exit(EXITCODE_BADARGUMENTS);
    }

    // assert: s contains a decimal number

    // use base 10 but detect wrap around
    if (n < UINT32_MAX / 10)
      n = n * 10 + c;
    else if (n == UINT32_MAX / 10)
      if (c <= UINT32_MAX % 10)
        n = n * 10 + c;
      else {
        // s contains a decimal number larger than UINT32_MAX
        printf2((uint32_t*) "%s: cannot convert out-of-bound number %s\n", selfie_name, s);

        exit(EXITCODE_BADARGUMENTS);
      }
    else {
      // s contains a decimal number larger than UINT32_MAX
      printf2((uint32_t*) "%s: cannot convert out-of-bound number %s\n", selfie_name, s);

      exit(EXITCODE_BADARGUMENTS);
    }

    // go to the next digit
    i = i + 1;

    // load character (one byte) at index i in s from memory requires
    // bit shifting since memory access can only be done in words
    c = load_character(s, i);
  }

  return n;
}

uint32_t* itoa(uint32_t n, uint32_t* s, uint32_t b, uint32_t a) {
  // assert: b in {2,4,8,10,16}

  uint32_t i;
  uint32_t sign;

  // the conversion of the integer n to an ASCII string in s with
  // base b and alignment a begins with the leftmost digit in s
  i = 0;

  // for now assuming n is positive
  sign = 0;

  if (n == 0) {
    store_character(s, 0, '0');

    i = 1;
  } else if (signed_less_than(n, 0)) {
    if (b == 10) {
      // n is represented as two's complement
      // convert n to a positive number but remember the sign
      n = -n;

      sign = 1;
    }
  }

  while (n != 0) {
    if (n % b > 9)
      // the ASCII code of hexadecimal digits larger than 9
      // is offset by the ASCII code of 'A' (which is 65)
      store_character(s, i, n % b - 10 + 'A');
    else
      // the ASCII code of digits less than or equal to 9
      // is offset by the ASCII code of '0' (which is 48)
      store_character(s, i, n % b + '0');

    // convert n by dividing n with base b
    n = n / b;

    i = i + 1;
  }

  if (b == 10) {
    if (sign) {
      store_character(s, i, '-'); // negative decimal numbers start with -

      i = i + 1;
    }

    while (i < a) {
      store_character(s, i, ' '); // align with spaces

      i = i + 1;
    }
  } else {
    while (i < a) {
      store_character(s, i, '0'); // align with 0s

      i = i + 1;
    }

    if (b == 8) {
      store_character(s, i, '0'); // octal numbers start with 00
      store_character(s, i + 1, '0');

      i = i + 2;
    } else if (b == 16) {
      store_character(s, i, 'x'); // hexadecimal numbers start with 0x
      store_character(s, i + 1, '0');

      i = i + 2;
    }
  }

  store_character(s, i, 0); // null-terminated string

  // our numeral system is positional hindu-arabic, that is,
  // the weight of digits increases right to left, which means
  // that we need to reverse the string we computed above
  string_reverse(s);

  return s;
}

uint32_t fixed_point_ratio(uint32_t a, uint32_t b, uint32_t f) {
  // compute fixed point ratio with f fractional digits
  // multiply a/b with 10^f but avoid wrap around

  uint32_t p;

  p = f;

  while (p > 0) {
    if (a <= UINT32_MAX / ten_to_the_power_of(p)) {
      if (b / ten_to_the_power_of(f - p) != 0)
        return (a * ten_to_the_power_of(p)) / (b / ten_to_the_power_of(f - p));
    }

    p = p - 1;
  }

  return 0;
}

uint32_t fixed_point_percentage(uint32_t r, uint32_t f) {
  if (r != 0)
    // 10^4 (for 100.00%) * 10^f (for f fractional digits of r)
    return ten_to_the_power_of(4 + f) / r;
  else
    return 0;
}

void put_character(uint32_t c) {
  *character_buffer = c;

  // assert: character_buffer is mapped

  // try to write 1 character from character_buffer
  // into file with output_fd file descriptor
  if (write(output_fd, character_buffer, 1) == 1) {
    if (output_fd != 1)
      // count number of characters written to a file,
      // not the console which has file descriptor 1
      number_of_written_characters = number_of_written_characters + 1;
  } else {
    // write failed
    if (output_fd != 1) {
      // failed write was not to the console which has file descriptor 1
      // to report the error we may thus still write to the console
      output_fd = 1;

      printf2((uint32_t*) "%s: could not write character to output file %s\n", selfie_name, output_name);
    }

    exit(EXITCODE_IOERROR);
  }
}

void print(uint32_t* s) {
  uint32_t i;

  if (s == (uint32_t*) 0)
    print((uint32_t*) "NULL");
  else {
    i = 0;

    while (load_character(s, i) != 0) {
      put_character(load_character(s, i));

      i = i + 1;
    }
  }
}

void println() {
  put_character(CHAR_LF);
}

void print_character(uint32_t c) {
  put_character(CHAR_SINGLEQUOTE);

  if (c == CHAR_EOF)
    print((uint32_t*) "end of file");
  else if (c == CHAR_TAB)
    print((uint32_t*) "tabulator");
  else if (c == CHAR_LF)
    print((uint32_t*) "line feed");
  else if (c == CHAR_CR)
    print((uint32_t*) "carriage return");
  else
    put_character(c);

  put_character(CHAR_SINGLEQUOTE);
}

void print_string(uint32_t* s) {
  put_character(CHAR_DOUBLEQUOTE);

  print(s);

  put_character(CHAR_DOUBLEQUOTE);
}

void print_integer(uint32_t n) {
  print(itoa(n, integer_buffer, 10, 0));
}

void unprint_integer(uint32_t n) {
  n = string_length(itoa(n, integer_buffer, 10, 0));

  while (n > 0) {
    put_character(CHAR_BACKSPACE);

    n = n - 1;
  }
}

void print_hexadecimal(uint32_t n, uint32_t a) {
  print(itoa(n, integer_buffer, 16, a));
}

void print_octal(uint32_t n, uint32_t a) {
  print(itoa(n, integer_buffer, 8, a));
}

void print_binary(uint32_t n, uint32_t a) {
  print(itoa(n, integer_buffer, 2, a));
}

uint32_t print_format0(uint32_t* s, uint32_t i) {
  // print string s from index i on
  // ignore % formatting codes except for %%
  if (s == (uint32_t*) 0)
    return 0;
  else {
    while (load_character(s, i) != 0) {
      if (load_character(s, i) != '%') {
        put_character(load_character(s, i));

        i = i + 1;
      } else if (load_character(s, i + 1) == '%') {
        // for %% print just one %
        put_character('%');

        i = i + 2;
      } else {
        put_character(load_character(s, i));

        i = i + 1;
      }
    }

    return i;
  }
}

uint32_t print_format1(uint32_t* s, uint32_t i, uint32_t* a) {
  // print string s from index i on until next % formatting code except for %%
  // then print argument a according to the encountered % formatting code

  uint32_t p;

  if (s == (uint32_t*) 0)
    return 0;
  else {
    while (load_character(s, i) != 0) {
      if (load_character(s, i) != '%') {
        put_character(load_character(s, i));

        i = i + 1;
      } else if (load_character(s, i + 1) == 's') {
        print(a);

        return i + 2;
      } else if (load_character(s, i + 1) == 'c') {
        put_character((uint32_t) a);

        return i + 2;
      } else if (load_character(s, i + 1) == 'd') {
        print_integer((uint32_t) a);

        return i + 2;
      } else if (load_character(s, i + 1) == '.') {
        // for simplicity we support a single digit only
        p = load_character(s, i + 2) - '0';

        if (p < 10) {
          // the character at i + 2 is in fact a digit
          print_integer((uint32_t) a / ten_to_the_power_of(p));

          if (p > 0) {
            // using integer_buffer here is ok since we are not using print_integer
            itoa((uint32_t) a % ten_to_the_power_of(p), integer_buffer, 10, 0);
            p = p - string_length(integer_buffer);

            put_character('.');
            while (p > 0) {
              put_character('0');

              p = p - 1;
            }
            print(integer_buffer);
          }

          return i + 4;
        } else {
          put_character(load_character(s, i));

          i = i + 1;
        }
      } else if (load_character(s, i + 1) == 'p') {
        print_hexadecimal((uint32_t) a, SIZEOFUINT32STAR);

        return i + 2;
      } else if (load_character(s, i + 1) == 'x') {
        print_hexadecimal((uint32_t) a, 0);

        return i + 2;
      } else if (load_character(s, i + 1) == 'o') {
        print_octal((uint32_t) a, 0);

        return i + 2;
      } else if (load_character(s, i + 1) == 'b') {
        print_binary((uint32_t) a, 0);

        return i + 2;
      } else if (load_character(s, i + 1) == '%') {
        // for %% print just one %
        put_character('%');

        i = i + 2;
      } else {
        put_character(load_character(s, i));

        i = i + 1;
      }
    }

    return i;
  }
}

void printf1(uint32_t* s, uint32_t* a1) {
  print_format0(s, print_format1(s, 0, a1));
}

void printf2(uint32_t* s, uint32_t* a1, uint32_t* a2) {
  print_format0(s, print_format1(s, print_format1(s, 0, a1), a2));
}

void printf3(uint32_t* s, uint32_t* a1, uint32_t* a2, uint32_t* a3) {
  print_format0(s, print_format1(s, print_format1(s, print_format1(s, 0, a1), a2), a3));
}

void printf4(uint32_t* s, uint32_t* a1, uint32_t* a2, uint32_t* a3, uint32_t* a4) {
  print_format0(s, print_format1(s, print_format1(s, print_format1(s, print_format1(s, 0, a1), a2), a3), a4));
}

void printf5(uint32_t* s, uint32_t* a1, uint32_t* a2, uint32_t* a3, uint32_t* a4, uint32_t* a5) {
  print_format0(s, print_format1(s, print_format1(s, print_format1(s, print_format1(s, print_format1(s, 0, a1), a2), a3), a4), a5));
}

void printf6(uint32_t* s, uint32_t* a1, uint32_t* a2, uint32_t* a3, uint32_t* a4, uint32_t* a5, uint32_t* a6) {
  print_format0(s, print_format1(s, print_format1(s, print_format1(s, print_format1(s, print_format1(s, print_format1(s, 0, a1), a2), a3), a4), a5), a6));
}

uint32_t round_up(uint32_t n, uint32_t m) {
  if (n % m == 0)
    return n;
  else
    return n - n % m + m;
}

uint32_t* smalloc(uint32_t size) {
  // this procedure ensures a defined program exit,
  // if no memory can be allocated
  uint32_t* memory;

  memory = malloc(size);

  if (size == 0)
    // any address including null
    return memory;
  else if ((uint32_t) memory == 0) {
    printf1((uint32_t*) "%s: malloc out of memory\n", selfie_name);

    exit(EXITCODE_OUTOFVIRTUALMEMORY);
  }

  return memory;
}

uint32_t* zalloc(uint32_t size) {
  // this procedure is only executed at boot level zero
  // zalloc allocates size bytes rounded up to word size
  // and then zeroes that memory, similar to calloc, but
  // called zalloc to avoid redeclaring calloc
  uint32_t* memory;
  uint32_t  i;

  size = round_up(size, REGISTERSIZE);

  memory = smalloc(size);

  size = size / REGISTERSIZE;

  i = 0;

  while (i < size) {
    // erase memory by setting it to 0
    *(memory + i) = 0;

    i = i + 1;
  }

  return memory;
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------    C O M P I L E R    ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- SCANNER ----------------------------
// -----------------------------------------------------------------

void print_symbol(uint32_t symbol) {
  put_character(CHAR_DOUBLEQUOTE);

  if (symbol == SYM_EOF)
    print((uint32_t*) "end of file");
  else
    print((uint32_t*) *(SYMBOLS + symbol));

  put_character(CHAR_DOUBLEQUOTE);
}

void print_line_number(uint32_t* message, uint32_t line) {
  printf4((uint32_t*) "%s: %s in %s in line %d: ", selfie_name, message, source_name, (uint32_t*) line);
}

void syntax_error_message(uint32_t* message) {
  print_line_number((uint32_t*) "syntax error", line_number);
  printf1((uint32_t*) "%s\n", message);
}

void syntax_error_character(uint32_t expected) {
  print_line_number((uint32_t*) "syntax error", line_number);
  print_character(expected);
  print((uint32_t*) " expected but ");
  print_character(character);
  print((uint32_t*) " found\n");
}

void syntax_error_identifier(uint32_t* expected) {
  print_line_number((uint32_t*) "syntax error", line_number);
  print_string(expected);
  print((uint32_t*) " expected but ");
  print_string(identifier);
  print((uint32_t*) " found\n");
}

void get_character() {
  uint32_t number_of_read_bytes;

  // assert: character_buffer is mapped

  // try to read 1 character into character_buffer
  // from file with source_fd file descriptor
  number_of_read_bytes = read(source_fd, character_buffer, 1);

  if (number_of_read_bytes == 1) {
    // store the read character in the global variable called character
    character = *character_buffer;

    number_of_read_characters = number_of_read_characters + 1;
  } else if (number_of_read_bytes == 0)
    // reached end of file
    character = CHAR_EOF;
  else {
    printf2((uint32_t*) "%s: could not read character from input file %s\n", selfie_name, source_name);

    exit(EXITCODE_IOERROR);
  }
}

uint32_t is_character_new_line() {
  if (character == CHAR_LF)
    return 1;
  else if (character == CHAR_CR)
    return 1;
  else
    return 0;
}

uint32_t is_character_whitespace() {
  if (character == CHAR_SPACE)
    return 1;
  else if (character == CHAR_TAB)
    return 1;
  else
    return is_character_new_line();
}

uint32_t find_next_character() {
  uint32_t in_single_line_comment;
  uint32_t in_multi_line_comment;

  // assuming we are not in a comment
  in_single_line_comment = 0;
  in_multi_line_comment  = 0;

  // read and discard all whitespace and comments until a character is found
  // that is not whitespace and does not occur in a comment, or the file ends
  while (1) {
    if (in_single_line_comment) {
      get_character();

      if (is_character_new_line())
        // single-line comments end with new line
        in_single_line_comment = 0;
      else if (character == CHAR_EOF)
        // or end of file
        return character;
      else
        // count the characters in comments as ignored characters
        number_of_ignored_characters = number_of_ignored_characters + 1;

    } else if (in_multi_line_comment) {
      get_character();

      if (character == CHAR_ASTERISK) {
        // look for '*/' and here count '*' as ignored character
        number_of_ignored_characters = number_of_ignored_characters + 1;

        get_character();

        if (character == CHAR_SLASH) {
          // multi-line comments end with "*/"
          in_multi_line_comment = 0;

          get_character();
        }
      }


      if (in_multi_line_comment) {
        // keep track of line numbers for error reporting and code annotation
        if (character == CHAR_LF)
          // only linefeeds count, not carriage returns
          line_number = line_number + 1;
        else if (character == CHAR_EOF) {
          // multi-line comment is not terminated
          syntax_error_message((uint32_t*) "runaway multi-line comment");

          exit(EXITCODE_SCANNERERROR);
        }
      }

      // count the characters in comments as ignored characters including '/' in '*/'
      number_of_ignored_characters = number_of_ignored_characters + 1;

    } else if (is_character_whitespace()) {
      if (character == CHAR_LF)
        line_number = line_number + 1;

      // also count line feed and carriage return as ignored characters
      number_of_ignored_characters = number_of_ignored_characters + 1;

      get_character();

    } else if (character == CHAR_SLASH) {
      get_character();

      if (character == CHAR_SLASH) {
        // "//" begins a comment
        in_single_line_comment = 1;

        // count both slashes as ignored characters
        number_of_ignored_characters = number_of_ignored_characters + 2;

        number_of_comments = number_of_comments + 1;
      } else if (character == CHAR_ASTERISK) {
        // "/*" begins a multi-line comment
        in_multi_line_comment = 1;

        // count both slash and asterisk as ignored characters
        number_of_ignored_characters = number_of_ignored_characters + 2;

        number_of_comments = number_of_comments + 1;
      } else {
        // while looking for "//" and "/*" we actually found '/'
        symbol = SYM_DIV;

        return character;
      }

    } else
      // character found that is not whitespace and not occurring in a comment
      return character;
  }
}

uint32_t is_character_letter() {
  // ASCII codes for lower- and uppercase letters are in contiguous intervals
  if (character >= 'a')
    if (character <= 'z')
      return 1;
    else
      return 0;
  else if (character >= 'A')
    if (character <= 'Z')
      return 1;
    else
      return 0;
  else
    return 0;
}

uint32_t is_character_digit() {
  // ASCII codes for digits are in a contiguous interval
  if (character >= '0')
    if (character <= '9')
      return 1;
    else
      return 0;
  else
    return 0;
}

uint32_t is_character_letter_or_digit_or_underscore() {
  if (is_character_letter())
    return 1;
  else if (is_character_digit())
    return 1;
  else if (character == CHAR_UNDERSCORE)
    return 1;
  else
    return 0;
}

uint32_t is_character_not_double_quote_or_new_line_or_eof() {
  if (character == CHAR_DOUBLEQUOTE)
    return 0;
  else if (is_character_new_line())
    return 0;
  else if (character == CHAR_EOF)
    return 0;
  else
    return 1;
}

uint32_t identifier_string_match(uint32_t keyword) {
  return string_compare(identifier, (uint32_t*) *(SYMBOLS + keyword));
}

uint32_t identifier_or_keyword() {
  if (identifier_string_match(SYM_WHILE))
    return SYM_WHILE;
  if (identifier_string_match(SYM_IF))
    return SYM_IF;
  if (identifier_string_match(SYM_UINT32))
    return SYM_UINT32;
  if (identifier_string_match(SYM_ELSE))
    return SYM_ELSE;
  if (identifier_string_match(SYM_RETURN))
    return SYM_RETURN;
  if (identifier_string_match(SYM_VOID))
    return SYM_VOID;
  else
    return SYM_IDENTIFIER;
}

void get_symbol() {
  uint32_t i;

  // reset previously scanned symbol
  symbol = SYM_EOF;

  if (find_next_character() != CHAR_EOF) {
    if (symbol != SYM_DIV) {
      // '/' may have already been recognized
      // while looking for whitespace and "//"
      if (is_character_letter()) {
        // accommodate identifier and null for termination
        identifier = smalloc(MAX_IDENTIFIER_LENGTH + 1);

        i = 0;

        while (is_character_letter_or_digit_or_underscore()) {
          if (i >= MAX_IDENTIFIER_LENGTH) {
            syntax_error_message((uint32_t*) "identifier too long");

            exit(EXITCODE_SCANNERERROR);
          }

          store_character(identifier, i, character);

          i = i + 1;

          get_character();
        }

        store_character(identifier, i, 0); // null-terminated string

        symbol = identifier_or_keyword();

      } else if (is_character_digit()) {
        // accommodate integer and null for termination
        integer = smalloc(MAX_INTEGER_LENGTH + 1);

        i = 0;

        while (is_character_digit()) {
          if (i >= MAX_INTEGER_LENGTH) {
            if (integer_is_signed)
              syntax_error_message((uint32_t*) "signed integer out of bound");
            else
              syntax_error_message((uint32_t*) "integer out of bound");

            exit(EXITCODE_SCANNERERROR);
          }

          store_character(integer, i, character);

          i = i + 1;

          get_character();
        }

        store_character(integer, i, 0); // null-terminated string

        literal = atoi(integer);

        if (integer_is_signed)
          if (literal > INT32_MIN) {
              syntax_error_message((uint32_t*) "signed integer out of bound");

              exit(EXITCODE_SCANNERERROR);
            }

        symbol = SYM_INTEGER;

      } else if (character == CHAR_SINGLEQUOTE) {
        get_character();

        literal = 0;

        if (character == CHAR_EOF) {
          syntax_error_message((uint32_t*) "reached end of file looking for a character literal");

          exit(EXITCODE_SCANNERERROR);
        } else
          literal = character;

        get_character();

        if (character == CHAR_SINGLEQUOTE)
          get_character();
        else if (character == CHAR_EOF) {
          syntax_error_character(CHAR_SINGLEQUOTE);

          exit(EXITCODE_SCANNERERROR);
        } else
          syntax_error_character(CHAR_SINGLEQUOTE);

        symbol = SYM_CHARACTER;

      } else if (character == CHAR_DOUBLEQUOTE) {
        get_character();

        // accommodate string and null for termination,
        // allocate zeroed memory since strings are emitted
        // in words but may end non-word-aligned
        string = zalloc(MAX_STRING_LENGTH + 1);

        i = 0;

        while (is_character_not_double_quote_or_new_line_or_eof()) {
          if (i >= MAX_STRING_LENGTH) {
            syntax_error_message((uint32_t*) "string too long");

            exit(EXITCODE_SCANNERERROR);
          }

          if (character == CHAR_BACKSLASH)
            handle_escape_sequence();

          store_character(string, i, character);

          i = i + 1;

          get_character();
        }

        if (character == CHAR_DOUBLEQUOTE)
          get_character();
        else {
          syntax_error_character(CHAR_DOUBLEQUOTE);

          exit(EXITCODE_SCANNERERROR);
        }

        store_character(string, i, 0); // null-terminated string

        symbol = SYM_STRING;

      } else if (character == CHAR_SEMICOLON) {
        get_character();

        symbol = SYM_SEMICOLON;

      } else if (character == CHAR_PLUS) {
        get_character();

        symbol = SYM_PLUS;

      } else if (character == CHAR_DASH) {
        get_character();

        symbol = SYM_MINUS;

      } else if (character == CHAR_ASTERISK) {
        get_character();

        symbol = SYM_ASTERISK;

      } else if (character == CHAR_EQUAL) {
        get_character();

        if (character == CHAR_EQUAL) {
          get_character();

          symbol = SYM_EQUALITY;
        } else
          symbol = SYM_ASSIGN;

      } else if (character == CHAR_LPARENTHESIS) {
        get_character();

        symbol = SYM_LPARENTHESIS;

      } else if (character == CHAR_RPARENTHESIS) {
        get_character();

        symbol = SYM_RPARENTHESIS;

      } else if (character == CHAR_LBRACE) {
        get_character();

        symbol = SYM_LBRACE;

      } else if (character == CHAR_RBRACE) {
        get_character();

        symbol = SYM_RBRACE;

      } else if (character == CHAR_COMMA) {
        get_character();

        symbol = SYM_COMMA;

      } else if (character == CHAR_LT) {
        get_character();

        if (character == CHAR_EQUAL) {
          get_character();

          symbol = SYM_LEQ;
        } else
          symbol = SYM_LT;

      } else if (character == CHAR_GT) {
        get_character();

        if (character == CHAR_EQUAL) {
          get_character();

          symbol = SYM_GEQ;
        } else
          symbol = SYM_GT;

      } else if (character == CHAR_EXCLAMATION) {
        get_character();

        if (character == CHAR_EQUAL)
          get_character();
        else
          syntax_error_character(CHAR_EQUAL);

        symbol = SYM_NOTEQ;

      } else if (character == CHAR_PERCENTAGE) {
        get_character();

        symbol = SYM_MOD;

      } else {
        print_line_number((uint32_t*) "syntax error", line_number);
        print((uint32_t*) "found unknown character ");
        print_character(character);
        println();

        exit(EXITCODE_SCANNERERROR);
      }
    }

    number_of_scanned_symbols = number_of_scanned_symbols + 1;
  }
}

void handle_escape_sequence() {
  // ignoring the backslash
  number_of_ignored_characters = number_of_ignored_characters + 1;

  get_character();

  if (character == 'n')
    character = CHAR_LF;
  else if (character == 't')
    character = CHAR_TAB;
  else if (character == 'b')
    character = CHAR_BACKSPACE;
  else if (character == CHAR_SINGLEQUOTE)
    character = CHAR_SINGLEQUOTE;
  else if (character == CHAR_DOUBLEQUOTE)
    character = CHAR_DOUBLEQUOTE;
  else if (character == CHAR_PERCENTAGE)
    character = CHAR_PERCENTAGE;
  else if (character == CHAR_BACKSLASH)
    character = CHAR_BACKSLASH;
  else {
    syntax_error_message((uint32_t*) "unknown escape sequence found");

    exit(EXITCODE_SCANNERERROR);
  }
}

// -----------------------------------------------------------------
// ------------------------- SYMBOL TABLE --------------------------
// -----------------------------------------------------------------

uint32_t hash(uint32_t* key) {
  // assert: key != (uint32_t*) 0
  return (*key + (*key + (*key + (*key + (*key + *key / HASH_TABLE_SIZE) / HASH_TABLE_SIZE) / HASH_TABLE_SIZE) / HASH_TABLE_SIZE) / HASH_TABLE_SIZE) % HASH_TABLE_SIZE;
}

void create_symbol_table_entry(uint32_t which_table, uint32_t* string, uint32_t line, uint32_t class, uint32_t type, uint32_t value, uint32_t address) {
  uint32_t* new_entry;
  uint32_t* hashed_entry_address;

  new_entry = smalloc(2 * SIZEOFUINT32STAR + 6 * SIZEOFUINT32);

  set_string(new_entry, string);
  set_line_number(new_entry, line);
  set_class(new_entry, class);
  set_type(new_entry, type);
  set_value(new_entry, value);
  set_address(new_entry, address);

  // create entry at head of list of symbols
  if (which_table == GLOBAL_TABLE) {
    set_scope(new_entry, REG_GP);

    hashed_entry_address = global_symbol_table + hash(string);

    set_next_entry(new_entry, (uint32_t*) *hashed_entry_address);
    *hashed_entry_address = (uint32_t) new_entry;

    if (class == VARIABLE)
      number_of_global_variables = number_of_global_variables + 1;
    else if (class == PROCEDURE)
      number_of_procedures = number_of_procedures + 1;
    else if (class == STRING)
      number_of_strings = number_of_strings + 1;
  } else if (which_table == LOCAL_TABLE) {
    set_scope(new_entry, REG_FP);
    set_next_entry(new_entry, local_symbol_table);
    local_symbol_table = new_entry;
  } else {
    // library procedures
    set_scope(new_entry, REG_GP);
    set_next_entry(new_entry, library_symbol_table);
    library_symbol_table = new_entry;
  }
}

uint32_t* search_symbol_table(uint32_t* entry, uint32_t* string, uint32_t class) {
  number_of_searches = number_of_searches + 1;

  while (entry != (uint32_t*) 0) {
    total_search_time = total_search_time + 1;

    if (string_compare(string, get_string(entry)))
      if (class == get_class(entry))
        return entry;

    // keep looking
    entry = get_next_entry(entry);
  }

  return (uint32_t*) 0;
}

uint32_t* search_global_symbol_table(uint32_t* string, uint32_t class) {
  return search_symbol_table((uint32_t*) *(global_symbol_table + hash(string)), string, class);
}

uint32_t* get_scoped_symbol_table_entry(uint32_t* string, uint32_t class) {
  uint32_t* entry;

  if (class == VARIABLE)
    // local variables override global variables
    entry = search_symbol_table(local_symbol_table, string, VARIABLE);
  else if (class == PROCEDURE)
    // library procedures override declared or defined procedures
    entry = search_symbol_table(library_symbol_table, string, PROCEDURE);
  else
    entry = (uint32_t*) 0;

  if (entry == (uint32_t*) 0)
    return search_global_symbol_table(string, class);
  else
    return entry;
}

uint32_t is_undefined_procedure(uint32_t* entry) {
  uint32_t* library_entry;

  if (get_class(entry) == PROCEDURE) {
    // library procedures override declared or defined procedures
    library_entry = search_symbol_table(library_symbol_table, get_string(entry), PROCEDURE);

    if (library_entry != (uint32_t*) 0)
      // procedure is library procedure
      return 0;
    else if (get_address(entry) == 0)
      // procedure declared but not defined
      return 1;
    else if (get_opcode(load_instruction(get_address(entry))) == OP_JAL)
      // procedure called but not defined
      return 1;
  }

  return 0;
}

uint32_t report_undefined_procedures() {
  uint32_t undefined;
  uint32_t i;
  uint32_t* entry;

  undefined = 0;

  i = 0;

  while (i < HASH_TABLE_SIZE) {
    entry = (uint32_t*) *(global_symbol_table + i);

    while (entry != (uint32_t*) 0) {
      if (is_undefined_procedure(entry)) {
        undefined = 1;

        print_line_number((uint32_t*) "syntax error", get_line_number(entry));
        printf1((uint32_t*) "procedure %s undefined\n", get_string(entry));
      }

      // keep looking
      entry = get_next_entry(entry);
    }

    i = i + 1;
  }

  return undefined;
}

// -----------------------------------------------------------------
// ---------------------------- PARSER -----------------------------
// -----------------------------------------------------------------

uint32_t is_not_rbrace_or_eof() {
  if (symbol == SYM_RBRACE)
    return 0;
  else if (symbol == SYM_EOF)
    return 0;
  else
    return 1;
}

uint32_t is_expression() {
  if (symbol == SYM_MINUS)
    return 1;
  else if (symbol == SYM_LPARENTHESIS)
    return 1;
  else if (symbol == SYM_IDENTIFIER)
    return 1;
  else if (symbol == SYM_INTEGER)
    return 1;
  else if (symbol == SYM_ASTERISK)
    return 1;
  else if (symbol == SYM_STRING)
    return 1;
  else if (symbol == SYM_CHARACTER)
    return 1;
  else
    return 0;
}

uint32_t is_literal() {
  if (symbol == SYM_INTEGER)
    return 1;
  else if (symbol == SYM_CHARACTER)
    return 1;
  else
    return 0;
}

uint32_t is_star_or_div_or_modulo() {
  if (symbol == SYM_ASTERISK)
    return 1;
  else if (symbol == SYM_DIV)
    return 1;
  else if (symbol == SYM_MOD)
    return 1;
  else
    return 0;
}

uint32_t is_plus_or_minus() {
  if (symbol == SYM_MINUS)
    return 1;
  else if (symbol == SYM_PLUS)
    return 1;
  else
    return 0;
}

uint32_t is_comparison() {
  if (symbol == SYM_EQUALITY)
    return 1;
  else if (symbol == SYM_NOTEQ)
    return 1;
  else if (symbol == SYM_LT)
    return 1;
  else if (symbol == SYM_GT)
    return 1;
  else if (symbol == SYM_LEQ)
    return 1;
  else if (symbol == SYM_GEQ)
    return 1;
  else
    return 0;
}

uint32_t look_for_factor() {
  if (symbol == SYM_ASTERISK)
    return 0;
  else if (symbol == SYM_MINUS)
    return 0;
  else if (symbol == SYM_IDENTIFIER)
    return 0;
  else if (symbol == SYM_INTEGER)
    return 0;
  else if (symbol == SYM_CHARACTER)
    return 0;
  else if (symbol == SYM_STRING)
    return 0;
  else if (symbol == SYM_LPARENTHESIS)
    return 0;
  else if (symbol == SYM_EOF)
    return 0;
  else
    return 1;
}

uint32_t look_for_statement() {
  if (symbol == SYM_ASTERISK)
    return 0;
  else if (symbol == SYM_IDENTIFIER)
    return 0;
  else if (symbol == SYM_WHILE)
    return 0;
  else if (symbol == SYM_IF)
    return 0;
  else if (symbol == SYM_RETURN)
    return 0;
  else if (symbol == SYM_EOF)
    return 0;
  else
    return 1;
}

uint32_t look_for_type() {
  if (symbol == SYM_UINT32)
    return 0;
  else if (symbol == SYM_VOID)
    return 0;
  else if (symbol == SYM_EOF)
    return 0;
  else
    return 1;
}

void talloc() {
  // we use registers REG_T0-REG_T6 for temporaries
  if (allocated_temporaries < NUMBEROFTEMPORARIES)
    allocated_temporaries = allocated_temporaries + 1;
  else {
    syntax_error_message((uint32_t*) "out of registers");

    exit(EXITCODE_COMPILERERROR);
  }
}

uint32_t current_temporary() {
  if (allocated_temporaries > 0)
    if (allocated_temporaries < 4)
      return REG_TP + allocated_temporaries;
    else
      return REG_S11 + allocated_temporaries - 3;
  else {
    syntax_error_message((uint32_t*) "illegal register access");

    exit(EXITCODE_COMPILERERROR);
  }
}

uint32_t previous_temporary() {
  if (allocated_temporaries > 1)
    if (allocated_temporaries == 4)
      return REG_T2;
    else
      return current_temporary() - 1;
  else {
    syntax_error_message((uint32_t*) "illegal register access");

    exit(EXITCODE_COMPILERERROR);
  }
}

uint32_t next_temporary() {
  if (allocated_temporaries < NUMBEROFTEMPORARIES)
    if (allocated_temporaries == 3)
      return REG_T3;
    else
      return current_temporary() + 1;
  else {
    syntax_error_message((uint32_t*) "out of registers");

    exit(EXITCODE_COMPILERERROR);
  }
}

void tfree(uint32_t number_of_temporaries) {
  if (allocated_temporaries >= number_of_temporaries)
    allocated_temporaries = allocated_temporaries - number_of_temporaries;
  else {
    syntax_error_message((uint32_t*) "illegal register deallocation");

    exit(EXITCODE_COMPILERERROR);
  }
}

void save_temporaries() {
  while (allocated_temporaries > 0) {
    // push temporary onto stack
    emit_addi(REG_SP, REG_SP, -REGISTERSIZE);
    emit_sw(REG_SP, 0, current_temporary());

    tfree(1);
  }
}

void restore_temporaries(uint32_t number_of_temporaries) {
  while (allocated_temporaries < number_of_temporaries) {
    talloc();

    // restore temporary from stack
    emit_lw(current_temporary(), REG_SP, 0);
    emit_addi(REG_SP, REG_SP, REGISTERSIZE);
  }
}

void syntax_error_symbol(uint32_t expected) {
  print_line_number((uint32_t*) "syntax error", line_number);
  print_symbol(expected);
  print((uint32_t*) " expected but ");
  print_symbol(symbol);
  print((uint32_t*) " found\n");
}

void syntax_error_unexpected() {
  print_line_number((uint32_t*) "syntax error", line_number);
  print((uint32_t*) "unexpected symbol ");
  print_symbol(symbol);
  print((uint32_t*) " found\n");
}

void print_type(uint32_t type) {
  if (type == UINT32_T)
    print((uint32_t*) "uint32_t");
  else if (type == UINT32STAR_T)
    print((uint32_t*) "uint32_t*");
  else if (type == VOID_T)
    print((uint32_t*) "void");
  else
    print((uint32_t*) "unknown");
}

void type_warning(uint32_t expected, uint32_t found) {
  print_line_number((uint32_t*) "warning", line_number);
  print((uint32_t*) "type mismatch, ");
  print_type(expected);
  print((uint32_t*) " expected but ");
  print_type(found);
  print((uint32_t*) " found\n");
}

uint32_t* get_variable_or_big_int(uint32_t* variable_or_big_int, uint32_t class) {
  uint32_t* entry;

  if (class == BIGINT)
    return search_global_symbol_table(variable_or_big_int, class);
  else {
    entry = get_scoped_symbol_table_entry(variable_or_big_int, class);

    if (entry == (uint32_t*) 0) {
      print_line_number((uint32_t*) "syntax error", line_number);
      printf1((uint32_t*) "%s undeclared\n", variable_or_big_int);

      exit(EXITCODE_PARSERERROR);
    }

    return entry;
  }
}

void load_upper_base_address(uint32_t* entry) {
  uint32_t lower;
  uint32_t upper;

  // assert: n = allocated_temporaries

  lower = get_bits(get_address(entry),  0, 12);
  upper = get_bits(get_address(entry), 12, 20);

  if (lower >= two_to_the_power_of(11))
    // add 1 which is effectively 2^12 to cancel sign extension of lower
    upper = upper + 1;

  talloc();

  // calculate upper part of base address relative to global or frame pointer
  emit_lui(current_temporary(), sign_extend(upper, 20));
  emit_add(current_temporary(), get_scope(entry), current_temporary());

  // assert: allocated_temporaries == n + 1
}

uint32_t load_variable_or_big_int(uint32_t* variable_or_big_int, uint32_t class) {
  uint32_t* entry;
  uint32_t offset;

  // assert: n = allocated_temporaries

  entry = get_variable_or_big_int(variable_or_big_int, class);

  offset = get_address(entry);

  if (is_signed_integer(offset, 12)) {
    talloc();

    emit_lw(current_temporary(), get_scope(entry), offset);
  } else {
    load_upper_base_address(entry);

    emit_lw(current_temporary(), current_temporary(), sign_extend(get_bits(offset, 0, 12), 12));
  }

  // assert: allocated_temporaries == n + 1

  return get_type(entry);
}

void load_integer(uint32_t value) {
  uint32_t lower;
  uint32_t upper;
  uint32_t* entry;

  // assert: n = allocated_temporaries

  if (is_signed_integer(value, 12)) {
    // integers greater than or equal to -2^11 and less than 2^11
    // are loaded with one addi into a register

    talloc();

    emit_addi(current_temporary(), REG_ZR, value);

  } else if (is_signed_integer(value, 32)) {
    // integers greater than or equal to -2^31 and less than 2^31
    // are loaded with one lui and one addi into a register plus
    // an additional sub to cancel sign extension if necessary

    lower = get_bits(value,  0, 12);
    upper = get_bits(value, 12, 20);

    talloc();

    if (lower >= two_to_the_power_of(11)) {
      // add 1 which is effectively 2^12 to cancel sign extension of lower
      upper = upper + 1;

      // assert: 0 < upper <= 2^(32-12)
      emit_lui(current_temporary(), sign_extend(upper, 20));

      if (upper == two_to_the_power_of(19))
        // upper overflowed, cancel sign extension
        emit_sub(current_temporary(), REG_ZR, current_temporary());
    } else
      // assert: 0 < upper < 2^(32-12)
      emit_lui(current_temporary(), sign_extend(upper, 20));

    emit_addi(current_temporary(), current_temporary(), sign_extend(lower, 12));

  } else {
    // integers less than -2^31 or greater than or equal to 2^31 are stored in data segment
    entry = search_global_symbol_table(integer, BIGINT);

    if (entry == (uint32_t*) 0) {
      allocated_memory = allocated_memory + REGISTERSIZE;

      create_symbol_table_entry(GLOBAL_TABLE, integer, line_number, BIGINT, UINT32_T, value, -allocated_memory);
    }

    load_variable_or_big_int(integer, BIGINT);
  }

  // assert: allocated_temporaries == n + 1
}

void load_string(uint32_t* string) {
  uint32_t length;

  // assert: n = allocated_temporaries

  length = string_length(string) + 1;

  allocated_memory = allocated_memory + round_up(length, REGISTERSIZE);

  create_symbol_table_entry(GLOBAL_TABLE, string, line_number, STRING, UINT32STAR_T, 0, -allocated_memory);

  load_integer(-allocated_memory);

  emit_add(current_temporary(), REG_GP, current_temporary());

  // assert: allocated_temporaries == n + 1
}

uint32_t help_call_codegen(uint32_t* entry, uint32_t* procedure) {
  uint32_t type;

  if (entry == (uint32_t*) 0) {
    // procedure never called nor declared nor defined

    // default return type is "int"
    type = UINT32_T;

    create_symbol_table_entry(GLOBAL_TABLE, procedure, line_number, PROCEDURE, type, 0, binary_length);

    emit_jal(REG_RA, 0);

  } else {
    type = get_type(entry);

    if (get_address(entry) == 0) {
      // procedure declared but never called nor defined
      set_address(entry, binary_length);

      emit_jal(REG_RA, 0);
    } else if (get_opcode(load_instruction(get_address(entry))) == OP_JAL) {
      // procedure called and possibly declared but not defined

      // create fixup chain using absolute address
      emit_jal(REG_RA, get_address(entry));
      set_address(entry, binary_length - INSTRUCTIONSIZE);
    } else
      // procedure defined, use relative address
      emit_jal(REG_RA, get_address(entry) - binary_length);
  }

  return type;
}

void help_procedure_prologue(uint32_t number_of_local_variable_bytes) {
  // allocate memory for return address
  emit_addi(REG_SP, REG_SP, -REGISTERSIZE);

  // save return address
  emit_sw(REG_SP, 0, REG_RA);

  // allocate memory for caller's frame pointer
  emit_addi(REG_SP, REG_SP, -REGISTERSIZE);

  // save caller's frame pointer
  emit_sw(REG_SP, 0, REG_FP);

  // set callee's frame pointer
  emit_addi(REG_FP, REG_SP, 0);

  // allocate memory for callee's local variables
  if (number_of_local_variable_bytes > 0) {
    if (is_signed_integer(-number_of_local_variable_bytes, 12))
      emit_addi(REG_SP, REG_SP, -number_of_local_variable_bytes);
    else {
      load_integer(-number_of_local_variable_bytes);

      emit_add(REG_SP, REG_SP, current_temporary());

      tfree(1);
    }
  }
}

void help_procedure_epilogue(uint32_t number_of_parameter_bytes) {
  // deallocate memory for callee's frame pointer and local variables
  emit_addi(REG_SP, REG_FP, 0);

  // restore caller's frame pointer
  emit_lw(REG_FP, REG_SP, 0);

  // deallocate memory for caller's frame pointer
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  // restore return address
  emit_lw(REG_RA, REG_SP, 0);

  // deallocate memory for return address and parameters
  emit_addi(REG_SP, REG_SP, REGISTERSIZE + number_of_parameter_bytes);

  // return
  emit_jalr(REG_ZR, REG_RA, 0);
}

uint32_t compile_call(uint32_t* procedure) {
  uint32_t* entry;
  uint32_t number_of_temporaries;
  uint32_t type;

  // assert: n = allocated_temporaries

  entry = get_scoped_symbol_table_entry(procedure, PROCEDURE);

  number_of_temporaries = allocated_temporaries;

  save_temporaries();

  // assert: allocated_temporaries == 0

  if (is_expression()) {
    compile_expression();

    // TODO: check if types/number of parameters is correct

    // push first parameter onto stack
    emit_addi(REG_SP, REG_SP, -REGISTERSIZE);
    emit_sw(REG_SP, 0, current_temporary());

    tfree(1);

    while (symbol == SYM_COMMA) {
      get_symbol();

      compile_expression();

      // push more parameters onto stack
      emit_addi(REG_SP, REG_SP, -REGISTERSIZE);
      emit_sw(REG_SP, 0, current_temporary());

      tfree(1);
    }

    if (symbol == SYM_RPARENTHESIS) {
      get_symbol();

      type = help_call_codegen(entry, procedure);
    } else {
      syntax_error_symbol(SYM_RPARENTHESIS);

      type = UINT32_T;
    }
  } else if (symbol == SYM_RPARENTHESIS) {
    get_symbol();

    type = help_call_codegen(entry, procedure);
  } else {
    syntax_error_symbol(SYM_RPARENTHESIS);

    type = UINT32_T;
  }

  // assert: allocated_temporaries == 0

  restore_temporaries(number_of_temporaries);

  number_of_calls = number_of_calls + 1;

  // assert: allocated_temporaries == n

  return type;
}

uint32_t compile_factor() {
  uint32_t has_cast;
  uint32_t cast;
  uint32_t type;
  uint32_t negative;
  uint32_t dereference;
  uint32_t* variable_or_procedure_name;

  // assert: n = allocated_temporaries

  while (look_for_factor()) {
    syntax_error_unexpected();

    if (symbol == SYM_EOF)
      exit(EXITCODE_PARSERERROR);
    else
      get_symbol();
  }

  // optional: [ cast ]
  if (symbol == SYM_LPARENTHESIS) {
    get_symbol();

    // cast: "(" "uint32_t" [ "*" ] ")"
    if (symbol == SYM_UINT32) {
      has_cast = 1;

      cast = compile_type();

      if (symbol == SYM_RPARENTHESIS)
        get_symbol();
      else
        syntax_error_symbol(SYM_RPARENTHESIS);

    // not a cast: "(" expression ")"
    } else {
      type = compile_expression();

      if (symbol == SYM_RPARENTHESIS)
        get_symbol();
      else
        syntax_error_symbol(SYM_RPARENTHESIS);

      // assert: allocated_temporaries == n + 1

      return type;
    }
  } else
    has_cast = 0;

  // optional: -
  if (symbol == SYM_MINUS) {
    negative = 1;

    integer_is_signed = 1;

    get_symbol();

    integer_is_signed = 0;
  } else
    negative = 0;

  // optional: dereference
  if (symbol == SYM_ASTERISK) {
    dereference = 1;

    get_symbol();
  } else
    dereference = 0;

  // identifier or call?
  if (symbol == SYM_IDENTIFIER) {
    variable_or_procedure_name = identifier;

    get_symbol();

    if (symbol == SYM_LPARENTHESIS) {
      get_symbol();

      // procedure call: identifier "(" ... ")"
      type = compile_call(variable_or_procedure_name);

      talloc();

      // retrieve return value
      emit_addi(current_temporary(), REG_A0, 0);

      // reset return register to initial return value
      // for missing return expressions
      emit_addi(REG_A0, REG_ZR, 0);
    } else
      // variable access: identifier
      type = load_variable_or_big_int(variable_or_procedure_name, VARIABLE);

  // integer?
  } else if (symbol == SYM_INTEGER) {
    load_integer(literal);

    get_symbol();

    type = UINT32_T;

  // character?
  } else if (symbol == SYM_CHARACTER) {
    talloc();

    emit_addi(current_temporary(), REG_ZR, literal);

    get_symbol();

    type = UINT32_T;

  // string?
  } else if (symbol == SYM_STRING) {
    load_string(string);

    get_symbol();

    type = UINT32STAR_T;

  //  "(" expression ")"
  } else if (symbol == SYM_LPARENTHESIS) {
    get_symbol();

    type = compile_expression();

    if (symbol == SYM_RPARENTHESIS)
      get_symbol();
    else
      syntax_error_symbol(SYM_RPARENTHESIS);
  } else {
    syntax_error_unexpected();

    type = UINT32_T;
  }

  if (dereference) {
    if (type != UINT32STAR_T)
      type_warning(UINT32STAR_T, type);

    // dereference
    emit_lw(current_temporary(), current_temporary(), 0);

    type = UINT32_T;
  }

  if (negative) {
    if (type != UINT32_T) {
      type_warning(UINT32_T, type);

      type = UINT32_T;
    }

    emit_sub(current_temporary(), REG_ZR, current_temporary());
  }

  // assert: allocated_temporaries == n + 1

  if (has_cast)
    return cast;
  else
    return type;
}

uint32_t compile_term() {
  uint32_t ltype;
  uint32_t operator_symbol;
  uint32_t rtype;

  // assert: n = allocated_temporaries

  ltype = compile_factor();

  // assert: allocated_temporaries == n + 1

  // * / or % ?
  while (is_star_or_div_or_modulo()) {
    operator_symbol = symbol;

    get_symbol();

    rtype = compile_factor();

    // assert: allocated_temporaries == n + 2

    if (ltype != rtype)
      type_warning(ltype, rtype);

    if (operator_symbol == SYM_ASTERISK)
      emit_mul(previous_temporary(), previous_temporary(), current_temporary());
    else if (operator_symbol == SYM_DIV)
      emit_divu(previous_temporary(), previous_temporary(), current_temporary());
    else if (operator_symbol == SYM_MOD)
      emit_remu(previous_temporary(), previous_temporary(), current_temporary());

    tfree(1);
  }

  // assert: allocated_temporaries == n + 1

  return ltype;
}

uint32_t compile_simple_expression() {
  uint32_t ltype;
  uint32_t operator_symbol;
  uint32_t rtype;

  // assert: n = allocated_temporaries

  ltype = compile_term();

  // assert: allocated_temporaries == n + 1

  // + or - ?
  while (is_plus_or_minus()) {
    operator_symbol = symbol;

    get_symbol();

    rtype = compile_term();

    // assert: allocated_temporaries == n + 2

    if (operator_symbol == SYM_PLUS) {
      if (ltype == UINT32STAR_T) {
        if (rtype == UINT32_T)
          // UINT32STAR_T + UINT32_T
          // pointer arithmetic: factor of 2^2 of integer operand
          emit_left_shift_by(current_temporary(), 2);
        else
          // UINT32STAR_T + UINT32STAR_T
          syntax_error_message((uint32_t*) "(uint32_t*) + (uint32_t*) is undefined");
      } else if (rtype == UINT32STAR_T) {
        // UINT32_T + UINT32STAR_T
        // pointer arithmetic: factor of 2^2 of integer operand
        emit_left_shift_by(previous_temporary(), 2);

        ltype = UINT32STAR_T;
      }

      emit_add(previous_temporary(), previous_temporary(), current_temporary());

    } else if (operator_symbol == SYM_MINUS) {
      if (ltype == UINT32STAR_T) {
        if (rtype == UINT32_T) {
          // UINT32STAR_T - UINT32_T
          // pointer arithmetic: factor of 2^2 of integer operand
          emit_left_shift_by(current_temporary(), 2);
          emit_sub(previous_temporary(), previous_temporary(), current_temporary());
        } else {
          // UINT32STAR_T - UINT32STAR_T
          // pointer arithmetic: (left_term - right_term) / SIZEOFUINT32
          emit_sub(previous_temporary(), previous_temporary(), current_temporary());
          emit_addi(current_temporary(), REG_ZR, SIZEOFUINT32);
          emit_divu(previous_temporary(), previous_temporary(), current_temporary());

          ltype = UINT32_T;
        }
      } else if (rtype == UINT32STAR_T)
        // UINT32_T - UINT32STAR_T
        syntax_error_message((uint32_t*) "(uint32_t) - (uint32_t*) is undefined");
      else
        // UINT32_T - UINT32_T
        emit_sub(previous_temporary(), previous_temporary(), current_temporary());
    }

    tfree(1);
  }

  // assert: allocated_temporaries == n + 1

  return ltype;
}

uint32_t compile_expression() {
  uint32_t ltype;
  uint32_t operator_symbol;
  uint32_t rtype;

  // assert: n = allocated_temporaries

  ltype = compile_simple_expression();

  // assert: allocated_temporaries == n + 1

  //optional: ==, !=, <, >, <=, >= simple_expression
  if (is_comparison()) {
    operator_symbol = symbol;

    get_symbol();

    rtype = compile_simple_expression();

    // assert: allocated_temporaries == n + 2

    if (ltype != rtype)
      type_warning(ltype, rtype);

    if (operator_symbol == SYM_EQUALITY) {
      // a == b iff unsigned b - a < 1
      emit_sub(previous_temporary(), current_temporary(), previous_temporary());
      emit_addi(current_temporary(), REG_ZR, 1);
      emit_sltu(previous_temporary(), previous_temporary(), current_temporary());

      tfree(1);

    } else if (operator_symbol == SYM_NOTEQ) {
      // a != b iff unsigned 0 < b - a
      emit_sub(previous_temporary(), current_temporary(), previous_temporary());

      tfree(1);

      emit_sltu(current_temporary(), REG_ZR, current_temporary());

    } else if (operator_symbol == SYM_LT) {
      // a < b
      emit_sltu(previous_temporary(), previous_temporary(), current_temporary());

      tfree(1);

    } else if (operator_symbol == SYM_GT) {
      // a > b iff b < a
      emit_sltu(previous_temporary(), current_temporary(), previous_temporary());

      tfree(1);

    } else if (operator_symbol == SYM_LEQ) {
      // a <= b iff 1 - (b < a)
      emit_sltu(previous_temporary(), current_temporary(), previous_temporary());
      emit_addi(current_temporary(), REG_ZR, 1);
      emit_sub(previous_temporary(), current_temporary(), previous_temporary());

      tfree(1);

    } else if (operator_symbol == SYM_GEQ) {
      // a >= b iff 1 - (a < b)
      emit_sltu(previous_temporary(), previous_temporary(), current_temporary());
      emit_addi(current_temporary(), REG_ZR, 1);
      emit_sub(previous_temporary(), current_temporary(), previous_temporary());

      tfree(1);
    }
  }

  // assert: allocated_temporaries == n + 1

  return ltype;
}

void compile_while() {
  uint32_t jump_back_to_while;
  uint32_t branch_forward_to_end;

  // assert: allocated_temporaries == 0

  jump_back_to_while = binary_length;

  branch_forward_to_end = 0;

  // while ( expression )
  if (symbol == SYM_WHILE) {
    get_symbol();

    if (symbol == SYM_LPARENTHESIS) {
      get_symbol();

      compile_expression();

      // we do not know where to branch, fixup later
      branch_forward_to_end = binary_length;

      emit_beq(current_temporary(), REG_ZR, 0);

      tfree(1);

      if (symbol == SYM_RPARENTHESIS) {
        get_symbol();

        // zero or more statements: { statement }
        if (symbol == SYM_LBRACE) {
          get_symbol();

          while (is_not_rbrace_or_eof())
            compile_statement();

          if (symbol == SYM_RBRACE)
            get_symbol();
          else {
            syntax_error_symbol(SYM_RBRACE);

            exit(EXITCODE_PARSERERROR);
          }
        } else
          // only one statement without {}
          compile_statement();
      } else
        syntax_error_symbol(SYM_RPARENTHESIS);
    } else
      syntax_error_symbol(SYM_LPARENTHESIS);
  } else
    syntax_error_symbol(SYM_WHILE);

  // we use JAL for the unconditional jump back to the loop condition because:
  // 1. the RISC-V doc recommends to do so to not disturb branch prediction
  // 2. GCC also uses JAL for the unconditional back jump of a while loop
  emit_jal(REG_ZR, jump_back_to_while - binary_length);

  if (branch_forward_to_end != 0)
    // first instruction after loop body will be generated here
    // now we have the address for the conditional branch from above
    fixup_relative_BFormat(branch_forward_to_end);

  // assert: allocated_temporaries == 0

  number_of_while = number_of_while + 1;
}

void compile_if() {
  uint32_t branch_forward_to_else_or_end;
  uint32_t jump_forward_to_end;

  // assert: allocated_temporaries == 0

  // if ( expression )
  if (symbol == SYM_IF) {
    get_symbol();

    if (symbol == SYM_LPARENTHESIS) {
      get_symbol();

      compile_expression();

      // if the "if" case is not true we branch to "else" (if provided)
      branch_forward_to_else_or_end = binary_length;

      emit_beq(current_temporary(), REG_ZR, 0);

      tfree(1);

      if (symbol == SYM_RPARENTHESIS) {
        get_symbol();

        // zero or more statements: { statement }
        if (symbol == SYM_LBRACE) {
          get_symbol();

          while (is_not_rbrace_or_eof())
            compile_statement();

          if (symbol == SYM_RBRACE)
            get_symbol();
          else {
            syntax_error_symbol(SYM_RBRACE);

            exit(EXITCODE_PARSERERROR);
          }
        } else
        // only one statement without {}
          compile_statement();

        //optional: else
        if (symbol == SYM_ELSE) {
          get_symbol();

          // if the "if" case was true we skip the "else" case
          // by unconditionally jumping to the end
          jump_forward_to_end = binary_length;

          emit_jal(REG_ZR, 0);

          // if the "if" case was not true we branch here
          fixup_relative_BFormat(branch_forward_to_else_or_end);

          // zero or more statements: { statement }
          if (symbol == SYM_LBRACE) {
            get_symbol();

            while (is_not_rbrace_or_eof())
              compile_statement();

            if (symbol == SYM_RBRACE)
              get_symbol();
            else {
              syntax_error_symbol(SYM_RBRACE);

              exit(EXITCODE_PARSERERROR);
            }

          // only one statement without {}
          } else
            compile_statement();

          // if the "if" case was true we unconditionally jump here
          fixup_relative_JFormat(jump_forward_to_end, binary_length);
        } else
          // if the "if" case was not true we branch here
          fixup_relative_BFormat(branch_forward_to_else_or_end);
      } else
        syntax_error_symbol(SYM_RPARENTHESIS);
    } else
      syntax_error_symbol(SYM_LPARENTHESIS);
  } else
    syntax_error_symbol(SYM_IF);

  // assert: allocated_temporaries == 0

  number_of_if = number_of_if + 1;
}

void compile_return() {
  uint32_t type;

  // assert: allocated_temporaries == 0

  if (symbol == SYM_RETURN)
    get_symbol();
  else
    syntax_error_symbol(SYM_RETURN);

  // optional: expression
  if (symbol != SYM_SEMICOLON) {
    type = compile_expression();

    if (type != return_type)
      type_warning(return_type, type);

    // save value of expression in return register
    emit_addi(REG_A0, current_temporary(), 0);

    tfree(1);
  } else if (return_type != VOID_T)
    type_warning(return_type, VOID_T);

  // jump to procedure epilogue through fixup chain using absolute address
  emit_jal(REG_ZR, return_branches);

  // new head of fixup chain
  return_branches = binary_length - INSTRUCTIONSIZE;

  // assert: allocated_temporaries == 0

  number_of_return = number_of_return + 1;
}

void compile_statement() {
  uint32_t ltype;
  uint32_t rtype;
  uint32_t* variable_or_procedure_name;
  uint32_t* entry;
  uint32_t offset;

  // assert: allocated_temporaries == 0

  while (look_for_statement()) {
    syntax_error_unexpected();

    if (symbol == SYM_EOF)
      exit(EXITCODE_PARSERERROR);
    else
      get_symbol();
  }

  // ["*"]
  if (symbol == SYM_ASTERISK) {
    get_symbol();

    // "*" identifier
    if (symbol == SYM_IDENTIFIER) {
      ltype = load_variable_or_big_int(identifier, VARIABLE);

      if (ltype != UINT32STAR_T)
        type_warning(UINT32STAR_T, ltype);

      get_symbol();

      // "*" identifier "="
      if (symbol == SYM_ASSIGN) {
        get_symbol();

        rtype = compile_expression();

        if (rtype != UINT32_T)
          type_warning(UINT32_T, rtype);

        emit_sw(previous_temporary(), 0, current_temporary());

        tfree(2);

        number_of_assignments = number_of_assignments + 1;
      } else {
        syntax_error_symbol(SYM_ASSIGN);

        tfree(1);
      }

      if (symbol == SYM_SEMICOLON)
        get_symbol();
      else
        syntax_error_symbol(SYM_SEMICOLON);

    // "*" "(" expression ")"
    } else if (symbol == SYM_LPARENTHESIS) {
      get_symbol();

      ltype = compile_expression();

      if (ltype != UINT32STAR_T)
        type_warning(UINT32STAR_T, ltype);

      if (symbol == SYM_RPARENTHESIS) {
        get_symbol();

        // "*" "(" expression ")" "="
        if (symbol == SYM_ASSIGN) {
          get_symbol();

          rtype = compile_expression();

          if (rtype != UINT32_T)
            type_warning(UINT32_T, rtype);

          emit_sw(previous_temporary(), 0, current_temporary());

          tfree(2);

          number_of_assignments = number_of_assignments + 1;
        } else {
          syntax_error_symbol(SYM_ASSIGN);

          tfree(1);
        }

        if (symbol == SYM_SEMICOLON)
          get_symbol();
        else
          syntax_error_symbol(SYM_SEMICOLON);
      } else
        syntax_error_symbol(SYM_RPARENTHESIS);
    } else
      syntax_error_symbol(SYM_LPARENTHESIS);
  }
  // identifier "=" expression | call
  else if (symbol == SYM_IDENTIFIER) {
    variable_or_procedure_name = identifier;

    get_symbol();

    // procedure call
    if (symbol == SYM_LPARENTHESIS) {
      get_symbol();

      compile_call(variable_or_procedure_name);

      // reset return register to initial return value
      // for missing return expressions
      emit_addi(REG_A0, REG_ZR, 0);

      if (symbol == SYM_SEMICOLON)
        get_symbol();
      else
        syntax_error_symbol(SYM_SEMICOLON);

    // identifier = expression
    } else if (symbol == SYM_ASSIGN) {
      entry = get_variable_or_big_int(variable_or_procedure_name, VARIABLE);

      ltype = get_type(entry);

      get_symbol();

      rtype = compile_expression();

      if (ltype != rtype)
        type_warning(ltype, rtype);

      offset = get_address(entry);

      if (is_signed_integer(offset, 12)) {
        emit_sw(get_scope(entry), offset, current_temporary());

        tfree(1);
      } else {
        load_upper_base_address(entry);

        emit_sw(current_temporary(), sign_extend(get_bits(offset, 0, 12), 12), previous_temporary());

        tfree(2);
      }

      number_of_assignments = number_of_assignments + 1;

      if (symbol == SYM_SEMICOLON)
        get_symbol();
      else
        syntax_error_symbol(SYM_SEMICOLON);
    } else
      syntax_error_unexpected();
  }
  // while statement?
  else if (symbol == SYM_WHILE) {
    compile_while();
  }
  // if statement?
  else if (symbol == SYM_IF) {
    compile_if();
  }
  // return statement?
  else if (symbol == SYM_RETURN) {
    compile_return();

    if (symbol == SYM_SEMICOLON)
      get_symbol();
    else
      syntax_error_symbol(SYM_SEMICOLON);
  }
}

uint32_t compile_type() {
  uint32_t type;

  type = UINT32_T;

  if (symbol == SYM_UINT32) {
    get_symbol();

    if (symbol == SYM_ASTERISK) {
      type = UINT32STAR_T;

      get_symbol();
    }
  } else
    syntax_error_symbol(SYM_UINT32);

  return type;
}

void compile_variable(uint32_t offset) {
  uint32_t type;

  type = compile_type();

  if (symbol == SYM_IDENTIFIER) {
    // TODO: check if identifier has already been declared
    create_symbol_table_entry(LOCAL_TABLE, identifier, line_number, VARIABLE, type, 0, offset);

    get_symbol();
  } else {
    syntax_error_symbol(SYM_IDENTIFIER);

    create_symbol_table_entry(LOCAL_TABLE, (uint32_t*) "missing variable name", line_number, VARIABLE, type, 0, offset);
  }
}

uint32_t compile_initialization(uint32_t type) {
  uint32_t initial_value;
  uint32_t has_cast;
  uint32_t cast;

  initial_value = 0;

  has_cast = 0;

  if (symbol == SYM_ASSIGN) {
    get_symbol();

    // optional: [ cast ]
    if (symbol == SYM_LPARENTHESIS) {
      has_cast = 1;

      get_symbol();

      cast = compile_type();

      if (symbol == SYM_RPARENTHESIS)
        get_symbol();
      else
        syntax_error_symbol(SYM_RPARENTHESIS);
    }

    // optional: -
    if (symbol == SYM_MINUS) {
      integer_is_signed = 1;

      get_symbol();

      integer_is_signed = 0;

      initial_value = -literal;
    } else
      initial_value = literal;

    if (is_literal())
      get_symbol();
    else
      syntax_error_unexpected();

    if (symbol == SYM_SEMICOLON)
      get_symbol();
    else
      syntax_error_symbol(SYM_SEMICOLON);
  } else
    syntax_error_symbol(SYM_ASSIGN);

  if (has_cast) {
    if (type != cast)
      type_warning(type, cast);
  } else if (type != UINT32_T)
    type_warning(type, UINT32_T);

  return initial_value;
}

void compile_procedure(uint32_t* procedure, uint32_t type) {
  uint32_t is_undefined;
  uint32_t number_of_parameters;
  uint32_t parameters;
  uint32_t number_of_local_variable_bytes;
  uint32_t* entry;

  // assuming procedure is undefined
  is_undefined = 1;

  number_of_parameters = 0;

  // try parsing formal parameters
  if (symbol == SYM_LPARENTHESIS) {
    get_symbol();

    if (symbol != SYM_RPARENTHESIS) {
      compile_variable(0);

      number_of_parameters = 1;

      while (symbol == SYM_COMMA) {
        get_symbol();

        compile_variable(0);

        number_of_parameters = number_of_parameters + 1;
      }

      entry = local_symbol_table;

      parameters = 0;

      while (parameters < number_of_parameters) {
        // 8 bytes offset to skip frame pointer and link
        set_address(entry, parameters * REGISTERSIZE + 2 * REGISTERSIZE);

        parameters = parameters + 1;

        entry = get_next_entry(entry);
      }

      if (symbol == SYM_RPARENTHESIS)
        get_symbol();
      else
        syntax_error_symbol(SYM_RPARENTHESIS);
    } else
      get_symbol();
  } else
    syntax_error_symbol(SYM_LPARENTHESIS);

  entry = search_global_symbol_table(procedure, PROCEDURE);

  if (symbol == SYM_SEMICOLON) {
    // this is a procedure declaration
    if (entry == (uint32_t*) 0)
      // procedure never called nor declared nor defined
      create_symbol_table_entry(GLOBAL_TABLE, procedure, line_number, PROCEDURE, type, 0, 0);
    else if (get_type(entry) != type)
      // procedure already called, declared, or even defined
      // check return type but otherwise ignore
      type_warning(get_type(entry), type);

    get_symbol();

  } else if (symbol == SYM_LBRACE) {
    // this is a procedure definition
    if (entry == (uint32_t*) 0)
      // procedure never called nor declared nor defined
      create_symbol_table_entry(GLOBAL_TABLE, procedure, line_number, PROCEDURE, type, 0, binary_length);
    else {
      // procedure already called or declared or defined
      if (get_address(entry) != 0) {
        // procedure already called or defined
        if (get_opcode(load_instruction(get_address(entry))) == OP_JAL)
          // procedure already called but not defined
          fixlink_relative(get_address(entry), binary_length);
        else
          // procedure already defined
          is_undefined = 0;
      }

      if (is_undefined) {
        // procedure already called or declared but not defined
        set_line_number(entry, line_number);

        if (get_type(entry) != type)
          type_warning(get_type(entry), type);

        set_type(entry, type);
        set_address(entry, binary_length);

        if (string_compare(procedure, (uint32_t*) "main")) {
          // first source containing main procedure provides binary name
          binary_name = source_name;

          // account for initial call to main procedure
          number_of_calls = number_of_calls + 1;
        }
      } else {
        // procedure already defined
        print_line_number((uint32_t*) "warning", line_number);
        printf1((uint32_t*) "redefinition of procedure %s ignored\n", procedure);
      }
    }

    get_symbol();

    number_of_local_variable_bytes = 0;

    while (symbol == SYM_UINT32) {
      number_of_local_variable_bytes = number_of_local_variable_bytes + REGISTERSIZE;

      // offset of local variables relative to frame pointer is negative
      compile_variable(-number_of_local_variable_bytes);

      if (symbol == SYM_SEMICOLON)
        get_symbol();
      else
        syntax_error_symbol(SYM_SEMICOLON);
    }

    help_procedure_prologue(number_of_local_variable_bytes);

    // create a fixup chain for return statements
    return_branches = 0;

    return_type = type;

    while (is_not_rbrace_or_eof())
      compile_statement();

    return_type = 0;

    if (symbol == SYM_RBRACE)
      get_symbol();
    else {
      syntax_error_symbol(SYM_RBRACE);

      exit(EXITCODE_PARSERERROR);
    }

    fixlink_relative(return_branches, binary_length);

    return_branches = 0;

    help_procedure_epilogue(number_of_parameters * REGISTERSIZE);

  } else
    syntax_error_unexpected();

  local_symbol_table = (uint32_t*) 0;

  // assert: allocated_temporaries == 0
}

void compile_cstar() {
  uint32_t type;
  uint32_t* variable_or_procedure_name;
  uint32_t current_line_number;
  uint32_t initial_value;
  uint32_t* entry;

  while (symbol != SYM_EOF) {
    while (look_for_type()) {
      syntax_error_unexpected();

      if (symbol == SYM_EOF)
        exit(EXITCODE_PARSERERROR);
      else
        get_symbol();
    }

    if (symbol == SYM_VOID) {
      // void identifier ...
      // procedure declaration or definition
      type = VOID_T;

      get_symbol();

      if (symbol == SYM_IDENTIFIER) {
        variable_or_procedure_name = identifier;

        get_symbol();

        compile_procedure(variable_or_procedure_name, type);
      } else
        syntax_error_symbol(SYM_IDENTIFIER);
    } else {
      type = compile_type();

      if (symbol == SYM_IDENTIFIER) {
        variable_or_procedure_name = identifier;

        get_symbol();

        if (symbol == SYM_LPARENTHESIS)
          // type identifier "(" ...
          // procedure declaration or definition
          compile_procedure(variable_or_procedure_name, type);
        else {
          current_line_number = line_number;

          if (symbol == SYM_SEMICOLON) {
            // type identifier ";" ...
            // global variable declaration
            get_symbol();

            initial_value = 0;
          } else
            // type identifier "=" ...
            // global variable definition
            initial_value = compile_initialization(type);

          entry = search_global_symbol_table(variable_or_procedure_name, VARIABLE);

          if (entry == (uint32_t*) 0) {
            allocated_memory = allocated_memory + REGISTERSIZE;

            create_symbol_table_entry(GLOBAL_TABLE, variable_or_procedure_name, current_line_number, VARIABLE, type, initial_value, -allocated_memory);
          } else {
            // global variable already declared or defined
            print_line_number((uint32_t*) "warning", current_line_number);
            printf1((uint32_t*) "redefinition of global variable %s ignored\n", variable_or_procedure_name);
          }
        }
      } else
        syntax_error_symbol(SYM_IDENTIFIER);
    }
  }
}

// -----------------------------------------------------------------
// ------------------------ MACHINE CODE LIBRARY -------------------
// -----------------------------------------------------------------

void emit_round_up(uint32_t reg, uint32_t m) {
  talloc();

  // computes value(reg) + m - 1 - (value(reg) + m - 1) % m
  emit_addi(reg, reg, m - 1);
  emit_addi(current_temporary(), REG_ZR, m);
  emit_remu(current_temporary(), reg, current_temporary());
  emit_sub(reg, reg, current_temporary());

  tfree(1);
}

void emit_left_shift_by(uint32_t reg, uint32_t b) {
  // assert: 0 <= b < 11

  // load multiplication factor less than 2^11 to avoid sign extension
  emit_addi(next_temporary(), REG_ZR, two_to_the_power_of(b));
  emit_mul(reg, reg, next_temporary());
}

void emit_program_entry() {
  uint32_t i;

  i = 0;

  // allocate space for machine initialization code,
  // emit exactly 20 NOPs with source code line 1
  while (i < 20) {
    emit_nop();

    i = i + 1;
  }
}

void emit_bootstrapping() {
  /*
      1. initialize global pointer
      2. initialize malloc's _bump pointer
      3. push argv pointer onto stack
      4. call main procedure
      5. proceed to exit procedure
  */
  uint32_t gp;
  uint32_t padding;
  uint32_t lower;
  uint32_t upper;
  uint32_t* entry;

  // calculate the global pointer value
  gp = ELF_ENTRY_POINT + binary_length + allocated_memory;

  // make sure gp is word-aligned
  padding = gp % REGISTERSIZE;
  gp      = gp + padding;

  if (padding != 0)
    emit_nop();

  // no more allocation in code segment from now on
  code_length = binary_length;

  // reset code emission to program entry
  binary_length = 0;

  // assert: emitting no more than 20 instructions

  if (report_undefined_procedures()) {
    // if there are undefined procedures just exit
    // by loading exit code 0 into return register
    emit_addi(REG_A0, REG_ZR, 0);
  } else {
    // avoid sign extension that would result in an additional sub instruction
    if (gp < two_to_the_power_of(31) - two_to_the_power_of(11))
      // assert: generates no more than two instructions
      load_integer(gp);
    else {
      syntax_error_message((uint32_t*) "maximum program break exceeded");

      exit(EXITCODE_COMPILERERROR);
    }

    // initialize global pointer
    emit_addi(REG_GP, current_temporary(), 0);

    tfree(1);

    // retrieve current program break in return register
    emit_addi(REG_A0, REG_ZR, 0);
    emit_addi(REG_A7, REG_ZR, SYSCALL_BRK);
    emit_ecall();

    // align current program break for word access
    emit_round_up(REG_A0, SIZEOFUINT32);

    // set program break to aligned program break
    emit_addi(REG_A7, REG_ZR, SYSCALL_BRK);
    emit_ecall();

    // look up global variable _bump for storing malloc's bump pointer
    // copy "_bump" string into zeroed word to obtain unique hash
    entry = search_global_symbol_table(string_copy((uint32_t*) "_bump"), VARIABLE);

    // store aligned program break in _bump
    emit_sw(get_scope(entry), get_address(entry), REG_A0);

    // reset return register to initial return value
    emit_addi(REG_A0, REG_ZR, 0);

    // assert: stack is set up with argv pointer still missing
    //
    //    $sp
    //     |
    //     V
    // | argc | argv[0] | argv[1] | ... | argv[n]

    talloc();

    // first obtain pointer to argv
    //
    //    $sp + REGISTERSIZE
    //            |
    //            V
    // | argc | argv[0] | argv[1] | ... | argv[n]
    emit_addi(current_temporary(), REG_SP, REGISTERSIZE);

    // then push argv pointer onto the stack
    //      ______________
    //     |              V
    // | &argv | argc | argv[0] | argv[1] | ... | argv[n]
    emit_addi(REG_SP, REG_SP, -REGISTERSIZE);
    emit_sw(REG_SP, 0, current_temporary());

    tfree(1);

    // assert: global, _bump, and stack pointers are set up
    //         with all other non-temporary registers zeroed

    // copy "main" string into zeroed word to obtain unique hash
    entry = get_scoped_symbol_table_entry(string_copy((uint32_t*) "main"), PROCEDURE);

    help_call_codegen(entry, (uint32_t*) "main");
  }

  // we exit with exit code in return register pushed onto the stack
  emit_addi(REG_SP, REG_SP, -REGISTERSIZE);
  emit_sw(REG_SP, 0, REG_A0);

  // wrapper code for exit must follow here

  // discount NOPs in profile that were generated for program entry
  ic_addi = ic_addi - binary_length / INSTRUCTIONSIZE;

  // restore original binary length
  binary_length = code_length;
}

// -----------------------------------------------------------------
// --------------------------- COMPILER ----------------------------
// -----------------------------------------------------------------

void selfie_compile() {
  uint32_t link;
  uint32_t number_of_source_files;

  // link until next console option
  link = 1;

  number_of_source_files = 0;

  source_name = (uint32_t*) "library";

  binary_name = source_name;

  // allocate memory for storing binary
  binary       = smalloc(MAX_BINARY_LENGTH);
  binary_length = 0;

  // reset code length
  code_length = 0;

  // allocate zeroed memory for storing source code line numbers
  code_line_number = zalloc(MAX_CODE_LENGTH / INSTRUCTIONSIZE * SIZEOFUINT32);
  data_line_number = zalloc(MAX_DATA_LENGTH / REGISTERSIZE * SIZEOFUINT32);

  reset_symbol_tables();
  reset_instruction_counters();

  emit_program_entry();

  // emit system call wrappers
  // exit code must be first
  emit_exit();
  emit_read();
  emit_write();
  emit_open();
  emit_malloc();
  emit_switch();

  // implicitly declare main procedure in global symbol table
  // copy "main" string into zeroed word to obtain unique hash
  create_symbol_table_entry(GLOBAL_TABLE, string_copy((uint32_t*) "main"), 0, PROCEDURE, UINT32_T, 0, 0);

  while (link) {
    if (number_of_remaining_arguments() == 0)
      link = 0;
    else if (load_character(peek_argument(), 0) == '-')
      link = 0;
    else {
      source_name = get_argument();

      number_of_source_files = number_of_source_files + 1;

      printf2((uint32_t*) "%s: selfie compiling %s with starc\n", selfie_name, source_name);

      // assert: source_name is mapped and not longer than MAX_FILENAME_LENGTH

      source_fd = open(source_name, O_RDONLY, 0);

      if (signed_less_than(source_fd, 0)) {
        printf2((uint32_t*) "%s: could not open input file %s\n", selfie_name, source_name);

        exit(EXITCODE_IOERROR);
      }

      reset_scanner();
      reset_parser();

      compile_cstar();

      printf4((uint32_t*) "%s: %d characters read in %d lines and %d comments\n", selfie_name,
        (uint32_t*) number_of_read_characters,
        (uint32_t*) line_number,
        (uint32_t*) number_of_comments);

      printf4((uint32_t*) "%s: with %d(%.2d%%) characters in %d actual symbols\n", selfie_name,
        (uint32_t*) (number_of_read_characters - number_of_ignored_characters),
        (uint32_t*) fixed_point_percentage(fixed_point_ratio(number_of_read_characters, number_of_read_characters - number_of_ignored_characters, 4), 4),
        (uint32_t*) number_of_scanned_symbols);

      printf4((uint32_t*) "%s: %d global variables, %d procedures, %d string literals\n", selfie_name,
        (uint32_t*) number_of_global_variables,
        (uint32_t*) number_of_procedures,
        (uint32_t*) number_of_strings);

      printf6((uint32_t*) "%s: %d calls, %d assignments, %d while, %d if, %d return\n", selfie_name,
        (uint32_t*) number_of_calls,
        (uint32_t*) number_of_assignments,
        (uint32_t*) number_of_while,
        (uint32_t*) number_of_if,
        (uint32_t*) number_of_return);
    }
  }

  if (number_of_source_files == 0)
    printf1((uint32_t*) "%s: nothing to compile, only library generated\n", selfie_name);

  emit_bootstrapping();

  emit_data_segment();

  ELF_header = create_elf_header(binary_length);

  entry_point = ELF_ENTRY_POINT;

  printf3((uint32_t*) "%s: symbol table search time was %d iterations on average and %d in total\n", selfie_name, (uint32_t*) (total_search_time / number_of_searches), (uint32_t*) total_search_time);

  printf4((uint32_t*) "%s: %d bytes generated with %d instructions and %d bytes of data\n", selfie_name,
    (uint32_t*) binary_length,
    (uint32_t*) (code_length / INSTRUCTIONSIZE),
    (uint32_t*) (binary_length - code_length));

  print_instruction_counters();
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// -------------------     I N T E R F A C E     -------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// --------------------------- REGISTER ----------------------------
// -----------------------------------------------------------------

uint32_t* get_register_name(uint32_t reg) {
  return (uint32_t*) *(REGISTERS + reg);
}

void print_register_name(uint32_t reg) {
  print(get_register_name(reg));
}

// -----------------------------------------------------------------
// ------------------------ ENCODER/DECODER ------------------------
// -----------------------------------------------------------------

void check_immediate_range(uint32_t immediate, uint32_t bits) {
  if (is_signed_integer(immediate, bits) == 0) {
    print_line_number((uint32_t*) "encoding error", line_number);
    printf3((uint32_t*) "%d expected between %d and %d\n",
      (uint32_t*) immediate,
      (uint32_t*) -two_to_the_power_of(bits - 1),
      (uint32_t*) two_to_the_power_of(bits - 1) - 1);

    exit(EXITCODE_COMPILERERROR);
  }
}

// RISC-V R Format
// ----------------------------------------------------------------
// |        7         |  5  |  5  |  3   |        5        |  7   |
// +------------------+-----+-----+------+-----------------+------+
// |      funct7      | rs2 | rs1 |funct3|       rd        |opcode|
// +------------------+-----+-----+------+-----------------+------+
// |31              25|24 20|19 15|14  12|11              7|6    0|
// ----------------------------------------------------------------

uint32_t encode_r_format(uint32_t funct7, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t rd, uint32_t opcode) {
  // assert: 0 <= funct7 < 2^7
  // assert: 0 <= rs2 < 2^5
  // assert: 0 <= rs1 < 2^5
  // assert: 0 <= funct3 < 2^3
  // assert: 0 <= rd < 2^5
  // assert: 0 <= opcode < 2^7

  return left_shift(left_shift(left_shift(left_shift(left_shift(funct7, 5) + rs2, 5) + rs1, 3) + funct3, 5) + rd, 7) + opcode;
}

uint32_t get_funct7(uint32_t instruction) {
  return get_bits(instruction, 25, 7);
}

uint32_t get_rs2(uint32_t instruction) {
  return get_bits(instruction, 20, 5);
}

uint32_t get_rs1(uint32_t instruction) {
  return get_bits(instruction, 15, 5);
}

uint32_t get_funct3(uint32_t instruction) {
  return get_bits(instruction, 12, 3);
}

uint32_t get_rd(uint32_t instruction) {
  return get_bits(instruction, 7, 5);
}

uint32_t get_opcode(uint32_t instruction) {
  return get_bits(instruction, 0, 7);
}

void decode_r_format() {
  funct7 = get_funct7(ir);
  rs2    = get_rs2(ir);
  rs1    = get_rs1(ir);
  funct3 = get_funct3(ir);
  rd     = get_rd(ir);
  imm    = 0;
}

// RISC-V I Format
// ----------------------------------------------------------------
// |           12           |  5  |  3   |        5        |  7   |
// +------------------------+-----+------+-----------------+------+
// |    immediate[11:0]     | rs1 |funct3|       rd        |opcode|
// +------------------------+-----+------+-----------------+------+
// |31                    20|19 15|14  12|11              7|6    0|
// ----------------------------------------------------------------

uint32_t encode_i_format(uint32_t immediate, uint32_t rs1, uint32_t funct3, uint32_t rd, uint32_t opcode) {
  // assert: -2^11 <= immediate < 2^11
  // assert: 0 <= rs1 < 2^5
  // assert: 0 <= funct3 < 2^3
  // assert: 0 <= rd < 2^5
  // assert: 0 <= opcode < 2^7

  check_immediate_range(immediate, 12);

  immediate = sign_shrink(immediate, 12);

  return left_shift(left_shift(left_shift(left_shift(immediate, 5) + rs1, 3) + funct3, 5) + rd, 7) + opcode;
}

uint32_t get_immediate_i_format(uint32_t instruction) {
  return sign_extend(get_bits(instruction, 20, 12), 12);
}

void decode_i_format() {
  funct7 = 0;
  rs2    = 0;
  rs1    = get_rs1(ir);
  funct3 = get_funct3(ir);
  rd     = get_rd(ir);
  imm    = get_immediate_i_format(ir);
}

// RISC-V S Format
// ----------------------------------------------------------------
// |        7         |  5  |  5  |  3   |        5        |  7   |
// +------------------+-----+-----+------+-----------------+------+
// |    imm1[11:5]    | rs2 | rs1 |funct3|    imm2[4:0]    |opcode|
// +------------------+-----+-----+------+-----------------+------+
// |31              25|24 20|19 15|14  12|11              7|6    0|
// ----------------------------------------------------------------

uint32_t encode_s_format(uint32_t immediate, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t opcode) {
  // assert: -2^11 <= immediate < 2^11
  // assert: 0 <= rs2 < 2^5
  // assert: 0 <= rs1 < 2^5
  // assert: 0 <= funct3 < 2^3
  // assert: 0 <= opcode < 2^7
  uint32_t imm1;
  uint32_t imm2;

  check_immediate_range(immediate, 12);

  immediate = sign_shrink(immediate, 12);

  imm1 = get_bits(immediate, 5, 7);
  imm2 = get_bits(immediate, 0, 5);

  return left_shift(left_shift(left_shift(left_shift(left_shift(imm1, 5) + rs2, 5) + rs1, 3) + funct3, 5) + imm2, 7) + opcode;
}

uint32_t get_immediate_s_format(uint32_t instruction) {
  uint32_t imm1;
  uint32_t imm2;

  imm1 = get_bits(instruction, 25, 7);
  imm2 = get_bits(instruction,  7, 5);

  return sign_extend(left_shift(imm1, 5) + imm2, 12);
}

void decode_s_format() {
  funct7 = 0;
  rs2    = get_rs2(ir);
  rs1    = get_rs1(ir);
  funct3 = get_funct3(ir);
  rd     = 0;
  imm    = get_immediate_s_format(ir);
}

// RISC-V B Format
// ----------------------------------------------------------------
// |        7         |  5  |  5  |  3   |        5        |  7   |
// +------------------+-----+-----+------+-----------------+------+
// |imm1[12]imm2[10:5]| rs2 | rs1 |funct3|imm3[4:1]imm4[11]|opcode|
// +------------------+-----+-----+------+-----------------+------+
// |31              25|24 20|19 15|14  12|11              7|6    0|
// ----------------------------------------------------------------

uint32_t encode_b_format(uint32_t immediate, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t opcode) {
  // assert: -2^12 <= immediate < 2^12
  // assert: 0 <= rs2 < 2^5
  // assert: 0 <= rs1 < 2^5
  // assert: 0 <= funct3 < 2^3
  // assert: 0 <= opcode < 2^7
  uint32_t imm1;
  uint32_t imm2;
  uint32_t imm3;
  uint32_t imm4;

  check_immediate_range(immediate, 13);

  immediate = sign_shrink(immediate, 13);

  // LSB of immediate is lost
  imm1 = get_bits(immediate, 12, 1);
  imm2 = get_bits(immediate,  5, 6);
  imm3 = get_bits(immediate,  1, 4);
  imm4 = get_bits(immediate, 11, 1);

  return left_shift(left_shift(left_shift(left_shift(left_shift(left_shift(left_shift(imm1, 6) + imm2, 5) + rs2, 5) + rs1, 3) + funct3, 4) + imm3, 1) + imm4, 7) + opcode;
}

uint32_t get_immediate_b_format(uint32_t instruction) {
  uint32_t imm1;
  uint32_t imm2;
  uint32_t imm3;
  uint32_t imm4;

  imm1 = get_bits(instruction, 31, 1);
  imm2 = get_bits(instruction, 25, 6);
  imm3 = get_bits(instruction,  8, 4);
  imm4 = get_bits(instruction,  7, 1);

  // reassemble immediate and add trailing zero
  return sign_extend(left_shift(left_shift(left_shift(left_shift(imm1, 1) + imm4, 6) + imm2, 4) + imm3, 1), 13);
}

void decode_b_format() {
  funct7 = 0;
  rs2    = get_rs2(ir);
  rs1    = get_rs1(ir);
  funct3 = get_funct3(ir);
  rd     = 0;
  imm    = get_immediate_b_format(ir);
}

// RISC-V J Format
// ----------------------------------------------------------------
// |                  20                 |        5        |  7   |
// +-------------------------------------+-----------------+------+
// |imm1[20]imm2[10:1]imm3[11]imm4[19:12]|       rd        |opcode|
// +-------------------------------------+-----------------+------+
// |31                                 12|11              7|6    0|
// ----------------------------------------------------------------

uint32_t encode_j_format(uint32_t immediate, uint32_t rd, uint32_t opcode) {
  // assert: -2^20 <= immediate < 2^20
  // assert: 0 <= rd < 2^5
  // assert: 0 <= opcode < 2^7
  uint32_t imm1;
  uint32_t imm2;
  uint32_t imm3;
  uint32_t imm4;

  check_immediate_range(immediate, 21);

  immediate = sign_shrink(immediate, 21);

  // LSB of immediate is lost
  imm1 = get_bits(immediate, 20,  1);
  imm2 = get_bits(immediate,  1, 10);
  imm3 = get_bits(immediate, 11,  1);
  imm4 = get_bits(immediate, 12,  8);

  return left_shift(left_shift(left_shift(left_shift(left_shift(imm1, 10) + imm2, 1) + imm3, 8) + imm4, 5) + rd, 7) + opcode;
}

uint32_t get_immediate_j_format(uint32_t instruction) {
  uint32_t imm1;
  uint32_t imm2;
  uint32_t imm3;
  uint32_t imm4;

  imm1 = get_bits(instruction, 31,  1);
  imm2 = get_bits(instruction, 21, 10);
  imm3 = get_bits(instruction, 20,  1);
  imm4 = get_bits(instruction, 12,  8);

  // reassemble immediate and add trailing zero
  return sign_extend(left_shift(left_shift(left_shift(left_shift(imm1, 8) + imm4, 1) + imm3, 10) + imm2, 1), 21);
}

void decode_j_format() {
  funct7 = 0;
  rs2    = 0;
  rs1    = 0;
  funct3 = 0;
  rd     = get_rd(ir);
  imm    = get_immediate_j_format(ir);
}

// RISC-V U Format
// ----------------------------------------------------------------
// |                  20                 |        5        |  7   |
// +-------------------------------------+-----------------+------+
// |           immediate[19:0]           |       rd        |opcode|
// +-------------------------------------+-----------------+------+
// |31                                 12|11              7|6    0|
// ----------------------------------------------------------------

uint32_t encode_u_format(uint32_t immediate, uint32_t rd, uint32_t opcode) {
  // assert: -2^19 <= immediate < 2^19
  // assert: 0 <= rd < 2^5
  // assert: 0 <= opcode < 2^7

  check_immediate_range(immediate, 20);

  immediate = sign_shrink(immediate, 20);

  return left_shift(left_shift(immediate, 5) + rd, 7) + opcode;
}

uint32_t get_immediate_u_format(uint32_t instruction) {
  return sign_extend(get_bits(instruction, 12, 20), 20);
}

void decode_u_format() {
  funct7 = 0;
  rs2    = 0;
  rs1    = 0;
  funct3 = 0;
  rd     = get_rd(ir);
  imm    = get_immediate_u_format(ir);
}

// -----------------------------------------------------------------
// ---------------------------- BINARY -----------------------------
// -----------------------------------------------------------------

void reset_instruction_counters() {
  ic_lui   = 0;
  ic_addi  = 0;
  ic_add   = 0;
  ic_sub   = 0;
  ic_mul   = 0;
  ic_divu  = 0;
  ic_remu  = 0;
  ic_sltu  = 0;
  ic_lw    = 0;
  ic_sw    = 0;
  ic_beq   = 0;
  ic_jal   = 0;
  ic_jalr  = 0;
  ic_ecall = 0;
}

uint32_t get_total_number_of_instructions() {
  return ic_lui + ic_addi + ic_add + ic_sub + ic_mul + ic_divu + ic_remu + ic_sltu + ic_lw + ic_sw + ic_beq + ic_jal + ic_jalr + ic_ecall;
}

void print_instruction_counter(uint32_t total, uint32_t counter, uint32_t* mnemonics) {
  printf3((uint32_t*)
    "%s: %d(%.2d%%)",
    mnemonics,
    (uint32_t*) counter,
    (uint32_t*) fixed_point_percentage(fixed_point_ratio(total, counter, 4), 4));
}

void print_instruction_counters() {
  uint32_t ic;

  ic = get_total_number_of_instructions();

  printf1((uint32_t*) "%s: init:    ", selfie_name);
  print_instruction_counter(ic, ic_lui, (uint32_t*) "lui");
  print((uint32_t*) ", ");
  print_instruction_counter(ic, ic_addi, (uint32_t*) "addi");
  println();

  printf1((uint32_t*) "%s: memory:  ", selfie_name);
  print_instruction_counter(ic, ic_lw, (uint32_t*) "lw");
  print((uint32_t*) ", ");
  print_instruction_counter(ic, ic_sw, (uint32_t*) "sw");
  println();

  printf1((uint32_t*) "%s: compute: ", selfie_name);
  print_instruction_counter(ic, ic_add, (uint32_t*) "add");
  print((uint32_t*) ", ");
  print_instruction_counter(ic, ic_sub, (uint32_t*) "sub");
  print((uint32_t*) ", ");
  print_instruction_counter(ic, ic_mul, (uint32_t*) "mul");
  print((uint32_t*) ", ");
  print_instruction_counter(ic, ic_divu, (uint32_t*) "divu");
  print((uint32_t*) ", ");
  print_instruction_counter(ic, ic_remu, (uint32_t*) "remu");
  println();

  printf1((uint32_t*) "%s: control: ", selfie_name);
  print_instruction_counter(ic, ic_sltu, (uint32_t*) "sltu");
  print((uint32_t*) ", ");
  print_instruction_counter(ic, ic_beq, (uint32_t*) "beq");
  print((uint32_t*) ", ");
  print_instruction_counter(ic, ic_jal, (uint32_t*) "jal");
  print((uint32_t*) ", ");
  print_instruction_counter(ic, ic_jalr, (uint32_t*) "jalr");
  print((uint32_t*) ", ");
  print_instruction_counter(ic, ic_ecall, (uint32_t*) "ecall");
  println();
}

uint32_t load_instruction(uint32_t baddr) {
  return *(binary + baddr / REGISTERSIZE);
}

void store_instruction(uint32_t baddr, uint32_t instruction) {
  uint32_t temp;

  if (baddr >= MAX_CODE_LENGTH) {
    syntax_error_message((uint32_t*) "maximum code length exceeded");

    exit(EXITCODE_COMPILERERROR);
  }

  *(binary + baddr / REGISTERSIZE) = instruction;
}

uint32_t load_data(uint32_t baddr) {
  return *(binary + baddr / REGISTERSIZE);
}

void store_data(uint32_t baddr, uint32_t data) {
  if (baddr >= MAX_CODE_LENGTH + MAX_DATA_LENGTH) {
    syntax_error_message((uint32_t*) "maximum data length exceeded");

    exit(EXITCODE_COMPILERERROR);
  }

  *(binary + baddr / REGISTERSIZE) = data;
}

void emit_instruction(uint32_t instruction) {
  store_instruction(binary_length, instruction);

  if (*(code_line_number + binary_length / INSTRUCTIONSIZE) == 0)
    *(code_line_number + binary_length / INSTRUCTIONSIZE) = line_number;

  binary_length = binary_length + INSTRUCTIONSIZE;
}

void emit_nop() {
  emit_instruction(encode_i_format(0, REG_ZR, F3_NOP, REG_ZR, OP_IMM));

  ic_addi = ic_addi + 1;
}

void emit_lui(uint32_t rd, uint32_t immediate) {
  emit_instruction(encode_u_format(immediate, rd, OP_LUI));

  ic_lui = ic_lui + 1;
}

void emit_addi(uint32_t rd, uint32_t rs1, uint32_t immediate) {
  emit_instruction(encode_i_format(immediate, rs1, F3_ADDI, rd, OP_IMM));

  ic_addi = ic_addi + 1;
}

void emit_add(uint32_t rd, uint32_t rs1, uint32_t rs2) {
  emit_instruction(encode_r_format(F7_ADD, rs2, rs1, F3_ADD, rd, OP_OP));

  ic_add = ic_add + 1;
}

void emit_sub(uint32_t rd, uint32_t rs1, uint32_t rs2) {
  emit_instruction(encode_r_format(F7_SUB, rs2, rs1, F3_SUB, rd, OP_OP));

  ic_sub = ic_sub + 1;
}

void emit_mul(uint32_t rd, uint32_t rs1, uint32_t rs2) {
  emit_instruction(encode_r_format(F7_MUL, rs2, rs1, F3_MUL, rd, OP_OP));

  ic_mul = ic_mul + 1;
}

void emit_divu(uint32_t rd, uint32_t rs1, uint32_t rs2) {
  emit_instruction(encode_r_format(F7_DIVU, rs2, rs1, F3_DIVU, rd, OP_OP));

  ic_divu = ic_divu + 1;
}

void emit_remu(uint32_t rd, uint32_t rs1, uint32_t rs2) {
  emit_instruction(encode_r_format(F7_REMU, rs2, rs1, F3_REMU, rd, OP_OP));

  ic_remu = ic_remu + 1;
}

void emit_sltu(uint32_t rd, uint32_t rs1, uint32_t rs2) {
  emit_instruction(encode_r_format(F7_SLTU, rs2, rs1, F3_SLTU, rd, OP_OP));

  ic_sltu = ic_sltu + 1;
}

void emit_lw(uint32_t rd, uint32_t rs1, uint32_t immediate) {
  emit_instruction(encode_i_format(immediate, rs1, F3_LW, rd, OP_LW));

  ic_lw = ic_lw + 1;
}

void emit_sw(uint32_t rs1, uint32_t immediate, uint32_t rs2) {
  emit_instruction(encode_s_format(immediate, rs2, rs1, F3_SW, OP_SW));

  ic_sw = ic_sw + 1;
}

void emit_beq(uint32_t rs1, uint32_t rs2, uint32_t immediate) {
  emit_instruction(encode_b_format(immediate, rs2, rs1, F3_BEQ, OP_BRANCH));

  ic_beq = ic_beq + 1;
}

void emit_jal(uint32_t rd, uint32_t immediate) {
  emit_instruction(encode_j_format(immediate, rd, OP_JAL));

  ic_jal = ic_jal + 1;
}

void emit_jalr(uint32_t rd, uint32_t rs1, uint32_t immediate) {
  emit_instruction(encode_i_format(immediate, rs1, F3_JALR, rd, OP_JALR));

  ic_jalr = ic_jalr + 1;
}

void emit_ecall() {
  emit_instruction(encode_i_format(F12_ECALL, REG_ZR, F3_ECALL, REG_ZR, OP_SYSTEM));

  ic_ecall = ic_ecall + 1;
}

void fixup_relative_BFormat(uint32_t from_address) {
  uint32_t instruction;

  instruction = load_instruction(from_address);

  store_instruction(from_address,
    encode_b_format(binary_length - from_address,
      get_rs2(instruction),
      get_rs1(instruction),
      get_funct3(instruction),
      get_opcode(instruction)));
}

void fixup_relative_JFormat(uint32_t from_address, uint32_t to_address) {
  uint32_t instruction;

  instruction = load_instruction(from_address);

  store_instruction(from_address,
    encode_j_format(to_address - from_address,
      get_rd(instruction),
      get_opcode(instruction)));
}

void fixlink_relative(uint32_t from_address, uint32_t to_address) {
  uint32_t previous_address;

  while (from_address != 0) {
    previous_address = get_immediate_j_format(load_instruction(from_address));

    fixup_relative_JFormat(from_address, to_address);

    from_address = previous_address;
  }
}

void emit_data_word(uint32_t data, uint32_t offset, uint32_t source_line_number) {
  // assert: offset < 0

  store_data(binary_length + offset, data);

  if (data_line_number != (uint32_t*) 0)
    *(data_line_number + (allocated_memory + offset) / REGISTERSIZE) = source_line_number;
}

void emit_string_data(uint32_t* entry) {
  uint32_t* s;
  uint32_t i;
  uint32_t l;

  s = get_string(entry);

  i = 0;

  l = round_up(string_length(s) + 1, REGISTERSIZE);

  while (i < l) {
    emit_data_word(*s, get_address(entry) + i, get_line_number(entry));

    s = s + 1;

    i = i + REGISTERSIZE;
  }
}

void emit_data_segment() {
  uint32_t i;
  uint32_t* entry;

  binary_length = binary_length + allocated_memory;

  i = 0;

  while (i < HASH_TABLE_SIZE) {
    entry = (uint32_t*) *(global_symbol_table + i);

    // copy initial values of global variables, big integers and strings
    while ((uint32_t) entry != 0) {
      if (get_class(entry) == VARIABLE)
        emit_data_word(get_value(entry), get_address(entry), get_line_number(entry));
      else if (get_class(entry) == BIGINT)
        emit_data_word(get_value(entry), get_address(entry), get_line_number(entry));
      else if (get_class(entry) == STRING)
        emit_string_data(entry);

      entry = get_next_entry(entry);
    }

    i = i + 1;
  }

  allocated_memory = 0;
}

uint32_t* create_elf_header(uint32_t binary_length) {
  uint32_t* header;

  // store all numbers necessary to create a minimal and valid
  // ELF32 header including the program header
  header = smalloc(ELF_HEADER_LEN);

  // RISC-U ELF32 file header:
  *(header + 0) = 127                               // magic number part 0 is 0x7F
                + left_shift((uint32_t) 'E', 8)     // magic number part 1
                + left_shift((uint32_t) 'L', 16)    // magic number part 2
                + left_shift((uint32_t) 'F', 24);   // magic number part 3
  *(header + 1) = left_shift(1, 0)                  // file class is ELFCLASS32
                + left_shift(1, 8)                  // object file data structures endianess is ELFDATA2LSB
                + left_shift(1, 16);                // version of the object file format
  *(header + 2) = 0;                                // ABI version and start of padding bytes
  *(header + 3) = 0;                                // more padding bytes
  *(header + 4) = 2                                 // object file type is ET_EXEC
                + left_shift(243, 16);              // target architecture is RV32
  *(header + 5) = 1;                                // version of the object file format
  *(header + 6) = ELF_ENTRY_POINT;                  // entry point address
  *(header + 7) = 13 * SIZEOFUINT32;                // program header offset
  *(header + 8) = 0;                                // section header offset
  *(header + 9) = 0;                                // flags
  *(header + 10) = left_shift(13 * SIZEOFUINT32, 0) // elf header size
                + left_shift(8 * SIZEOFUINT32, 16); // size of program header entry
  *(header + 11) = 1;                               // number of program header entries
  *(header + 12) = 0;                               // number of section header entries

  // RISC-U ELF32 program header table:
  *(header + 13) = 1;                             // type of segment is LOAD
  *(header + 14) = ELF_HEADER_LEN + SIZEOFUINT32; // segment offset in file
  *(header + 15) = ELF_ENTRY_POINT;               // virtual address in memory
  *(header + 16) = 0;                             // physical address (reserved)
  *(header + 17) = binary_length;                 // size of segment in file
  *(header + 18) = binary_length;                 // size of segment in memory
  *(header + 19) = 7;                             // segment attributes is RWX
  *(header + 20) = PAGESIZE;                      // alignment of segment

  return header;
}

uint32_t validate_elf_header(uint32_t* header) {
  uint32_t  new_entry_point;
  uint32_t  new_binary_length;
  uint32_t  position;
  uint32_t* valid_header;

  new_entry_point   = *(header + 15);
  new_binary_length = *(header + 17);

  if (new_binary_length != *(header + 18))
    // segment size in file is not the same as segment size in memory
    return 0;

  if (new_entry_point > VIRTUALMEMORYSIZE - PAGESIZE - new_binary_length)
    // binary does not fit into virtual address space
    return 0;

  valid_header = create_elf_header(new_binary_length);

  position = 0;

  while (position < ELF_HEADER_LEN / SIZEOFUINT32) {
    if (*(header + position) != *(valid_header + position))
      return 0;

    position = position + 1;
  }

  entry_point   = new_entry_point;
  binary_length = new_binary_length;

  return 1;
}

uint32_t open_write_only(uint32_t* name) {
  // we try opening write-only files using platform-specific flags
  // to make selfie platform-independent, this may nevertheless
  // not always work and require intervention
  uint32_t fd;

  // try Mac flags
  fd = open(name, MAC_O_CREAT_TRUNC_WRONLY, S_IRUSR_IWUSR_IRGRP_IROTH);

  if (signed_less_than(fd, 0)) {
    // try Linux flags
    fd = open(name, LINUX_O_CREAT_TRUNC_WRONLY, S_IRUSR_IWUSR_IRGRP_IROTH);

    if (signed_less_than(fd, 0))
      // try Windows flags
      fd = open(name, WINDOWS_O_BINARY_CREAT_TRUNC_WRONLY, S_IRUSR_IWUSR_IRGRP_IROTH);
  }

  return fd;
}

void selfie_output() {
  uint32_t fd;

  binary_name = get_argument();

  if (binary_length == 0) {
    printf2((uint32_t*) "%s: nothing to emit to output file %s\n", selfie_name, binary_name);

    return;
  }

  // assert: binary_name is mapped and not longer than MAX_FILENAME_LENGTH

  fd = open_write_only(binary_name);

  if (signed_less_than(fd, 0)) {
    printf2((uint32_t*) "%s: could not create binary output file %s\n", selfie_name, binary_name);

    exit(EXITCODE_IOERROR);
  }

  // assert: ELF_header is mapped

  // first write ELF header
  if (write(fd, ELF_header, ELF_HEADER_LEN) != ELF_HEADER_LEN) {
    printf2((uint32_t*) "%s: could not write ELF header of binary output file %s\n", selfie_name, binary_name);

    exit(EXITCODE_IOERROR);
  }

  // then write code length
  *binary_buffer = code_length;

  if (write(fd, binary_buffer, SIZEOFUINT32) != SIZEOFUINT32) {
    printf2((uint32_t*) "%s: could not write code length of binary output file %s\n", selfie_name, binary_name);

    exit(EXITCODE_IOERROR);
  }

  // assert: binary is mapped

  // then write binary
  if (write(fd, binary, binary_length) != binary_length) {
    printf2((uint32_t*) "%s: could not write binary into binary output file %s\n", selfie_name, binary_name);

    exit(EXITCODE_IOERROR);
  }

  printf5((uint32_t*) "%s: %d bytes with %d instructions and %d bytes of data written into %s\n",
    selfie_name,
    (uint32_t*) (ELF_HEADER_LEN + SIZEOFUINT32 + binary_length),
    (uint32_t*) (code_length / INSTRUCTIONSIZE),
    (uint32_t*) (binary_length - code_length),
    binary_name);
}

uint32_t* touch(uint32_t* memory, uint32_t length) {
  uint32_t* m;
  uint32_t n;

  m = memory;

  if (length > 0)
    // touch memory at beginning
    n = *m;

  while (length > PAGESIZE) {
    length = length - PAGESIZE;

    m = m + PAGESIZE / REGISTERSIZE;

    // touch every following page
    n = *m;
  }

  if (length > 0) {
    m = m + (length - 1) / REGISTERSIZE;

    // touch at end
    n = *m;
  }

  // avoids unused warning for n
  n = 0; n = n + 1;

  return memory;
}

void selfie_load() {
  uint32_t fd;
  uint32_t number_of_read_bytes;

  binary_name = get_argument();

  // assert: binary_name is mapped and not longer than MAX_FILENAME_LENGTH

  fd = open(binary_name, O_RDONLY, 0);

  if (signed_less_than(fd, 0)) {
    printf2((uint32_t*) "%s: could not open input file %s\n", selfie_name, binary_name);

    exit(EXITCODE_IOERROR);
  }

  // make sure binary is mapped for reading into it
  binary = touch(smalloc(MAX_BINARY_LENGTH), MAX_BINARY_LENGTH);

  binary_length = 0;
  code_length   = 0;
  entry_point   = 0;

  // no source line numbers in binaries
  code_line_number = (uint32_t*) 0;
  data_line_number = (uint32_t*) 0;

  // make sure ELF_header is mapped for reading into it
  ELF_header = touch(smalloc(ELF_HEADER_LEN), ELF_HEADER_LEN);

  // read ELF_header first
  number_of_read_bytes = read(fd, ELF_header, ELF_HEADER_LEN);

  if (number_of_read_bytes == ELF_HEADER_LEN) {
    if (validate_elf_header(ELF_header)) {
      // now read code length
      number_of_read_bytes = read(fd, binary_buffer, SIZEOFUINT32);

      if (number_of_read_bytes == SIZEOFUINT32) {
        code_length = *binary_buffer;

        if (binary_length <= MAX_BINARY_LENGTH) {
          // now read binary including global variables and strings
          number_of_read_bytes = read(fd, binary, binary_length);

          if (signed_less_than(0, number_of_read_bytes)) {
            // check if we are really at EOF
            if (read(fd, binary_buffer, SIZEOFUINT32) == 0) {
              printf5((uint32_t*) "%s: %d bytes with %d instructions and %d bytes of data loaded from %s\n",
                selfie_name,
                (uint32_t*) (ELF_HEADER_LEN + SIZEOFUINT32 + binary_length),
                (uint32_t*) (code_length / INSTRUCTIONSIZE),
                (uint32_t*) (binary_length - code_length),
                binary_name);

              return;
            }
          }
        }
      }
    }
  }

  printf2((uint32_t*) "%s: failed to load code from input file %s\n", selfie_name, binary_name);

  exit(EXITCODE_IOERROR);
}

// -----------------------------------------------------------------
// ----------------------- MIPSTER SYSCALLS ------------------------
// -----------------------------------------------------------------

void emit_exit() {
  create_symbol_table_entry(LIBRARY_TABLE, (uint32_t*) "exit", 0, PROCEDURE, VOID_T, 0, binary_length);

  // load signed 32-bit integer argument for exit
  emit_lw(REG_A0, REG_SP, 0);

  // remove the argument from the stack
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  // load the correct syscall number and invoke syscall
  emit_addi(REG_A7, REG_ZR, SYSCALL_EXIT);

  emit_ecall();

  // never returns here
}

void implement_exit(uint32_t* context) {
  if (disassemble) {
    print((uint32_t*) "(exit): ");
    print_register_hexadecimal(REG_A0);
    print((uint32_t*) " |- ->\n");
  }

  set_exit_code(context, *(get_regs(context) + REG_A0));

  if (symbolic)
    return;

  printf4((uint32_t*)
    "%s: %s exiting with exit code %d and %.2dMB mallocated memory\n",
    selfie_name,
    get_name(context),
    (uint32_t*) get_exit_code(context),
    (uint32_t*) fixed_point_ratio(get_program_break(context) - get_original_break(context), MEGABYTE, 2));
}

void emit_read() {
  create_symbol_table_entry(LIBRARY_TABLE, (uint32_t*) "read", 0, PROCEDURE, UINT32_T, 0, binary_length);

  emit_lw(REG_A2, REG_SP, 0); // size
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  emit_lw(REG_A1, REG_SP, 0); // *buffer
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  emit_lw(REG_A0, REG_SP, 0); // fd
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  emit_addi(REG_A7, REG_ZR, SYSCALL_READ);

  emit_ecall();

  // jump back to caller, return value is in REG_A0
  emit_jalr(REG_ZR, REG_RA, 0);
}

void implement_read(uint32_t* context) {
  // parameters
  uint32_t fd;
  uint32_t vbuffer;
  uint32_t size;

  // local variables
  uint32_t read_total;
  uint32_t bytes_to_read;
  uint32_t failed;
  uint32_t* buffer;
  uint32_t actually_read;
  uint32_t value;
  uint32_t lo;
  uint32_t up;
  uint32_t mrvc;

  if (disassemble) {
    print((uint32_t*) "(read): ");
    print_register_value(REG_A0);
    print((uint32_t*) ",");
    print_register_hexadecimal(REG_A1);
    print((uint32_t*) ",");
    print_register_value(REG_A2);
    print((uint32_t*) " |- ");
    print_register_value(REG_A0);
  }

  fd      = *(get_regs(context) + REG_A0);
  vbuffer = *(get_regs(context) + REG_A1);
  size    = *(get_regs(context) + REG_A2);

  if (debug_read)
    printf4((uint32_t*) "%s: trying to read %d bytes from file with descriptor %d into buffer at virtual address %p\n", selfie_name, (uint32_t*) size, (uint32_t*) fd, (uint32_t*) vbuffer);

  read_total   = 0;
  bytes_to_read = SIZEOFUINT32;

  failed = 0;

  while (size > 0) {
    if (is_valid_virtual_address(vbuffer)) {
      if (is_virtual_address_mapped(get_pt(context), vbuffer)) {
        buffer = tlb(get_pt(context), vbuffer);

        if (size < bytes_to_read)
          bytes_to_read = size;

        if (symbolic) {
          if (is_trace_space_available()) {
            if (rc > 0) {
              // do not read but reuse value, lower and upper bound
              value = *(read_values + rc);

              lo = *(read_los + rc);
              up = *(read_ups + rc);

              actually_read = bytes_to_read;

              rc = rc - 1;
            } else {
              // save mrvc in buffer
              mrvc = load_physical_memory(buffer);

              // caution: read only overwrites bytes_to_read number of bytes
              // we therefore need to restore the actual value in buffer
              // to preserve the original read semantics
              store_physical_memory(buffer, *(values + load_symbolic_memory(get_pt(context), vbuffer)));

              actually_read = read(fd, buffer, bytes_to_read);

              // retrieve read value
              value = load_physical_memory(buffer);

              // fuzz read value
              lo = fuzz_lo(value);
              up = fuzz_up(value);

              // restore mrvc in buffer
              store_physical_memory(buffer, mrvc);
            }

            if (mrcc == 0)
              // no branching yet, we may overwrite symbolic memory
              store_symbolic_memory(get_pt(context), vbuffer, value, 0, lo, up, 0);
            else
              store_symbolic_memory(get_pt(context), vbuffer, value, 0, lo, up, tc);
          } else {
            actually_read = 0;

            throw_exception(EXCEPTION_MAXTRACE, 0);
          }
        } else
          actually_read = read(fd, buffer, bytes_to_read);

        if (actually_read == bytes_to_read) {
          read_total = read_total + actually_read;

          size = size - actually_read;

          if (size > 0)
            vbuffer = vbuffer + SIZEOFUINT32;
        } else {
          if (signed_less_than(0, actually_read))
            read_total = read_total + actually_read;

          size = 0;
        }
      } else {
        failed = 1;

        size = 0;

        if (debug_read)
          printf2((uint32_t*) "%s: reading into virtual address %p failed because the address is unmapped\n", selfie_name, (uint32_t*) vbuffer);
      }
    } else {
      failed = 1;

      size = 0;

      if (debug_read)
        printf2((uint32_t*) "%s: reading into virtual address %p failed because the address is invalid\n", selfie_name, (uint32_t*) vbuffer);
    }
  }

  if (failed == 0)
    *(get_regs(context) + REG_A0) = read_total;
  else
    *(get_regs(context) + REG_A0) = -1;

  if (symbolic) {
    *(reg_typ + REG_A0) = 0;

    *(reg_los + REG_A0) = *(get_regs(context) + REG_A0);
    *(reg_ups + REG_A0) = *(get_regs(context) + REG_A0);
  }

  set_pc(context, get_pc(context) + INSTRUCTIONSIZE);

  if (debug_read)
    printf3((uint32_t*) "%s: actually read %d bytes from file with descriptor %d\n", selfie_name, (uint32_t*) read_total, (uint32_t*) fd);

  if (disassemble) {
    print((uint32_t*) " -> ");
    print_register_value(REG_A0);
    println();
  }
}

void emit_write() {
  create_symbol_table_entry(LIBRARY_TABLE, (uint32_t*) "write", 0, PROCEDURE, UINT32_T, 0, binary_length);

  emit_lw(REG_A2, REG_SP, 0); // size
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  emit_lw(REG_A1, REG_SP, 0); // *buffer
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  emit_lw(REG_A0, REG_SP, 0); // fd
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  emit_addi(REG_A7, REG_ZR, SYSCALL_WRITE);

  emit_ecall();

  emit_jalr(REG_ZR, REG_RA, 0);
}

void implement_write(uint32_t* context) {
  // parameters
  uint32_t fd;
  uint32_t vbuffer;
  uint32_t size;

  // local variables
  uint32_t written_total;
  uint32_t bytes_to_write;
  uint32_t failed;
  uint32_t* buffer;
  uint32_t actually_written;

  if (disassemble) {
    print((uint32_t*) "(write): ");
    print_register_value(REG_A0);
    print((uint32_t*) ",");
    print_register_hexadecimal(REG_A1);
    print((uint32_t*) ",");
    print_register_value(REG_A2);
    print((uint32_t*) " |- ");
    print_register_value(REG_A0);
  }

  fd      = *(get_regs(context) + REG_A0);
  vbuffer = *(get_regs(context) + REG_A1);
  size    = *(get_regs(context) + REG_A2);

  if (debug_write)
    printf4((uint32_t*) "%s: trying to write %d bytes from buffer at virtual address %p into file with descriptor %d\n", selfie_name, (uint32_t*) size, (uint32_t*) vbuffer, (uint32_t*) fd);

  written_total = 0;
  bytes_to_write = SIZEOFUINT32;

  failed = 0;

  while (size > 0) {
    if (is_valid_virtual_address(vbuffer)) {
      if (is_virtual_address_mapped(get_pt(context), vbuffer)) {
        buffer = tlb(get_pt(context), vbuffer);

        if (size < bytes_to_write)
          bytes_to_write = size;

        if (symbolic)
          // TODO: What should symbolically executed code output?
          // buffer points to a trace counter that refers to the actual value
          // actually_written = write(fd, values + load_physical_memory(buffer), bytes_to_write);
          actually_written = bytes_to_write;
        else
          actually_written = write(fd, buffer, bytes_to_write);

        if (actually_written == bytes_to_write) {
          written_total = written_total + actually_written;

          size = size - actually_written;

          if (size > 0)
            vbuffer = vbuffer + SIZEOFUINT32;
        } else {
          if (signed_less_than(0, actually_written))
            written_total = written_total + actually_written;

          size = 0;
        }
      } else {
        failed = 1;

        size = 0;

        if (debug_write)
          printf2((uint32_t*) "%s: writing into virtual address %p failed because the address is unmapped\n", selfie_name, (uint32_t*) vbuffer);
      }
    } else {
      failed = 1;

      size = 0;

      if (debug_write)
        printf2((uint32_t*) "%s: writing into virtual address %p failed because the address is invalid\n", selfie_name, (uint32_t*) vbuffer);
    }
  }

  if (failed == 0)
    *(get_regs(context) + REG_A0) = written_total;
  else
    *(get_regs(context) + REG_A0) = -1;

  if (symbolic) {
    *(reg_typ + REG_A0) = 0;

    *(reg_los + REG_A0) = *(get_regs(context) + REG_A0);
    *(reg_ups + REG_A0) = *(get_regs(context) + REG_A0);
  }

  set_pc(context, get_pc(context) + INSTRUCTIONSIZE);

  if (debug_write)
    printf3((uint32_t*) "%s: actually wrote %d bytes into file with descriptor %d\n", selfie_name, (uint32_t*) written_total, (uint32_t*) fd);

  if (disassemble) {
    print((uint32_t*) " -> ");
    print_register_value(REG_A0);
    println();
  }
}

void emit_open() {
  create_symbol_table_entry(LIBRARY_TABLE, (uint32_t*) "open", 0, PROCEDURE, UINT32_T, 0, binary_length);

  emit_lw(REG_A2, REG_SP, 0); // mode
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  emit_lw(REG_A1, REG_SP, 0); // flags
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  emit_lw(REG_A0, REG_SP, 0); // filename
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  emit_addi(REG_A7, REG_ZR, SYSCALL_OPEN);

  emit_ecall();

  emit_jalr(REG_ZR, REG_RA, 0);
}

uint32_t down_load_string(uint32_t* table, uint32_t vaddr, uint32_t* s) {
  uint32_t mrvc;
  uint32_t i;
  uint32_t j;

  i = 0;

  while (i < MAX_FILENAME_LENGTH / SIZEOFUINT32) {
    if (is_valid_virtual_address(vaddr)) {
      if (is_virtual_address_mapped(table, vaddr)) {
        if (symbolic) {
          mrvc = load_symbolic_memory(table, vaddr);

          *(s + i) = *(values + mrvc);

          if (is_symbolic_value(*(types + mrvc), *(los + mrvc), *(ups + mrvc))) {
            printf1((uint32_t*) "%s: detected symbolic value ", selfie_name);
            print_symbolic_memory(mrvc);
            print((uint32_t*) " in filename of open call\n");

            exit(EXITCODE_SYMBOLICEXECUTIONERROR);
          }
        } else
          *(s + i) = load_virtual_memory(table, vaddr);

        j = 0;

        // check if string ends in the current machine word
        while (j < SIZEOFUINT32) {
          if (load_character(s + i, j) == 0)
            return 1;

          j = j + 1;
        }

        // advance to the next machine word in virtual memory
        vaddr = vaddr + SIZEOFUINT32;

        // advance to the next machine word in our memory
        i = i + 1;
      } else if (debug_open)
        printf2((uint32_t*) "%s: opening file with name at virtual address %p failed because the address is unmapped\n", selfie_name, (uint32_t*) vaddr);
    } else if (debug_open)
      printf2((uint32_t*) "%s: opening file with name at virtual address %p failed because the address is invalid\n", selfie_name, (uint32_t*) vaddr);
  }

  return 0;
}

void implement_open(uint32_t* context) {
  // parameters
  uint32_t vfilename;
  uint32_t flags;
  uint32_t mode;

  // return value
  uint32_t fd;

  if (disassemble) {
    print((uint32_t*) "(open): ");
    print_register_hexadecimal(REG_A0);
    print((uint32_t*) ",");
    print_register_hexadecimal(REG_A1);
    print((uint32_t*) ",");
    print_register_octal(REG_A2);
    print((uint32_t*) " |- ");
    print_register_value(REG_A0);
  }

  vfilename = *(get_regs(context) + REG_A0);
  flags     = *(get_regs(context) + REG_A1);
  mode      = *(get_regs(context) + REG_A2);

  if (down_load_string(get_pt(context), vfilename, filename_buffer)) {
    fd = open(filename_buffer, flags, mode);

    *(get_regs(context) + REG_A0) = fd;

    if (debug_open)
      printf5((uint32_t*) "%s: opened file %s with flags %x and mode %o returning file descriptor %d\n", selfie_name, filename_buffer, (uint32_t*) flags, (uint32_t*) mode, (uint32_t*) fd);
  } else {
    *(get_regs(context) + REG_A0) = -1;

    if (debug_open)
      printf2((uint32_t*) "%s: opening file with name at virtual address %p failed because the name is too long\n", selfie_name, (uint32_t*) vfilename);
  }

  if (symbolic) {
    *(reg_typ + REG_A0) = 0;

    *(reg_los + REG_A0) = *(get_regs(context) + REG_A0);
    *(reg_ups + REG_A0) = *(get_regs(context) + REG_A0);
  }

  set_pc(context, get_pc(context) + INSTRUCTIONSIZE);

  if (disassemble) {
    print((uint32_t*) " -> ");
    print_register_value(REG_A0);
    println();
  }
}

void emit_malloc() {
  uint32_t* entry;

  create_symbol_table_entry(LIBRARY_TABLE, (uint32_t*) "malloc", 0, PROCEDURE, UINT32STAR_T, 0, binary_length);

  // on boot levels higher than zero, zalloc falls back to malloc
  // assuming that page frames are zeroed on boot level zero
  create_symbol_table_entry(LIBRARY_TABLE, (uint32_t*) "zalloc", 0, PROCEDURE, UINT32STAR_T, 0, binary_length);

  // allocate memory in data segment for recording state of
  // malloc (bump pointer) in compiler-declared global variable
  allocated_memory = allocated_memory + REGISTERSIZE;

  // define global variable _bump for storing malloc's bump pointer
  // copy "_bump" string into zeroed word to obtain unique hash
  create_symbol_table_entry(GLOBAL_TABLE, string_copy((uint32_t*) "_bump"), 1, VARIABLE, UINT32_T, 0, -allocated_memory);

  // do not account for _bump as global variable
  number_of_global_variables = number_of_global_variables - 1;

  entry = search_global_symbol_table(string_copy((uint32_t*) "_bump"), VARIABLE);

  // allocate register for size parameter
  talloc();

  emit_lw(current_temporary(), REG_SP, 0); // size
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  // round up size to word alignment
  emit_round_up(current_temporary(), SIZEOFUINT32);

  // allocate register to compute new bump pointer
  talloc();

  // get current _bump which will be returned upon success
  emit_lw(current_temporary(), get_scope(entry), get_address(entry));

  // call brk syscall to set new program break to _bump + size
  emit_add(REG_A0, current_temporary(), previous_temporary());
  emit_addi(REG_A7, REG_ZR, SYSCALL_BRK);
  emit_ecall();

  // return 0 if memory allocation failed, that is,
  // if new program break is still _bump and size !=0
  emit_beq(REG_A0, current_temporary(), 2 * INSTRUCTIONSIZE);
  emit_beq(REG_ZR, REG_ZR, 4 * INSTRUCTIONSIZE);
  emit_beq(REG_ZR, previous_temporary(), 3 * INSTRUCTIONSIZE);
  emit_addi(REG_A0, REG_ZR, 0);
  emit_beq(REG_ZR, REG_ZR, 3 * INSTRUCTIONSIZE);

  // if memory was successfully allocated
  // set _bump to new program break
  // and then return original _bump
  emit_sw(get_scope(entry), get_address(entry), REG_A0);
  emit_addi(REG_A0, current_temporary(), 0);

  tfree(2);

  emit_jalr(REG_ZR, REG_RA,0);
}

void implement_brk(uint32_t* context) {
  // parameter
  uint32_t program_break;

  // local variables
  uint32_t previous_program_break;
  uint32_t valid;
  uint32_t size;

  if (disassemble) {
    print((uint32_t*) "(brk): ");
    print_register_hexadecimal(REG_A0);
  }

  program_break = *(get_regs(context) + REG_A0);

  previous_program_break = get_program_break(context);

  valid = 0;

  if (program_break >= previous_program_break)
    if (program_break < *(get_regs(context) + REG_SP))
      if (program_break % SIZEOFUINT32 == 0)
        valid = 1;

  if (valid) {
    if (disassemble)
      print((uint32_t*) " |- ->\n");

    if (debug_brk)
      printf2((uint32_t*) "%s: setting program break to %p\n", selfie_name, (uint32_t*) program_break);

    set_program_break(context, program_break);

    if (symbolic) {
      size = program_break - previous_program_break;

      // interval is memory range, not symbolic value
      *(reg_typ + REG_A0) = 1;

      // remember start and size of memory block for checking memory safety
      *(reg_los + REG_A0) = previous_program_break;
      *(reg_ups + REG_A0) = size;

      if (mrcc > 0) {
        if (is_trace_space_available())
          // since there has been branching record brk using vaddr == 0
          store_symbolic_memory(get_pt(context), 0, previous_program_break, 1, previous_program_break, size, tc);
        else {
          throw_exception(EXCEPTION_MAXTRACE, 0);

          return;
        }
      }
    }
  } else {
    // error returns current program break
    program_break = previous_program_break;

    if (debug_brk)
      printf2((uint32_t*) "%s: retrieving current program break %p\n", selfie_name, (uint32_t*) program_break);

    if (disassemble) {
      print((uint32_t*) " |- ");
      print_register_hexadecimal(REG_A0);
    }

    *(get_regs(context) + REG_A0) = program_break;

    if (disassemble) {
      print((uint32_t*) " -> ");
      print_register_hexadecimal(REG_A0);
      println();
    }

    if (symbolic) {
      *(reg_typ + REG_A0) = 0;

      *(reg_los + REG_A0) = 0;
      *(reg_ups + REG_A0) = 0;
    }
  }

  set_pc(context, get_pc(context) + INSTRUCTIONSIZE);
}


// -----------------------------------------------------------------
// ----------------------- HYPSTER SYSCALLS ------------------------
// -----------------------------------------------------------------

void emit_switch() {
  create_symbol_table_entry(LIBRARY_TABLE, (uint32_t*) "hypster_switch", 0, PROCEDURE, UINT32STAR_T, 0, binary_length);

  emit_lw(REG_A1, REG_SP, 0); // number of instructions to execute
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  emit_lw(REG_A0, REG_SP, 0); // context to which we switch
  emit_addi(REG_SP, REG_SP, REGISTERSIZE);

  emit_addi(REG_A7, REG_ZR, SYSCALL_SWITCH);

  emit_ecall();

  // save context from which we are switching here in return register
  emit_addi(REG_A0, REG_A1, 0);

  emit_jalr(REG_ZR, REG_RA, 0);
}

void do_switch(uint32_t* to_context, uint32_t timeout) {
  uint32_t* from_context;

  from_context = current_context;

  restore_context(to_context);

  // restore machine state
  pc        = get_pc(to_context);
  registers = get_regs(to_context);
  pt        = get_pt(to_context);

  // use REG_A1 instead of REG_A0 to avoid race condition with interrupt
  if (get_parent(from_context) != MY_CONTEXT)
    *(registers + REG_A1) = (uint32_t) get_virtual_context(from_context);
  else
    *(registers + REG_A1) = (uint32_t) from_context;

  current_context = to_context;

  timer = timeout;

  if (debug_switch) {
    printf3((uint32_t*) "%s: switched from context %p to context %p", selfie_name, from_context, to_context);
    if (timer != TIMEROFF)
      printf1((uint32_t*) " to execute %d instructions", (uint32_t*) timer);
    println();
  }
}

void implement_switch() {
  if (disassemble) {
    print((uint32_t*) "(switch): ");
    print_register_hexadecimal(REG_A0);
    print((uint32_t*) ",");
    print_register_value(REG_A1);
    print((uint32_t*) " |- ");
    print_register_value(REG_A1);
  }

  save_context(current_context);

  // cache context on my boot level before switching
  do_switch(cache_context((uint32_t*) *(registers + REG_A0)), *(registers + REG_A1));

  if (disassemble) {
    print((uint32_t*) " -> ");
    print_register_hexadecimal(REG_A1);
    println();
  }
}

uint32_t* mipster_switch(uint32_t* to_context, uint32_t timeout) {
  do_switch(to_context, timeout);

  run_until_exception();

  save_context(current_context);

  return current_context;
}

uint32_t* hypster_switch(uint32_t* to_context, uint32_t timeout) {
  // this procedure is only executed at boot level zero
  return mipster_switch(to_context, timeout);
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ----------------------    R U N T I M E    ----------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- MEMORY -----------------------------
// -----------------------------------------------------------------

uint32_t load_physical_memory(uint32_t* paddr) {
  return *paddr;
}

void store_physical_memory(uint32_t* paddr, uint32_t data) {
  *paddr = data;
}

uint32_t frame_for_page(uint32_t* table, uint32_t page) {
  return (uint32_t) (table + page);
}

uint32_t get_frame_for_page(uint32_t* table, uint32_t page) {
  return *(table + page);
}

uint32_t is_page_mapped(uint32_t* table, uint32_t page) {
  if (get_frame_for_page(table, page) != 0)
    return 1;
  else
    return 0;
}

uint32_t is_valid_virtual_address(uint32_t vaddr) {
  if (vaddr < VIRTUALMEMORYSIZE)
    // memory must be word-addressed for lack of byte-sized data type
    if (vaddr % REGISTERSIZE == 0)
      return 1;

  return 0;
}

uint32_t get_page_of_virtual_address(uint32_t vaddr) {
  return vaddr / PAGESIZE;
}

uint32_t is_virtual_address_mapped(uint32_t* table, uint32_t vaddr) {
  // assert: is_valid_virtual_address(vaddr) == 1

  return is_page_mapped(table, get_page_of_virtual_address(vaddr));
}

uint32_t* tlb(uint32_t* table, uint32_t vaddr) {
  uint32_t page;
  uint32_t frame;
  uint32_t paddr;

  // assert: is_valid_virtual_address(vaddr) == 1
  // assert: is_virtual_address_mapped(table, vaddr) == 1

  page = get_page_of_virtual_address(vaddr);

  frame = get_frame_for_page(table, page);

  // map virtual address to physical address
  paddr = vaddr - page * PAGESIZE + frame;

  if (debug_tlb)
    printf5((uint32_t*) "%s: tlb access:\n vaddr: %p\n page:  %p\n frame: %p\n paddr: %p\n", selfie_name, (uint32_t*) vaddr, (uint32_t*) (page * PAGESIZE), (uint32_t*) frame, (uint32_t*) paddr);

  return (uint32_t*) paddr;
}

uint32_t load_virtual_memory(uint32_t* table, uint32_t vaddr) {
  // assert: is_valid_virtual_address(vaddr) == 1
  // assert: is_virtual_address_mapped(table, vaddr) == 1

  return load_physical_memory(tlb(table, vaddr));
}

void store_virtual_memory(uint32_t* table, uint32_t vaddr, uint32_t data) {
  // assert: is_valid_virtual_address(vaddr) == 1
  // assert: is_virtual_address_mapped(table, vaddr) == 1

  store_physical_memory(tlb(table, vaddr), data);
}

// -----------------------------------------------------------------
// ------------------------- INSTRUCTIONS --------------------------
// -----------------------------------------------------------------

void print_code_line_number_for_instruction(uint32_t a) {
  if (code_line_number != (uint32_t*) 0)
    printf1((uint32_t*) "(~%d)", (uint32_t*) *(code_line_number + a / INSTRUCTIONSIZE));
}

void print_code_context_for_instruction(uint32_t a) {
  if (execute) {
    printf2((uint32_t*) "%s: $pc=%x", binary_name, (uint32_t*) pc);
    print_code_line_number_for_instruction(pc - entry_point);
  } else {
    printf1((uint32_t*) "%x", (uint32_t*) pc);
    if (disassemble_verbose) {
      print_code_line_number_for_instruction(pc);
      printf1((uint32_t*) ": %p", (uint32_t*) ir);
    }
  }
  print((uint32_t*) ": ");
}

void print_lui() {
  print_code_context_for_instruction(pc);
  printf2((uint32_t*) "lui %s,%x", get_register_name(rd), (uint32_t*) sign_shrink(imm, 20));
}

void print_lui_before() {
  print((uint32_t*) ": |- ");
  print_register_hexadecimal(rd);
}

void print_lui_after() {
  print((uint32_t*) " -> ");
  print_register_hexadecimal(rd);
}

void record_lui_addi_add_sub_mul_sltu_jal_jalr() {
  record_state(*(registers + rd));
}

void do_lui() {
  // load upper immediate

  if (rd != REG_ZR)
    // semantics of lui
    *(registers + rd) = left_shift(imm, 12);

  pc = pc + INSTRUCTIONSIZE;

  ic_lui = ic_lui + 1;
}

void undo_lui_addi_add_sub_mul_divu_remu_sltu_lw_jal_jalr() {
  *(registers + rd) = *(values + (tc % MAX_REPLAY_LENGTH));
}

void constrain_lui() {
  if (rd != REG_ZR) {
    *(reg_typ + rd) = 0;

    // interval semantics of lui
    *(reg_los + rd) = left_shift(imm, 12);
    *(reg_ups + rd) = left_shift(imm, 12);

    // rd has no constraint
    set_constraint(rd, 0, 0, 0, 0, 0);
  }
}

void print_addi() {
  print_code_context_for_instruction(pc);

  if (rd == REG_ZR)
    if (rs1 == REG_ZR)
      if (imm == 0) {
        print((uint32_t*) "nop");

        return;
      }

  printf3((uint32_t*) "addi %s,%s,%d", get_register_name(rd), get_register_name(rs1), (uint32_t*) imm);
}

void print_addi_before() {
  print((uint32_t*) ": ");
  print_register_value(rs1);
  print((uint32_t*) " |- ");
  print_register_value(rd);
}

void print_addi_add_sub_mul_divu_remu_sltu_after() {
  print((uint32_t*) " -> ");
  print_register_value(rd);
}

void do_addi() {
  // add immediate

  if (rd != REG_ZR)
    // semantics of addi
    *(registers + rd) = *(registers + rs1) + imm;

  pc = pc + INSTRUCTIONSIZE;

  ic_addi = ic_addi + 1;
}

void constrain_addi() {
  if (rd != REG_ZR) {
    if (*(reg_typ + rs1)) {
      *(reg_typ + rd) = *(reg_typ + rs1);

      *(reg_los + rd) = *(reg_los + rs1);
      *(reg_ups + rd) = *(reg_ups + rs1);

      // rd has no constraint if rs1 is memory range
      set_constraint(rd, 0, 0, 0, 0, 0);

      return;
    }

    *(reg_typ + rd) = 0;

    // interval semantics of addi
    *(reg_los + rd) = *(reg_los + rs1) + imm;
    *(reg_ups + rd) = *(reg_ups + rs1) + imm;

    if (*(reg_hasco + rs1)) {
      if (*(reg_hasmn + rs1)) {
        // rs1 constraint has already minuend and cannot have another addend
        printf2((uint32_t*) "%s: detected invalid minuend expression in operand of addi at %x", selfie_name, (uint32_t*) pc);
        print_code_line_number_for_instruction(pc - entry_point);
        println();

        exit(EXITCODE_SYMBOLICEXECUTIONERROR);
      } else
        // rd inherits rs1 constraint
        set_constraint(rd, *(reg_hasco + rs1), *(reg_vaddr + rs1), 0, *(reg_colos + rs1) + imm, *(reg_coups + rs1) + imm);
    } else
      // rd has no constraint if rs1 has none
      set_constraint(rd, 0, 0, 0, 0, 0);
  }
}

void print_add_sub_mul_divu_remu_sltu(uint32_t *mnemonics) {
  print_code_context_for_instruction(pc);
  printf4((uint32_t*) "%s %s,%s,%s", mnemonics, get_register_name(rd), get_register_name(rs1), get_register_name(rs2));
}

void print_add_sub_mul_divu_remu_sltu_before() {
  print((uint32_t*) ": ");
  print_register_value(rs1);
  print((uint32_t*) ",");
  print_register_value(rs2);
  print((uint32_t*) " |- ");
  print_register_value(rd);
}

void do_add() {
  if (rd != REG_ZR)
    // semantics of add
    *(registers + rd) = *(registers + rs1) + *(registers + rs2);

  pc = pc + INSTRUCTIONSIZE;

  ic_add = ic_add + 1;
}

void constrain_add() {
  if (rd != REG_ZR) {
    if (*(reg_typ + rs1)) {
      if (*(reg_typ + rs2)) {
        // adding two pointers is undefined
        printf2((uint32_t*) "%s: undefined addition of two pointers at %x", selfie_name, (uint32_t*) pc);
        print_code_line_number_for_instruction(pc - entry_point);
        println();

        exit(EXITCODE_SYMBOLICEXECUTIONERROR);
      }

      *(reg_typ + rd) = *(reg_typ + rs1);

      *(reg_los + rd) = *(reg_los + rs1);
      *(reg_ups + rd) = *(reg_ups + rs1);

      // rd has no constraint if rs1 is memory range
      set_constraint(rd, 0, 0, 0, 0, 0);

      return;
    } else if (*(reg_typ + rs2)) {
      *(reg_typ + rd) = *(reg_typ + rs2);

      *(reg_los + rd) = *(reg_los + rs2);
      *(reg_ups + rd) = *(reg_ups + rs2);

      // rd has no constraint if rs2 is memory range
      set_constraint(rd, 0, 0, 0, 0, 0);

      return;
    }

    *(reg_typ + rd) = 0;

    // interval semantics of add
    if (combined_cardinality(*(reg_los + rs1), *(reg_ups + rs1), *(reg_los + rs2), *(reg_ups + rs2)) == 0) {
      *(reg_los + rd) = 0;
      *(reg_ups + rd) = UINT32_MAX;
    } else {
      *(reg_los + rd) = *(reg_los + rs1) + *(reg_los + rs2);
      *(reg_ups + rd) = *(reg_ups + rs1) + *(reg_ups + rs2);
    }

    if (*(reg_hasco + rs1)) {
      if (*(reg_hasco + rs2))
        // we cannot keep track of more than one constraint for add but
        // need to warn about their earlier presence if used in comparisons
        set_constraint(rd, *(reg_hasco + rs1) + *(reg_hasco + rs2), 0, 0, 0, 0);
      else if (*(reg_hasmn + rs1)) {
        // rs1 constraint has already minuend and cannot have another addend
        printf2((uint32_t*) "%s: detected invalid minuend expression in left operand of add at %x", selfie_name, (uint32_t*) pc);
        print_code_line_number_for_instruction(pc - entry_point);
        println();

        exit(EXITCODE_SYMBOLICEXECUTIONERROR);
      } else
        // rd inherits rs1 constraint since rs2 has none
        set_constraint(rd, *(reg_hasco + rs1), *(reg_vaddr + rs1), 0, *(reg_colos + rs1) + *(reg_los + rs2), *(reg_coups + rs1) + *(reg_ups + rs2));
    } else if (*(reg_hasco + rs2)) {
      if (*(reg_hasmn + rs2)) {
        // rs2 constraint has already minuend and cannot have another addend
        printf2((uint32_t*) "%s: detected invalid minuend expression in right operand of add at %x", selfie_name, (uint32_t*) pc);
        print_code_line_number_for_instruction(pc - entry_point);
        println();

        exit(EXITCODE_SYMBOLICEXECUTIONERROR);
      } else
        // rd inherits rs2 constraint since rs1 has none
        set_constraint(rd, *(reg_hasco + rs2), *(reg_vaddr + rs2), 0, *(reg_los + rs1) + *(reg_colos + rs2), *(reg_ups + rs1) + *(reg_coups + rs2));
    } else
      // rd has no constraint if both rs1 and rs2 have no constraints
      set_constraint(rd, 0, 0, 0, 0, 0);
  }
}

void do_sub() {
  if (rd != REG_ZR)
    // semantics of sub
    *(registers + rd) = *(registers + rs1) - *(registers + rs2);

  pc = pc + INSTRUCTIONSIZE;

  ic_sub = ic_sub + 1;
}

void constrain_sub() {
  uint32_t sub_los;
  uint32_t sub_ups;

  if (rd != REG_ZR) {
    if (*(reg_typ + rs1)) {
      if (*(reg_typ + rs2)) {
        if (*(reg_los + rs1) == *(reg_los + rs2))
          if (*(reg_ups + rs1) == *(reg_ups + rs2)) {
            *(reg_typ + rd) = 0;

            *(reg_los + rd) = *(registers + rd);
            *(reg_ups + rd) = *(registers + rd);

            // rd has no constraint if rs1 and rs2 are memory range
            set_constraint(rd, 0, 0, 0, 0, 0);

            return;
          }

        // subtracting incompatible pointers
        throw_exception(EXCEPTION_INVALIDADDRESS, 0);

        return;
      } else {
        *(reg_typ + rd) = *(reg_typ + rs1);

        *(reg_los + rd) = *(reg_los + rs1);
        *(reg_ups + rd) = *(reg_ups + rs1);

        // rd has no constraint if rs1 is memory range
        set_constraint(rd, 0, 0, 0, 0, 0);

        return;
      }
    } else if (*(reg_typ + rs2)) {
      *(reg_typ + rd) = *(reg_typ + rs2);

      *(reg_los + rd) = *(reg_los + rs2);
      *(reg_ups + rd) = *(reg_ups + rs2);

      // rd has no constraint if rs2 is memory range
      set_constraint(rd, 0, 0, 0, 0, 0);

      return;
    }

    *(reg_typ + rd) = 0;

    // interval semantics of sub
    if (combined_cardinality(*(reg_los + rs1), *(reg_ups + rs1), *(reg_los + rs2), *(reg_ups + rs2)) == 0) {
      *(reg_los + rd) = 0;
      *(reg_ups + rd) = UINT32_MAX;
    } else {
      // use temporary variables since rd may be rs1 or rs2
      sub_los = *(reg_los + rs1) - *(reg_ups + rs2);
      sub_ups = *(reg_ups + rs1) - *(reg_los + rs2);

      *(reg_los + rd) = sub_los;
      *(reg_ups + rd) = sub_ups;
    }

    if (*(reg_hasco + rs1)) {
      if (*(reg_hasco + rs2))
        // we cannot keep track of more than one constraint for sub but
        // need to warn about their earlier presence if used in comparisons
        set_constraint(rd, *(reg_hasco + rs1) + *(reg_hasco + rs2), 0, 0, 0, 0);
      else if (*(reg_hasmn + rs1)) {
        // rs1 constraint has already minuend and cannot have another subtrahend
        printf2((uint32_t*) "%s: detected invalid minuend expression in left operand of sub at %x", selfie_name, (uint32_t*) pc);
        print_code_line_number_for_instruction(pc - entry_point);
        println();

        exit(EXITCODE_SYMBOLICEXECUTIONERROR);
      } else
        // rd inherits rs1 constraint since rs2 has none
        set_constraint(rd, *(reg_hasco + rs1), *(reg_vaddr + rs1), 0, *(reg_colos + rs1) - *(reg_ups + rs2), *(reg_coups + rs1) - *(reg_los + rs2));
    } else if (*(reg_hasco + rs2)) {
      if (*(reg_hasmn + rs2)) {
        // rs2 constraint has already minuend and cannot have another minuend
        printf2((uint32_t*) "%s: detected invalid minuend expression in right operand of sub at %x", selfie_name, (uint32_t*) pc);
        print_code_line_number_for_instruction(pc - entry_point);
        println();

        exit(EXITCODE_SYMBOLICEXECUTIONERROR);
      } else
        // rd inherits rs2 constraint since rs1 has none
        set_constraint(rd, *(reg_hasco + rs2), *(reg_vaddr + rs2), 1, *(reg_los + rs1) - *(reg_coups + rs2), *(reg_ups + rs1) - *(reg_colos + rs2));
    } else
      // rd has no constraint if both rs1 and rs2 have no constraints
      set_constraint(rd, 0, 0, 0, 0, 0);
  }
}

void do_mul() {
  if (rd != REG_ZR)
    // semantics of mul
    *(registers + rd) = *(registers + rs1) * *(registers + rs2);

  // TODO: 128-bit resolution currently not supported

  pc = pc + INSTRUCTIONSIZE;

  ic_mul = ic_mul + 1;
}

void constrain_mul() {
  if (rd != REG_ZR) {
    *(reg_typ + rd) = 0;

    // interval semantics of mul
    *(reg_los + rd) = *(reg_los + rs1) * *(reg_los + rs2);
    *(reg_ups + rd) = *(reg_ups + rs1) * *(reg_ups + rs2);

    if (*(reg_hasco + rs1)) {
      if (*(reg_hasco + rs2)) {
        // non-linear expressions are not supported
        printf2((uint32_t*) "%s: detected non-linear expression in mul at %x", selfie_name, (uint32_t*) pc);
        print_code_line_number_for_instruction(pc - entry_point);
        println();

        exit(EXITCODE_SYMBOLICEXECUTIONERROR);
      } else if (*(reg_hasmn + rs1)) {
        // rs1 constraint has already minuend and cannot have another multiplier
        printf2((uint32_t*) "%s: detected invalid minuend expression in left operand of mul at %x", selfie_name, (uint32_t*) pc);
        print_code_line_number_for_instruction(pc - entry_point);
        println();

        exit(EXITCODE_SYMBOLICEXECUTIONERROR);
      } else
        // rd inherits rs1 constraint since rs2 has none
        // assert: rs2 interval is singleton
        set_constraint(rd, *(reg_hasco + rs1), *(reg_vaddr + rs1), 0,
          *(reg_colos + rs1) + *(reg_los + rs1) * (*(reg_los + rs2) - 1), *(reg_coups + rs1) + *(reg_ups + rs1) * (*(reg_ups + rs2) - 1));
    } else if (*(reg_hasco + rs2)) {
      if (*(reg_hasmn + rs2)) {
        // rs2 constraint has already minuend and cannot have another multiplicand
        printf2((uint32_t*) "%s: detected invalid minuend expression in right operand of mul at %x", selfie_name, (uint32_t*) pc);
        print_code_line_number_for_instruction(pc - entry_point);
        println();

        exit(EXITCODE_SYMBOLICEXECUTIONERROR);
      } else
        // rd inherits rs2 constraint since rs1 has none
        // assert: rs1 interval is singleton
        set_constraint(rd, *(reg_hasco + rs2), *(reg_vaddr + rs2), 0,
          (*(reg_los + rs1) - 1) * *(reg_los + rs2) + *(reg_colos + rs2),
          (*(reg_ups + rs1) - 1) * *(reg_ups + rs2) + *(reg_coups + rs2));
    } else
      // rd has no constraint if both rs1 and rs2 have no constraints
      set_constraint(rd, 0, 0, 0, 0, 0);
  }
}

void record_divu_remu() {
  // record even for division by zero
  record_state(*(registers + rd));
}

void do_divu() {
  // division unsigned

  if (*(registers + rs2) != 0) {
    if (rd != REG_ZR)
      // semantics of divu
      *(registers + rd) = *(registers + rs1) / *(registers + rs2);

    pc = pc + INSTRUCTIONSIZE;

    ic_divu = ic_divu + 1;
  } else
    throw_exception(EXCEPTION_DIVISIONBYZERO, 0);
}

void constrain_divu() {
  if (*(reg_los + rs2) != 0) {
    if (*(reg_ups + rs2) >= *(reg_los + rs2)) {
      // 0 is not in interval
      if (rd != REG_ZR) {
        *(reg_typ + rd) = 0;

        // interval semantics of divu
        *(reg_los + rd) = *(reg_los + rs1) / *(reg_los + rs2);
        *(reg_ups + rd) = *(reg_ups + rs1) / *(reg_ups + rs2);

        if (*(reg_hasco + rs1)) {
          if (*(reg_hasco + rs2)) {
            // non-linear expressions are not supported
            printf2((uint32_t*) "%s: detected non-linear expression in divu at %x", selfie_name, (uint32_t*) pc);
            print_code_line_number_for_instruction(pc - entry_point);
            println();

            exit(EXITCODE_SYMBOLICEXECUTIONERROR);
          } else if (*(reg_hasmn + rs1)) {
            // rs1 constraint has already minuend and cannot have another divisor
            printf2((uint32_t*) "%s: detected invalid minuend expression in left operand of divu at %x", selfie_name, (uint32_t*) pc);
            print_code_line_number_for_instruction(pc - entry_point);
            println();

            exit(EXITCODE_SYMBOLICEXECUTIONERROR);
          } else
            // rd inherits rs1 constraint since rs2 has none
            // assert: rs2 interval is singleton
            set_constraint(rd, *(reg_hasco + rs1), *(reg_vaddr + rs1), 0,
              *(reg_colos + rs1) -
                (*(reg_los + rs1) - *(reg_los + rs1) / *(reg_los + rs2)),
              *(reg_coups + rs1) -
                (*(reg_ups + rs1) - *(reg_ups + rs1) / *(reg_ups + rs2)));
        } else if (*(reg_hasco + rs2)) {
          if (*(reg_hasmn + rs2)) {
            // rs2 constraint has already minuend and cannot have another dividend
            printf2((uint32_t*) "%s: detected invalid minuend expression in right operand of divu at %x", selfie_name, (uint32_t*) pc);
            print_code_line_number_for_instruction(pc - entry_point);
            println();

            exit(EXITCODE_SYMBOLICEXECUTIONERROR);
          } else
            // rd inherits rs2 constraint since rs1 has none
            // assert: rs1 interval is singleton
            set_constraint(rd, *(reg_hasco + rs2), *(reg_vaddr + rs2), 0,
              *(reg_colos + rs2) -
                (*(reg_los + rs2) - *(reg_los + rs1) / *(reg_los + rs2)),
              *(reg_coups + rs2) -
                (*(reg_ups + rs2) - *(reg_ups + rs1) / *(reg_ups + rs2)));
        } else
          // rd has no constraint if both rs1 and rs2 have no constraints
          set_constraint(rd, 0, 0, 0, 0, 0);
      }
    } else
      throw_exception(EXCEPTION_DIVISIONBYZERO, 0);
  }
}

void do_remu() {
  // remainder unsigned

  if (*(registers + rs2) != 0) {
    if (rd != REG_ZR)
      // semantics of remu
      *(registers + rd) = *(registers + rs1) % *(registers + rs2);

    pc = pc + INSTRUCTIONSIZE;

    ic_remu = ic_remu + 1;
  } else
    throw_exception(EXCEPTION_DIVISIONBYZERO, 0);
}

void constrain_remu() {
  if (*(reg_los + rs2) != 0) {
    if (*(reg_ups + rs2) >= *(reg_los + rs2)) {
      // 0 is not in interval
      if (rd != REG_ZR) {
        *(reg_typ + rd) = 0;

        // interval semantics of remu
        *(reg_los + rd) = *(reg_los + rs1) % *(reg_los + rs2);
        *(reg_ups + rd) = *(reg_ups + rs1) % *(reg_ups + rs2);

        if (*(reg_hasco + rs1)) {
          if (*(reg_hasco + rs2)) {
            // non-linear expressions are not supported
            printf2((uint32_t*) "%s: detected non-linear expression in remu at %x", selfie_name, (uint32_t*) pc);
            print_code_line_number_for_instruction(pc - entry_point);
            println();

            exit(EXITCODE_SYMBOLICEXECUTIONERROR);
          } else if (*(reg_hasmn + rs1)) {
            // rs1 constraint has already minuend and cannot have another divisor
            printf2((uint32_t*) "%s: detected invalid minuend expression in left operand of remu at %x", selfie_name, (uint32_t*) pc);
            print_code_line_number_for_instruction(pc - entry_point);
            println();

            exit(EXITCODE_SYMBOLICEXECUTIONERROR);
          } else
            // rd inherits rs1 constraint since rs2 has none
            // assert: rs2 interval is singleton
            set_constraint(rd, *(reg_hasco + rs1), *(reg_vaddr + rs1), 0,
              *(reg_colos + rs1) -
                (*(reg_los + rs1) - *(reg_los + rs1) % *(reg_los + rs2)),
              *(reg_coups + rs1) -
                (*(reg_ups + rs1) - *(reg_ups + rs1) % *(reg_ups + rs2)));
        } else if (*(reg_hasco + rs2)) {
          if (*(reg_hasmn + rs2)) {
            // rs2 constraint has already minuend and cannot have another dividend
            printf2((uint32_t*) "%s: detected invalid minuend expression in right operand of remu at %x", selfie_name, (uint32_t*) pc);
            print_code_line_number_for_instruction(pc - entry_point);
            println();

            exit(EXITCODE_SYMBOLICEXECUTIONERROR);
          } else
            // rd inherits rs2 constraint since rs1 has none
            // assert: rs1 interval is singleton
            set_constraint(rd, *(reg_hasco + rs2), *(reg_vaddr + rs2), 0,
              *(reg_colos + rs2) -
                (*(reg_los + rs2) - *(reg_los + rs1) % *(reg_los + rs2)),
              *(reg_coups + rs2) -
                (*(reg_ups + rs2) - *(reg_ups + rs1) % *(reg_ups + rs2)));
        } else
          // rd has no constraint if both rs1 and rs2 have no constraints
          set_constraint(rd, 0, 0, 0, 0, 0);
      }
    } else
      throw_exception(EXCEPTION_DIVISIONBYZERO, 0);
  }
}

void do_sltu() {
  // set on less than unsigned

  if (rd != REG_ZR) {
    // semantics of sltu
    if (*(registers + rs1) < *(registers + rs2))
      *(registers + rd) = 1;
    else
      *(registers + rd) = 0;
  }

  pc = pc + INSTRUCTIONSIZE;

  ic_sltu = ic_sltu + 1;
}

void constrain_sltu() {
  // interval semantics of sltu
  if (rd != REG_ZR) {
    if (*(reg_hasco + rs1)) {
      if (*(reg_vaddr + rs1) == 0) {
        // constrained memory at vaddr 0 means that there is more than
        // one constrained memory location in the sltu operand
        printf3((uint32_t*) "%s: %d constrained memory locations in left sltu operand at %x", selfie_name, (uint32_t*) *(reg_hasco + rs1), (uint32_t*) pc);
        print_code_line_number_for_instruction(pc - entry_point);
        println();

        exit(EXITCODE_SYMBOLICEXECUTIONERROR);
      }
    }

    if (*(reg_hasco + rs2)) {
      if (*(reg_vaddr + rs2) == 0) {
        // constrained memory at vaddr 0 means that there is more than
        // one constrained memory location in the sltu operand
        printf3((uint32_t*) "%s: %d constrained memory locations in right sltu operand at %x", selfie_name, (uint32_t*) *(reg_hasco + rs2), (uint32_t*) pc);
        print_code_line_number_for_instruction(pc - entry_point);
        println();

        exit(EXITCODE_SYMBOLICEXECUTIONERROR);
      }
    }

    // take local copy of mrcc to make sure that alias check considers old mrcc
    if (*(reg_typ + rs1))
      if (*(reg_typ + rs2))
        create_constraints(*(registers + rs1), *(registers + rs1), *(registers + rs2), *(registers + rs2), mrcc, 0);
      else
        create_constraints(*(registers + rs1), *(registers + rs1), *(reg_los + rs2), *(reg_ups + rs2), mrcc, 0);
    else if (*(reg_typ + rs2))
      create_constraints(*(reg_los + rs1), *(reg_ups + rs1), *(registers + rs2), *(registers + rs2), mrcc, 0);
    else
      create_constraints(*(reg_los + rs1), *(reg_ups + rs1), *(reg_los + rs2), *(reg_ups + rs2), mrcc, 0);
  }

  pc = pc + INSTRUCTIONSIZE;

  ic_sltu = ic_sltu + 1;
}

void backtrack_sltu() {
  uint32_t vaddr;

  if (debug_symbolic) {
    printf1((uint32_t*) "%s: backtracking sltu ", selfie_name);
    print_symbolic_memory(tc);
  }

  vaddr = *(vaddrs + tc);

  if (vaddr < NUMBEROFREGISTERS) {
    if (vaddr > 0) {
      // the register is identified by vaddr
      *(registers + vaddr) = *(values + tc);

      *(reg_typ + vaddr) = *(types + tc);

      *(reg_los + vaddr) = *(los + tc);
      *(reg_ups + vaddr) = *(ups + tc);

      set_constraint(vaddr, 0, 0, 0, 0, 0);

      // restoring mrcc
      mrcc = *(tcs + tc);

      if (vaddr != REG_FP)
        if (vaddr != REG_SP) {
          // stop backtracking and try next case
          pc = pc + INSTRUCTIONSIZE;

          ic_sltu = ic_sltu + 1;
        }
    }
  } else
    store_virtual_memory(pt, vaddr, *(tcs + tc));

  efree();
}

void print_lw() {
  print_code_context_for_instruction(pc);
  printf3((uint32_t*) "lw %s,%d(%s)", get_register_name(rd), (uint32_t*) imm, get_register_name(rs1));
}

void print_lw_before() {
  uint32_t vaddr;

  vaddr = *(registers + rs1) + imm;

  print((uint32_t*) ": ");
  print_register_hexadecimal(rs1);

  if (is_valid_virtual_address(vaddr))
    if (is_virtual_address_mapped(pt, vaddr)) {
      if (is_system_register(rd))
        printf2((uint32_t*) ",mem[%x]=%x |- ", (uint32_t*) vaddr, (uint32_t*) load_virtual_memory(pt, vaddr));
      else
        printf2((uint32_t*) ",mem[%x]=%d |- ", (uint32_t*) vaddr, (uint32_t*) load_virtual_memory(pt, vaddr));
      print_register_value(rd);

      return;
    }

  print((uint32_t*) " |-");
}

void print_lw_after(uint32_t vaddr) {
  if (is_valid_virtual_address(vaddr))
    if (is_virtual_address_mapped(pt, vaddr)) {
      print((uint32_t*) " -> ");
      print_register_value(rd);
      printf1((uint32_t*) "=mem[%x]", (uint32_t*) vaddr);
    }
}

void record_lw() {
  uint32_t vaddr;

  vaddr = *(registers + rs1) + imm;

  if (is_valid_virtual_address(vaddr))
    if (is_virtual_address_mapped(pt, vaddr))
      record_state(*(registers + rd));
}

uint32_t do_lw() {
  uint32_t vaddr;
  uint32_t a;

  // load word

  vaddr = *(registers + rs1) + imm;

  if (is_valid_virtual_address(vaddr)) {
    if (is_virtual_address_mapped(pt, vaddr)) {
      if (rd != REG_ZR)
        // semantics of lw
        *(registers + rd) = load_virtual_memory(pt, vaddr);

      // keep track of instruction address for profiling loads
      a = (pc - entry_point) / INSTRUCTIONSIZE;

      pc = pc + INSTRUCTIONSIZE;

      // keep track of number of loads in total
      ic_lw = ic_lw + 1;

      // and individually
      *(loads_per_instruction + a) = *(loads_per_instruction + a) + 1;
    } else
      throw_exception(EXCEPTION_PAGEFAULT, get_page_of_virtual_address(vaddr));
  } else
    throw_exception(EXCEPTION_INVALIDADDRESS, vaddr);

  return vaddr;
}

uint32_t constrain_lw() {
  uint32_t vaddr;
  uint32_t mrvc;
  uint32_t a;

  // load word

  vaddr = *(registers + rs1) + imm;

  if (is_safe_address(vaddr, rs1)) {
    if (is_virtual_address_mapped(pt, vaddr)) {
      if (rd != REG_ZR) {
        mrvc = load_symbolic_memory(pt, vaddr);

        // interval semantics of lw
        *(registers + rd) = *(values + mrvc);

        *(reg_typ + rd) = *(types + mrvc);

        *(reg_los + rd) = *(los + mrvc);
        *(reg_ups + rd) = *(ups + mrvc);

        // assert: vaddr == *(vaddrs + mrvc)

        if (is_symbolic_value(*(reg_typ + rd), *(reg_los + rd), *(reg_ups + rd)))
          // vaddr is constrained by rd if value interval is not singleton
          set_constraint(rd, 1, vaddr, 0, 0, 0);
        else
          set_constraint(rd, 0, 0, 0, 0, 0);
      }

      // keep track of instruction address for profiling loads
      a = (pc - entry_point) / INSTRUCTIONSIZE;

      pc = pc + INSTRUCTIONSIZE;

      // keep track of number of loads in total
      ic_lw = ic_lw + 1;

      // and individually
      *(loads_per_instruction + a) = *(loads_per_instruction + a) + 1;
    } else
      throw_exception(EXCEPTION_PAGEFAULT, get_page_of_virtual_address(vaddr));
  } else
    throw_exception(EXCEPTION_INVALIDADDRESS, vaddr);

  return vaddr;
}

void print_sw() {
  print_code_context_for_instruction(pc);
  printf3((uint32_t*) "sw %s,%d(%s)", get_register_name(rs2), (uint32_t*) imm, get_register_name(rs1));
}

void print_sw_before() {
  uint32_t vaddr;

  vaddr = *(registers + rs1) + imm;

  print((uint32_t*) ": ");
  print_register_hexadecimal(rs1);

  if (is_valid_virtual_address(vaddr))
    if (is_virtual_address_mapped(pt, vaddr)) {
      print((uint32_t*) ",");
      print_register_value(rs2);
      if (is_system_register(rd))
        printf2((uint32_t*) " |- mem[%x]=%x", (uint32_t*) vaddr, (uint32_t*) load_virtual_memory(pt, vaddr));
      else
        printf2((uint32_t*) " |- mem[%x]=%d", (uint32_t*) vaddr, (uint32_t*) load_virtual_memory(pt, vaddr));

      return;
    }

  print((uint32_t*) " |-");
}

void print_sw_after(uint32_t vaddr) {
  if (is_valid_virtual_address(vaddr))
    if (is_virtual_address_mapped(pt, vaddr)) {
      printf1((uint32_t*) " -> mem[%x]=", (uint32_t*) vaddr);
      print_register_value(rs2);
    }
}

void record_sw() {
  uint32_t vaddr;

  vaddr = *(registers + rs1) + imm;

  if (is_valid_virtual_address(vaddr))
    if (is_virtual_address_mapped(pt, vaddr))
      record_state(load_virtual_memory(pt, vaddr));
}

uint32_t do_sw() {
  uint32_t vaddr;
  uint32_t a;

  // store word

  vaddr = *(registers + rs1) + imm;

  if (is_valid_virtual_address(vaddr)) {
    if (is_virtual_address_mapped(pt, vaddr)) {
      // semantics of sw
      store_virtual_memory(pt, vaddr, *(registers + rs2));

      // keep track of instruction address for profiling stores
      a = (pc - entry_point) / INSTRUCTIONSIZE;

      pc = pc + INSTRUCTIONSIZE;

      // keep track of number of stores in total
      ic_sw = ic_sw + 1;

      // and individually
      *(stores_per_instruction + a) = *(stores_per_instruction + a) + 1;
    } else
      throw_exception(EXCEPTION_PAGEFAULT, get_page_of_virtual_address(vaddr));
  } else
    throw_exception(EXCEPTION_INVALIDADDRESS, vaddr);

  return vaddr;
}

uint32_t constrain_sw() {
  uint32_t vaddr;
  uint32_t a;

  // store word

  vaddr = *(registers + rs1) + imm;

  if (is_safe_address(vaddr, rs1)) {
    if (is_virtual_address_mapped(pt, vaddr)) {
      // interval semantics of sw
      if (*(reg_hasco + rs2)) {
        if (*(reg_vaddr + rs2) == 0) {
          // constrained memory at vaddr 0 means that there is more than
          // one constrained memory location in the sw operand
          printf3((uint32_t*) "%s: %d constrained memory locations in sw operand at %x", selfie_name, (uint32_t*) *(reg_hasco + rs2), (uint32_t*) pc);
          print_code_line_number_for_instruction(pc - entry_point);
          println();

          //exit(EXITCODE_SYMBOLICEXECUTIONERROR);
        }
      }

      store_symbolic_memory(pt, vaddr, *(registers + rs2), *(reg_typ + rs2), *(reg_los + rs2), *(reg_ups + rs2), mrcc);

      // keep track of instruction address for profiling stores
      a = (pc - entry_point) / INSTRUCTIONSIZE;

      pc = pc + INSTRUCTIONSIZE;

      // keep track of number of stores in total
      ic_sw = ic_sw + 1;

      // and individually
      *(stores_per_instruction + a) = *(stores_per_instruction + a) + 1;
    } else
      throw_exception(EXCEPTION_PAGEFAULT, get_page_of_virtual_address(vaddr));
  } else
    throw_exception(EXCEPTION_INVALIDADDRESS, vaddr);

  return vaddr;
}

void backtrack_sw() {
  if (debug_symbolic) {
    printf1((uint32_t*) "%s: backtracking sw ", selfie_name);
    print_symbolic_memory(tc);
  }

  store_virtual_memory(pt, *(vaddrs + tc), *(tcs + tc));

  efree();
}

void undo_sw() {
  uint32_t vaddr;

  vaddr = *(registers + rs1) + imm;

  store_virtual_memory(pt, vaddr, *(values + (tc % MAX_REPLAY_LENGTH)));
}

void print_beq() {
  print_code_context_for_instruction(pc);
  printf4((uint32_t*) "beq %s,%s,%d[%x]", get_register_name(rs1), get_register_name(rs2), (uint32_t*) signed_division(imm, INSTRUCTIONSIZE), (uint32_t*) (pc + imm));
}

void print_beq_before() {
  print((uint32_t*) ": ");
  print_register_value(rs1);
  print((uint32_t*) ",");
  print_register_value(rs2);
  printf1((uint32_t*) " |- $pc=%x", (uint32_t*) pc);
}

void print_beq_after() {
  printf1((uint32_t*) " -> $pc=%x", (uint32_t*) pc);
}

void record_beq() {
  record_state(0);
}

void do_beq() {
  // branch on equal

  // semantics of beq
  if (*(registers + rs1) == *(registers + rs2))
    pc = pc + imm;
  else
    pc = pc + INSTRUCTIONSIZE;

  ic_beq = ic_beq + 1;
}

void print_jal() {
  print_code_context_for_instruction(pc);
  printf3((uint32_t*) "jal %s,%d[%x]", get_register_name(rd), (uint32_t*) signed_division(imm, INSTRUCTIONSIZE), (uint32_t*) (pc + imm));
}

void print_jal_before() {
  print((uint32_t*) ": |- ");
  if (rd != REG_ZR) {
    print_register_hexadecimal(rd);
    print((uint32_t*) ",");
  }
  printf1((uint32_t*) "$pc=%x", (uint32_t*) pc);
}

void print_jal_jalr_after() {
  print_beq_after();
  if (rd != REG_ZR) {
    print((uint32_t*) ",");
    print_register_hexadecimal(rd);
  }
}

void do_jal() {
  uint32_t a;

  // jump and link

  if (rd != REG_ZR) {
    // first link
    *(registers + rd) = pc + INSTRUCTIONSIZE;

    // then jump for procedure calls
    pc = pc + imm;

    // prologue address for profiling procedure calls
    a = (pc - entry_point) / INSTRUCTIONSIZE;

    // keep track of number of procedure calls in total
    calls = calls + 1;

    // and individually
    *(calls_per_procedure + a) = *(calls_per_procedure + a) + 1;
  } else if (signed_less_than(imm, 0)) {
    // jump backwards to check for another loop iteration
    pc = pc + imm;

    // first loop instruction address for profiling loop iterations
    a = (pc - entry_point) / INSTRUCTIONSIZE;

    // keep track of number of loop iterations in total
    iterations = iterations + 1;

    // and individually
    *(iterations_per_loop + a) = *(iterations_per_loop + a) + 1;
  } else
    // just jump forward
    pc = pc + imm;

  ic_jal = ic_jal + 1;
}

void constrain_jal_jalr() {
  if (rd != REG_ZR) {
    *(reg_los + rd) = *(registers + rd);
    *(reg_ups + rd) = *(registers + rd);
  }
}

void print_jalr() {
  print_code_context_for_instruction(pc);
  printf3((uint32_t*) "jalr %s,%d(%s)", get_register_name(rd), (uint32_t*) signed_division(imm, INSTRUCTIONSIZE), get_register_name(rs1));
}

void print_jalr_before() {
  print((uint32_t*) ": ");
  print_register_hexadecimal(rs1);
  print((uint32_t*) " |- ");
  if (rd != REG_ZR) {
    print_register_hexadecimal(rd);
    print((uint32_t*) ",");
  }
  printf1((uint32_t*) "$pc=%x", (uint32_t*) pc);
}

void do_jalr() {
  uint32_t next_pc;

  // jump and link register

  if (rd == REG_ZR)
    // fast path: just return by jumping rs1-relative with LSB reset
    pc = left_shift(right_shift(*(registers + rs1) + imm, 1), 1);
  else {
    // slow path: first prepare jump, then link, just in case rd == rs1

    // prepare jump with LSB reset
    next_pc = left_shift(right_shift(*(registers + rs1) + imm, 1), 1);

    // link to next instruction
    *(registers + rd) = pc + INSTRUCTIONSIZE;

    // jump
    pc = next_pc;
  }

  ic_jalr = ic_jalr + 1;
}

void print_ecall() {
  print_code_context_for_instruction(pc);
  print((uint32_t*) "ecall");
}

void record_ecall() {
  // TODO: record all side effects
  record_state(*(registers + REG_A0));
}

void do_ecall() {
  ic_ecall = ic_ecall + 1;

  if (redo) {
    // TODO: redo all side effects
    *(registers + REG_A0) = *(values + (tc % MAX_REPLAY_LENGTH));

    pc = pc + INSTRUCTIONSIZE;
  } else if (*(registers + REG_A7) == SYSCALL_SWITCH)
    if (record) {
      printf1((uint32_t*) "%s: context switching during recording is unsupported\n", selfie_name);

      exit(EXITCODE_BADARGUMENTS);
    } else if (symbolic) {
      printf1((uint32_t*) "%s: context switching during symbolic execution is unsupported\n", selfie_name);

      exit(EXITCODE_BADARGUMENTS);
    } else {
      pc = pc + INSTRUCTIONSIZE;

      implement_switch();
    }
  else
    // all system calls other than switch are handled by exception
    throw_exception(EXCEPTION_SYSCALL, 0);
}

void undo_ecall() {
  uint32_t a0;

  a0 = *(registers + REG_A0);

  // TODO: undo all side effects
  *(registers + REG_A0) = *(values + (tc % MAX_REPLAY_LENGTH));

  // save register a0 for redoing system call
  *(values + (tc % MAX_REPLAY_LENGTH)) = a0;
}

void backtrack_ecall() {
  if (debug_symbolic) {
    printf1((uint32_t*) "%s: backtracking ecall ", selfie_name);
    print_symbolic_memory(tc);
  }

  if (*(vaddrs + tc) == 0) {
    // backtracking malloc
    if (get_program_break(current_context) == *(los + tc) + *(ups + tc))
      set_program_break(current_context, *(los + tc));
    else {
      printf1((uint32_t*) "%s: malloc backtracking error at ", selfie_name);
      print_symbolic_memory(tc);
      printf4((uint32_t*) " with current program break %x unequal %x which is previous program break %x plus size %d\n",
        (uint32_t*) get_program_break(current_context),
        (uint32_t*) (*(los + tc) + *(ups + tc)),
        (uint32_t*) *(los + tc),
        (uint32_t*) *(ups + tc));

      exit(EXITCODE_SYMBOLICEXECUTIONERROR);
    }
  } else {
    // backtracking read
    rc = rc + 1;

    // record value, lower and upper bound
    *(read_values + rc) = *(values + tc);

    *(read_los + rc) = *(los + tc);
    *(read_ups + rc) = *(ups + tc);

    store_virtual_memory(pt, *(vaddrs + tc), *(tcs + tc));
  }

  efree();
}

void print_data_line_number() {
  if (data_line_number != (uint32_t*) 0)
    printf1((uint32_t*) "(~%d)", (uint32_t*) *(data_line_number + (pc - code_length) / REGISTERSIZE));
}

void print_data_context(uint32_t data) {
  printf1((uint32_t*) "%x", (uint32_t*) pc);

  if (disassemble_verbose) {
    print_data_line_number();
    print((uint32_t*) ": ");
    print_hexadecimal(data, SIZEOFUINT32 * 2);
    print((uint32_t*) " ");
  } else
    print((uint32_t*) ": ");
}

void print_data(uint32_t data) {
  print_data_context(data);
  printf1((uint32_t*) ".word %x", (uint32_t*) data);
}

// -----------------------------------------------------------------
// -------------------------- REPLAY ENGINE ------------------------
// -----------------------------------------------------------------

void record_state(uint32_t value) {
  *(pcs + (tc % MAX_REPLAY_LENGTH))    = pc;
  *(values + (tc % MAX_REPLAY_LENGTH)) = value;

  tc = tc + 1;
}

void replay_trace() {
  uint32_t trace_length;
  uint32_t tl;

  if (tc < MAX_REPLAY_LENGTH)
    trace_length = tc;
  else
    trace_length = MAX_REPLAY_LENGTH;

  record = 0;
  undo   = 1;

  tl = trace_length;

  // undo trace_length number of instructions
  while (tl > 0) {
    tc = tc - 1;

    pc = *(pcs + (tc % MAX_REPLAY_LENGTH));

    fetch();
    decode_execute();

    tl = tl - 1;
  }

  undo = 0;
  redo = 1;

  disassemble = 1;

  tl = trace_length;

  // redo trace_length number of instructions
  while (tl > 0) {
    // assert: pc == *(pcs + (tc % MAX_REPLAY_LENGTH))

    fetch();
    decode_execute();

    tc = tc + 1;
    tl = tl - 1;
  }

  disassemble = 0;

  redo   = 0;
  record = 1;
}

// -----------------------------------------------------------------
// ------------------- SYMBOLIC EXECUTION ENGINE -------------------
// -----------------------------------------------------------------

void print_symbolic_memory(uint32_t svc) {
  printf3((uint32_t*) "@%d{@%d@%x", (uint32_t*) svc, (uint32_t*) *(tcs + svc), (uint32_t*) *(pcs + svc));
  if (*(pcs + svc) >= entry_point)
    print_code_line_number_for_instruction(*(pcs + svc) - entry_point);
  if (*(vaddrs + svc) == 0) {
    printf3((uint32_t*) ";%x=%x=malloc(%d)}\n", (uint32_t*) *(values + svc), (uint32_t*) *(los + svc), (uint32_t*) *(ups + svc));
    return;
  } else if (*(vaddrs + svc) < NUMBEROFREGISTERS)
    printf2((uint32_t*) ";%s=%d", get_register_name(*(vaddrs + svc)), (uint32_t*) *(values + svc));
  else
    printf2((uint32_t*) ";%x=%d", (uint32_t*) *(vaddrs + svc), (uint32_t*) *(values + svc));
  if (*(types + svc))
    if (*(los + svc) == *(ups + svc))
      printf1((uint32_t*) "(%d)}\n", (uint32_t*) *(los + svc));
    else
      printf2((uint32_t*) "(%d,%d)}\n", (uint32_t*) *(los + svc), (uint32_t*) *(ups + svc));
  else if (*(los + svc) == *(ups + svc))
    printf1((uint32_t*) "[%d]}\n", (uint32_t*) *(los + svc));
  else
    printf2((uint32_t*) "[%d,%d]}\n", (uint32_t*) *(los + svc), (uint32_t*) *(ups + svc));
}

uint32_t cardinality(uint32_t lo, uint32_t up) {
  // there are 2^32 values if the result is 0
  return up - lo + 1;
}

uint32_t combined_cardinality(uint32_t lo1, uint32_t up1, uint32_t lo2, uint32_t up2) {
  uint32_t c1;
  uint32_t c2;

  c1 = cardinality(lo1, up1);
  c2 = cardinality(lo2, up2);

  if (c1 + c2 <= c1)
    // there are at least 2^32 values
    return 0;
  else if (c1 + c2 <= c2)
    // there are at least 2^32 values
    return 0;
  else
    return c1 + c2;
}

uint32_t is_symbolic_value(uint32_t type, uint32_t lo, uint32_t up) {
  if (type)
    // memory range
    return 0;
  else if (lo == up)
    // singleton interval
    return 0;
  else
    // non-singleton interval
    return 1;
}

uint32_t is_safe_address(uint32_t vaddr, uint32_t reg) {
  if (*(reg_typ + reg)) {
    if (vaddr < *(reg_los + reg))
      // memory access below start address of mallocated block
      return 0;
    else if (vaddr - *(reg_los + reg) >= *(reg_ups + reg))
      // memory access above end address of mallocated block
      return 0;
    else
      return 1;
  } else if (*(reg_los + reg) == *(reg_ups + reg))
    return 1;
  else {
    printf2((uint32_t*) "%s: detected unsupported symbolic access of memory interval at %x", selfie_name, (uint32_t*) pc);
    print_code_line_number_for_instruction(pc - entry_point);
    println();

    exit(EXITCODE_SYMBOLICEXECUTIONERROR);
  }
}

uint32_t load_symbolic_memory(uint32_t* pt, uint32_t vaddr) {
  uint32_t mrvc;

  // assert: vaddr is valid and mapped
  mrvc = load_virtual_memory(pt, vaddr);

  if (mrvc <= tc)
    return mrvc;
  else {
    printf4((uint32_t*) "%s: detected most recent value counter %d at vaddr %x greater than current trace counter %d\n", selfie_name, (uint32_t*) mrvc, (uint32_t*) vaddr, (uint32_t*) tc);

    exit(EXITCODE_SYMBOLICEXECUTIONERROR);
  }
}

uint32_t is_trace_space_available() {
  return tc + 1 < MAX_TRACE_LENGTH;
}

void ealloc() {
  tc = tc + 1;
}

void efree() {
  // assert: tc > 0
  tc = tc - 1;
}

void store_symbolic_memory(uint32_t* pt, uint32_t vaddr, uint32_t value, uint32_t type, uint32_t lo, uint32_t up, uint32_t trb) {
  uint32_t mrvc;

  if (vaddr == 0)
    // tracking program break and size for malloc
    mrvc = 0;
  else if (vaddr < NUMBEROFREGISTERS)
    // tracking a register value for sltu
    mrvc = mrcc;
  else {
    // assert: vaddr is valid and mapped
    mrvc = load_symbolic_memory(pt, vaddr);

    if (value == *(values + mrvc))
      if (type == *(types + mrvc))
        if (lo == *(los + mrvc))
          if (up == *(ups + mrvc))
            // prevent tracking identical updates
            return;
  }

  if (trb < mrvc) {
    // current value at vaddr does not need to be tracked,
    // just overwrite it in the trace
    *(values + mrvc) = value;

    *(types + mrvc) = type;

    *(los + mrvc) = lo;
    *(ups + mrvc) = up;

    // assert: vaddr == *(vaddrs + mrvc)

    if (debug_symbolic) {
      printf1((uint32_t*) "%s: overwriting ", selfie_name);
      print_symbolic_memory(mrvc);
    }
  } else if (is_trace_space_available()) {
    // current value at vaddr is from before most recent branch,
    // track that value by creating a new trace event
    ealloc();

    *(pcs + tc) = pc;
    *(tcs + tc) = mrvc;

    *(values + tc) = value;

    *(types + tc) = type;

    *(los + tc) = lo;
    *(ups + tc) = up;

    *(vaddrs + tc) = vaddr;

    if (vaddr < NUMBEROFREGISTERS) {
      if (vaddr > 0)
        // register tracking marks most recent constraint
        mrcc = tc;
    } else
      // assert: vaddr is valid and mapped
      store_virtual_memory(pt, vaddr, tc);

    if (debug_symbolic) {
      printf1((uint32_t*) "%s: storing ", selfie_name);
      print_symbolic_memory(tc);
    }
  } else
    throw_exception(EXCEPTION_MAXTRACE, 0);
}

void store_constrained_memory(uint32_t vaddr, uint32_t lo, uint32_t up, uint32_t trb) {
  uint32_t mrvc;

  if (vaddr >= get_program_break(current_context))
    if (vaddr < *(registers + REG_SP))
      // do not constrain free memory
      return;

  mrvc = load_virtual_memory(pt, vaddr);

  if (mrvc < trb) {
    // we do not support potentially aliased constrained memory
    printf1((uint32_t*) "%s: detected potentially aliased constrained memory\n", selfie_name);

    exit(EXITCODE_SYMBOLICEXECUTIONERROR);
  }

  // always track constrained memory by using tc as most recent branch
  store_symbolic_memory(pt, vaddr, lo, 0, lo, up, tc);
}

void store_register_memory(uint32_t reg, uint32_t value) {
  // always track register memory by using tc as most recent branch
  store_symbolic_memory(pt, reg, value, 0, value, value, tc);
}

void constrain_memory(uint32_t reg, uint32_t lo, uint32_t up, uint32_t trb) {
  if (*(reg_hasco + reg)) {
    if (*(reg_hasmn + reg))
      store_constrained_memory(*(reg_vaddr + reg), *(reg_colos + reg) - lo, *(reg_coups + reg) - up, trb);
    else
      store_constrained_memory(*(reg_vaddr + reg), lo - *(reg_colos + reg), up - *(reg_coups + reg), trb);
  }
}

void set_constraint(uint32_t reg, uint32_t hasco, uint32_t vaddr, uint32_t hasmn, uint32_t colos, uint32_t coups) {
  *(reg_hasco + reg) = hasco;
  *(reg_vaddr + reg) = vaddr;
  *(reg_hasmn + reg) = hasmn;
  *(reg_colos + reg) = colos;
  *(reg_coups + reg) = coups;
}

void take_branch(uint32_t b, uint32_t how_many_more) {
  if (how_many_more > 0) {
    // record that we need to set rd to true
    store_register_memory(rd, b);

    // record frame and stack pointer
    store_register_memory(REG_FP, *(registers + REG_FP));
    store_register_memory(REG_SP, *(registers + REG_SP));
  } else {
    *(registers + rd) = b;

    *(reg_typ + rd) = 0;

    *(reg_los + rd) = b;
    *(reg_ups + rd) = b;

    set_constraint(rd, 0, 0, 0, 0, 0);
  }
}

void create_constraints(uint32_t lo1, uint32_t up1, uint32_t lo2, uint32_t up2, uint32_t trb, uint32_t how_many_more) {
  if (lo1 <= up1) {
    // rs1 interval is not wrapped around
    if (lo2 <= up2) {
      // both rs1 and rs2 intervals are not wrapped around
      if (up1 < lo2) {
        // rs1 interval is strictly less than rs2 interval
        constrain_memory(rs1, lo1, up1, trb);
        constrain_memory(rs2, lo2, up2, trb);

        take_branch(1, how_many_more);
      } else if (up2 <= lo1) {
        // rs2 interval is less than or equal to rs1 interval
        constrain_memory(rs1, lo1, up1, trb);
        constrain_memory(rs2, lo2, up2, trb);

        take_branch(0, how_many_more);
      } else if (lo2 == up2) {
        // rs2 interval is a singleton

        // construct constraint for false case
        constrain_memory(rs1, lo2, up1, trb);
        constrain_memory(rs2, lo2, up2, trb);

        // record that we need to set rd to false
        store_register_memory(rd, 0);

        // record frame and stack pointer
        store_register_memory(REG_FP, *(registers + REG_FP));
        store_register_memory(REG_SP, *(registers + REG_SP));

        // construct constraint for true case
        constrain_memory(rs1, lo1, lo2 - 1, trb);
        constrain_memory(rs2, lo2, up2, trb);

        take_branch(1, how_many_more);
      } else if (lo1 == up1) {
        // rs1 interval is a singleton

        // construct constraint for false case
        constrain_memory(rs1, lo1, up1, trb);
        constrain_memory(rs2, lo2, lo1, trb);

        // record that we need to set rd to false
        store_register_memory(rd, 0);

        // record frame and stack pointer
        store_register_memory(REG_FP, *(registers + REG_FP));
        store_register_memory(REG_SP, *(registers + REG_SP));

        // construct constraint for true case
        constrain_memory(rs1, lo1, up1, trb);
        constrain_memory(rs2, lo1 + 1, up2, trb);

        take_branch(1, how_many_more);
      } else {
        // we cannot handle non-singleton interval intersections in comparison
        printf1((uint32_t*) "%s: detected non-singleton interval intersection\n", selfie_name);

        exit(EXITCODE_SYMBOLICEXECUTIONERROR);
      }
    } else {
      // rs1 interval is not wrapped around but rs2 is

      // unwrap rs2 interval and use higher portion first
      create_constraints(lo1, up1, lo2, UINT32_MAX, trb, 1);

      // then use lower portion of rs2 interval
      create_constraints(lo1, up1, 0, up2, trb, 0);
    }
  } else if (lo2 <= up2) {
    // rs2 interval is not wrapped around but rs1 is

    // unwrap rs1 interval and use higher portion first
    create_constraints(lo1, UINT32_MAX, lo2, up2, trb, 1);

    // then use lower portion of rs1 interval
    create_constraints(0, up1, lo2, up2, trb, 0);
  } else {
    // both rs1 and rs2 intervals are wrapped around

    // unwrap rs1 and rs2 intervals and use higher portions
    create_constraints(lo1, UINT32_MAX, lo2, UINT32_MAX, trb, 3);

    // use higher portion of rs1 interval and lower portion of rs2 interval
    create_constraints(lo1, UINT32_MAX, 0, up2, trb, 2);

    // use lower portions of rs1 and rs2 intervals
    create_constraints(0, up1, 0, up2, trb, 1);

    // use lower portion of rs1 interval and higher portion of rs2 interval
    create_constraints(0, up1, lo2, UINT32_MAX, trb, 0);
  }
}

uint32_t fuzz_lo(uint32_t value) {
  if (fuzz >= CPUBITWIDTH)
    return 0;
  else if (value > (two_to_the_power_of(fuzz) - 1) / 2)
    return value - (two_to_the_power_of(fuzz) - 1) / 2;
  else
    return 0;
}

uint32_t fuzz_up(uint32_t value) {
  if (fuzz >= CPUBITWIDTH)
    return UINT32_MAX;
  else if (UINT32_MAX - value < two_to_the_power_of(fuzz) / 2)
    return UINT32_MAX;
  else if (value > (two_to_the_power_of(fuzz) - 1) / 2)
    return value + two_to_the_power_of(fuzz) / 2;
  else
    return two_to_the_power_of(fuzz) - 1;
}

// -----------------------------------------------------------------
// -------------------------- INTERPRETER --------------------------
// -----------------------------------------------------------------

void print_register_hexadecimal(uint32_t reg) {
  printf2((uint32_t*) "%s=%x", get_register_name(reg), (uint32_t*) *(registers + reg));
}

void print_register_octal(uint32_t reg) {
  printf2((uint32_t*) "%s=%o", get_register_name(reg), (uint32_t*) *(registers + reg));
}

uint32_t is_system_register(uint32_t reg) {
  if (reg == REG_GP)
    return 1;
  else if (reg == REG_FP)
    return 1;
  else if (reg == REG_RA)
    return 1;
  else if (reg == REG_SP)
    return 1;
  else
    return 0;
}

void print_register_value(uint32_t reg) {
  if (is_system_register(reg))
    print_register_hexadecimal(reg);
  else
    printf3((uint32_t*) "%s=%d(%x)", get_register_name(reg), (uint32_t*) *(registers + reg), (uint32_t*) *(registers + reg));
}

void print_exception(uint32_t exception, uint32_t faulting_page) {
  print((uint32_t*) *(EXCEPTIONS + exception));

  if (exception == EXCEPTION_PAGEFAULT)
    printf1((uint32_t*) " at %p", (uint32_t*) faulting_page);
}

void throw_exception(uint32_t exception, uint32_t faulting_page) {
  if (get_exception(current_context) != EXCEPTION_NOEXCEPTION)
    if (get_exception(current_context) != exception) {
      printf2((uint32_t*) "%s: context %p throws ", selfie_name, current_context);
      print_exception(exception, faulting_page);
      print((uint32_t*) " exception in presence of ");
      print_exception(get_exception(current_context), get_faulting_page(current_context));
      print((uint32_t*) " exception\n");

      exit(EXITCODE_MULTIPLEEXCEPTIONERROR);
    }

  set_exception(current_context, exception);
  set_faulting_page(current_context, faulting_page);

  trap = 1;

  if (debug_exception) {
    printf2((uint32_t*) "%s: context %p throws ", selfie_name, current_context);
    print_exception(exception, faulting_page);
    print((uint32_t*) " exception\n");
  }
}

void fetch() {
  // assert: is_valid_virtual_address(pc) == 1
  // assert: is_virtual_address_mapped(pt, pc) == 1

  ir = load_virtual_memory(pt, pc);
}

void decode_execute() {
  opcode = get_opcode(ir);

  if (opcode == OP_IMM) {
    decode_i_format();

    if (funct3 == F3_ADDI) {
      if (debug) {
        if (record) {
          record_lui_addi_add_sub_mul_sltu_jal_jalr();
          do_addi();
        } else if (undo)
          undo_lui_addi_add_sub_mul_divu_remu_sltu_lw_jal_jalr();
        else if (disassemble) {
          print_addi();
          if (execute) {
            print_addi_before();
            do_addi();
            print_addi_add_sub_mul_divu_remu_sltu_after();
          }
          println();
        } else if (symbolic) {
          do_addi();
          constrain_addi();
        }
      } else
        do_addi();

      return;
    }
  } else if (opcode == OP_LW) {
    decode_i_format();

    if (funct3 == F3_LW) {
      if (debug) {
        if (record) {
          record_lw();
          do_lw();
        } else if (undo)
          undo_lui_addi_add_sub_mul_divu_remu_sltu_lw_jal_jalr();
        else if (disassemble) {
          print_lw();
          if (execute) {
            print_lw_before();
            print_lw_after(do_lw());
          }
          println();
        } else if (symbolic)
          constrain_lw();
      } else
        do_lw();

      return;
    }
  } else if (opcode == OP_SW) {
    decode_s_format();

    if (funct3 == F3_SW) {
      if (debug) {
        if (record) {
          record_sw();
          do_sw();
        } else if (undo)
          undo_sw();
        else if (disassemble) {
          print_sw();
          if (execute) {
            print_sw_before();
            print_sw_after(do_sw());
          }
          println();
        } else if (symbolic)
          constrain_sw();
        else if (backtrack)
          backtrack_sw();
      } else
        do_sw();

      return;
    }
  } else if (opcode == OP_OP) { // coucreate_symbol_table_entry be ADD, SUB, MUL, DIVU, REMU, SLTU
    decode_r_format();

    if (funct3 == F3_ADD) { // = F3_SUB = F3_MUL
      if (funct7 == F7_ADD) {
        if (debug) {
          if (record) {
            record_lui_addi_add_sub_mul_sltu_jal_jalr();
            do_add();
          } else if (disassemble) {
            print_add_sub_mul_divu_remu_sltu((uint32_t*) "add");
            if (execute) {
              print_add_sub_mul_divu_remu_sltu_before();
              do_add();
              print_addi_add_sub_mul_divu_remu_sltu_after();
            }
            println();
          } else if (symbolic) {
            do_add();
            constrain_add();
          }
        } else
          do_add();

        return;
      } else if (funct7 == F7_SUB) {
        if (debug) {
          if (record) {
            record_lui_addi_add_sub_mul_sltu_jal_jalr();
            do_sub();
          } else if (undo)
            undo_lui_addi_add_sub_mul_divu_remu_sltu_lw_jal_jalr();
          else if (disassemble) {
            print_add_sub_mul_divu_remu_sltu((uint32_t*) "sub");
            if (execute) {
              print_add_sub_mul_divu_remu_sltu_before();
              do_sub();
              print_addi_add_sub_mul_divu_remu_sltu_after();
            }
            println();
          } else if (symbolic) {
            do_sub();
            constrain_sub();
          }
        } else
          do_sub();

        return;
      } else if (funct7 == F7_MUL) {
        if (debug) {
          if (record) {
            record_lui_addi_add_sub_mul_sltu_jal_jalr();
            do_mul();
          } else if (undo)
            undo_lui_addi_add_sub_mul_divu_remu_sltu_lw_jal_jalr();
          else if (disassemble) {
            print_add_sub_mul_divu_remu_sltu((uint32_t*) "mul");
            if (execute) {
              print_add_sub_mul_divu_remu_sltu_before();
              do_mul();
              print_addi_add_sub_mul_divu_remu_sltu_after();
            }
            println();
          } else if (symbolic) {
            do_mul();
            constrain_mul();
          }
        } else
          do_mul();

        return;
      }
    } else if (funct3 == F3_DIVU) {
      if (funct7 == F7_DIVU) {
        if (debug) {
          if (record) {
            record_divu_remu();
            do_divu();
          } else if (undo)
            undo_lui_addi_add_sub_mul_divu_remu_sltu_lw_jal_jalr();
          else if (disassemble) {
            print_add_sub_mul_divu_remu_sltu((uint32_t*) "divu");
            if (execute) {
              print_add_sub_mul_divu_remu_sltu_before();
              do_divu();
              print_addi_add_sub_mul_divu_remu_sltu_after();
            }
            println();
          } else if (symbolic) {
            do_divu();
            constrain_divu();
          }
        } else
          do_divu();

        return;
      }
    } else if (funct3 == F3_REMU) {
      if (funct7 == F7_REMU) {
        if (debug) {
          if (record) {
            record_divu_remu();
            do_remu();
          } else if (undo)
            undo_lui_addi_add_sub_mul_divu_remu_sltu_lw_jal_jalr();
          else if (disassemble) {
            print_add_sub_mul_divu_remu_sltu((uint32_t*) "remu");
            if (execute) {
              print_add_sub_mul_divu_remu_sltu_before();
              do_remu();
              print_addi_add_sub_mul_divu_remu_sltu_after();
            }
            println();
          } else if (symbolic) {
            do_remu();
            constrain_remu();
          }
        } else
          do_remu();

        return;
      }
    } else if (funct3 == F3_SLTU) {
      if (funct7 == F7_SLTU) {
        if (debug) {
          if (record) {
            record_lui_addi_add_sub_mul_sltu_jal_jalr();
            do_sltu();
          } else if (undo)
            undo_lui_addi_add_sub_mul_divu_remu_sltu_lw_jal_jalr();
          else if (disassemble) {
            print_add_sub_mul_divu_remu_sltu((uint32_t*) "sltu");
            if (execute) {
              print_add_sub_mul_divu_remu_sltu_before();
              do_sltu();
              print_addi_add_sub_mul_divu_remu_sltu_after();
            }
            println();
          } else if (symbolic)
            constrain_sltu();
          else if (backtrack)
            backtrack_sltu();
        } else
          do_sltu();

        return;
      }
    }
  } else if (opcode == OP_BRANCH) {
    decode_b_format();

    if (funct3 == F3_BEQ) {
      if (debug) {
        if (record) {
          record_beq();
          do_beq();
        } if (disassemble) {
          print_beq();
          if (execute) {
            print_beq_before();
            do_beq();
            print_beq_after();
          }
          println();
        } else if (symbolic)
          do_beq();
      } else
        do_beq();

      return;
    }
  } else if (opcode == OP_JAL) {
    decode_j_format();

    if (debug) {
      if (record) {
        record_lui_addi_add_sub_mul_sltu_jal_jalr();
        do_jal();
      } else if (undo)
        undo_lui_addi_add_sub_mul_divu_remu_sltu_lw_jal_jalr();
      else if (disassemble) {
        print_jal();
        if (execute) {
          print_jal_before();
          do_jal();
          print_jal_jalr_after();
        }
        println();
      } else if (symbolic) {
        do_jal();
        constrain_jal_jalr();
      }
    } else
      do_jal();

    return;
  } else if (opcode == OP_JALR) {
    decode_i_format();

    if (funct3 == F3_JALR) {
      if (debug) {
        if (record) {
          record_lui_addi_add_sub_mul_sltu_jal_jalr();
          do_jalr();
        } else if (undo)
          undo_lui_addi_add_sub_mul_divu_remu_sltu_lw_jal_jalr();
        else if (disassemble) {
          print_jalr();
          if (execute) {
            print_jalr_before();
            do_jalr();
            print_jal_jalr_after();
          }
          println();
        } else if (symbolic) {
          do_jalr();
          constrain_jal_jalr();
        }
      } else
        do_jalr();

      return;
    }
  } else if (opcode == OP_LUI) {
    decode_u_format();

    if (debug) {
      if (record) {
        record_lui_addi_add_sub_mul_sltu_jal_jalr();
        do_lui();
      } else if (undo)
        undo_lui_addi_add_sub_mul_divu_remu_sltu_lw_jal_jalr();
      else if (disassemble) {
        print_lui();
        if (execute) {
          print_lui_before();
          do_lui();
          print_lui_after();
        }
        println();
      } else if (symbolic) {
        do_lui();
        constrain_lui();
      }
    } else
      do_lui();

    return;
  } else if (opcode == OP_SYSTEM) {
    decode_i_format();

    if (funct3 == F3_ECALL) {
      if (debug) {
        if (record) {
          record_ecall();
          do_ecall();
        } else if (undo)
          undo_ecall();
        else if (disassemble) {
          print_ecall();
          if (execute)
            do_ecall();
          else
            println();
        } else if (symbolic)
          do_ecall();
        else if (backtrack)
          backtrack_ecall();
      } else
        do_ecall();

      return;
    }
  }

  if (execute)
    throw_exception(EXCEPTION_UNKNOWNINSTRUCTION, 0);
  else {
    //report the error on the console
    output_fd = 1;

    printf2((uint32_t*) "%s: unknown instruction with %x opcode detected\n", selfie_name, (uint32_t*) opcode);

    exit(EXITCODE_UNKNOWNINSTRUCTION);
  }
}

void interrupt() {
  if (timer != TIMEROFF) {
    timer = timer - 1;

    if (timer == 0) {
      if (get_exception(current_context) == EXCEPTION_NOEXCEPTION)
        // only throw exception if no other is pending
        // TODO: handle multiple pending exceptions
        throw_exception(EXCEPTION_TIMER, 0);
      else
        // trigger timer in the next interrupt cycle
        timer = 1;
    }
  }
}

uint32_t* run_until_exception() {
  trap = 0;

  while (trap == 0) {
    fetch();
    decode_execute();
    interrupt();
  }

  trap = 0;

  return current_context;
}

uint32_t instruction_with_max_counter(uint32_t* counters, uint32_t max) {
  uint32_t a;
  uint32_t n;
  uint32_t i;
  uint32_t c;

  a = -1;
  n = 0;
  i = 0;

  while (i < code_length / INSTRUCTIONSIZE) {
    c = *(counters + i);

    if (n < c) {
      if (c < max) {
        n = c;
        a = i;
      } else
        return i * INSTRUCTIONSIZE;
    }

    i = i + 1;
  }

  if (a != -1)
    return a * INSTRUCTIONSIZE;
  else
    return -1;
}

uint32_t print_per_instruction_counter(uint32_t total, uint32_t* counters, uint32_t max) {
  uint32_t a;
  uint32_t c;

  a = instruction_with_max_counter(counters, max);

  if (a != -1) {
    c = *(counters + a / INSTRUCTIONSIZE);

    // CAUTION: we reset counter to avoid reporting it again
    *(counters + a / INSTRUCTIONSIZE) = 0;

    printf3((uint32_t*) ",%d(%.2d%%)@%x", (uint32_t*) c, (uint32_t*) fixed_point_percentage(fixed_point_ratio(total, c, 4), 4), (uint32_t*) a);
    print_code_line_number_for_instruction(a);

    return c;
  } else {
    print((uint32_t*) ",0(0.00%)");

    return 0;
  }
}

void print_per_instruction_profile(uint32_t* message, uint32_t total, uint32_t* counters) {
  printf3((uint32_t*) "%s%s%d", selfie_name, message, (uint32_t*) total);
  print_per_instruction_counter(total, counters, print_per_instruction_counter(total, counters, print_per_instruction_counter(total, counters, UINT32_MAX)));
  println();
}

void print_profile() {
  printf4((uint32_t*)
    "%s: summary: %d executed instructions and %.2dMB(%.2d%%) mapped memory\n",
    selfie_name,
    (uint32_t*) get_total_number_of_instructions(),
    (uint32_t*) fixed_point_ratio(pused(), MEGABYTE, 2),
    (uint32_t*) fixed_point_percentage(fixed_point_ratio(page_frame_memory, pused(), 4), 4));

  if (get_total_number_of_instructions() > 0) {
    print_instruction_counters();

    if (code_line_number != (uint32_t*) 0)
      printf1((uint32_t*) "%s: profile: total,max(ratio%%)@addr(line#),2max,3max\n", selfie_name);
    else
      printf1((uint32_t*) "%s: profile: total,max(ratio%%)@addr,2max,3max\n", selfie_name);

    print_per_instruction_profile((uint32_t*) ": calls:   ", calls, calls_per_procedure);
    print_per_instruction_profile((uint32_t*) ": loops:   ", iterations, iterations_per_loop);
    print_per_instruction_profile((uint32_t*) ": loads:   ", ic_lw, loads_per_instruction);
    print_per_instruction_profile((uint32_t*) ": stores:  ", ic_sw, stores_per_instruction);
  }
}

void selfie_disassemble(uint32_t verbose) {
  uint32_t data;

  assembly_name = get_argument();

  if (code_length == 0) {
    printf2((uint32_t*) "%s: nothing to disassemble to output file %s\n", selfie_name, assembly_name);

    return;
  }

  // assert: assembly_name is mapped and not longer than MAX_FILENAME_LENGTH

  assembly_fd = open_write_only(assembly_name);

  if (signed_less_than(assembly_fd, 0)) {
    printf2((uint32_t*) "%s: could not create assembly output file %s\n", selfie_name, assembly_name);

    exit(EXITCODE_IOERROR);
  }

  output_name = assembly_name;
  output_fd   = assembly_fd;

  execute = 0;

  reset_library();
  reset_interpreter();

  debug               = 1;
  disassemble         = 1;
  disassemble_verbose = verbose;

  while (pc < code_length) {
    ir = load_instruction(pc);

    decode_execute();

    pc = pc + INSTRUCTIONSIZE;
  }

  while (pc < binary_length) {
    data = load_data(pc);

    print_data(data);
    println();

    pc = pc + REGISTERSIZE;
  }

  disassemble_verbose = 0;
  disassemble         = 0;
  debug               = 0;

  output_name = (uint32_t*) 0;
  output_fd   = 1;

  printf5((uint32_t*) "%s: %d characters of assembly with %d instructions and %d bytes of data written into %s\n", selfie_name,
    (uint32_t*) number_of_written_characters,
    (uint32_t*) (code_length / INSTRUCTIONSIZE),
    (uint32_t*) (binary_length - code_length),
    assembly_name);
}

// -----------------------------------------------------------------
// ---------------------------- CONTEXTS ---------------------------
// -----------------------------------------------------------------

uint32_t* allocate_context(uint32_t* parent, uint32_t* vctxt, uint32_t* in) {
  uint32_t* context;

  if (free_contexts == (uint32_t*) 0)
    context = smalloc(7 * SIZEOFUINT32STAR + 9 * SIZEOFUINT32);
  else {
    context = free_contexts;

    free_contexts = get_next_context(free_contexts);
  }

  set_next_context(context, in);
  set_prev_context(context, (uint32_t*) 0);

  if (in != (uint32_t*) 0)
    set_prev_context(in, context);

  set_pc(context, 0);

  // allocate zeroed memory for general purpose registers
  // TODO: reuse memory
  set_regs(context, zalloc(NUMBEROFREGISTERS * REGISTERSIZE));

  // allocate zeroed memory for page table
  // TODO: save and reuse memory for page table
  set_pt(context, zalloc(VIRTUALMEMORYSIZE / PAGESIZE * REGISTERSIZE));

  // determine range of recently mapped pages
  set_lo_page(context, 0);
  set_me_page(context, 0);
  set_hi_page(context, get_page_of_virtual_address(VIRTUALMEMORYSIZE - REGISTERSIZE));

  set_exception(context, EXCEPTION_NOEXCEPTION);
  set_faulting_page(context, 0);

  set_exit_code(context, EXITCODE_NOERROR);

  set_parent(context, parent);
  set_virtual_context(context, vctxt);

  set_name(context, (uint32_t*) 0);

  return context;
}

uint32_t* find_context(uint32_t* parent, uint32_t* vctxt, uint32_t* in) {
  uint32_t* context;

  context = in;

  while (context != (uint32_t*) 0) {
    if (get_parent(context) == parent)
      if (get_virtual_context(context) == vctxt)
        return context;

    context = get_next_context(context);
  }

  return (uint32_t*) 0;
}

void free_context(uint32_t* context) {
  set_next_context(context, free_contexts);

  free_contexts = context;
}

uint32_t* delete_context(uint32_t* context, uint32_t* from) {
  if (get_next_context(context) != (uint32_t*) 0)
    set_prev_context(get_next_context(context), get_prev_context(context));

  if (get_prev_context(context) != (uint32_t*) 0) {
    set_next_context(get_prev_context(context), get_next_context(context));
    set_prev_context(context, (uint32_t*) 0);
  } else
    from = get_next_context(context);

  free_context(context);

  return from;
}

// -----------------------------------------------------------------
// -------------------------- MICROKERNEL --------------------------
// -----------------------------------------------------------------

uint32_t* create_context(uint32_t* parent, uint32_t* vctxt) {
  // TODO: check if context already exists
  used_contexts = allocate_context(parent, vctxt, used_contexts);

  if (current_context == (uint32_t*) 0)
    current_context = used_contexts;

  if (debug_create)
    printf3((uint32_t*) "%s: parent context %p created child context %p\n", selfie_name, parent, used_contexts);

  return used_contexts;
}

uint32_t* cache_context(uint32_t* vctxt) {
  uint32_t* context;

  // find cached context on my boot level
  context = find_context(current_context, vctxt, used_contexts);

  if (context == (uint32_t*) 0)
    // create cached context on my boot level
    context = create_context(current_context, vctxt);

  return context;
}

void save_context(uint32_t* context) {
  uint32_t* parent_table;
  uint32_t* vctxt;
  uint32_t r;
  uint32_t* registers;
  uint32_t* vregisters;

  // save machine state
  set_pc(context, pc);

  if (get_parent(context) != MY_CONTEXT) {
    parent_table = get_pt(get_parent(context));

    vctxt = get_virtual_context(context);

    store_virtual_memory(parent_table, program_counter(vctxt), get_pc(context));

    r = 0;

    registers = get_regs(context);

    vregisters = (uint32_t*) load_virtual_memory(parent_table, regs(vctxt));

    while (r < NUMBEROFREGISTERS) {
      store_virtual_memory(parent_table, (uint32_t) (vregisters + r), *(registers + r));

      r = r + 1;
    }

    store_virtual_memory(parent_table, program_break(vctxt), get_program_break(context));

    store_virtual_memory(parent_table, exception(vctxt), get_exception(context));
    store_virtual_memory(parent_table, faulting_page(vctxt), get_faulting_page(context));
    store_virtual_memory(parent_table, exit_code(vctxt), get_exit_code(context));
  }
}

void map_page(uint32_t* context, uint32_t page, uint32_t frame) {
  uint32_t* table;

  table = get_pt(context);

  // assert: 0 <= page < VIRTUALMEMORYSIZE / PAGESIZE

  *(table + page) = frame;

  if (page <= get_page_of_virtual_address(get_program_break(context) - REGISTERSIZE)) {
    // exploit spatial locality in page table caching
    if (page < get_lo_page(context))
      set_lo_page(context, page);
    else if (page > get_me_page(context))
      set_me_page(context, page);
  }

  if (debug_map) {
    printf1((uint32_t*) "%s: page ", selfie_name);
    print_hexadecimal(page, 4);
    printf2((uint32_t*) " mapped to frame %p in context %p\n", (uint32_t*) frame, context);
  }
}

void restore_context(uint32_t* context) {
  uint32_t* parent_table;
  uint32_t* vctxt;
  uint32_t r;
  uint32_t* registers;
  uint32_t* vregisters;
  uint32_t* table;
  uint32_t page;
  uint32_t me;
  uint32_t frame;

  if (get_parent(context) != MY_CONTEXT) {
    parent_table = get_pt(get_parent(context));

    vctxt = get_virtual_context(context);

    set_pc(context, load_virtual_memory(parent_table, program_counter(vctxt)));

    r = 0;

    registers = get_regs(context);

    vregisters = (uint32_t*) load_virtual_memory(parent_table, regs(vctxt));

    while (r < NUMBEROFREGISTERS) {
      *(registers + r) = load_virtual_memory(parent_table, (uint32_t) (vregisters + r));

      r = r + 1;
    }

    set_program_break(context, load_virtual_memory(parent_table, program_break(vctxt)));

    set_exception(context, load_virtual_memory(parent_table, exception(vctxt)));
    set_faulting_page(context, load_virtual_memory(parent_table, faulting_page(vctxt)));
    set_exit_code(context, load_virtual_memory(parent_table, exit_code(vctxt)));

    table = (uint32_t*) load_virtual_memory(parent_table, page_table(vctxt));

    // assert: context page table is only mapped from beginning up and end down

    page = load_virtual_memory(parent_table, lo_page(vctxt));
    me   = load_virtual_memory(parent_table, me_page(vctxt));

    while (page <= me) {
      if (is_virtual_address_mapped(parent_table, frame_for_page(table, page))) {
        frame = load_virtual_memory(parent_table, frame_for_page(table, page));

        map_page(context, page, get_frame_for_page(parent_table, get_page_of_virtual_address(frame)));
      }

      page = page + 1;
    }

    store_virtual_memory(parent_table, lo_page(vctxt), page);

    page = load_virtual_memory(parent_table, hi_page(vctxt));

    if (is_virtual_address_mapped(parent_table, frame_for_page(table, page)))
      frame = load_virtual_memory(parent_table, frame_for_page(table, page));
    else
      frame = 0;

    while (frame != 0) {
      map_page(context, page, get_frame_for_page(parent_table, get_page_of_virtual_address(frame)));

      page  = page - 1;

      if (is_virtual_address_mapped(parent_table, frame_for_page(table, page)))
        frame = load_virtual_memory(parent_table, frame_for_page(table, page));
      else
        frame = 0;
    }

    store_virtual_memory(parent_table, hi_page(vctxt), page);
  }
}

// -----------------------------------------------------------------
// ---------------------------- KERNEL -----------------------------
// -----------------------------------------------------------------

uint32_t pavailable() {
  if (free_page_frame_memory > 0)
    return 1;
  else if (allocated_page_frame_memory + MEGABYTE <= page_frame_memory)
    return 1;
  else
    return 0;
}

uint32_t pexcess() {
  if (pavailable())
    return 1;
  else if (allocated_page_frame_memory + MEGABYTE <= 2 * page_frame_memory)
    // tolerate twice as much memory mapped on demand than physically available
    return 1;
  else
    return 0;
}

uint32_t pused() {
  return allocated_page_frame_memory - free_page_frame_memory;
}

uint32_t* palloc() {
  uint32_t block;
  uint32_t frame;

  // assert: page_frame_memory is equal to or a multiple of MEGABYTE
  // assert: PAGESIZE is a factor of MEGABYTE strictly less than MEGABYTE

  if (free_page_frame_memory == 0) {
    if (pexcess()) {
      free_page_frame_memory = MEGABYTE;

      // on boot level zero allocate zeroed memory
      block = (uint32_t) zalloc(free_page_frame_memory);

      allocated_page_frame_memory = allocated_page_frame_memory + free_page_frame_memory;

      // page frames must be page-aligned to work as page table index
      next_page_frame = round_up(block, PAGESIZE);

      if (next_page_frame > block)
        // losing one page frame to fragmentation
        free_page_frame_memory = free_page_frame_memory - PAGESIZE;
    } else {
      print(selfie_name);
      print((uint32_t*) ": palloc out of physical memory\n");

      exit(EXITCODE_OUTOFPHYSICALMEMORY);
    }
  }

  frame = next_page_frame;

  next_page_frame = next_page_frame + PAGESIZE;

  free_page_frame_memory = free_page_frame_memory - PAGESIZE;

  // strictly, touching is only necessary on boot levels higher than zero
  return touch((uint32_t*) frame, PAGESIZE);
}

void pfree(uint32_t* frame) {
  // TODO: implement free list of page frames
}

void map_and_store(uint32_t* context, uint32_t vaddr, uint32_t data) {
  // assert: is_valid_virtual_address(vaddr) == 1

  if (is_virtual_address_mapped(get_pt(context), vaddr) == 0)
    map_page(context, get_page_of_virtual_address(vaddr), (uint32_t) palloc());

  if (symbolic) {
    if (is_trace_space_available())
      // always track initialized memory by using tc as most recent branch
      store_symbolic_memory(get_pt(context), vaddr, data, 0, data, data, tc);
    else {
      printf1((uint32_t*) "%s: ealloc out of memory\n", selfie_name);

      exit(EXITCODE_OUTOFTRACEMEMORY);
    }
  } else
    store_virtual_memory(get_pt(context), vaddr, data);
}

void up_load_binary(uint32_t* context) {
  uint32_t baddr;

  // assert: entry_point is multiple of PAGESIZE and REGISTERSIZE

  set_pc(context, entry_point);
  set_lo_page(context, get_page_of_virtual_address(entry_point));
  set_me_page(context, get_page_of_virtual_address(entry_point));
  set_original_break(context, entry_point + binary_length);
  set_program_break(context, get_original_break(context));

  baddr = 0;

  if (symbolic) {
    // code is never constrained...
    symbolic = 0;

    while (baddr < code_length) {
      map_and_store(context, entry_point + baddr, load_data(baddr));

      baddr = baddr + REGISTERSIZE;
    }

    // ... but data is
    symbolic = 1;
  }

  while (baddr < binary_length) {
    map_and_store(context, entry_point + baddr, load_data(baddr));

    baddr = baddr + REGISTERSIZE;
  }

  set_name(context, binary_name);
}

uint32_t up_load_string(uint32_t* context, uint32_t* s, uint32_t SP) {
  uint32_t bytes;
  uint32_t i;

  bytes = round_up(string_length(s) + 1, REGISTERSIZE);

  // allocate memory for storing string
  SP = SP - bytes;

  i = 0;

  while (i < bytes) {
    map_and_store(context, SP + i, *s);

    s = s + 1;

    i = i + REGISTERSIZE;
  }

  return SP;
}

void up_load_arguments(uint32_t* context, uint32_t argc, uint32_t* argv) {
  /* upload arguments like a UNIX system

      SP
      |
      V
   | argc | argv[0] | ... | argv[n] | 0 | env[0] | ... | env[m] | 0 |

     with argc > 0, n == argc - 1, and m == 0 (that is, env is empty) */
  uint32_t SP;
  uint32_t* vargv;
  uint32_t i;

  // the call stack grows top down
  SP = VIRTUALMEMORYSIZE;

  vargv = smalloc(argc * SIZEOFUINT32STAR);

  i = 0;

  // push program parameters onto the stack
  while (i < argc) {
    SP = up_load_string(context, (uint32_t*) *(argv + i), SP);

    // store pointer in virtual *argv
    *(vargv + i) = SP;

    i = i + 1;
  }

  // allocate memory for termination of env table
  SP = SP - REGISTERSIZE;

  // push null value to terminate env table
  map_and_store(context, SP, 0);

  // allocate memory for termination of argv table
  SP = SP - REGISTERSIZE;

  // push null value to terminate argv table
  map_and_store(context, SP, 0);

  // assert: i == argc

  // push argv table onto the stack
  while (i > 0) {
    // allocate memory for argv table entry
    SP = SP - REGISTERSIZE;

    i = i - 1;

    // push argv table entry
    map_and_store(context, SP, *(vargv + i));
  }

  // allocate memory for argc
  SP = SP - REGISTERSIZE;

  // push argc
  map_and_store(context, SP, argc);

  // store stack pointer value in stack pointer register
  *(get_regs(context) + REG_SP) = SP;

  // set bounds to register value for symbolic execution
  if (symbolic) {
    *(reg_typ + REG_SP) = 0;

    *(reg_los + REG_SP) = SP;
    *(reg_ups + REG_SP) = SP;
  }
}

uint32_t handle_system_call(uint32_t* context) {
  uint32_t a7;

  set_exception(context, EXCEPTION_NOEXCEPTION);

  a7 = *(get_regs(context) + REG_A7);

  if (a7 == SYSCALL_BRK)
    implement_brk(context);
  else if (a7 == SYSCALL_READ)
    implement_read(context);
  else if (a7 == SYSCALL_WRITE)
    implement_write(context);
  else if (a7 == SYSCALL_OPEN)
    implement_open(context);
  else if (a7 == SYSCALL_EXIT) {
    implement_exit(context);

    // TODO: exit only if all contexts have exited
    return EXIT;
  } else {
    printf2((uint32_t*) "%s: unknown system call %d\n", selfie_name, (uint32_t*) a7);

    set_exit_code(context, EXITCODE_UNKNOWNSYSCALL);

    return EXIT;
  }

  if (get_exception(context) == EXCEPTION_MAXTRACE) {
    // exiting during symbolic execution, no exit code necessary
    set_exception(context, EXCEPTION_NOEXCEPTION);

    return EXIT;
  } else
    return DONOTEXIT;
}

uint32_t handle_page_fault(uint32_t* context) {
  set_exception(context, EXCEPTION_NOEXCEPTION);

  // TODO: use this table to unmap and reuse frames
  map_page(context, get_faulting_page(context), (uint32_t) palloc());

  return DONOTEXIT;
}

uint32_t handle_division_by_zero(uint32_t* context) {
  set_exception(context, EXCEPTION_NOEXCEPTION);

  if (record) {
    printf1((uint32_t*) "%s: division by zero, replaying...\n", selfie_name);

    replay_trace();

    set_exit_code(context, EXITCODE_NOERROR);
  } else {
    printf1((uint32_t*) "%s: division by zero\n", selfie_name);

    set_exit_code(context, EXITCODE_DIVISIONBYZERO);
  }

  return EXIT;
}

uint32_t handle_max_trace(uint32_t* context) {
  set_exception(context, EXCEPTION_NOEXCEPTION);

  set_exit_code(context, EXITCODE_OUTOFTRACEMEMORY);

  return EXIT;
}

uint32_t handle_timer(uint32_t* context) {
  set_exception(context, EXCEPTION_NOEXCEPTION);

  return DONOTEXIT;
}

uint32_t handle_exception(uint32_t* context) {
  uint32_t exception;

  exception = get_exception(context);

  if (exception == EXCEPTION_SYSCALL)
    return handle_system_call(context);
  else if (exception == EXCEPTION_PAGEFAULT)
    return handle_page_fault(context);
  else if (exception == EXCEPTION_DIVISIONBYZERO)
    return handle_division_by_zero(context);
  else if (exception == EXCEPTION_MAXTRACE)
    return handle_max_trace(context);
  else if (exception == EXCEPTION_TIMER)
    return handle_timer(context);
  else {
    printf2((uint32_t*) "%s: context %s throws uncaught ", selfie_name, get_name(context));
    print_exception(exception, get_faulting_page(context));
    println();

    set_exit_code(context, EXITCODE_UNCAUGHTEXCEPTION);

    return EXIT;
  }
}

uint32_t mipster(uint32_t* to_context) {
  uint32_t timeout;
  uint32_t* from_context;

  print((uint32_t*) "mipster\n");

  timeout = TIMESLICE;

  while (1) {
    from_context = mipster_switch(to_context, timeout);

    if (get_parent(from_context) != MY_CONTEXT) {
      // switch to parent which is in charge of handling exceptions
      to_context = get_parent(from_context);

      timeout = TIMEROFF;
    } else if (handle_exception(from_context) == EXIT)
      return get_exit_code(from_context);
    else {
      // TODO: scheduler should go here
      to_context = from_context;

      timeout = TIMESLICE;
    }
  }
}

uint32_t hypster(uint32_t* to_context) {
  uint32_t* from_context;

  print((uint32_t*) "hypster\n");

  while (1) {
    from_context = hypster_switch(to_context, TIMESLICE);

    if (handle_exception(from_context) == EXIT)
      return get_exit_code(from_context);
    else
      // TODO: scheduler should go here
      to_context = from_context;
  }
}

uint32_t mixter(uint32_t* to_context, uint32_t mix) {
  // works with mipsters and hypsters
  uint32_t mslice;
  uint32_t timeout;
  uint32_t* from_context;

  printf2((uint32_t*) "mixter (%d%% mipster/%d%% hypster)\n", (uint32_t*) mix, (uint32_t*) (100 - mix));

  mslice = TIMESLICE;

  if (mslice <= UINT32_MAX / 100)
    mslice = mslice * mix / 100;
  else if (mslice <= UINT32_MAX / 10)
    mslice = mslice / 10 * (mix / 10);
  else
    mslice = mslice / 100 * mix;

  if (mslice > 0) {
    mix = 1;

    timeout = mslice;
  } else {
    mix = 0;

    timeout = TIMESLICE;
  }

  while (1) {
    if (mix)
      from_context = mipster_switch(to_context, TIMESLICE);
    else
      from_context = hypster_switch(to_context, TIMESLICE);

    if (get_parent(from_context) != MY_CONTEXT) {
      // switch to parent which is in charge of handling exceptions
      to_context = get_parent(from_context);

      timeout = TIMEROFF;
    } else if (handle_exception(from_context) == EXIT)
      return get_exit_code(from_context);
    else {
      // TODO: scheduler should go here
      to_context = from_context;

      if (mix) {
        if (mslice != TIMESLICE) {
          mix = 0;

          timeout = TIMESLICE - mslice;
        }
      } else if (mslice > 0) {
        mix = 1;

        timeout = mslice;
      }
    }
  }
}

uint32_t minmob(uint32_t* to_context) {
  uint32_t timeout;
  uint32_t* from_context;

  timeout = TIMESLICE;

  while (1) {
    from_context = mipster_switch(to_context, TIMESLICE);

    if (get_parent(from_context) != MY_CONTEXT) {
      // switch to parent which is in charge of handling exceptions
      to_context = get_parent(from_context);

      timeout = TIMEROFF;
    } else {
      // minster and mobster do not handle page faults
      if (get_exception(from_context) == EXCEPTION_PAGEFAULT) {
        printf2((uint32_t*) "%s: context %s throws uncaught ", selfie_name, get_name(from_context));
        print_exception(get_exception(from_context), get_faulting_page(from_context));
        println();

        return EXITCODE_UNCAUGHTEXCEPTION;
      } else if (handle_exception(from_context) == EXIT)
        return get_exit_code(from_context);

      // TODO: scheduler should go here
      to_context = from_context;

      timeout = TIMESLICE;
    }
  }
}

void map_unmapped_pages(uint32_t* context) {
  uint32_t page;

  // assert: page table is only mapped from beginning up and end down

  page = get_lo_page(context);

  while (is_page_mapped(get_pt(context), page))
    page = page + 1;

  while (pavailable()) {
    map_page(context, page, (uint32_t) palloc());

    page = page + 1;
  }
}

uint32_t minster(uint32_t* to_context) {
  print((uint32_t*) "minster\n");

  // virtual is like physical memory in initial context up to memory size
  // by mapping unmapped pages (for the heap) to all available page frames
  // CAUTION: consumes memory even when not accessed
  map_unmapped_pages(to_context);

  // does not handle page faults, works only until running out of mapped pages
  return minmob(to_context);
}

uint32_t mobster(uint32_t* to_context) {
  print((uint32_t*) "mobster\n");

  // does not handle page faults, relies on fancy hypsters to do that
  return minmob(to_context);
}

void backtrack_trace(uint32_t* context) {
  uint32_t savepc;

  if (debug_symbolic)
    printf3((uint32_t*) "%s: backtracking %s from exit code %d\n", selfie_name, get_name(context), (uint32_t*) get_exit_code(context));

  symbolic = 0;

  backtrack = 1;

  while (backtrack) {
    pc = *(pcs + tc);

    if (pc == 0)
      // we have backtracked all code back to the data segment
      backtrack = 0;
    else {
      savepc = pc;

      fetch();
      decode_execute();

      if (pc != savepc)
        // backtracking stopped by sltu
        backtrack = 0;
    }
  }

  symbolic = 1;

  set_pc(context, pc);
}

uint32_t monster(uint32_t* to_context) {
  uint32_t b;
  uint32_t timeout;
  uint32_t* from_context;

  print((uint32_t*) "monster\n");

  b = 0;

  timeout = TIMESLICE;

  while (1) {
    from_context = mipster_switch(to_context, timeout);

    if (get_parent(from_context) != MY_CONTEXT) {
      // switch to parent which is in charge of handling exceptions
      to_context = get_parent(from_context);

      timeout = TIMEROFF;
    } else {
      if (handle_exception(from_context) == EXIT) {
        backtrack_trace(from_context);

        if (b == 0)
          printf1((uint32_t*) "%s: backtracking ", selfie_name);
        else
          unprint_integer(b);

        b = b + 1;

        print_integer(b);

        if (pc == 0) {
          println();

          return EXITCODE_NOERROR;
        }
      }

      // TODO: scheduler should go here
      to_context = from_context;

      timeout = TIMESLICE;
    }
  }
}

uint32_t is_boot_level_zero() {
  // in C99 malloc(0) returns either a null pointer or a unique pointer.
  // (see http://pubs.opengroup.org/onlinepubs/9699919799/)
  // selfie's malloc implementation, on the other hand,
  // returns the same not null address, if malloc(0) is called consecutively.
  uint32_t first_malloc;
  uint32_t second_malloc;

  first_malloc = (uint32_t) malloc(0);
  second_malloc = (uint32_t) malloc(0);

  if (first_malloc == 0)
    return 1;
  if (first_malloc != second_malloc)
    return 1;

  // it is selfie's malloc, so it can not be boot level zero.
  return 0;
}

uint32_t selfie_run(uint32_t machine) {
  uint32_t exit_code;

  if (binary_length == 0) {
    printf1((uint32_t*) "%s: nothing to run, debug, or host\n", selfie_name);

    return EXITCODE_BADARGUMENTS;
  }

  if (machine == DIPSTER) {
    debug       = 1;
    disassemble = 1;
  } else if (machine == RIPSTER) {
    debug  = 1;
    record = 1;

    init_replay_engine();
  } else if (machine == MONSTER) {
    debug    = 1;
    symbolic = 1;

    init_symbolic_engine();
  }

  if (machine == MONSTER) {
    init_memory(round_up(MAX_TRACE_LENGTH * SIZEOFUINT32, MEGABYTE) / MEGABYTE + 1);

    fuzz = atoi(peek_argument());
  } else
    init_memory(atoi(peek_argument()));

  execute = 1;

  reset_interpreter();
  reset_microkernel();

  create_context(MY_CONTEXT, 0);

  up_load_binary(current_context);

  // pass binary name as first argument by replacing memory size
  set_argument(binary_name);

  up_load_arguments(current_context, number_of_remaining_arguments(), remaining_arguments());

  printf3((uint32_t*) "%s: selfie executing %s with %dMB physical memory on ", selfie_name, binary_name, (uint32_t*) (page_frame_memory / MEGABYTE));

  if (machine == MIPSTER)
    exit_code = mipster(current_context);
  else if (machine == DIPSTER)
    exit_code = mipster(current_context);
  else if (machine == RIPSTER)
    exit_code = mipster(current_context);
  else if (machine == MONSTER)
    exit_code = monster(current_context);
  else if (machine == MINSTER)
    exit_code = minster(current_context);
  else if (machine == MOBSTER)
    exit_code = mobster(current_context);
  else if (machine == HYPSTER)
    if (is_boot_level_zero())
      // no hypster on boot level zero
      exit_code = mipster(current_context);
    else
      exit_code = hypster(current_context);
  else
    // change 0 to anywhere between 0% to 100% mipster
    exit_code = mixter(current_context, 0);

  execute = 0;

  printf3((uint32_t*) "%s: selfie terminating %s with exit code %d\n", selfie_name, get_name(current_context), (uint32_t*) exit_code);

  print_profile();

  symbolic    = 0;
  record      = 0;
  disassemble = 0;
  debug       = 0;

  fuzz = 0;

  return exit_code;
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ----------------   T H E O R E M  P R O V E R    ----------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// -------------------------- SAT Solver ---------------------------
// -----------------------------------------------------------------

uint32_t clause_may_be_true(uint32_t* clause_address, uint32_t depth) {
  uint32_t variable;

  variable = 0;

  while (variable <= depth) {
    if (*(sat_assignment + variable) == TRUE) {
      if (*(clause_address + 2 * variable))
        return TRUE;
    } else if (*(clause_address + 2 * variable + 1))
      // variable must be FALSE because variable <= depth
      return TRUE;

    variable = variable + 1;
  }

  while (variable < number_of_sat_variables) {
    // variable must be unassigned because variable > depth
    if (*(clause_address + 2 * variable))
      return TRUE;
    else if (*(clause_address + 2 * variable + 1))
      return TRUE;

    variable = variable + 1;
  }

  return FALSE;
}

uint32_t instance_may_be_true(uint32_t depth) {
  uint32_t clause;

  clause = 0;

  while (clause < number_of_sat_clauses) {
    if (clause_may_be_true(sat_instance + clause * 2 * number_of_sat_variables, depth))
      clause = clause + 1;
    else
      // clause is FALSE under current assignment
      return FALSE;
  }

  return TRUE;
}

uint32_t babysat(uint32_t depth) {
  if (depth == number_of_sat_variables)
    return SAT;

  *(sat_assignment + depth) = TRUE;

  if (instance_may_be_true(depth)) if (babysat(depth + 1) == SAT)
    return SAT;

  *(sat_assignment + depth) = FALSE;

  if (instance_may_be_true(depth)) if (babysat(depth + 1) == SAT)
    return SAT;

  return UNSAT;
}

// -----------------------------------------------------------------
// ----------------------- DIMACS CNF PARSER -----------------------
// -----------------------------------------------------------------

void selfie_print_dimacs() {
  uint32_t clause;
  uint32_t variable;

  printf2((uint32_t*) "p cnf %d %d\n", (uint32_t*) number_of_sat_variables, (uint32_t*) number_of_sat_clauses);

  clause = 0;

  while (clause < number_of_sat_clauses) {
    variable = 0;

    while (variable < number_of_sat_variables) {
      if (*(sat_instance + clause * 2 * number_of_sat_variables + 2 * variable) == TRUE) {
        print_integer(variable + 1);
        print((uint32_t*) " ");
      } else if (*(sat_instance + clause * 2 * number_of_sat_variables + 2 * variable + 1) == TRUE) {
        print_integer(-(variable + 1));
        print((uint32_t*) " ");
      }

      variable = variable + 1;
    }

    print((uint32_t*) "0\n");

    clause = clause + 1;
  }
}

void dimacs_find_next_character(uint32_t new_line) {
  uint32_t in_comment;

  // assuming we are not in a comment
  in_comment = 0;

  // read and discard all whitespace and comments until a character is found
  // that is not whitespace and does not occur in a comment, or the file ends
  while (1) {
    if (in_comment) {
      get_character();

      if (is_character_new_line())
        // comments end with new line
        in_comment = 0;
      else if (character == CHAR_EOF)
        return;
      else
        // count the characters in comments as ignored characters
        // line feed and carriage return are counted below
        number_of_ignored_characters = number_of_ignored_characters + 1;
    } else if (new_line) {
      new_line = 0;

      if (character == 'c') {
        // 'c' at beginning of a line begins a comment
        in_comment = 1;

        // count the number of comments
        number_of_comments = number_of_comments + 1;
      }
    } else if (is_character_whitespace()) {
      if (is_character_new_line())
        new_line = 1;
      else
        new_line = 0;

      // count whitespace as ignored characters
      number_of_ignored_characters = number_of_ignored_characters + 1;

      get_character();
    } else
      // character found that is not whitespace and not occurring in a comment
      return;
  }
}

void dimacs_get_symbol() {
  dimacs_find_next_character(0);

  get_symbol();
}

void dimacs_word(uint32_t* word) {
  if (symbol == SYM_IDENTIFIER) {
    if (string_compare(identifier, word)) {
      dimacs_get_symbol();

      return;
    } else
      syntax_error_identifier(word);
  } else
    syntax_error_symbol(SYM_IDENTIFIER);

  exit(EXITCODE_PARSERERROR);
}

uint32_t dimacs_number() {
  uint32_t number;

  if (symbol == SYM_INTEGER) {
    number = literal;

    dimacs_get_symbol();

    return number;
  } else
    syntax_error_symbol(SYM_INTEGER);

  exit(EXITCODE_PARSERERROR);
}

void dimacs_get_clause(uint32_t clause) {
  uint32_t not;

  while (1) {
    not = 0;

    if (symbol == SYM_MINUS) {
      not = 1;

      dimacs_get_symbol();
    }

    if (symbol == SYM_INTEGER) {
      if (literal == 0) {
        dimacs_get_symbol();

        return;
      } else if (literal > number_of_sat_variables) {
        syntax_error_message((uint32_t*) "clause exceeds declared number of variables");

        exit(EXITCODE_PARSERERROR);
      }

      // literal encoding starts at 0
      literal = literal - 1;

      if (not)
        *(sat_instance + clause * 2 * number_of_sat_variables + 2 * literal + 1) = TRUE;
      else
        *(sat_instance + clause * 2 * number_of_sat_variables + 2 * literal) = TRUE;
    } else if (symbol == SYM_EOF)
      return;
    else
      syntax_error_symbol(SYM_INTEGER);

    dimacs_get_symbol();
  }
}

void dimacs_get_instance() {
  uint32_t clauses;

  clauses = 0;

  while (clauses < number_of_sat_clauses)
    if (symbol != SYM_EOF) {
      dimacs_get_clause(clauses);

      clauses = clauses + 1;
    } else {
      syntax_error_message((uint32_t*) "instance has fewer clauses than declared");

      exit(EXITCODE_PARSERERROR);
    }

  if (symbol != SYM_EOF) {
    syntax_error_message((uint32_t*) "instance has more clauses than declared");

    exit(EXITCODE_PARSERERROR);
  }
}

void selfie_load_dimacs() {
  source_name = get_argument();

  printf2((uint32_t*) "%s: selfie loading SAT instance %s\n", selfie_name, source_name);

  // assert: source_name is mapped and not longer than MAX_FILENAME_LENGTH

  source_fd = open(source_name, O_RDONLY, 0);

  if (signed_less_than(source_fd, 0)) {
    printf2((uint32_t*) "%s: could not open input file %s\n", selfie_name, source_name);

    exit(EXITCODE_IOERROR);
  }

  reset_scanner();

  // ignore all comments before problem
  dimacs_find_next_character(1);

  dimacs_get_symbol();

  dimacs_word((uint32_t*) "p");
  dimacs_word((uint32_t*) "cnf");

  number_of_sat_variables = dimacs_number();

  sat_assignment = (uint32_t*) smalloc(number_of_sat_variables * SIZEOFUINT32);

  number_of_sat_clauses = dimacs_number();

  sat_instance = (uint32_t*) smalloc(number_of_sat_clauses * 2 * number_of_sat_variables * SIZEOFUINT32);

  dimacs_get_instance();

  printf4((uint32_t*) "%s: %d clauses with %d declared variables loaded from %s\n", selfie_name, (uint32_t*) number_of_sat_clauses, (uint32_t*) number_of_sat_variables, source_name);

  dimacs_name = source_name;
}

void selfie_sat() {
  uint32_t variable;

  selfie_load_dimacs();

  if (dimacs_name == (uint32_t*) 0) {
    printf1((uint32_t*) "%s: nothing to SAT solve\n", selfie_name);

    return;
  }

  selfie_print_dimacs();

  if (babysat(0) == SAT) {
    printf2((uint32_t*) "%s: %s is satisfiable with ", selfie_name, dimacs_name);

    variable = 0;

    while (variable < number_of_sat_variables) {
      if (*(sat_assignment + variable) == FALSE)
        printf1((uint32_t*) "-%d ", (uint32_t*) (variable + 1));
      else
        printf1((uint32_t*) "%d ", (uint32_t*) (variable + 1));

      variable = variable + 1;
    }
  } else
    printf2((uint32_t*) "%s: %s is unsatisfiable", selfie_name, dimacs_name);

  println();
}

// -----------------------------------------------------------------
// ----------------------------- MAIN ------------------------------
// -----------------------------------------------------------------

uint32_t number_of_remaining_arguments() {
  return selfie_argc;
}

uint32_t* remaining_arguments() {
  return selfie_argv;
}

uint32_t* peek_argument() {
  if (number_of_remaining_arguments() > 0)
    return (uint32_t*) *selfie_argv;
  else
    return (uint32_t*) 0;
}

uint32_t* get_argument() {
  uint32_t* argument;

  argument = peek_argument();

  if (number_of_remaining_arguments() > 0) {
    selfie_argc = selfie_argc - 1;
    selfie_argv = selfie_argv + 1;
  }

  return argument;
}

void set_argument(uint32_t* argv) {
  *selfie_argv = (uint32_t) argv;
}

void print_usage() {
  printf3((uint32_t*) "%s: usage: selfie { %s } [ %s ]\n",
    selfie_name,
      (uint32_t*) "-c { source } | -o binary | [ -s | -S ] assembly | -l binary | -sat dimacs",
      (uint32_t*) "( -m | -d | -r | -n | -y | -min | -mob ) 0-32 ...");
}

uint32_t selfie() {
  uint32_t* option;

  if (number_of_remaining_arguments() == 0)
    print_usage();
  else {
    init_scanner();
    init_register();
    init_interpreter();

    while (number_of_remaining_arguments() > 0) {
      option = get_argument();

      if (string_compare(option, (uint32_t*) "-c"))
        selfie_compile();

      else if (number_of_remaining_arguments() == 0) {
        // remaining options have at least one argument
        print_usage();

        return EXITCODE_BADARGUMENTS;
      } else if (string_compare(option, (uint32_t*) "-o"))
        selfie_output();
      else if (string_compare(option, (uint32_t*) "-s"))
        selfie_disassemble(0);
      else if (string_compare(option, (uint32_t*) "-S"))
        selfie_disassemble(1);
      else if (string_compare(option, (uint32_t*) "-l"))
        selfie_load();
      else if (string_compare(option, (uint32_t*) "-sat"))
        selfie_sat();
      else if (string_compare(option, (uint32_t*) "-m"))
        return selfie_run(MIPSTER);
      else if (string_compare(option, (uint32_t*) "-d"))
        return selfie_run(DIPSTER);
      else if (string_compare(option, (uint32_t*) "-r"))
        return selfie_run(RIPSTER);
      else if (string_compare(option, (uint32_t*) "-n"))
        return selfie_run(MONSTER);
      else if (string_compare(option, (uint32_t*) "-y"))
        return selfie_run(HYPSTER);
      else if (string_compare(option, (uint32_t*) "-min"))
        return selfie_run(MINSTER);
      else if (string_compare(option, (uint32_t*) "-mob"))
        return selfie_run(MOBSTER);
      else {
        print_usage();

        return EXITCODE_BADARGUMENTS;
      }
    }
  }

  return EXITCODE_NOERROR;
}

uint32_t main(uint32_t argc, uint32_t* argv) {
  init_selfie((uint32_t) argc, (uint32_t*) argv);

  init_library();

  return selfie();
}
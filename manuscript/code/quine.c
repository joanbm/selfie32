// This C* code outputs its own source code: quine.c in C*
void init_library();
void print(uint32_t* s);
void print_integer(uint32_t i);
void print_string(uint32_t* s);
void println();
uint32_t main() {
  uint32_t* source;
  uint32_t i;
  init_library();
  source = malloc(41*4);
  *(source + 0) = (uint32_t) "// This C* code outputs its own source code: quine.c in C*";
  *(source + 1) = (uint32_t) "void init_library();";
  *(source + 2) = (uint32_t) "void print(uint32_t* s);";
  *(source + 3) = (uint32_t) "void print_integer(uint32_t i);";
  *(source + 4) = (uint32_t) "void print_string(uint32_t* s);";
  *(source + 5) = (uint32_t) "void println();";
  *(source + 6) = (uint32_t) "uint32_t main() {";
  *(source + 7) = (uint32_t) "  uint32_t* source;";
  *(source + 8) = (uint32_t) "  uint32_t i;";
  *(source + 9) = (uint32_t) "  init_library();";
  *(source + 10) = (uint32_t) "  source = malloc(41*4);";
  *(source + 11) = (uint32_t) "  // printing source code before stored code";
  *(source + 12) = (uint32_t) "  i = 0;";
  *(source + 13) = (uint32_t) "  while (i < 11) {";
  *(source + 14) = (uint32_t) "    print(*(source + i));";
  *(source + 15) = (uint32_t) "    println();";
  *(source + 16) = (uint32_t) "    i = i + 1;";
  *(source + 17) = (uint32_t) "  }";
  *(source + 18) = (uint32_t) "  // printing stored source code";
  *(source + 19) = (uint32_t) "  i = 0;";
  *(source + 20) = (uint32_t) "  while (i < 41) {";
  *(source + 21) = (uint32_t) "    print(*(source + 38));";
  *(source + 22) = (uint32_t) "    print_integer(i);";
  *(source + 23) = (uint32_t) "    print(*(source + 39));";
  *(source + 24) = (uint32_t) "    print_string(*(source + i));";
  *(source + 25) = (uint32_t) "    print(*(source + 40));";
  *(source + 26) = (uint32_t) "    println();";
  *(source + 27) = (uint32_t) "    i = i + 1;";
  *(source + 28) = (uint32_t) "  }";
  *(source + 29) = (uint32_t) "  // printing source code after stored code";
  *(source + 30) = (uint32_t) "  i = 11;";
  *(source + 31) = (uint32_t) "  while (i < 38) {";
  *(source + 32) = (uint32_t) "    print(*(source + i));";
  *(source + 33) = (uint32_t) "    println();";
  *(source + 34) = (uint32_t) "    i = i + 1;";
  *(source + 35) = (uint32_t) "  }";
  *(source + 36) = (uint32_t) "  return 0;";
  *(source + 37) = (uint32_t) "}";
  *(source + 38) = (uint32_t) "  *(source + ";
  *(source + 39) = (uint32_t) ") = (uint32_t) ";
  *(source + 40) = (uint32_t) ";";
  // printing source code before stored code
  i = 0;
  while (i < 11) {
    print(*(source + i));
    println();
    i = i + 1;
  }
  // printing stored source code
  i = 0;
  while (i < 41) {
    print(*(source + 38));
    print_integer(i);
    print(*(source + 39));
    print_string(*(source + i));
    print(*(source + 40));
    println();
    i = i + 1;
  }
  // printing source code after stored code
  i = 11;
  while (i < 38) {
    print(*(source + i));
    println();
    i = i + 1;
  }
  return 0;
}

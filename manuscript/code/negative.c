// printing the negative decimal number -85 in C*

// libcstar procedures for printing
void init_library();
void print(uint32_t* s);
void print_integer(uint32_t n);
void print_hexadecimal(uint32_t n, uint32_t a);
void print_octal(uint32_t n, uint32_t a);
void print_binary(uint32_t n, uint32_t a);
void println();

uint32_t UINT32_MAX;

uint32_t INT32_MAX;
uint32_t INT32_MIN;

uint32_t main() {
  // initialize selfie's libcstar library
  init_library();

  // print the integer literal -85 in decimal
  print("       -85 in decimal:     ");
  print_integer(-85);
  println();

  // print the integer literal -85 in hexadecimal
  print("       -85 in hexadecimal: ");
  print_hexadecimal(-85, 0);
  println();

  // print the integer literal -85 in octal
  print("       -85 in octal:       ");
  print_octal(-85, 0);
  println();

  // print the integer literal -85 in binary
  print("       -85 in binary:      ");
  print_binary(-85, 0);
  println();

  // print UINT32_MAX in decimal
  print("UINT32_MAX in decimal:     ");
  print_integer(UINT32_MAX);
  println();

  // print UINT32_MAX in hexadecimal
  print("UINT32_MAX in hexadecimal: ");
  print_hexadecimal(UINT32_MAX, 0);
  println();

  // print UINT32_MAX in octal
  print("UINT32_MAX in octal:       ");
  print_octal(UINT32_MAX, 0);
  println();

  // print UINT32_MAX in binary
  print("UINT32_MAX in binary:      ");
  print_binary(UINT32_MAX, 32);
  println();

  // print INT32_MAX in decimal
  print(" INT32_MAX in decimal:     ");
  print_integer(INT32_MAX);
  println();

  // print INT32_MAX in hexadecimal
  print(" INT32_MAX in hexadecimal: ");
  print_hexadecimal(INT32_MAX, 0);
  println();

  // print INT32_MAX in octal
  print(" INT32_MAX in octal:       ");
  print_octal(INT32_MAX, 0);
  println();

  // print INT32_MAX in binary
  print(" INT32_MAX in binary:      ");
  print_binary(INT32_MAX, 32);
  println();

  // print INT32_MIN in decimal
  print(" INT32_MIN in decimal:     ");
  print_integer(INT32_MIN);
  println();

  // print INT32_MIN in hexadecimal
  print(" INT32_MIN in hexadecimal: ");
  print_hexadecimal(INT32_MIN, 0);
  println();

  // print INT32_MIN in octal
  print(" INT32_MIN in octal:       ");
  print_octal(INT32_MIN, 0);
  println();

  // print INT32_MIN in binary
  print(" INT32_MIN in binary:      ");
  print_binary(INT32_MIN, 32);
  println();
}
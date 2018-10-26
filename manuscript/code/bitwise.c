// left shifting, bitwise ORing, and logical right shifting in C*

// libcstar procedures for printing
void init_library();
void print(uint32_t* s);
void print_integer(uint32_t n);
void print_binary(uint32_t n, uint32_t a);
void println();

// libcstar procedures for left and right shifting
uint32_t left_shift(uint32_t n, uint32_t b);
uint32_t right_shift(uint32_t n, uint32_t b);

uint32_t main() {
  uint32_t i;
  uint32_t j;
  uint32_t u;

  // initialize selfie's libcstar library
  init_library();

  // initialize the integer i to binary 0000000000000000000000000000000000000000000000000000000000000011
  i = 3;

  // initialize the integer u to i
  u = i;

  // repeat until i is equal to 0
  while (i != 0) {
    // print i in binary
    print_binary(i, 32);
    print(" in binary = ");
    print_integer(i);
    print(" in decimal");
    println();

    // remember value of i
    j = i;

    // shift i to the left by 6 bits
    i = left_shift(i, 6);

    // signed integer addition here amounts to bitwise OR because
    // the bits at the same index in u and i are never both 1 so
    // there will not be any carry bit set
    u = u + i;
  }

  // print u in binary
  print_binary(u, 32);
  print(" in binary = ");
  print_integer(u);
  print(" in decimal");
  println();

  // set i to its most recent value before it became 0
  i = j;

  // repeat until i is equal to 0
  while (i != 0) {
    // print i in binary
    print_binary(i, 32);
    print(" in binary = ");
    print_integer(i);
    print(" in decimal");
    println();

    // shift i to the right by 6 bits
    i = right_shift(i, 6);
  }
}
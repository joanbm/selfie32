// global variable for pointing to the "Hello World!    " string
uint32_t* foo;

// main procedure for printing "Hello World!    " on the console
uint32_t* main() {
  // point to the "Hello World!    " string
  foo = "Hello World!    ";

  // strings are actually stored in chunks of 4 characters in memory,
  // that is, here as "Hell", "o Wo", "rld!" and "    " which allows us to
  // print them conveniently in chunks of 4 characters at a time

  // as long as there are characters print them
  while (*foo != 0) {
    // 1 means that we print to the console
    // foo points to a chunk of 4 characters
    // 4 means that we print 4 characters
    write(1, foo, 4);

    // go to the next chunk of 4 characters
    foo = foo + 1;
  }
}
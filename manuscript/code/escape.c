uint32_t main() {
  uint32_t* string;

  // looks bad but shows what is possible
  string = "\nThis is a demonstration of \"escape sequences\".\n\nselfie supports:\n\t\\n\n\t\\t\n\t\\b\n\t\\\'\n\t\\\"\n\t\\\%\n\t\\\\\n\n";

  while (*string != 0) {
    // 1 means that we print to the console
    // foo points to a chunk of 4 characters
    // 4 means that we print 4 characters
    write(1, string, 4);

    // go to the next chunk of 4 characters
    string = string + 1;
  }

  return 0;
}

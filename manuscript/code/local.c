uint32_t f(uint32_t x) {
  while (x > 0)
    x = x - 1;

  return x;
}

uint32_t main() {
  uint32_t x;
  
  x = 0;

  x = x + 1;

  if (x == 1)
    x = x + 1;
  else
    x = x - 1;

  return f(x);
}
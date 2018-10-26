uint32_t f(uint32_t x, uint32_t y) {
  while (y > 0) {
    x = x + 1;
    y = y - 1;
  }

  return x;
}

uint32_t g(uint32_t x, uint32_t y) {
  if (y > 0)
    return g(x, y - 1) + 1;
  else
    return x;
}

uint32_t main() {
  return f(1,2) - g(1,2);
}
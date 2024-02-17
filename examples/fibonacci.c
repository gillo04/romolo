int printf(char*, short, int);

int fib(short);

int main() {
  printf("Fibonacci sequence from 0 to 10:\n", 0, 0);
  for (short i = 0; i <= 10; ++i) {
    printf("%hu: %d\n", i, fib(i));
  }
  return 0;
}

int fib(short n) {
  if (n < 2) {
    return n;
  }

  return fib(n-1) + fib(n-2);
}

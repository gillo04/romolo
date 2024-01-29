int printf(char*, int, int);

int fib(int);

int main() {
  printf("Fibonacci sequence from 0 to 10:\n", 0, 0);
  for (int i = 0; i <= 10; ++i) {
    printf("%d: %d\n", i, fib(i));
  }
  return 0;
}

int fib(int n) {
  if (n < 2) {
    return n;
  }

  return fib(n-1) + fib(n-2);
}

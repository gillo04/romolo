int printf(char*, int, int);
// int scanf(char*, int*);

int fib(int);

int main() {
  printf("Fibonacci sequence from 0 to: ", 0, 0);
  // int num = 0;
  // scanf("%d", &num);
  for (int i = 0; i <= 10; ++i) {
    printf("%d:\t%d\n", i, fib(i));
  }
  return 0;
}

int fib(int n) {
  if (n < 2) {
    return n;
  }

  return fib(n-1) + fib(n-2);
}

// Returns the 10th number of the fibonacci sequence

int fib(int n) {
  if (n < 2) {
    return n;
  }

  return fib(n-1) + fib(n-2);
}

int main() {
  int num = 10;
  return fib(num);
}

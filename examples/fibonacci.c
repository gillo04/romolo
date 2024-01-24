// Returns the 10th number of the fibonacci sequence
int main() {
  int f0 = 0;
  int f1 = 1;

  while (f0 == 0) {
    do {
      for (int i = 2; i <= 10; ++i) {
        int tmp = f0 + f1;
        f0 = f1;
        f1 = tmp;
      }
    } while (f1 == 1);
  }

  return f1;
}

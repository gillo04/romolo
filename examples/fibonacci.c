// Returns the 10th number of the fibonacci sequence
int main() {
  int f0 = 0;
  int f1 = 1;

  int i = 2;
  while (i <= 10) {
    int tmp = f0 + f1;
    f0 = f1;
    f1 = tmp;
    ++i;
  }
  int lol = 0;

  return f1;
}

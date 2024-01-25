// Returns the 10th number of the fibonacci sequence
int main() {
  int f0 = 0;
  int f1 = 1;

  int i = 2;
  while (1) {
    int tmp = f0 + f1;
    f0 = f1;
    f1 = tmp;
    ++i;

    if (i > 10) {
      break;
    } else {
      continue;
    }
  }

  return f1;
}

// Returns the 10th number of the fibonacci sequence
int main() {
  long f0 = 0;
  long f1 = 1;

  long i = 2;
  while (1) {
    long tmp = f0 + f1;
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

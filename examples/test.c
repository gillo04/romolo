int printf(char*, int);

int main() {
  int a = 1234;
  int *b = &a;
  int **c = &b;

  printf("%d\n", **c);
  return 0;
}


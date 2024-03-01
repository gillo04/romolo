int printf(char*, int);

int main() {
  int a = 1234, *b = &a, **c = &b;

  printf("%d\n", *b);
  printf("%d\n", b[0]);
  return 0;
}


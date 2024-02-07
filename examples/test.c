int printf(char*, int);

int main() {
  int a = 1234;
  int ***b = &a;

  printf("%d\n", a);
  printf("%d\n", *b);
  return 0;
}


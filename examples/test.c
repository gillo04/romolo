int printf(char*, int);

int main() {
  int a = 1234;
  int ***b = &a;

  printf("%d\n", a);
  printf("%d\n", *(0+b+0));
  return 0;
}


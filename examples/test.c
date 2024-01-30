int printf(char*, int);

int main() {
  int a = 1234;
  int* ptr = &a;
  int** b;
  printf("num: %d\n", *ptr);
  return 0;
}


int main() {
  int a = 1;
  int b = 2;
  {
    int c = 3;
    int d = 4;
    {
      int e = 5;
    }
    int e = 5;
  }
  int e = 5;
  
  return 42;
}


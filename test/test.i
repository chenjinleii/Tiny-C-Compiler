# 1 "../test/test.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 348 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "../test/test.c" 2
int putchar(int c);

int fuck(int i) {
  return i;
}

int main() {
  printf("Hello World! %d\n", 42);
  for (double y = 1.5; y > -1.5; y -= 0.1) {
    for (double x = -1.5; x < 1.5; x += 0.05) {
      double a = x * x + y * y - 1;
      if (a * a * a - x * x * y * y * y <= 0.0) {
        putchar('*');
      } else {
        putchar(' ');
      }
    }
    putchar('\n');
  }

  if (!fuck(0))
    putchar(48);
  if (fuck(2))
    putchar(49);
  if (~fuck(2))
    putchar(50);

  putchar(fuck(48));
  putchar(~2);
  putchar(2);
  putchar(!2);
  putchar(!2.0);

  putchar(10);

  int i = 9;
  while (i > 0) {
    putchar(i + 48);
    --i;
  }

  return 0;
}

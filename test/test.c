int putchar(int c);

int fuck(int i) {
  return i;
}

double shit(double i) {
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
    putchar(48); //true
  if (fuck(2))
    putchar(49); //false
  if (~fuck(2))
    putchar(50); //false

  putchar(fuck(48));
  putchar(~2);
  putchar(2);
  putchar(!2);
  putchar(!2.0);
  //putchar(~shit(3.4));

  putchar(10);

  int i = 9;
  while (i > 0) {
    putchar(i + 48);
    --i;
  }
  double a = 1;
  printf("%.2f\n", a);
  putchar(48.6);
  return 0;
}
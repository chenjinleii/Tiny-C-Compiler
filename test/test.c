int putchar(int c);

int main() {
  if(){

  }
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
  return 0;
}
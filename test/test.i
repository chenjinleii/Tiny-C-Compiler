# 1 "/home/kaiser/CLionProjects/Tiny-C-Compiler/test/test.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "/home/kaiser/CLionProjects/Tiny-C-Compiler/test/test.c"
int putchar(int c);

int main() {
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

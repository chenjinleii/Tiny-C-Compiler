int a = 10;

extern void shit(double x, int u);

int fcuk(int a, int b) {
    return a + b;
}

int main() {
    double x;
    double y;

    fcuk(1, 2);

    for (y = 1.5; y > -1.5; y -= 0.1) {
        double x;
        for (x = -1.5; x < 1.5; x += 0.05) {
            double a = x * x + y * y - 1;
            double x;
            if (a * a * a - x * x * y * y * y <= 0.0) {
                putchar('*');
            } else {
                putchar(' ');
            }
        }
        double x;
        while (x) {
            putchar('\n');
            double x;
            return x * 2 - 1;
        }
        putchar('\n');
        double x;
    }
    shit(1.6, 9);
    return 0;
}

int fcuk(int a, int b) {
    return 1 + 2 + a * b;
}

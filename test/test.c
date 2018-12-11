//int main() {
//    int a = 1 + 2;
//}

int main() {
    for (y = 1.5; y > -1.5; y -= 0.1) {
        for (x = -1.5; x < 1.5; x += 0.05) {
            double a = x * x + y * y - 1;
            putchar(a * a * a - x * x * y * y * y);
        }
        putchar('\n');
    }
}

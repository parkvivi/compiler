/* fibonacci bottom-up */

int fibonacci(int n) {
    int a;  /* n-1 */
    int b;  /* n-2 */
    int c;
    int i;

    a = 1;
    b = 1;

    if (n == 1) {
        return b;
    }

    if (n == 2) {
        return a;
    }

    i = 2;
    while (i < n) {
        c = a + b;
        b = a;
        a = c;
        i = i + 1;
    }

    return c;
}

void main(void) {
    int n;

    n = input();
    output(fibonacci(n));
}
/* a % b */

int resto(int a, int b) {
    return a - a/b*b;
}

void main(void) {
    int a;
    int b;

    a = input();
    b = input();

    output(resto(a,b));
}
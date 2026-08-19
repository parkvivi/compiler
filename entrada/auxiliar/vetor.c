void main(void) {
    int v[5];
    int i;

    i = 0;
    while (i < 5) {
        v[i] = i;
        i = i + 1;
    }

    output(v[3]);
}
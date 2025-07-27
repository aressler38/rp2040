//
// Created by alexander on 7/5/24.
//

#import "morse_code.h"

static struct {
    int
            a[21], b[4], c[4], d[3], e[1], f[4], g[3], h[4], i[2],
            j[4], k[3], l[4], m[2], n[2], o[3], p[4], q[4], r[3],
            s[3], t[1], u[3], v[4], w[3], x[4], y[4], z[4];
    int (*list[])[];
} MORSE_TABLE = {
        .a = {MORSE_CODE_DIT, MORSE_CODE_DAH,},
        .b = {MORSE_CODE_DAH, MORSE_CODE_DIT, MORSE_CODE_DIT, MORSE_CODE_DIT,},
        .c = {MORSE_CODE_DAH, MORSE_CODE_DIT, MORSE_CODE_DAH, MORSE_CODE_DIT,},
        .d = {MORSE_CODE_DAH, MORSE_CODE_DIT, MORSE_CODE_DIT,},
        .e = {MORSE_CODE_DIT,},
        .f = {MORSE_CODE_DIT, MORSE_CODE_DIT, MORSE_CODE_DAH, MORSE_CODE_DIT,},
        .g = {MORSE_CODE_DAH, MORSE_CODE_DAH, MORSE_CODE_DIT,},
        .h = {MORSE_CODE_DIT, MORSE_CODE_DIT, MORSE_CODE_DIT, MORSE_CODE_DIT,},
        .i = {MORSE_CODE_DIT, MORSE_CODE_DIT,},
        .j = {MORSE_CODE_DIT, MORSE_CODE_DAH, MORSE_CODE_DAH, MORSE_CODE_DAH,},
        .k = {MORSE_CODE_DAH, MORSE_CODE_DIT, MORSE_CODE_DAH,},
        .l = {MORSE_CODE_DIT, MORSE_CODE_DAH, MORSE_CODE_DIT, MORSE_CODE_DIT,},
        .m = {MORSE_CODE_DAH, MORSE_CODE_DAH,},
        .n = {MORSE_CODE_DAH, MORSE_CODE_DIT,},
        .o = {MORSE_CODE_DAH, MORSE_CODE_DAH, MORSE_CODE_DAH,},
        .p = {MORSE_CODE_DIT, MORSE_CODE_DAH, MORSE_CODE_DAH, MORSE_CODE_DIT,},
        .q = {MORSE_CODE_DAH, MORSE_CODE_DAH, MORSE_CODE_DIT, MORSE_CODE_DAH,},
        .r = {MORSE_CODE_DIT, MORSE_CODE_DAH, MORSE_CODE_DIT,},
        .s = {MORSE_CODE_DIT, MORSE_CODE_DIT, MORSE_CODE_DIT,},
        .t = {MORSE_CODE_DAH,},
        .u = {MORSE_CODE_DIT, MORSE_CODE_DIT, MORSE_CODE_DAH,},
        .v = {MORSE_CODE_DIT, MORSE_CODE_DIT, MORSE_CODE_DIT, MORSE_CODE_DAH,},
        .w = {MORSE_CODE_DIT, MORSE_CODE_DAH, MORSE_CODE_DAH,},
        .x = {MORSE_CODE_DAH, MORSE_CODE_DIT, MORSE_CODE_DIT, MORSE_CODE_DAH,},
        .y = {MORSE_CODE_DAH, MORSE_CODE_DIT, MORSE_CODE_DAH, MORSE_CODE_DAH,},
        .z = {MORSE_CODE_DAH, MORSE_CODE_DAH, MORSE_CODE_DIT, MORSE_CODE_DIT,},

        .list = {&MORSE_TABLE.a, &MORSE_TABLE.b, &MORSE_TABLE.c,
                 &MORSE_TABLE.d, &MORSE_TABLE.e, &MORSE_TABLE.f,
                 &MORSE_TABLE.g, &MORSE_TABLE.h, &MORSE_TABLE.i,
                 &MORSE_TABLE.j, &MORSE_TABLE.k, &MORSE_TABLE.l,
                 &MORSE_TABLE.m, &MORSE_TABLE.n, &MORSE_TABLE.o,
                 &MORSE_TABLE.p, &MORSE_TABLE.q, &MORSE_TABLE.r,
                 &MORSE_TABLE.s, &MORSE_TABLE.t, &MORSE_TABLE.u,
                 &MORSE_TABLE.v, &MORSE_TABLE.w, &MORSE_TABLE.x,
                 &MORSE_TABLE.y, &MORSE_TABLE.z,
        },
};


void morse_code_flash(char *words[], size_t count) {
    //cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    //sleep_ms(100);
    //cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

    /*
    for (int i = 0; i < count; ++i) {
        for (int j = 0; words[i][j] != '\0'; ++j) {
            char c = words[i][j];
            int ci = c;
            //printf("%c %d\n", c, ci);
        }
    }
    */

    for (int i = 'a'; i < 'z'; ++i) {
        if (i % 4 == 0) printf("\n");
        printf("%3d | %c | ", i, (char) i);
    }
    printf("\n");
}

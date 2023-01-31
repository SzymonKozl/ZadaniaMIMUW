#include<stdio.h>
#include<assert.h>
#include<math.h>

#include "ary.h"


const double ___ZERO_VAL = 1e-10;


void batch1();
void batch2();
void batch3();
void batch4();
void batch5();
void batch6();
void batch7();

void special1();
void special2();
void special3();
void special4();
void special5();
void special6();

void dbg(wartosc w);

int eq(double d1, double d2);


int main(){
    batch1();
    batch2();
    batch3();
    batch4();
    batch5();
    batch6();
    batch7();

    special1();
    special2();
    special3();
    special4();
    special5();
    special6();

    return 0;
}


void batch1(){ // plus
    wartosc w1 = wartosc_od_do(-6, 10), w2 = wartosc_dokladnosc(10, 10);
    wartosc w3 = plus(w1, w2);
    assert(eq(w3.min, 3));
    assert(in_wartosc(w3, 21));
    assert(!in_wartosc(w3, 21.001));
    wartosc w4 = wartosc_dokladna(0);
    w3 = plus(w3, w4);
    assert(eq(w3.max, 21));
    assert(in_wartosc(w3, 21));
}


void batch2(){ // minus
    wartosc w1 = wartosc_od_do(-6.77, 10.1299), w2 = wartosc_dokladna(.01);
    wartosc w3 = minus(w1, w2);
    assert(eq(w3.max, 10.1199));
    assert(in_wartosc(w3, -6.78));
    assert(!in_wartosc(w3, 10.11991));
    wartosc w4 = wartosc_dokladnosc(-1, 45);
    w3 = minus(w2, w4);
    assert(eq(w3.max, 1.46));
    assert(eq(w3.min, 0.56));
    assert(in_wartosc(w3, 1.46000000000001));
    assert(!in_wartosc(w3, 0.559999));
}


void batch3(){ // razy
    wartosc w1 = wartosc_dokladna(-0.0), w2 = wartosc_dokladnosc(7.1, 12), w3 = wartosc_od_do(-0.9, 0.22), w5 = wartosc_dokladna(.0);
    wartosc w4 = razy(w2, w3);
    assert(eq(w4.min, -7.1568));
    assert(eq(w4.max, 1.74944));
    assert(in_wartosc(w4, 0));
    assert(!in_wartosc(w4, 1.7494400002));
    assert(!in_wartosc(w4, -7.15680000010001));
    assert(in_wartosc(w4, 1.7494400001));
    w4 = razy(w1, w3);
    assert(eq(w4.min, 0.0));
    assert(eq(w4.min, -0.0));
    assert(eq(w4.max, 0.0));
    assert(in_wartosc(w4, 0.));
    assert(in_wartosc(w4, -0.));
    assert(!in_wartosc(w4, 0.000001));
    assert(in_wartosc(w4, -0.0));
    w4 = razy(w2, w5);
    assert(in_wartosc(w4, 0.));
    assert(in_wartosc(w4, -0.));
}


void batch4(){ // podzielic
    wartosc w1 = wartosc_dokladna(1.0), w2 = wartosc_dokladna(0.0), w3 = wartosc_od_do(-1.0, 2.0), w4 = wartosc_dokladnosc(2, 100), w5 = wartosc_od_do(3, 4);
    wartosc w6 = podzielic(w1, w2);
    assert(!in_wartosc(w6, 0));
    assert(!in_wartosc(w6, -0.0));
    assert(isnan(w6.min));
    assert(isnan(w6.max));
    w6 = podzielic(w1, w3);
    assert(w6.dopelnienie);
    assert(in_wartosc(w6, -9999));
    assert(in_wartosc(w6, 999));
    assert(in_wartosc(w6, -1.00001));
    assert(!in_wartosc(w6, -0.9999999998));
    assert(in_wartosc(w6, -0.99999999992));
    assert(in_wartosc(w6, 0.500001));
    assert(!in_wartosc(w6, 0.4999999998));
    assert(in_wartosc(w6, 0.49999999991));
    assert(eq(w6.min, -1));
    assert(eq(w6.max, 0.5));
    w6 = podzielic(w2, w4);
    assert(eq(w6.min, 0));
    assert(eq(w6.min, -0.0));
    assert(eq(w6.max, 0));
    assert(in_wartosc(w6, 0.0));
    assert(in_wartosc(w6, -0.0));
    w6 = podzielic(w3, w4);
    assert(isinf(w6.min));
    assert(isinf(w6.max));
    w6 = podzielic(w3, w5);
    assert(eq(w6.min, -(1. / 3)));
    assert(eq(w6.max, 2. / 3));
    w6 = podzielic(w4, w3);
    assert(!w6.dopelnienie);
    assert(isinf(w6.min));
    assert(isinf(w6.max));
    w6 = podzielic(w3, w3);
    assert(!w6.dopelnienie);
    assert(isinf(w6.min));
    assert(isinf(w6.max));
    assert(isnan(w6.min + w6.max));
    w6 = podzielic(w5, w4);
    assert(isinf(w6.max));
    assert(eq(w6.min, 0.75));
    assert(in_wartosc(w6, 0.75));
    assert(!in_wartosc(w6, 0.74));
    assert(!w6.dopelnienie);
}


void batch5(){ // dodawanie z brzydkimi zakresami
    wartosc w1 = wartosc_od_do(-120, 20), w2 = wartosc_dokladna(-99), w3 = wartosc_dokladnosc(100, 10), w5 = wartosc_od_do(0, 1), w6 = wartosc_dokladna(0.);
    w1.dopelnienie = true;
    wartosc w4 = plus(w3, w1);
    assert(w4.dopelnienie);
    assert(eq(w4.min, -10.));
    assert(eq(w4.max, 110.));
    assert(!in_wartosc(w4, 0.));
    assert(in_wartosc(w4, -10.));
    wartosc w7 = podzielic(w2, w6);
    assert(isnan(w7.min));
    assert(isnan(w7.max));
    assert(!w7.dopelnienie);
    w4 = plus(w7, w1);
    assert(isnan(w4.min));
    assert(isnan(w4.max));
    assert(!w4.dopelnienie);
    w4 = plus(w3, w7);
    assert(isnan(w4.min));
    assert(isnan(w4.max));
    assert(!w4.dopelnienie);
    w4 = plus(w1, w1);
    assert(!w4.dopelnienie);
    assert(isinf(w4.min));
    assert(isinf(w4.max));
    assert(in_wartosc(w4, -9999999));
    assert(in_wartosc(w4, -0.));
    assert(in_wartosc(w4, 999999999));
    w4 = plus(w3, w6);
    assert(eq(w3.min, w4.min));
    assert(eq(w3.max, w4.max));
    assert(w3.dopelnienie == w6.dopelnienie);
    wartosc w8 = podzielic(w2, w5);
    assert(isinf(w8.min));
    assert(in_wartosc(w8, -9999999));
    assert(eq(w8.max, -99));
    assert(!w8.dopelnienie);
    w4 = plus(w8, w2);
    assert(isinf(w4.min));
    assert(in_wartosc(w4, -9999999));
    assert(eq(w4.max, -198));
}


void batch6(){ // syfne mnożenie + min, max, srednia
    wartosc w1 = wartosc_dokladna(0.), w2 = wartosc_od_do(-3, 0.23), w3 = wartosc_dokladnosc(-2, 100), w4 = wartosc_od_do(-5.0, 0), w5 = wartosc_dokladna(1.), w6, w7, w8, w9, w10 = wartosc_od_do(-11, -9.1), w11 = wartosc_dokladna(-1);
    w2.dopelnienie = true;
    w6 = podzielic(w5, w1); // empty
    w7 = podzielic(w2, w2); // R
    assert(isinf(w7.min));
    assert(isinf(w7.max));
    assert(in_wartosc(w7, .0));
    w8 = podzielic(w5, w4);
    assert(isinf(w8.min));
    assert(eq(w8.max, -0.2));
    w9 = razy(w4, w8); // R+
    assert(eq(w9.min, 0.));
    assert(isinf(w9.max));
    w9 = razy(w9, w11); // R-
    wartosc t = razy(w1, w5);
    assert(in_wartosc(t, 0));
    assert(eq(t.min, 0));
    assert(eq(t.max, 0));
    t = razy(w1, w2);
    assert(in_wartosc(t, 0));
    assert(eq(t.min, 0));
    assert(eq(t.max, 0));
    t = razy(w5, w2);
    assert(eq(t.min, -3));
    assert(eq(t.max, 0.23));
    assert(t.dopelnienie);
    assert(!in_wartosc(t, 0.22));
    t = razy(w10, w2);
    assert(eq(t.min, -2.093));
    assert(eq(t.max, 27.3));
    assert(t.dopelnienie);
    assert(isinf(min_wartosc(t)));
    assert(isinf(max_wartosc(t)));
    assert(isnan(sr_wartosc(t)));
    t = razy(w10, w10);
    assert(eq(t.min, 82.81));
    assert(eq(t.max, 121));
    assert(eq(sr_wartosc(t), 101.905));
    assert(eq(min_wartosc(t), t.min));
    assert(eq(max_wartosc(t), t.max));
    t = razy(w10, w9);
    assert(isinf(t.max));
    assert(eq(t.min, 0));
    assert(in_wartosc(t, 0));
    assert(!in_wartosc(t, -0.000001));
    assert(isinf(sr_wartosc(t)));
    assert(sr_wartosc(t) > 0);
    assert(eq(min_wartosc(t), 0));
    assert(isinf(max_wartosc(t)));
    t = razy(w6, w1);
    assert(isnan(t.min));
    assert(isnan(t.max));
    assert(!t.dopelnienie);
    assert(!in_wartosc(t, 0));
    assert(isnan(sr_wartosc(t)));
    assert(isnan(max_wartosc(t)));
    assert(isnan(min_wartosc(t)));
    t = razy(w6, w7);
    assert(isnan(t.min));
    assert(isnan(t.max));
    assert(!t.dopelnienie);
    assert(!in_wartosc(t, 0));
    assert(isnan(sr_wartosc(t)));
    assert(isnan(max_wartosc(t)));
    assert(isnan(min_wartosc(t)));
    wartosc t2 = razy(w9, w11);
    t = razy(t2, w9);
    assert(eq(t.max, 0));
    assert(isinf(t.min));
    assert(!t.dopelnienie);
    assert(isinf(sr_wartosc(t)));
    assert(isinf(min_wartosc(t)));
    assert(eq(0, max_wartosc(t)));
    t2.min = -2;
    t = razy(w9, t2);
    assert(!t.dopelnienie);
    assert(isinf(t.min));
    assert(isinf(t.max));
    assert(isnan(sr_wartosc(t)));
    assert(isinf(max_wartosc(t)));
    assert(isinf(min_wartosc(t)));
    assert(in_wartosc(t, 839.83293));
    assert(in_wartosc(t, -0.0));
    assert(in_wartosc(t, -99999999));
    t = razy(w3, w2);
    assert(!t.dopelnienie);
    assert(isinf(t.min));
    assert(isinf(t.max));
    assert(isnan(sr_wartosc(t)));
    assert(isinf(max_wartosc(t)));
    assert(isinf(min_wartosc(t)));
    assert(in_wartosc(t, 839.83293));
    assert(in_wartosc(t, -0.0));
    assert(in_wartosc(t, -99999999));
}


void batch7(){ // syfne dzielenie
    wartosc w0 = wartosc_od_do(-2, 1), w1 = wartosc_dokladna(0.), w2 = wartosc_dokladnosc(0, 10), w3 = wartosc_dokladna(1.), w4 = wartosc_od_do(-9.1, 0.), w5 = wartosc_dokladnosc(0.1, 100), w6 = wartosc_od_do(8, 9), w7 = wartosc_od_do(-7, -6);
    wartosc w8 = podzielic(w3, w0); // R - (-0.5, 1)
    assert(eq(w8.min, -0.5));
    assert(eq(w8.max, 1.));
    assert(!in_wartosc(w8, 0.99));
    assert(isinf(min_wartosc(w8)));
    assert(w8.dopelnienie);
    assert(isnan(sr_wartosc(w8)));
    wartosc w9 = plus(w8, w6); // R - (8.5, 9)
    assert(eq(w9.min, 8.5));
    assert(eq(w9.max, 9));
    assert(w9.dopelnienie);
    wartosc w10 = plus(w8, w7); // R - (-6.5 -6)
    assert(eq(w10.min, -6.5));
    assert(eq(w10.max, -6));
    assert(w10.dopelnienie);
    wartosc t = podzielic(w3, w8);
    assert(eq(t.min, -2.));
    assert(eq(t.max, 1));
    assert(!t.dopelnienie);
    assert(in_wartosc(t, 0));
    assert(eq(sr_wartosc(t), -.5));
    wartosc w11 = plus(wartosc_dokladna(fabs(w8.min)), w8); // R - (0, 1.5)
    assert(eq(w11.min, 0));
    assert(isinf(max_wartosc(w11)));
    t = podzielic(w3, w11);
    assert(isinf(t.min));
    assert(t.min < 0);
    assert(eq(t.max, 2./3));
    assert(!t.dopelnienie);
    assert(isinf(sr_wartosc(t)));
    assert(eq(max_wartosc(t), 2./3));
    wartosc w12 = razy(w11, wartosc_dokladna(-4)); // R - (-6, 0)
    assert(w12.dopelnienie);
    assert(!in_wartosc(w12, -5.999));
    assert(in_wartosc(w12, -6.0));
    assert(!in_wartosc(w12, -0.000001));
    assert(in_wartosc(w12, -1e-15));
    assert(eq(min_wartosc(w2), 0));
    assert(eq(max_wartosc(w2), 0));
    assert(!w2.dopelnienie);
    assert(eq(sr_wartosc(w2), 0));
    t = podzielic(w2, w12);
    assert(eq(min_wartosc(t), 0));
    assert(eq(max_wartosc(t), 0));
    assert(!t.dopelnienie);
    assert(eq(sr_wartosc(t), 0));
    assert(in_wartosc(t, 0));
    t = podzielic(w4, w12);
    assert(isinf(t.min));
    assert(t.min < 0);
    assert(isinf(min_wartosc(t)));
    assert(!t.dopelnienie);
    assert(eq(t.max, 1.5166666666666));
    assert(eq(max_wartosc(t), 1.5166666666666));
    assert(in_wartosc(t, 1.51));
    assert(!in_wartosc(t, 1.52));
    t = podzielic(w8, w1);
    assert(isnan(min_wartosc(t)));
    assert(isnan(max_wartosc(t)));
    assert(!in_wartosc(t, 0));
    assert(isnan(sr_wartosc(t)));
    assert(!t.dopelnienie);
    t = podzielic(w4, w5);
    assert(isinf(t.min));
    assert(min_wartosc(t) < 0);
    assert(!t.dopelnienie);
    assert(eq(max_wartosc(t), 0));
    t = podzielic(w10, w9);
    assert(!t.dopelnienie);
    assert(isinf(t.min));
    assert(isinf(max_wartosc(t)));
    assert(isnan(sr_wartosc(t)));
    t = podzielic(w10, w8);
    assert(!t.dopelnienie);
    assert(isinf(t.min));
    assert(isinf(max_wartosc(t)));
    assert(isnan(sr_wartosc(t)));
    t = podzielic(w8, w8);
    assert(!t.dopelnienie);
    assert(isinf(t.min));
    assert(isinf(max_wartosc(t)));
    assert(isnan(sr_wartosc(t)));
    t = podzielic(w1, w1);
    assert(isnan(min_wartosc(t)));
    assert(isnan(max_wartosc(t)));
    assert(!in_wartosc(t, 0.00001));
    assert(!t.dopelnienie);
}


void special1(){ // czy dodawanie zamienia liczby xd
    wartosc w1 = wartosc_od_do(12, 13);
    wartosc w2 = wartosc_od_do(2, 10);
    w2.dopelnienie = true;
    wartosc w3 = plus(w1, w2);
    assert(!w1.dopelnienie);
    assert(w2.dopelnienie);
    assert(eq(w3.min, 15));
    assert(eq(w3.max, 22));
    assert(w3.dopelnienie);
}


void special2(){
    wartosc w1 = wartosc_od_do(-1e-12, 1);
    wartosc w2 = wartosc_dokladna(1);
    wartosc w3 = podzielic(w2, w1);
    assert(eq(min_wartosc(w3), 1.));
    assert(isinf(max_wartosc(w3)));
}


void special3(){
    wartosc w2 = wartosc_dokladna(1);
    wartosc w4 = wartosc_od_do(-1, 1);
    wartosc w5 = podzielic(w2, w4); // [-inf -1]u[1, +inf]
    wartosc w6 = plus(w5, wartosc_dokladna(4.)); // [-inf 3]u[5, +inf]
    wartosc w7 = razy(w7, w6); // [-inf, inf]
    wartosc w8 = podzielic(wartosc_dokladna(0.), w7); //{0}
    assert(in_wartosc(w7, -7));
    assert(eq(min_wartosc(w8), 0));
    assert(eq(max_wartosc(w8), 0));
    assert(!in_wartosc(w8, 10));
}


void special4(){
    wartosc w1 = wartosc_od_do(-2, -1);
    w1.dopelnienie = true; // w1 = [-inf;-2]u[-1;inf]
    wartosc w2 = podzielic(wartosc_dokladna(1.), w1); // w2 = [-inf; -1]u[-0.5; inf]
    assert(in_wartosc(w2, -10));
    assert(!in_wartosc(w2, -0.75));
    assert(isinf(min_wartosc(w2)));
    assert(isinf(max_wartosc(w2)));
}


void special5(){
    wartosc w1 = wartosc_od_do(300000000., 500000000.);
    wartosc w2 = wartosc_od_do(-1e-12, 10);
    w2.dopelnienie = true;
    wartosc w3 = razy(w1, w2);
    assert(w3.dopelnienie);
    assert(eq(w3.max, 3000000000.));
}


void special6(){
    wartosc w1 = wartosc_od_do(-1, HUGE_VAL);
    wartosc w2 = wartosc_od_do(- HUGE_VAL, 0);
    wartosc w3 = razy(w1, w2); // [-inf, inf]
    assert(isinf(min_wartosc(w3)));
    assert(isinf(max_wartosc(w3)));
    assert(in_wartosc(w3, 0));
    assert(in_wartosc(w3, 10000));
    wartosc w4 = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
    wartosc w5 = wartosc_dokladna(0.);
    wartosc w6 = podzielic(w4, w5); // emptyset
    assert(!in_wartosc(w6, 0));
    assert(isnan(max_wartosc(w6)));
    assert(isnan(min_wartosc(w6)));
    wartosc w7 = podzielic(w5, w5); // emptyset
    assert(isnan(min_wartosc(w7)));
    assert(isnan(max_wartosc(w7)));
    wartosc a = wartosc_od_do(1, 2);
    wartosc b = wartosc_od_do(1, 2);
    b.dopelnienie = true;
    wartosc c = razy(a, b);
    assert(isinf(min_wartosc(c)));
    wartosc d = wartosc_dokladna(1);
    wartosc e = razy(d, b);
    assert(!in_wartosc(e, 1.5));
    wartosc x1 = {.min = 0, .max = 5, .dopelnienie = true};
    wartosc x2 = {.min = -5., .max = 0., .dopelnienie = true};
    wartosc x3 = razy(x2, x1);
    assert(isinf(min_wartosc(x3)));
    assert(isinf(max_wartosc(x3)));
    assert(in_wartosc(x3, 5.));
}


int eq(double d1, double d2){ return fabs(d1 - d2) < ___ZERO_VAL;}


void dbg(wartosc w){
    printf("%lf %lf %d\n", w.min, w.max, w.dopelnienie);
}


/*
watpliwosci:
srednia z [inf, inf]

*/
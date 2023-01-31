#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "ary.h"


const double __ZERO_VAL = 1e-10;


// pomocnicze funkcje niedostepne dla użytkownika końcowego

// sprawdza czy wśród wartości w znajduje się dodatnie otoczenie zera
bool _dod_otoczenie_zera(wartosc w);

// sprawdza czy wśród wartości w znajduje się ujemne otoczenie zera
bool _uje_otoczenie_zera(wartosc w);

// sprawdza czy w zakresie jest zero z obustronnym otoczeniem
bool _otoczenie_zera(wartosc w);

// próbuje połączyć zakres będący dopełnieniem tak aby stał sie zakresem (-HUGE_VAL, HUGE_VAL) nie będącym dopełnieniem
void _lacz(wartosc *w);

// zwraca zakres^-1
wartosc _odwrotnosc(wartosc w);

// zwraca wartośc przeciwną do wartości
wartosc _przeciwna(wartosc w);

// zamienia granice wartości z zakresu (-1e-10;1e-10) na 0. oraz łączy dopełnienie w "zwykły"
// zakres wartości gdy to możliwe
void _standaryzuj(wartosc *w);


bool _rowna_zero(double x) { return fabs(x) < __ZERO_VAL; }


bool _dodatnia(double a){ return a > 0 && !_rowna_zero(a); }


bool _ujemna(double a){ return a < 0 && !_rowna_zero(a); }


// definicje funkcji z ary.h

wartosc wartosc_dokladnosc(double x, double p){
    if (_dodatnia(x)){
        wartosc res = {.min = x - (p * x) / 100, .max = x + (p * x) / 100, .dopelnienie = false};
        return res;
    }
    if (_rowna_zero(x)) x = 0.;
    wartosc res = {.min = x + (p * x) / 100, .max = x - (p * x) / 100, .dopelnienie = false};
    _standaryzuj(&res);
    return res;
}


wartosc wartosc_od_do(double x, double y){
    wartosc res = {.min = x, .max = y, .dopelnienie = false};
    _standaryzuj(&res);
    return res;
}


wartosc wartosc_dokladna(double x){
    wartosc res = {.min = x, .max = x, .dopelnienie = false};
    _standaryzuj(&res);
    return res;
}


bool in_wartosc(wartosc w, double x){
    if (isnan(w.min)) return false;
    if (w.dopelnienie) return !_ujemna(w.min - x) || !_dodatnia(w.max - x);
    return !_dodatnia(w.min - x) && !_ujemna(w.max - x);
}


double min_wartosc(wartosc w){
    if (w.dopelnienie) return - HUGE_VAL;
    return w.min;
}


double max_wartosc(wartosc w){
    if (w.dopelnienie) return HUGE_VAL;
    return w.max;
}


double sr_wartosc(wartosc w){
    if (w.dopelnienie) return NAN;
    return (max_wartosc(w) + min_wartosc(w)) / 2;
}


wartosc plus(wartosc a, wartosc b){
    wartosc res;
    if (isnan(a.min) || isnan(b.min)){
        res.min = res.max = NAN;
        res.dopelnienie = false;
    }
    else if (a.dopelnienie ^ b.dopelnienie){
        if (b.dopelnienie){
            wartosc t = b;
            b = a;
            a = t;
        }
        if (a.min + b.max > a.max+ b.min){
            res.dopelnienie = false;
            res.min = - HUGE_VAL;
            res.max = HUGE_VAL; 
        }
        else {
            res.dopelnienie = true;
            res.min = a.min + b.max;
            res.max = a.max + b.min;
        }
    }
    else if (a.dopelnienie && b.dopelnienie){
        res.dopelnienie = false;
        res.min = - HUGE_VAL;
        res.max = HUGE_VAL;
    }
    else {
        res.dopelnienie = false;
        res.min = a.min + b.min;
        res.max = a.max + b.max;
    }
    _standaryzuj(&res);
    return res;
}


wartosc minus(wartosc a, wartosc b){
    return plus(a, _przeciwna(b));
}


wartosc razy(wartosc a, wartosc b){
    wartosc res;
    if (isnan(a.min) || isnan(b.min)){
        res.dopelnienie = false;
        res.min = res.max = NAN;
    }
    else if ((_rowna_zero(a.min) && _rowna_zero(a.max)) || (_rowna_zero(b.min) && _rowna_zero(b.max))){
        res.dopelnienie = false;
        res.min = 0;
        res.max = 0;
    }
    else if (a.dopelnienie && b.dopelnienie){
        res.dopelnienie = true;
        res.min = (_otoczenie_zera(a) || _otoczenie_zera(b)) ? 0. : MAX(a.min * b.max, a.max * b.min);
        res.max = (_otoczenie_zera(a) || _otoczenie_zera(b)) ? 0. : MIN(a.min * b.min, a.max * b.max);
    }
    else if (a.dopelnienie || b.dopelnienie){
        if (b.dopelnienie){
            wartosc t = b;
            b = a;
            a = t;
        }
        res.dopelnienie = true;
        // a jest dopelnieniem, b nie
        if (_otoczenie_zera(b)){
            res.min = -HUGE_VAL;
            res.max = HUGE_VAL;
            res.dopelnienie = false;
        }
        else if (_otoczenie_zera(a)){
            res.min = ((_ujemna(a.min)) ? b.min : b.max) * ((_ujemna(b.min)) ? a.max : a.min);
            res.max = ((_ujemna(a.min)) ? b.max : b.min) * ((_ujemna(b.min)) ? a.min : a.max);
        }
        else {
            res.min = (!_ujemna(b.min)) ? b.min * a.min : b.max * a.max;
            res.max = (!_ujemna(b.min)) ? b.min * a.max : b.max * a.min;
        }
    }
    else {
        res.dopelnienie = false;
        double x = ((isinf(a.min) && _rowna_zero(b.min)) || (isinf(b.min) && _rowna_zero(a.min))) ? 0.0 : a.min * b.min;
        double y = ((isinf(a.min) && _rowna_zero(b.max)) || (isinf(b.max) && _rowna_zero(a.min))) ? 0.0 : a.min * b.max;
        double z = ((isinf(a.max) && _rowna_zero(b.min)) || (isinf(b.min) && _rowna_zero(a.max))) ? 0.0 : a.max * b.min;
        double w = ((isinf(a.max) && _rowna_zero(b.max)) || (isinf(b.max) && _rowna_zero(a.max))) ? 0.0 : a.max * b.max;
        //printf("%lf %lf %lf %lf\n", w, x, y, z);
        res.min = MIN(MIN(x, y), MIN(w, z));
        res.max = MAX(MAX(x, y), MAX(w, z));
    }
    _standaryzuj(&res);
    return res;
}


wartosc podzielic(wartosc a, wartosc b){
    wartosc r = razy(a, _odwrotnosc(b));
    return r;
}


// definicje funkcji pomocniczych

wartosc _przeciwna(wartosc w){
    wartosc res;
    res.dopelnienie = w.dopelnienie;
    res.min = - w.max;
    res.max = - w.min;
    _standaryzuj(&res);
    return res;
}


wartosc _odwrotnosc(wartosc w){
    wartosc res;
    if (isnan(w.min)){
        res.dopelnienie = false;
        res.min = res.max = NAN;
    }
    else if (w.dopelnienie){
        if (!_otoczenie_zera(w)){
            res.dopelnienie = false;
            if (!_ujemna(w.min)) res.min = - HUGE_VAL;
            else res.min = 1. / w.min;
            if (!_dodatnia(w.max)) res.max = HUGE_VAL;
            else res.max = 1. / w.max;
        }
        else {
            res.dopelnienie = true;
            res.min = MIN(1. / w.min, 1. / w.max);
            res.max = MAX(1. / w.min, 1. / w.max);
        }
    }
    else {
        res.dopelnienie = false;
        if (_otoczenie_zera(w)){
            res.dopelnienie = true;
            res.min = 1. / w.min;
            res.max = 1. / w.max;
        }
        else if (_dod_otoczenie_zera(w)){
            res.min = 1. / w.max;
            res.max = HUGE_VAL;
        }
        else if (_uje_otoczenie_zera(w)){
            res.max = 1. / w.min;
            res.min = - HUGE_VAL;
        }
        else if (_rowna_zero(w.min) && _rowna_zero(w.max)){
            res.min = NAN;
            res.max = NAN;
            res.dopelnienie = false;
        }
        else {
            res.min = MIN(1. / w.min, 1. / w.max);
            res.max = MAX(1. / w.min, 1. / w.max);
        }
    }
    _standaryzuj(&res);
    return res;
}


void _lacz(wartosc *w){
    if (!(w -> dopelnienie)) return;
    if (_rowna_zero(w -> min - w -> max)){
        w -> dopelnienie = false;
        w -> min = - HUGE_VAL;
        w -> max = HUGE_VAL;
    }
}


bool _dod_otoczenie_zera(wartosc w){
    if (isnan(w.min)) return false;
    if (w.dopelnienie) return _dodatnia(w.min) || !_dodatnia(w.max);
    return !_dodatnia(w.min) && _dodatnia(w.max);
}


bool _uje_otoczenie_zera(wartosc w){
    if (isnan(w.min)) return false;
    if (w.dopelnienie) return !_ujemna(w.min) || _ujemna(w.max);
    return _ujemna(w.min) && !_ujemna(w.max);
}


bool _otoczenie_zera(wartosc w){
    return _dod_otoczenie_zera(w) && _uje_otoczenie_zera(w);
}


void _standaryzuj(wartosc *w){
    if (_rowna_zero(w -> min)) w -> min = 0.;
    if (_rowna_zero(w -> max)) w -> max = 0.;
    _lacz(w);
}

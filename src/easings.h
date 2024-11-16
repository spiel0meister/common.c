#ifndef EASINGS_H_
#define EASINGS_H_
#include <math.h>

#ifndef PI
#define PI 3.14159 
#endif // PI

#ifndef EASINGS_POWF
#include <math.h>
#define EASINGS_POWF powf
#endif

#ifndef EASINGS_SQRTF
#include <math.h>
#define EASINGS_SQRTF sqrtf
#endif

#ifndef EASINGS_EXP2F
#include <math.h>
#define EASINGS_EXP2F exp2f
#endif

float ease_in_sine(float t);
float ease_out_sine(float t);
float ease_in_out_sine(float t);

float ease_in_quad(float t);
float ease_out_quad(float t);
float ease_in_out_quad(float t);

float ease_in_cubic(float t);
float ease_out_cubic(float t);
float ease_in_out_cubic(float t);

float ease_in_quart(float t);
float ease_out_quart(float t);
float ease_in_out_quart(float t);

float ease_in_quint(float t);
float ease_out_quint(float t);
float ease_in_out_quint(float t);

float ease_in_expo(float t);
float ease_out_expo(float t);
float ease_in_out_expo(float t);

float ease_in_circ(float t);
float ease_out_circ(float t);
float ease_in_out_circ(float t);

float ease_in_back(float t);
float ease_out_back(float t);
float ease_in_out_back(float t);

float ease_in_elastic(float t);
float ease_out_elastic(float t);
float ease_in_out_elastic(float t);

float ease_in_bounce(float t);
float ease_out_bounce(float t);
float ease_in_out_bounce(float t);

#endif // EASINGS_H_

#ifdef EASINGS_IMPLEMENTATION
#undef EASINGS_IMPLEMENTATION

float ease_in_sine(float t) {
    return 1 - cos((t * PI) / 2);
}

float ease_out_sine(float t) {
    return sin((t * PI) / 2);
}

float ease_in_out_sine(float t) {
    return -(cos(PI * t) - 1) / 2;
}

float ease_in_quad(float t) {
    return t * t;
}

float ease_out_quad(float t) {
    return 1 - (1 - t) * (1 - t);
}

float ease_in_out_quad(float t) {
    return t < 0.5 ? 2 * t * t : 1 - EASINGS_POWF(-2 * t + 2, 2) / 2;
}

float ease_in_cubic(float t) {
    return t * t * t;
}

float ease_out_cubic(float t) {
    return 1 - EASINGS_POWF(1 - t, 3);
}

float ease_in_out_cubic(float t) {
    return t < 0.5 ? 4 * t * t * t : 1 - EASINGS_POWF(-2 * t + 2, 3) / 2;
}

float ease_in_quart(float t) {
    return t * t * t * t;
}

float ease_out_quart(float t) {
    return 1 - EASINGS_POWF(1 - t, 4);
}

float ease_in_out_quart(float t) {
    return t < 0.5 ? 8 * t * t * t * t : 1 - EASINGS_POWF(-2 * t + 2, 4) / 2;
}

float ease_in_quint(float t) {
    return t * t * t * t * t;
}

float ease_out_quint(float t) {
    return 1 - EASINGS_POWF(1 - t, 5);
}

float ease_in_out_quint(float t) {
    return t < 0.5 ? 16 * t * t * t * t * t : 1 - EASINGS_POWF(-2 * t + 2, 5) / 2;
}

float ease_in_expo(float t) {
    return t == 0.0f ? 0.0f : EASINGS_EXP2F(10 * t - 10);
}

float ease_out_expo(float t) {
    return t == 1.0f ? 1.0f : 1 - EASINGS_EXP2F(-10 * t);
}

float ease_in_out_expo(float t) {
    if (t == 0) return 0;
    else if (t == 1) return 1;
    else if (t < 0.5) return EASINGS_EXP2F(20 * t - 10) / 2;
    else return (2 - EASINGS_EXP2F(-20 * t + 10)) / 2;
}

float ease_in_circ(float t) {
    return 1 - EASINGS_SQRTF(1 - t * t);
}

float ease_out_circ(float t) {
    return EASINGS_SQRTF(1 - EASINGS_POWF(t - 1, 2));
}

float ease_in_out_circ(float t) {
    if (t < 0.5) return (1 - EASINGS_SQRTF(1 - EASINGS_POWF(2 * t, 2))) / 2;
    else return (EASINGS_SQRTF(1 - EASINGS_POWF(-2 * t + 2, 2)) + 1) / 2;
}

float ease_in_back(float t) {
    float c1 = 1.70158f;
    float c3 = c1 + 1;
    return c3 * t * t * t - c1 * t * t;
}

float ease_out_back(float t) {
    float c1 = 1.70158f;
    float c3 = c1 + 1;
    return 1 + c3 * EASINGS_POWF(t - 1, 3) + c1 * EASINGS_POWF(t - 1, 2);
}

float ease_in_out_back(float t) {
    float c1 = 1.70158f;
    float c3 = c1 + 1;

    if (t < 0.5) return (EASINGS_POWF(2 * t, 2) * ((c3 + 1) * 2 * t - c3)) / 2;
    else return (EASINGS_POWF(2 * t - 2, 2) * ((c3 + 1) * (t * 2 - 2) + c3) + 2) / 2;
}

float ease_in_elastic(float t) {
    float c1 = (2 * PI) / 3;

    return t == 0
        ? 0
        : t == 1
        ? 1
        : -EASINGS_POWF(2, 10 * t - 10) * sinf((t * 10 - 10.75) * c1);
}

float ease_out_elastic(float t) {
    float c1 = (2 * PI) / 3;

    return t == 0
        ? 0
        : t == 1
        ? 1
        : EASINGS_POWF(2, -10 * t) * sinf((t * 10 - 0.75) * c1) + 1;
}

float ease_in_out_elastic(float t) {
    float c1 = (2 * PI) / 4.5;

    return t == 0
        ? 0
        : t == 1
        ? 1
        : t < 0.5
        ? -(EASINGS_POWF(2, 20 * t - 10) * sinf((20 * t - 11.125) * c1)) / 2
        : (EASINGS_POWF(2, -20 * t + 10) * sinf((20 * t - 11.125) * c1)) / 2 + 1;
}

float ease_in_bounce(float t) {
    return 1 - ease_out_bounce(1 - t);
}

float ease_out_bounce(float t) {
    float n1 = 7.5625;
    float d1 = 2.75;

    if (t < 1 / d1) {
        return n1 * t * t;
    } else if (t < 2 / d1) {
        t -= 1.5 / d1;
        return n1 * t * t + 0.75;
    } else if (t < 2.5 / d1) {
        t -= 2.25 / d1;
        return n1 * t * t + 0.9375;
    } else {
        t -= 2.625 / d1;
        return n1 * t * t + 0.984375;
    }
}


float ease_in_out_bounce(float t) {
    return t < 0.5
        ? (1 - ease_out_bounce(1 - 2 * t)) / 2
        : (1 + ease_out_bounce(2 * t - 1)) / 2;
}

#endif // EASINGS_IMPLEMENTATION

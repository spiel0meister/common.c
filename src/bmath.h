#ifndef BMATH_H
#define BMATH_H

// Generic functions (macros)

#define lerp(t, min, max) _Generic((min), \
            int: lerpi, \
            long: lerpl, \
            float: lerpf, \
            double: lerpd \
        )(t, min, max)

#define map(t, nmin, nmax, rmin, rmax) _Generic((nmin), \
            int: mapi, \
            long: mapl, \
            float: mapf, \
            double: mapd \
        )(t, nmin, nmax, rmin, rmax)

#define gcd(a, b) _Generic((a), \
            int: gcdi, \
            long: gcdl)(a, b)

#define lcm(a, b) _Generic((a), \
            int: lcmi, \
            long: lcml)(a, b)

#define vec2_add(v1, v2) _Generic((v1), \
            vec2i: vec2i_add, \
            vec2l: vec2l_add, \
            vec2f: vec2f_add, \
            vec2d: vec2d_add \
        )(v1, v2)

#define vec2_sub(v1, v2) _Generic((v1), \
            vec2i: vec2i_sub, \
            vec2l: vec2l_sub, \
            vec2f: vec2f_sub, \
            vec2d: vec2d_sub \
        )(v1, v2)

#define vec2_mul(v1, v2) _Generic((v1), \
            vec2i: vec2i_mul, \
            vec2l: vec2l_mul, \
            vec2f: vec2f_mul, \
            vec2d: vec2d_mul \
        )(v1, v2)

#define vec2_div(v1, v2) _Generic((v1), \
            vec2i: vec2i_div, \
            vec2l: vec2l_div, \
            vec2f: vec2f_div, \
            vec2d: vec2d_div \
        )(v1, v2)

#define vec3_add(v1, v2) _Generic((v1), \
            vec3i: vec3i_add, \
            vec3l: vec3l_add, \
            vec3f: vec3f_add, \
            vec3d: vec3d_add \
        )(v1, v2)

#define vec3_sub(v1, v2) _Generic((v1), \
            vec3i: vec3i_sub, \
            vec3l: vec3l_sub, \
            vec3f: vec3f_sub, \
            vec3d: vec3d_sub \
        )(v1, v2)

#define vec3_mul(v1, v2) _Generic((v1), \
            vec3i: vec3i_mul, \
            vec3l: vec3l_mul, \
            vec3f: vec3f_mul, \
            vec3d: vec3d_mul \
        )(v1, v2)

#define vec3_div(v1, v2) _Generic((v1), \
            vec3i: vec3i_div, \
            vec3l: vec3l_div, \
            vec3f: vec3f_div, \
            vec3d: vec3d_div \
        )(v1, v2)

// Types

#define def_vec2(type, name) \
typedef struct { \
    type x, y; \
}name;

def_vec2(int, vec2i);
def_vec2(long, vec2l);
def_vec2(float, vec2f);
def_vec2(double, vec2d);

#undef def_vec2

#define def_vec3(type, name) \
typedef struct { \
    type x, y, z; \
}name;

def_vec3(int, vec3i);
def_vec3(long, vec3l);
def_vec3(float, vec3f);
def_vec3(double, vec3d);

#undef def_vec3

// Functions

int gcdi(int a, int b);
long gcdl(long a, long b);

int lcmi(int a, int b);
long lcml(long a, long b);

#define decl_lerp(type, name) \
type name(float t, type min, type max)

decl_lerp(int, lerpi);
decl_lerp(long, lerpl);
decl_lerp(float, lerpf);
decl_lerp(double, lerpd);

#undef decl_lerp

#define decl_map(type, name) \
type name(type n, type nmin, type nmax, type rmin, type rmax)

decl_map(int, mapi);
decl_map(long, mapl);
decl_map(float, mapf);
decl_map(double, mapd);

#undef decl_map

#define decl_vec_op(type, name) \
type name(type v1, type v2)

decl_vec_op(vec2i, vec2i_add);
decl_vec_op(vec2l, vec2l_add);
decl_vec_op(vec2f, vec2f_add);
decl_vec_op(vec2d, vec2d_add);
decl_vec_op(vec3i, vec3i_add);
decl_vec_op(vec3l, vec3l_add);
decl_vec_op(vec3f, vec3f_add);
decl_vec_op(vec3d, vec3d_add);

decl_vec_op(vec2i, vec2i_sub);
decl_vec_op(vec2l, vec2l_sub);
decl_vec_op(vec2f, vec2f_sub);
decl_vec_op(vec2d, vec2d_sub);
decl_vec_op(vec3i, vec3i_sub);
decl_vec_op(vec3l, vec3l_sub);
decl_vec_op(vec3f, vec3f_sub);
decl_vec_op(vec3d, vec3d_sub);

decl_vec_op(vec2i, vec2i_mul);
decl_vec_op(vec2l, vec2l_mul);
decl_vec_op(vec2f, vec2f_mul);
decl_vec_op(vec2d, vec2d_mul);
decl_vec_op(vec3i, vec3i_mul);
decl_vec_op(vec3l, vec3l_mul);
decl_vec_op(vec3f, vec3f_mul);
decl_vec_op(vec3d, vec3d_mul);

decl_vec_op(vec2i, vec2i_div);
decl_vec_op(vec2l, vec2l_div);
decl_vec_op(vec2f, vec2f_div);
decl_vec_op(vec2d, vec2d_div);
decl_vec_op(vec3i, vec3i_div);
decl_vec_op(vec3l, vec3l_div);
decl_vec_op(vec3f, vec3f_div);
decl_vec_op(vec3d, vec3d_div);

#undef decl_vec_op

#endif // BMATH_H

#ifdef BMATH_IMPLEMENTATION
#include <assert.h>
#include <stdlib.h>

#define impl_lerp(type, name) \
inline type name(float t, type min, type max) { \
    assert(max > min); \
    return min + t * (max - min); \
}

impl_lerp(int, lerpi);
impl_lerp(long, lerpl);
impl_lerp(float, lerpf);
impl_lerp(double, lerpd);

#undef impl_lerp

#define impl_map(type, name) \
inline type name(type n, type nmin, type nmax, type rmin, type rmax) { \
    assert(nmax > nmin); \
    assert(rmax > rmin); \
    float t = (float)(n - nmin) / (float)(nmax - nmin); \
    return lerp(t, rmin, rmax); \
}

impl_map(int, mapi);
impl_map(long, mapl);
impl_map(float, mapf);
impl_map(double, mapd);

#undef impl_map

#define impl_vec2_op(type, name, op) \
inline type name(type v1, type v2) { \
    return (type) { \
        v1.x op v2.x, \
        v1.y op v2.y, \
    }; \
}

impl_vec2_op(vec2i, vec2i_add, +);
impl_vec2_op(vec2l, vec2l_add, +);
impl_vec2_op(vec2f, vec2f_add, +);
impl_vec2_op(vec2d, vec2d_add, +);

impl_vec2_op(vec2i, vec2i_sub, -);
impl_vec2_op(vec2l, vec2l_sub, -);
impl_vec2_op(vec2f, vec2f_sub, -);
impl_vec2_op(vec2d, vec2d_sub, -);

impl_vec2_op(vec2i, vec2i_mul, *);
impl_vec2_op(vec2l, vec2l_mul, *);
impl_vec2_op(vec2f, vec2f_mul, *);
impl_vec2_op(vec2d, vec2d_mul, *);

impl_vec2_op(vec2i, vec2i_div, /);
impl_vec2_op(vec2l, vec2l_div, /);
impl_vec2_op(vec2f, vec2f_div, /);
impl_vec2_op(vec2d, vec2d_div, /);

#undef impl_vec2_op

#define impl_vec3_op(type, name, op) \
inline type name(type v1, type v2) { \
    return (type) { \
        v1.x op v2.x, \
        v1.y op v2.y, \
        v1.z op v2.z, \
    }; \
}

impl_vec3_op(vec3i, vec3i_add, +);
impl_vec3_op(vec3l, vec3l_add, +);
impl_vec3_op(vec3f, vec3f_add, +);
impl_vec3_op(vec3d, vec3d_add, +);

impl_vec3_op(vec3i, vec3i_sub, -);
impl_vec3_op(vec3l, vec3l_sub, -);
impl_vec3_op(vec3f, vec3f_sub, -);
impl_vec3_op(vec3d, vec3d_sub, -);

impl_vec3_op(vec3i, vec3i_mul, *);
impl_vec3_op(vec3l, vec3l_mul, *);
impl_vec3_op(vec3f, vec3f_mul, *);
impl_vec3_op(vec3d, vec3d_mul, *);

impl_vec3_op(vec3i, vec3i_div, /);
impl_vec3_op(vec3l, vec3l_div, /);
impl_vec3_op(vec3f, vec3f_div, /);
impl_vec3_op(vec3d, vec3d_div, /);

#undef impl_vec3_op

inline int gcdi(int a, int b) {
    int a_tmp = a;
    while (b != 0) {
        a_tmp = a;
        a = b;
        b = a_tmp % b;
    }
    
    return a;
}

inline long gcdl(long a, long b) {
    long a_tmp = a;
    while (b != 0) {
        a_tmp = a;
        a = b;
        b = a_tmp % b;
    }
    
    return a;
}

inline int lcmi(int a, int b) {
    if (a == 0 || b == 0) return 0;

    return abs(a * b) / gcd(a, b);
}

inline long lcml(long a, long b) {
    if (a == 0 || b == 0) return 0;

    return labs(a * b) / gcd(a, b);
}

#endif // BMATH_IMPLEMENTATION

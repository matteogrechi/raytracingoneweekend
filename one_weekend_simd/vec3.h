// See https://github.com/pelletier/vector3

#ifndef VEC3_H
#define VEC3_H

#pragma once

#include <smmintrin.h>
#include <cstdlib>
#if __APPLE__
# include <stdlib.h>
#else
# include <malloc.h>
#endif

#include <cmath>
#include <iostream>
#include "rtweekend.h"

using std::sqrt;

// Useful constant
static const __m128 SIGNMASK = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));

/**
** 16-bytes aligned memory allocation function.
** \param size Size of the memory chunk to allocate in bytes.
** \return A pointer to the newly aligned memory, or nullptr.
*/
void* malloc_simd(const size_t size);

/**
** 16-bytes aligned memory free function.
** \param v Memory pointer to free, which must have been allocated using
** malloc_simd.
*/
void free_simd(void* v);

/**
** 3D floating-point precission mathematical vector class.
*/
#ifdef __GNUC__
class __attribute__((aligned (16))) vec3
#else
_MM_ALIGN16 class vec3
#endif
{
    public:
        inline vec3() : mmvalue(_mm_setzero_ps()) {}
        inline vec3(float e0, float e1, float e2) : mmvalue(_mm_set_ps(0, e2, e1, e0)) {}
        inline vec3(__m128 m) : mmvalue(m) {}

        inline void* operator new[](size_t x) { return malloc_simd(x); }
        inline void operator delete[](void* x) { if (x) free_simd(x); }

        inline float x() const { return this->e0; }
        inline float y() const { return this->e1; }
        inline float z() const { return this->e2; }

        inline vec3 operator-() const { 
            return _mm_xor_ps(mmvalue, SIGNMASK); 
        }
        inline float operator[](int i) const {
            switch(i){
                case 0:
                    return this->e0;
                case 1:
                    return this->e1;
                case 2:
                    return this->e2;
                default:
                    return std::numeric_limits<float>::quiet_NaN();
            }
        }
        /*inline float& operator[](int i) { 
            switch(i){
                case 0:
                    return this->e0;
                case 1:
                    return this->e1;
                case 2:
                    return this->e2;
                default:
                    return std::numeric_limits<float&>::quiet_NaN();
            }
        }*/

        inline vec3& operator+=(const vec3 &v) {
            mmvalue = _mm_add_ps(mmvalue, v.mmvalue);
            return *this;
        }
        inline vec3& operator*=(const float t) {
            mmvalue = _mm_mul_ps(mmvalue, _mm_set1_ps(t));
            return *this;
        }
        inline vec3& operator/=(const float t) {
            mmvalue = _mm_div_ps(mmvalue, _mm_set1_ps(t));
            return *this;
        }

        inline float length() const {
            return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(mmvalue, mmvalue, 0x71)));
        }
        inline float length_squared() const {
            return _mm_cvtss_f32(_mm_dp_ps(mmvalue, mmvalue, 0x71));
        }
        inline vec3 normalize() const {
            return _mm_div_ps(mmvalue, _mm_sqrt_ps(_mm_dp_ps(mmvalue, mmvalue, 0xFF)));
        }
        inline vec3 qnormalize() const {
            return _mm_mul_ps(mmvalue, _mm_rsqrt_ps(_mm_dp_ps(mmvalue, mmvalue, 0xFF)));
        }

        bool near_zero() const {
            const float s = 1e-6;
            return (fabs(e0) < s) && (fabs(e1) < s) && (fabs(e2) < s);
        }

        inline static vec3 random() {
            return vec3(random_float(), random_float(), random_float());
        }

        inline static vec3 random(float min, float max) {
            return vec3(random_float(min,max), random_float(min,max), random_float(min,max));
        }

    public:
        union {
            struct { float e0, e1, e2; };
            __m128 mmvalue;
        };
};

// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color

// vec3 Utility Functions
inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    return out << v.e0 << ' ' << v.e1 << ' ' << v.e2;
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return _mm_add_ps(u.mmvalue, v.mmvalue);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return _mm_sub_ps(u.mmvalue, v.mmvalue);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return _mm_mul_ps(u.mmvalue, v.mmvalue);
}

inline vec3 operator*(float t, const vec3 &v) {
    return _mm_mul_ps(v.mmvalue, _mm_set1_ps(t));
}

inline vec3 operator*(const vec3 &v, float t) {
    return t * v;
}

inline vec3 operator/(vec3 v, float t) {
    return (1/t) * v;
}

inline float dot(const vec3 &u, const vec3 &v) {
    return _mm_cvtss_f32(_mm_dp_ps(u.mmvalue, v.mmvalue, 0x71));
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return _mm_sub_ps(
                _mm_mul_ps(_mm_shuffle_ps(u.mmvalue, u.mmvalue, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(v.mmvalue, v.mmvalue, _MM_SHUFFLE(3, 1, 0, 2))),
                _mm_mul_ps(_mm_shuffle_ps(u.mmvalue, u.mmvalue, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(v.mmvalue, v.mmvalue, _MM_SHUFFLE(3, 0, 2, 1)))
            );
}

inline float det(const vec3 &a, const vec3 &b, const vec3 &c) {
    return dot(a,cross(b,c));
}

vec3 random_in_unit_sphere() {
    while(true) {
        vec3 p = vec3::random(-1,1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_float(-1,1), random_float(-1,1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

vec3 random_unit_vector() {
    return random_in_unit_sphere().qnormalize();
}

vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // Same emisphere as normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2.0 * dot(v,n) * n;
}

vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat) {
    float cos_theta = fmin(dot(-uv,n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

void* malloc_simd(const size_t size) {
#if defined WIN32
    return _aligned_malloc(size, 16);
#elif defined __linux__
    return memalign(16, size);
#elif defined __MACH__
    return malloc(size);
#else // use page-aligned memory for other systems
    return valloc(size);
#endif
}

void free_simd(void* v) {
#if defined WIN32
    return _aligned_free(v);
#else
    return free(v);
#endif
}

#endif
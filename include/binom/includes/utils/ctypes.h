#ifndef BINOM_C_TYPES_H
#define BINOM_C_TYPES_H

#include <cstdint>

#define elif else if

namespace binom {

typedef uint8_t ui8;
typedef int8_t i8;
typedef uint16_t ui16;
typedef int16_t i16;
typedef uint32_t ui32;
typedef int32_t i32;
typedef uint64_t ui64;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef float f_t;
typedef double lf_t;
typedef long double llf_t;

typedef int i_t;
typedef unsigned int ui_t;
typedef short s_t;
typedef unsigned short us_t;
typedef long l_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;

typedef ui8 byte;

constexpr ui8 operator "" _ui8 (ull_t val) {return val;}
constexpr i8 operator "" _i8 (ull_t val) {return val;}
constexpr ui16 operator "" _ui16 (ull_t val) {return val;}
constexpr i16 operator "" _i16 (ull_t val) {return val;}
constexpr ui32 operator "" _ui32 (ull_t val) {return val;}
constexpr i32 operator "" _i32 (ull_t val) {return val;}
constexpr ui64 operator "" _ui64 (ull_t val) {return val;}
constexpr i64 operator "" _i64 (ull_t val) {return val;}

constexpr f32 operator "" _f32 (llf_t val) {return val;}
constexpr f64 operator "" _f64 (llf_t val) {return val;}
constexpr llf_t operator ""_llf (llf_t val) {return val;}

constexpr i_t operator "" _i (ull_t val) {return val;}
constexpr ui_t operator "" _ui (ull_t val) {return val;}
constexpr s_t operator "" _s (ull_t val) {return val;}
constexpr us_t operator "" _us (ull_t val) {return val;}
constexpr l_t operator "" _l (ull_t val) {return val;}
constexpr ul_t operator "" _ul (ull_t val) {return val;}
constexpr ll_t operator "" _ll (ull_t val) {return val;}
constexpr ull_t operator "" _ull (ull_t val) {return val;}

}

#endif

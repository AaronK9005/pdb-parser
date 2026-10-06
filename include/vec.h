#pragma once

#include <math.h>

typedef union vec3f {
    struct {
        double x, y, z;
    };
    double v[3];
} vec3;

typedef vec3 vec;

// adds two vectors
static vec3 vec3_add(vec3 v1, vec3 v2)
{
    return (vec3){
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
        .z = v1.z + v2.z
    };
}

// returns vector from v1 to v2
static vec3 vec3_dif(vec3 v1, vec3 v2)
{
    return (vec3){
        .x = v2.x - v1.x,
        .y = v2.y - v1.y,
        .z = v2.z - v1.z
    };
}

// divide all parts with a scalar
static vec3 vec3_div(vec3 v, double scalar)
{
    //? if (scalar == 0) 

    return (vec3){
        .x = v.x / scalar,
        .y = v.y / scalar,
        .z = v.z / scalar
    };
}

static double vec3_norm(vec3 v)
{
    return sqrt(
        v.x * v.x +
        v.y * v.y +
        v.z * v.z
    );
}
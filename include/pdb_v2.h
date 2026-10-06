#pragma once

#include "vec.h"

typedef enum chlorophyl_type {
    CHLOROPHYL_NONE = 0,
    CLA = 1,
    CHL = 2,
    CL0 = 4,
    CHLOROPHYL_AB = CLA | CHL,
    CHLOROPHYL_ANY = CLA | CHL | CL0,

    CHLOROPHYL_A = CLA,
    CHLOROPHYL_B = CHL,
    CHLOROPHYL_ISOMER = CL0
} chloph_type_e;

typedef enum nitrogen_type {
    NITROGEN_NONE,
    NITROGEN_A,
    NITROGEN_B,
    NITROGEN_C,
    NITROGEN_D
} nitro_type_e;

typedef struct _nitrogen {
    char name[4];
    char resName[4];
    int serial;
    int resSeq;
    union {
        struct {
            struct {
                double x, y, z;
            };
            double pos[3]; // double[3]
        };
        vec3 position; // vec3
    };
} nitrogen_t;

enum parse_retval {
    PARSE_SUCCESS = 0,
    PARSE_ERR_NOFILE,
    PARSE_ERR_NOENTRY,
    PARSE_ERR_FGETS,
    PARSE_ERR_NONEXT
};

enum parse_retval get_next_nitrogen(void * file, nitrogen_t* nit);

typedef struct _chlorophyl {
    chloph_type_e type;
    int serial;
    vec3 center;
    vec3 Qx, Qy;
} chlorophyl_t;
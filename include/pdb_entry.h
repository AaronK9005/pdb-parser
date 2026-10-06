#pragma once

#include <stddef.h>

#include "vec.h"

#pragma region ATOM

#define PDB_ATOM_RECOR_NAME_LEN 6
#define PDB_ATOM_ATOM_NAME_LEN 4
#define PDB_ATOM_RES_NAME_LEN 3
#define PDB_ATOM_ELEMENT_LEN 2
#define PDB_ATOM_CHARGE_LEN 2

typedef struct _pdb_atom {
    char recordName[PDB_ATOM_RECOR_NAME_LEN];
    int serial;
    char atomName[PDB_ATOM_ATOM_NAME_LEN];
    char altLoc;
    char resName[PDB_ATOM_RES_NAME_LEN];
    char chainID;
    int resSeq;
    char iCode;
    union {
        struct {
            double x, y, z;
        };
        double pos[3];
    };
    double occupancy;
    double tempFactor;
    char element[PDB_ATOM_ELEMENT_LEN];
    char charge[PDB_ATOM_CHARGE_LEN];
} pdb_atom_t;

#pragma endregion

#pragma region MODEL

#define PDB_MODEL_RECORD_NAME_LEN 6

typedef struct _pdb_model {
    char recordName[PDB_MODEL_RECORD_NAME_LEN];
    int serial;
} pdb_model_t;

#pragma endregion

#pragma region ANISOU

// ANISOU i don't need it rn
typedef struct _pdb_anisou {
    int _;
} pdb_anisou_t;

#pragma endregion

#pragma region TER

// TER i don't need it rn
typedef struct _pdb_ter {
    int _;
} pdb_ter_t;

#pragma endregion

#pragma region HETATM

#define PDB_HETATM_RECOR_NAME_LEN 6
#define PDB_HETATM_ATOM_NAME_LEN 4
#define PDB_HETATM_RES_NAME_LEN 3
#define PDB_HETATM_ELEMENT_LEN 2
#define PDB_HETATM_CHARGE_LEN 2

typedef struct _pdb_hetatm {
    char recordName[PDB_ATOM_RECOR_NAME_LEN];
    int serial;
    char atomName[PDB_ATOM_ATOM_NAME_LEN];
    char altLoc;
    char resName[PDB_ATOM_RES_NAME_LEN];
    char chainID;
    int resSeq;
    char iCode;
    union {
        struct {
            double x, y, z;
        };
        double pos[3];
    };
    double occupancy;
    double tempFactor;
    char element[PDB_ATOM_ELEMENT_LEN];
    char charge[PDB_ATOM_CHARGE_LEN];
} pdb_hetatm_t;

#pragma endregion

#pragma region ENDMDL

#define PDB_ENDMDL_RECORD_NAME_LEN 6

typedef struct _pdb_endmdl {
    char recordName[PDB_ENDMDL_RECORD_NAME_LEN];
} pdb_endmdl_t;

#pragma endregion

#pragma region entry

enum pdb_type {
    PDB_TYPE_NONE = 0,
    PDB_TYPE_MODEL,
    PDB_TYPE_ATOM,
    PDB_TYPE_ANISOU,
    PDB_TYPE_TER,
    PDB_TYPE_ENDMDL
};

typedef struct _pdb_entry {
    enum pdb_type type;
    union {
        pdb_model_t model;
        pdb_atom_t atom;
        // pdb_anisou_t anisou;
        // pdb_ter_t ter;
        pdb_endmdl_t endmdl;
    };
} pdb_entry_t;

enum parse_retval {
    PARSE_SUCCESS = 0,
    PARSE_ERR_NOFILE,
    PARSE_ERR_NOENTRY,
    PARSE_ERR_FGETS,
    PARSE_ERR_UNKNOWNREC
};

enum parse_retval parse_n_entries(void * file, pdb_entry_t* entry, size_t n);

#pragma endregion

#pragma region chlorophyls

enum chlorophyl_type {
    CHLOROPHYL_NONE = 0,
    CLA = 1,
    CHL = 2,
    CL0 = 4,
    CHLOROPHYL_AB = CLA | CHL,
    CHLOROPHYL_ANY = CLA | CHL | CL0,

    CHLOROPHYL_A = CLA,
    CHLOROPHYL_B = CHL,
    CHLOROPHYL_ISOMER = CL0
};

typedef struct _chlorophyl {
    enum chlorophyl_type type;
    vec3 center, Qx, Qy;
    // union {
    //     struct {
    //         double center_x, center_y, center_z;
    //     };
    //     double center_pos[3];
    // };
    // union {
    //     struct {
    //         double Qx_x, Qx_y, Qx_z;
    //     };
    //     double Qx_pos[3];
    // };
    // union {
    //     struct {
    //         double Qy_x, Qy_y, Qy_z;
    //     };
    //     double Qy_pos[3];
    // };
} chlorophyl_t;

extern const ssize_t CHLS_ALL; // all chlorphyls in file

const char* get_chlorophyl_name(enum chlorophyl_type type);

chlorophyl_t *get_next_chlorophyls(void * file, chlorophyl_t* chls, ssize_t n, enum chlorophyl_type type);

#pragma endregion
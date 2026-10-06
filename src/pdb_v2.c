#include "pdb_v2.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_LEN 128
#define SERIAL_IDX 6
#define ATOM1_NAME_IDX 13
#define ATOM2_NAME_IDX 12
#define RESIDUE_IDX 22

#define RES_NAME_IDX 17

enum parse_retval get_next_nitrogen(void * file, nitrogen_t* nit)
{
    if (!file) return PARSE_ERR_NOFILE;
    if (!nit) return PARSE_ERR_NOENTRY;

    char buffer[BUFFER_LEN];
    memset(buffer, 0, sizeof(buffer));

    while(1)
    {
        if (fgets(buffer, sizeof(buffer), file))
        {
            if (buffer[ATOM1_NAME_IDX] != 'N') continue;

            //* parse nitrogen

            // serial
            sscanf(&buffer[SERIAL_IDX], "%d", &nit->serial);

            // atom name
            sscanf(&buffer[ATOM1_NAME_IDX], "%3s", nit->name); // sanitize_name( memcpy(&nit->name, &buffer[ATOM1_NAME_IDX], 3) );

            // residue name
            sscanf(&buffer[RES_NAME_IDX], "%3s", nit->resName); // sanitize_name( memcpy(&nit->resName, &buffer[RES_NAME_IDX], 3) );

            // res seq, position
            sscanf(&buffer[RESIDUE_IDX], "%4d %8lf%8lf%8lf",
                &nit->resSeq, &nit->x, &nit->y, &nit->z
            );

            return PARSE_SUCCESS;
        }
        else
        {
            if (feof(file)) {   return PARSE_ERR_NONEXT; } else 
            if (ferror(file)) { return PARSE_ERR_FGETS; }
        }
    }

    return PARSE_SUCCESS;
}




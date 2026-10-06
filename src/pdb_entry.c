#include "pdb_entry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_LEN 128
#define when break; case

const ssize_t CHLS_ALL = -1; // all chlorphyls in file

#pragma region parse parts

// todo: needs length of current line in buffer maybe ?
enum parse_retval parse_A(char buffer[BUFFER_LEN], pdb_entry_t * entry)
{
    if (!entry)
    {
        return PARSE_ERR_NOENTRY;
    }

    if (strncmp(buffer, "ATOM", 4) == 0)
    {
        /*
        // type
        entry->type = PDB_TYPE_ATOM;

        // record name
        strncpy(entry->atom.recordName, buffer, 4);
        entry->atom.recordName[4] = '\0';

        // serial
        buffer[11] = '\0';
        entry->atom.serial = atoi(&buffer[6]);

        // atom name
        strncpy(entry->atom.atomName, &buffer[12], PDB_ATOM_ATOM_NAME_LEN);

        // alt loc
        entry->atom.altLoc = buffer[16];

        // residue name
        strncpy(entry->atom.resName, &buffer[17], PDB_ATOM_RES_NAME_LEN);

        // chainID
        entry->atom.chainID = buffer[21];

        // iCode
        entry->atom.iCode = buffer[26];

        // element symbol
        strncpy(entry->atom.element, &buffer[76], PDB_ATOM_ELEMENT_LEN);

        // charge
        if (buffer[79] != '\n')// && buffer[79] != EOF)
        {
            entry->atom.charge[0] = buffer[79];
        }
        if (buffer[80] != '\n')
        {
            entry->atom.charge[1] = buffer[80];
        }

        //* need to parse from back so i can insert '\0' where needed

        // tempFactor
        buffer[66] = '\0';
        entry->atom.tempFactor = atof(&buffer[60]);

        // occupancy
        buffer[60] = '\0';
        entry->atom.occupancy = atof(&buffer[54]);

        // z
        buffer[54] = '\0';
        entry->atom.z = atof(&buffer[46]);

        // y
        buffer[46] = '\0';
        entry->atom.y = atof(&buffer[38]);

        // x
        buffer[38] = '\0'; 
        entry->atom.x = atof(&buffer[30]);

        // residue seq
        buffer[26] = '\0';
        entry->atom.resSeq = atoi(&buffer[22]);

        return PARSE_SUCCESS;
        */

        sscanf(buffer, "%6s%5d%4s%c%3s%c%4d%c%8lf%8lf%8lf%6lf%6lf %2s",
            entry->atom.recordName,
            entry->atom.serial,
            entry->atom.atomName,
            entry->atom.altLoc,
            entry->atom.resName,
            entry->atom.chainID,
            entry->atom.resSeq,
            entry->atom.iCode,
            entry->atom.x,
            entry->atom.y,
            entry->atom.z,
            entry->atom.occupancy,
            entry->atom.tempFactor,
            entry->atom.element
            // entry->atom.charge
        );
    }
    else if (strncmp(buffer, "ANISOU", 6) == 0)
    {
        return PARSE_SUCCESS;
    }
    
    return PARSE_ERR_UNKNOWNREC;
}

enum parse_retval parse_E(char buffer[BUFFER_LEN], pdb_entry_t * entry)
{
    if (strncmp(buffer, "ENDMDL", 6) == 0)
    {
        return PARSE_SUCCESS;
    }

    return PARSE_ERR_UNKNOWNREC;
}

enum parse_retval parse_M(char buffer[BUFFER_LEN], pdb_entry_t * entry)
{
    if (strncmp(buffer, "MODEL", 5) == 0)
    {
        return PARSE_SUCCESS;
    }

    return PARSE_ERR_UNKNOWNREC;
}

enum parse_retval parse_T(char buffer[BUFFER_LEN], pdb_entry_t * entry)
{
    if (strncmp(buffer, "TER", 3) == 0)
    {
        return PARSE_SUCCESS;
    }

    return PARSE_ERR_UNKNOWNREC;
}

#pragma endregion

enum parse_retval parse_n_entries(void * file, pdb_entry_t* entry, size_t n)
{
    if (!file)
    {
        fprintf(stderr, "parse_entry: Error - missing file\n");
        return PARSE_ERR_NOFILE;
    }
    
    if (!entry)
    {
        fprintf(stderr, "parse_entry: Error - missing entry\n");
        return PARSE_ERR_NOENTRY;
    }

    char buffer[BUFFER_LEN];
    int line = 1;

    for (size_t i = 0; i < n; i++)
    {
        if (fgets(buffer, sizeof(buffer), file) != NULL)
        {
            switch (buffer[0])
            {
            case 'A':
                if (parse_A(buffer, entry) != PARSE_SUCCESS)
                {
                    goto unknown_record;
                }
            when 'E':
                if (parse_E(buffer, entry) != PARSE_SUCCESS)
                {
                    goto unknown_record;
                }
            when 'M':
                if (parse_M(buffer, entry) != PARSE_SUCCESS)
                {
                    goto unknown_record;
                }
            when 'T':
                if (parse_T(buffer, entry) != PARSE_SUCCESS)
                {
                    goto unknown_record;
                }
                break;
            default:
                unknown_record:
                for (int i = 0; i < sizeof(buffer); i++)
                {
                    if (isspace(buffer[i]))
                    {
                        buffer[i] = '\0';
                        break;
                    }
                }
                fprintf(stderr, "parse_entry: Error - unknown record name '%s'\n", buffer);
                return PARSE_ERR_UNKNOWNREC;
            }

            line++;
        }
        else if (ferror(file))
        {
            fprintf(stderr, "parse_entry: Error - failed to read line %d\n", line);
            return PARSE_ERR_FGETS;
        }
        else break;
    }

    return PARSE_SUCCESS;
}

const char* get_chlorophyl_name(enum chlorophyl_type type)
{
    switch (type)
    {
    case CHLOROPHYL_A: return "CLA";
    case CHLOROPHYL_B: return "CHL";
    case CHLOROPHYL_ISOMER: return "CL0";
    case CHLOROPHYL_AB: return "CAB";
    case CHLOROPHYL_ANY: return "ALL";
    default: return NULL;
    }
}

chlorophyl_t *get_chlorophyl(void * file, chlorophyl_t* chlorophyl, enum chlorophyl_type type)
{
    if (
        !file || 
        !chlorophyl ||
        type <= CHLOROPHYL_NONE ||
        type > CHLOROPHYL_ANY
    ) return NULL;

    vec nitrogens[4] = { 0 };
    int nitro_found = 0;
    // int residueSeq = 0;

    char buffer[BUFFER_LEN] = { 0 };

    const int cmp_seq_len = 10;
    char cmpSeq[cmp_seq_len];
        cmpSeq[0] = '\0';

    while (nitro_found != 4 && fgets(buffer, sizeof(buffer), file))
    {
        if (strncmp(&buffer[17], "CLA", 3) == 0 && (type & CHLOROPHYL_A))
        {
            if (buffer[13] != 'N') continue;

            nitro_found++;
            int nit_idx = buffer[14] - 'A'; //  N - A/B/C/D -> 0..3

            if (cmpSeq[0] == '\0')
            {
                memcpy(cmpSeq, &buffer[17], 10);
                cmpSeq[cmp_seq_len - 1] = '\0';
            }
            else if (memcmp(cmpSeq, &buffer[17], 10) != 0)
            {
                // not equal
            }

            // get position

            // z
            buffer[54] = '\0';
            nitrogens[nit_idx].z = atof(&buffer[46]);

            // y
            buffer[46] = '\0';
            nitrogens[nit_idx].y = atof(&buffer[38]);

            // x
            buffer[38] = '\0'; 
            nitrogens[nit_idx].x = atof(&buffer[30]);

        }
        else if (strncmp(&buffer[17], "CHL", 3) == 0 && (type & CHLOROPHYL_B))
        {

        }
        else if (strncmp(&buffer[17], "CL0", 3) == 0 && (type & CHLOROPHYL_ISOMER))
        {

        }
    }

    return chlorophyl;
}

chlorophyl_t *get_next_chlorophyls(void * file, chlorophyl_t* chls, ssize_t n, enum chlorophyl_type type)
{
    if (
        !file || 
        !chls ||
        n < CHLS_ALL ||
        type <= CHLOROPHYL_NONE ||
        type > CHLOROPHYL_ANY
    ) return NULL;

    char buffer[BUFFER_LEN];
    int index = 0;
    int line = 1;

    if (n == CHLS_ALL)
    {
        n = SSIZE_MAX;
    }

    for (ssize_t i = 0; i < n; i++)
    {
        vec nitrogens[4] = { 0 };
        int nitro_found = 0;
        int residueSeq = 0;

        while (nitro_found != 4 && fgets(buffer, sizeof(buffer), file))
        {
            if (strncmp(&buffer[17], "CLA", 3) == 0 && (type & CHLOROPHYL_A))
            {
                if (buffer[13] != 'N') continue;

                nitro_found++;
                int ntype = buffer[14] - 'A'; //  N - A/B/C/D -> 0..3

                // check matching resSeq
                if (residueSeq == 0)
                {
                    buffer[26] = '\0';
                    residueSeq = atoi(&buffer[22]);
                }
                else
                {
                    // check if same resSeq
                }

                // get position



            }
            else if (strncmp(&buffer[17], "CHL", 3) == 0 && (type & CHLOROPHYL_B))
            {

            }
            else if (strncmp(&buffer[17], "CL0", 3) == 0 && (type & CHLOROPHYL_ISOMER))
            {

            }
        }
        
        
    }

    return chls;
}



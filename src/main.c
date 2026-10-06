#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>

#include <float.h>

// #include "pdb_entry.h"
#include "pdb_v2.h"

int main(int argc, char* argv[])
{
    #pragma region start
    if (argc < 2)
    {
        printf("Error: Missing file name\n");
        return 1;
    }

    const char* filename = NULL;

    #pragma region |> parse args <|
    bool show_serial_number = false;
    {
        int opt;
        while ((opt = getopt(argc, argv, "f:s")) !=  -1)
        {
            switch (opt)
            {
            case 'f':
                filename = optarg;
                break;
            case 's':
                show_serial_number = true;
                break;
            }
        }
    }
    #pragma endregion


    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Unable to open file '%s'\n", filename);
        return 1;
    }
    #pragma endregion

    int nit_count = 0;
    const int size = 10000;
    nitrogen_t nits[size];
    memset(nits, 0, sizeof(nits));

    bool exit_loop = false;

    for (int i = 0; i < size; i++)
    {
        if (exit_loop) break;

        switch ( get_next_nitrogen(file, &nits[i]) )
        {
        case PARSE_SUCCESS:
            nit_count++;
            continue;
        case PARSE_ERR_NONEXT:
            exit_loop = true;
            break;
        default:
            printf("Error while getting nitrogen number %d\n", i + 1);
            exit_loop = true;
            break;            
        }
    }

    const int chsize = 200;
    chlorophyl_t ch[chsize];
    memset(ch, 0, sizeof(ch));

    int nit_idx = 0;
    int ch_count = 0;

    bool exit_chlorophyl_loop = false;

    // make chlorophyls from nitrogens
    for (int i = 0; i < chsize; i++)
    {
        //* printf("log: checking if nitrogen %d is part of chlorophyl\n", i + 1);

        if (exit_chlorophyl_loop) break;

        chloph_type_e chType = CHLOROPHYL_NONE;

        // find next chlorophyl's nitrogen
        while (1)
        {
            //* printf("nitrogen[%d].resName = %s\n", nit_idx, nits[nit_idx].resName);

            if (nit_idx == nit_count)
            {
                exit_chlorophyl_loop = true;
                break;
            }

            if (strncmp(nits[nit_idx].resName, "CLA", 3) == 0)
            {
                chType = CHLOROPHYL_A;
                break;
            } else
            if (strncmp(nits[nit_idx].resName, "CHL", 3) == 0)
            {
                chType = CHLOROPHYL_B;
                break;
            } else
            if (strncmp(nits[nit_idx].resName, "CL0", 3) == 0)
            {
                chType = CHLOROPHYL_ISOMER;
                break;
            } else
            {
                nit_idx++;
            } /*
            {
                if (nit_idx + 1 < nit_count) nit_idx++;
                else if (nit_count >= 4)
                {
                    break; //? infinite ?
                }
                else
                {
                    printf("Error: none nitrogens in chlorophyls found\n");
                    if (fclose(file) != 0)
                    {
                        printf("Warning: difficulties closing file '%s'\n", filename);
                        return 1;
                    }
                    return 1;
                }
            }*/
        }
        
        // storage for nitrogens A([0]), B([1]), C(...), D(...)
        int nit_abcd[4] = { -1, -1, -1, -1 };

        // chlorophyl only has 4 nitrogens which will be the next four
        for (int a = 0; a < 4; a++)
        {
            // index into nit_abcd
            const int type_idx = nits[nit_idx].name[1] - 'A';
            nit_abcd[type_idx] = nit_idx;

            nit_idx++;
        }

        vec3 qx = vec3_dif(nits[nit_abcd[0]].position, nits[nit_abcd[1]].position);
        vec3 qy = vec3_dif(nits[nit_abcd[2]].position, nits[nit_abcd[3]].position);

        ch[i] = (chlorophyl_t){
            .type = chType,
            .serial = nits[nit_abcd[0]].serial,
            .Qx = qx,
            .Qy = qy,
            .center = vec3_div(
                vec3_add(
                    vec3_add(
                        nits[nit_abcd[0]].position,
                        nits[nit_abcd[1]].position
                    ),
                    vec3_add(
                        nits[nit_abcd[2]].position,
                        nits[nit_abcd[3]].position
                    )
                ),
                4.0)
        };

        ch_count++;
    }

    printf("Found %d chlorophyls (max %d)\n", ch_count, chsize);

    double smallest_distance = DBL_MAX;
    double largetst_distance = 0.0;

    // arrays for storing indexes
    int sm_dist[2] = { 0, 0 };
    int lr_dist[2] = { 0, 0 };

    for (int i = 0; i < ch_count; i++)
    {
        puts("------------------------------------------------------------------");
        for (int j = i; j < ch_count; j++)
        {
            if (i == j) continue;

            double distance = vec3_norm( vec3_dif(ch[i].center, ch[j].center));

            if (distance < smallest_distance)
            {
                smallest_distance = distance;
                sm_dist[0] = i;
                sm_dist[1] = j;
            }
            
            if (distance > largetst_distance)
            {
                largetst_distance = distance;
                lr_dist[0] = i;
                lr_dist[1] = j;
            }

            if (show_serial_number)
            {
                printf("distance of chlorophyls %3d (ser: %3d) -> %3d (ser: %3d) is: %6.3lf angstrems\n", 
                    i + 1, ch[i].serial, j + 1, ch[j].serial, distance);
            }
            else {
                printf("distance of chlorophyls %3d -> %3d is: %6.3lf angstrems\n", i + 1, j + 1, distance);
            }
        }
        
    }

    printf("Number of chlorophyls: %d\n", ch_count);
    printf("Closest: %2d -> %2d: %6.3f angstrems\n", sm_dist[0] + 1, sm_dist[1] + 1, smallest_distance);
    printf("Closest: %2d -> %2d: %6.3f angstrems\n", lr_dist[0] + 1, lr_dist[1] + 1, largetst_distance);
    
    /*
    const int parse_count = 10;
    pdb_entry_t entry[parse_count] = {};

    if (parse_n_entries(file, entry, parse_count) != PARSE_SUCCESS)
    {
        printf("Error: Failed to parse entry\n");
        if (fclose(file) != 0)
        {
            printf("Warning: difficulties closing file '%s'\n", filename);
        }
        return 1;
    }

    for (int i = 0; i < parse_count; i++)
    {
        printf("Entry[%d]: %.*s %.*s %.3f %.3f %.3f\n",
            PDB_ATOM_ELEMENT_LEN, entry[i].atom.element,
            PDB_ATOM_RES_NAME_LEN, entry[i].atom.resName,
            entry[i].atom.x, entry[i].atom.y, entry[i].atom.z
        );
    }
        */

    #pragma region end
    if (fclose(file) != 0)
    {
        printf("Warning: difficulties closing file '%s'\n", filename);
        return 1;
    }
    #pragma endregion

    return 0;
}
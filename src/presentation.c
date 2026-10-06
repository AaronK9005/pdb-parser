#include "pdb_v2.h"
#include "vec.h"

#include <stdio.h>
#include <math.h>

static int max(int x, int y)
{
    return x > y ? x : y;
}

static int min(int x, int y)
{
    return x < y ? x : y;
}

void presentation_table(chlorophyl_t* chData, int ch_count, int seg_width, int seg_hight)
{
    /*
    if (
        !chData ||
        ch_count <= 0 ||
        seg_width <= 0 ||
        seg_hight <= 0
    )
    {
        printf("Cannot presentate: invalid params(chData = %p, ch_count = %d, seg_width = %d, seg_height = %d)\n",
            (void *)chData, ch_count, seg_width, seg_hight
        );
        return;
    }

    const int segment_size = seg_width * seg_hight;
    const int segment_count = (ch_count + segment_count - 1) / segment_count;

    for (int sc = 0; sc < segment_count; sc++)
    {
        printf("table: %d\n", sc + 1);

        const int format_width = 6;

        for (int i = 0; i < format_width; i++)
        {
            putchar(' ');
        }
        
        for (int i = 0; i < seg_width; i++)
        {
            printf(" %*.2d", i + 1);
        }
        putchar('\n');

        for (int i = 0; i < seg_hight; i++) {
            for (int j = 0; j < seg_width; j++)
            {

            }
        }
    }
    */
}





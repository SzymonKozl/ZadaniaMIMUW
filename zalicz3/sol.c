#include "geometry.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main()
{
    int N, q;
    assert(scanf("%d %d", &N, &q));
    getchar();
    Shape_S *figures = (Shape_S *)malloc((unsigned)N * sizeof(Shape_S));
    char cmd;
    unsigned index;
    double x1, x2, y1, y2, x, y, r;
    for (int i = 0; i < N; i++)
    {
        assert(scanf("%c", &cmd));
        switch (cmd)
        {
        case 'P':
            assert(scanf("%lf %lf %lf %lf", &x1, &y1, &x2, &y2));
            make_rect(figures + i, x1, x2, y1, y2);
            break;
        case 'Z':
            assert(scanf("%u %lf %lf %lf %lf", &index, &x1, &y1, &x2, &y2));
            cpy(figures + i, figures + index - 1);
            fold(figures + i, x1, x2, y1, y2);
            break;
        case 'K':
            assert(scanf("%lf %lf %lf", &x, &y, &r));
            make_circ(figures + i, x, y, r);
            break;
        default:
            break;
        }
        getchar();
    }
    for (int i = 0; i < q; i++)
    {
        assert(scanf("%u %lf %lf", &index, &x, &y));
        getchar();
        printf("%i\n", count_layers(figures + index - 1, x, y));
    }
    // memory cleanup
    for (int i = 0; i < N; i++)
        destroy(figures + i);
    free(figures);
    return 0;
}

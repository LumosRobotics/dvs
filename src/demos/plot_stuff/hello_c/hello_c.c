
#include "duoplot/duoplot.h"

int main()
{
    const size_t num_rows = 100, num_cols = 100;
    const duoplot_MatrixPairD mp = duoplot_meshgridD(0.0, 4.0, 1.0, 5.0, num_cols, num_rows);

    const duoplot_MatrixD x = mp.m0, y = mp.m1;

    const duoplot_MatrixD rx = duoplot_Matrix_sinD(x);
    const duoplot_MatrixD ry = duoplot_Matrix_cosD(y);
    const duoplot_MatrixD z = duoplot_Matrix_elementWiseMultiplyD(rx, ry);

    duoplot_setActiveView("hello_c");
    duoplot_clearView();

    duoplot_surf(x, y, z, duoplot_ColorMap_JET_SOFT, duoplot_Alpha(0.7));
    duoplot_setTitle("Hello from C!");
    duoplot_view(-128, 23);

    free(x.data);
    free(y.data);
    free(z.data);

    free(rx.data);
    free(ry.data);

    return 0;
}
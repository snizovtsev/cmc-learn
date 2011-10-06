#include "hog.h"
#include "util.h"

#include <algorithm>
#include <cmath>

HOG::HOG(int left, int top, const QImageF &xvec, const QImageF &yvec)
{
    std::fill(weight, weight+NSTEPS, 0.0);

    for (int i = 0; i < CELL_SIZE; ++i) {
        for (int j = 0; j < CELL_SIZE; ++j) {
            qreal x = xvec.pixel(left + j, top + i);
            qreal y = yvec.pixel(left + j, top + i);

            weight[HOG::hash(x, y)] += 1.0; // FIXME
        }
    }
}

int HOG::hash(qreal x, qreal y)
{
    qreal tan = y / fabs(x);
    bool right = x >= 0.0;

    if (0 <= tan && tan <= 0.5)
        return right ? 0 : 3;
    else if (tan > 0.5)
        return right ? 1 : 2;
    else if (tan < -0.5)
        return right ? 6 : 5;
    else
        return right ? 7 : 4;
}

OrientedGradients::OrientedGradients(QImage image)
    : xvec(image), yvec(xvec)
{
    static const QVector<qreal> gauss = gaussKernel(1.5);
    static const QVector<qreal> diff = diffKernel(1.5);

    xvec.rowFilter(diff);
    xvec.columnFilter(gauss);

    yvec.rowFilter(gauss);
    yvec.columnFilter(diff);
}

struct feature_node* makeDescriptor(int left, int top, const QImageF& xvec, const QImageF& yvec)
{
    struct feature_node* result = MALLOC(struct feature_node, NFEATURES + 1);
    if (result == NULL)
        return result;

    for (int i = 0; i < NFEATURES; ++i)
        result[i].index = i + 1;
    result[NFEATURES].index = -1;

    HOG cell[CELL_ROWS][CELL_COLUMNS];
    for (int r = 0; r < CELL_ROWS; ++r) {
        for (int c = 0; c < CELL_COLUMNS; ++c) {
            cell[r][c] = HOG(left + c * CELL_SIZE, top + r * CELL_SIZE, xvec, yvec);
        }
    }

    struct feature_node* ptr = result;
    for (int r = 0; r < CELL_ROWS; ++r) {
        for (int c = 0; c < CELL_COLUMNS; ++c) {
            for (int s = 0; s < HOG::NSTEPS; ++s)
                (ptr++)->value = cell[r][c].weight[s];
        }
    }

    return result;
}

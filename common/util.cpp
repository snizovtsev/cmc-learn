#include "util.h"
#include <cmath>

QVector<qreal> diffKernel(qreal sigma)
{
    int n = ceil(sigma * 3 - 0.001);

    QVector<qreal> result(2 * n -  1);
    for (int x = -(n - 1), i = 0; i < result.size(); ++x,++i)
        result[i] = exp(-0.5 * x / sigma * x / sigma) / (sqrt(2 * M_PI) * sigma) * (-x / sigma / sigma);

    return result;
}

QVector<qreal> gaussKernel(qreal sigma)
{
    int n = ceil(sigma * 3 - 0.001);

    QVector<qreal> result(2 * n -  1);
    for (int x = -(n - 1), i = 0; i < result.size(); ++x,++i)
        result[i] = exp(-0.5 * x / sigma * x / sigma) / (sqrt(2 * M_PI) * sigma);

    return result;
}


#include "hog.h"
#include "util.h"

#include <QtCore>

HOG::HOG(int left, int top, const OrientedGradients& gradients)
{
    qFill(weight, weight+NSTEPS, 0.0);

    for (int i = 0; i < CELL_SIZE; ++i) {
        for (int j = 0; j < CELL_SIZE; ++j) {
            qreal x = gradients.xvec.pixel(left + j, top + i);
            qreal y = gradients.yvec.pixel(left + j, top + i);

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
    : xvec(image), yvec(xvec), cacheKey(image.cacheKey())
{
    static const QVector<qreal> gauss = gaussKernel(1.5);
    static const QVector<qreal> diff = diffKernel(1.5);

    xvec.rowFilter(diff);
    xvec.columnFilter(gauss);

    yvec.rowFilter(gauss);
    yvec.columnFilter(diff);
}

struct CacheKey {
    int x, y;
    qint64 image;

    operator QByteArray() const {
        return QByteArray::fromRawData((const char*) this, sizeof(*this));
    }
};

static void approximate(struct feature_node* ptr, double x)
{
    double L = 0.3;

    for (int n = -NONLINEAR_N; n <= NONLINEAR_N; ++n) {
        if (x == 0.0) {
            (ptr++)->value = 0.0;
            (ptr++)->value = 0.0;
            continue;
        }

        double lambda = n * L;
        double sqrt_k = x / cosh(M_PI * lambda);

        bool minus = sqrt_k < 0.0;
        sqrt_k = sqrt(fabs(sqrt_k));

        double arg = -lambda * log(x);
        (ptr++)->value = cos(arg) * sqrt_k;
        (ptr++)->value = sin(arg) * sqrt_k * (minus ? -1: 1);
    }
}

struct feature_node* makeDescriptor(int left, int top, const OrientedGradients& gradients)
{
    struct feature_node* result = MALLOC(struct feature_node, NFEATURES + 1);
    if (result == NULL)
        return result;

    for (int i = 0; i < NFEATURES; ++i)
        result[i].index = i + 1;
    result[NFEATURES].index = -1;

    static QCache<CacheKey, HOG> cache(CELL_ROWS * PATCH_WIDTH);
    struct feature_node* ptr = result;

    for (int r = 0; r < CELL_ROWS; ++r) {
        for (int c = 0; c < CELL_COLUMNS; ++c) {
            int x = left + c * CELL_SIZE;
            int y = top + r * CELL_SIZE;

            CacheKey key = {x, y, gradients.cacheKey};
            if (!cache.contains(key))
                cache.insert(key, new HOG(x, y, gradients));

            HOG* cell = cache[key];
            for (int s = 0; s < HOG::NSTEPS; ++s)
                approximate(ptr, cell->weight[s]);
        }
    }

    return result;
}

#ifndef HOG_H
#define HOG_H

#include <QtGlobal>
#include "qimagef.h"
#include "linear.h"
#include "tweaks.h"

struct OrientedGradients {
    QImageF xvec;
    QImageF yvec;
    qint64 cacheKey;

    explicit OrientedGradients(QImage image);
};

struct HOG {
    static const int NSTEPS = 8;
    qreal weight[NSTEPS];

    HOG() { }
    HOG(int left, int top, const OrientedGradients& gradients);
    static int hash(qreal x, qreal y);
};

const int NONLINEAR_N = 2;
const int NFEATURES = CELL_ROWS * CELL_COLUMNS *
        HOG::NSTEPS * (NONLINEAR_N * 2 + 1) * 2 /* real and imagine part */;

struct feature_node* makeDescriptor(int left, int top, const OrientedGradients& gradients);

#endif // HOG_H

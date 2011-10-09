#ifndef HOG_H
#define HOG_H

#include <QtGlobal>
#include "qimagef.h"
#include "linear.h"

const int PATCH_WIDTH = 80;
const int PATCH_HEIGHT = 200;
const int CELL_SIZE = 10;
const int CELL_ROWS = PATCH_HEIGHT / CELL_SIZE;
const int CELL_COLUMNS = PATCH_WIDTH / CELL_SIZE;

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

const int NFEATURES = CELL_ROWS * CELL_COLUMNS * HOG::NSTEPS;

struct feature_node* makeDescriptor(int left, int top, const OrientedGradients& gradients);

#endif // HOG_H

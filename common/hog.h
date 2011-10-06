#ifndef HOG_H
#define HOG_H

#include <QtGlobal>
#include "qimagef.h"
#include "linear.h"

const int PATCH_WIDTH = 75;
const int PATCH_HEIGHT = 200;
const int PATCH_HMARGIN = 2;
const int CELL_SIZE = 15;
const int CELL_ROWS = PATCH_HEIGHT / CELL_SIZE;
const int CELL_COLUMNS = PATCH_WIDTH / CELL_SIZE;

struct HOG {
    static const int NSTEPS = 8;
    qreal weight[NSTEPS];

    HOG() { }
    HOG(int left, int top, const QImageF& xvec, const QImageF& yvec);
    static int hash(qreal x, qreal y);
};

const int NFEATURES = CELL_ROWS * CELL_COLUMNS * HOG::NSTEPS;

struct OrientedGradients {
    QImageF xvec;
    QImageF yvec;
    explicit OrientedGradients(QImage image);
};

struct feature_node* makeDescriptor(int left, int top, const QImageF& xvec, const QImageF& yvec);

#endif // HOG_H

#ifndef DETECTION_H
#define DETECTION_H

#include <QVector>
#include <QImage>
#include <linear.h>
#include "hog.h"

QVector<int> detectPedestrians(const struct model* model, const OrientedGradients& gradients);

#endif // DETECTION_H

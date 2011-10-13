#include "detection.h"
#include "hog.h"

QVector<int> detectPedestrians(const struct model* model, const OrientedGradients& gradients)
{
    const int width = gradients.xvec.width();
    QVector<int> result;

    bool pending = false;
    double pValue = -1;
    int pLoc = -PATCH_WIDTH;

    for (int x = 0; x + PATCH_WIDTH < width; x += 2) {
        struct feature_node* descr = makeDescriptor(x, 0, gradients);
        double value;

        if (1 == predict_values(model, descr, &value)) {
            if ((pending && pValue < value) || !pending) {
                pValue = value;
                pLoc = x;
                pending = true;
            }
        }

        if (pending && x - pLoc > PATCH_WIDTH) {
            result.push_back(pLoc);
            pending = false;
        }

        free(descr);
    }

    if (pending && width - pLoc > PATCH_WIDTH)
        result.push_back(pLoc);

    return result;
}

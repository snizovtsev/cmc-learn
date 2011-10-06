#include "hog.h"

#include <QtCore>
#include <linear.h>

struct TrainItem {
    int id;
    int left, top;
    int right, bottom;
    QImage image;
};

typedef QVector < struct feature_node* > Instances;
typedef QVector < int > Labels;

void genFeatures(Instances& features, Labels& labels, const TrainItem& item)
{
    OrientedGradients g(item.image);

    Q_ASSERT(item.top == 0);
    Q_ASSERT(item.bottom == 200);
    Q_ASSERT((item.right - item.left) == 80);

    features.append(makeDescriptor(item.left + PATCH_HMARGIN, item.top, g.xvec, g.yvec));
    labels.append(true);

    for (int x = 0; (x + PATCH_WIDTH) < item.left; x += PATCH_WIDTH / 3) {
        features.append(makeDescriptor(x, item.top, g.xvec, g.yvec));
        labels.append(false);
    }
    for (int x = item.right; (x + PATCH_WIDTH) < g.xvec.width(); x += PATCH_WIDTH / 3) {
        features.append(makeDescriptor(x, item.top, g.xvec, g.yvec));
        labels.append(false);
    }
}

int main(int, char* [])
{
    Instances features;
    Labels labels;
    QTextStream in(stdin);

    while (!(in.skipWhiteSpace(), in.atEnd())) {
        TrainItem item;
        in >> item.id >> item.top >> item.left;
        in >> item.bottom >> item.right;
        QString filename = QString::number(item.id) + ".png";

        qDebug() << "Loading " << filename;
        item.image.load(filename);

        genFeatures(features, labels, item);
    }

    Q_ASSERT(features.size() == labels.size());
    qDebug() << "Total number of descriptors: " << features.size();

    struct problem problem;
    problem.l = features.size();
    problem.n = NFEATURES;
    problem.bias = 0;
    problem.y = labels.data();
    problem.x = features.data();

    struct parameter param;
    param.solver_type = L2R_L2LOSS_SVC_DUAL;
    param.C = 1;
    param.eps = 1e-4;
    param.nr_weight = 0;
    param.weight_label = NULL;
    param.weight = NULL;

    qDebug() << "Training...";
    struct model* pedestrianModel = train(&problem, &param);

    if (save_model("model.txt", pedestrianModel))
        qCritical() << "Can't save the model";

    free_and_destroy_model(&pedestrianModel);
    destroy_param(&param);

    foreach(struct feature_node* descr, features)
        free(descr);
}

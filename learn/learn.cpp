#include "hog.h"

#include <QtCore>
#include <linear.h>

struct TrainItem {
    QString id;
    int left, top;
    int right, bottom;
    QImage image;
};

typedef QVector < struct feature_node* > Features;
typedef QVector < int > Labels;

void genFeatures(Features& features, Labels& labels, const TrainItem& item)
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

bool readInstances(const char* fileName, Features& features, Labels& labels)
{
    QFile file(fileName);
    const QString path= QFileInfo(file).path();

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("Can't open \"%s\": %s",
                  fileName, qPrintable(file.errorString()));
        return false;
    }

    QTextStream markup(&file);
    while (!(markup.skipWhiteSpace(), markup.atEnd())) {
        TrainItem item;
        markup >> item.id >> item.top >> item.left;
        markup >> item.bottom >> item.right;

        QString imgFile = QString("%1/%2.png").arg(path).arg(item.id);
        qDebug() << "Loading " << imgFile;

        if (!item.image.load(imgFile)) {
            qCritical("Error loading \"%s\"", qPrintable(imgFile));
            return false;
        }
        genFeatures(features, labels, item);
    }

    return true;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        qCritical() << "Usage:" << argv[0] << "[markup file] [model file]";
        return 1;
    }

    Features features;
    Labels labels;
    readInstances(argv[1], features, labels);

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
    const char* err;
    if ((err = check_parameter(&problem, &param)) != NULL)
        qFatal("Internal error: %s", err);
    struct model* pedestrianModel = train(&problem, &param);

    if (save_model(argv[2], pedestrianModel))
        qCritical("Can't save the model");

    free_and_destroy_model(&pedestrianModel);
    destroy_param(&param);

    foreach(struct feature_node* descr, features)
        free(descr);
}

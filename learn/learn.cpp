#include <QtCore>
#include <linear.h>

#include "hog.h"
#include "idlparser.h"

typedef QVector < struct feature_node* > Features;
typedef QVector < int > Labels;

bool readInstances(const char* fileName, Features& features, Labels& labels)
{
    IdlParser parser(fileName);

    foreach(QString id, parser.images()) {
        IdlNode node = parser.node(id);
        QImage image;

        QString imageName = QString("%1/%2.png").arg(parser.path(), id);
        qDebug() << "Loading " << imageName;

        if (!image.load(imageName)) {
            qCritical("Error loading \"%s\"", qPrintable(imageName));
            return false;
        }

        OrientedGradients gradients(image);
        for (int x = 0; (x + PATCH_WIDTH) < image.width(); x += PATCH_WIDTH) {
            if (node.isPedestrian(x))
                continue;
            features.append(makeDescriptor(x, 0, gradients));
            labels.append(false);
        }

        foreach (int loc, node.pedestrians()) {
            features.append(makeDescriptor(loc, 0, gradients));
            labels.append(true);
        }
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
    param.C = 0.01; /* [Dalal and Triggs, 2005, Sect. 6.6] */
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

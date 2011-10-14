#include <QtCore>
#include <linear.h>

#include "hog.h"
#include "idlparser.h"
#include "tweaks.h"
#include "detection.h"

QVector <struct feature_node*> features;
QVector <int> labels;
QMap <QString, QImage> image;
QMap <QString, OrientedGradients*> gradients;

static bool initModel(const IdlParser& parser)
{
    int positive = 0;

    foreach (QString id, parser.images()) {
        IdlNode node = parser.node(id);

        QString imageName = QString("%1/%2.png").arg(parser.path(), id);
        qDebug() << "Loading " << imageName;

        if (!image[id].load(imageName)) {
            qCritical("Error loading \"%s\"", qPrintable(imageName));
            return false;
        }

        gradients[id] = new OrientedGradients(image[id]);

        foreach (int loc, node.pedestrians()) {
            QImage clipped = image[id].copy(loc, 0, PATCH_WIDTH, PATCH_HEIGHT);

            for (int i = 0; i < 6; ++i) {
                QImage patch = clipped.transformed(QMatrix()
                                                   .rotate(rand() % 20 - 10)
                                                   .scale((rand() % 2) ? 1.0 : -1.0, 1.0));
                patch = patch.copy(patch.rect().adjusted(
                                       (patch.width() - PATCH_WIDTH) / 2,
                                       (patch.height() - PATCH_HEIGHT) / 2,
                                       -(patch.width() - PATCH_WIDTH) / 2,
                                       -(patch.height() - PATCH_HEIGHT) / 2
                                       ));
                OrientedGradients gr(patch);

                features.append(makeDescriptor(loc, 0, gr));
                labels.append(true);
                ++positive;
            }
        }
    }

    qDebug() << "Generating backgrounds...";

    int negative = 0;
    int image_count = parser.images().size();

    while (negative < positive) {
        QString id = parser.images().at(rand() % image_count);
        const QImage img = image[id];
        IdlNode node = parser.node(id);

        if (img.width() <= 3 * PATCH_WIDTH)
            break;

        do {
            int x = rand() % (img.width() - PATCH_WIDTH - 1);
            if (node.intersects(x))
                continue;

            features.append(makeDescriptor(x, 0, *gradients[id]));
            labels.append(false);
            ++negative;
            break;
        } while (true);
    }

    return true;
}

static int bootstrap(const IdlParser& parser, const struct model* model)
{
    int appended = 0;

    foreach (QString id, image.keys()) {
        QVector <int> found = detectPedestrians(model, *gradients[id]);
        IdlNode truth = parser.node(id);

        foreach (int real, found) {
            if (truth.intersects(real))
                continue;
            qDebug() << "False at " << id << real << ", appending";

            features.append(makeDescriptor(real, 0, *gradients[id]));
            labels.append(false);

            ++appended;
        }
    }

    return appended;
}

int main(int argc, char* argv[])
{
    tweaks::load(argc, argv);

    if (argc != 3) {
        qCritical() << "Usage:" << argv[0] << "[tweaks] <markup> <model>";
        return 1;
    }

    IdlParser parser(argv[1]);
    initModel(parser);

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
    param.C = tweaks::svm_C;
    param.eps = 1e-4;
    param.nr_weight = 0;
    param.weight_label = NULL;
    param.weight = NULL;

    const char* err;
    if ((err = check_parameter(&problem, &param)) != NULL) {
        qFatal("Internal error: %s", err);
        return 1;
    }

    qDebug() << "Initial training...";
    struct model* pedestrianModel = train(&problem, &param);

    qDebug() << "Bootstrapping...";
    int max_steps = 3, false_hits;
    while (max_steps > 0 && (false_hits = bootstrap(parser, pedestrianModel)) > 0) {
        qDebug() << false_hits << "false hits added, retraining...";
        qDebug() << "Total number of descriptors: " << features.size();
        problem.l = features.size();
        problem.y = labels.data();
        problem.x = features.data();

        free_and_destroy_model(&pedestrianModel);
        pedestrianModel = train(&problem, &param);
        --max_steps;
    }

    if (save_model(argv[2], pedestrianModel))
        qCritical("Can't save the model");

    free_and_destroy_model(&pedestrianModel);
    destroy_param(&param);

    foreach(struct feature_node* descr, features)
        free(descr);
    foreach(OrientedGradients* gr, gradients)
        delete gr;
}

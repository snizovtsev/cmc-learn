#include <QtCore>
#include <QPainter>

#include <hog.h>
#include <linear.h>

QVector<int> detect(const model* model, QImage image)
{
    QVector<int> result;
    OrientedGradients g(image);

    double lastValue = -1;
    int lastX = -PATCH_WIDTH;

    for (int x = 0; x + PATCH_WIDTH < g.xvec.width(); x += 2) {
        struct feature_node* descr = makeDescriptor(x, 0, g.xvec, g.yvec);
        double value;

        if (1 == predict_values(model, descr, &value)) {
            if (x - lastX > PATCH_WIDTH * 0.7) {
                result.push_back(x);
                lastX = x;
                lastValue = value;
            } else if (lastValue < value) {
                result[result.size()-1] = x;
                lastX = x;
                lastValue = value;
            }
        }

        //qDebug("%d %lf", x, value);

        free(descr);
    }

    return result;
}

void label(QImage& image, const QVector<int> results)
{
    QPainter painter(&image);
    painter.setPen(Qt::green);

    foreach(int x, results) {
        painter.drawRect(x, 0, PATCH_WIDTH, PATCH_HEIGHT);
    }
}

void processFile(const struct model* model, const QString &fileName, bool labeled)
{
    qDebug() << "Processing file " << fileName;

    QImage image(fileName);
    if (image.isNull()) {
        qCritical("Can't load %s", qPrintable(fileName));
        return;
    }

    QVector <int> pedestrians = detect(model, image);
    QFileInfo info(fileName);

    QTextStream out(stdout);
    foreach (int x, pedestrians) {
        out << info.baseName() << " "
            << x << " 0 "
            << x + PATCH_WIDTH + 5 << " " << 0 + PATCH_HEIGHT
            << endl;
    }

    if (!labeled)
        return;

    label(image, pedestrians);
    QString resultFile = info.dir().filePath
            (QString("%1-labeled.%2").arg(info.baseName(), info.completeSuffix()));
    if (!image.save(resultFile))
        qCritical() << "Can't write to" << resultFile;
}

int main(int argc, char* argv[])
{
    if (argc <= 1) {
        qCritical() << "Usage:" << argv[0] << "model [--label] files_or_directories";
        return 1;
    }

    struct model* pedestrianModel = load_model(argv[1]);
    if (0 == pedestrianModel) {
        qCritical("Can't load %s", argv[1]);
    }

    bool label = false;
    for (int i = 2; i < argc; ++i) {
        if (strcmp("--label", argv[i]) == 0) {
            label = true;
            continue;
        }

        if (QFileInfo(argv[i]).isDir()) {
            QDir dir(argv[i],
                     "*.png *.jpg *.jpeg", /* name filters*/
                     QDir::Name, /* sort mode */
                     QDir::Files | QDir::Readable);

            QStringList list = dir.entryList();
            foreach(QString fileName, list)
                processFile(pedestrianModel, dir.filePath(fileName), label);
        } else {
            processFile(pedestrianModel, argv[i], label);
        }
    }

    free_and_destroy_model(&pedestrianModel);
    return 0;
}

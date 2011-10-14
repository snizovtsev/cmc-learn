#include <QtCore>
#include <QPainter>
#include <linear.h>

#include "detection.h"
#include "tweaks.h"

void label(QImage& image, const QVector<int> results)
{
    QPainter painter(&image);
    painter.setPen(Qt::green);

    foreach(int x, results)
        painter.drawRect(x, 0, PATCH_WIDTH, PATCH_HEIGHT);
}

void processFile(const struct model* model, const QString &fileName, bool labeled)
{
    qDebug() << "Processing file " << fileName;

    QImage image(fileName);
    if (image.isNull()) {
        qCritical("Can't load %s", qPrintable(fileName));
        return;
    }

    QVector <int> pedestrians = detectPedestrians(model, OrientedGradients(image));
    QFileInfo info(fileName);

    QTextStream out(stdout);
    foreach (int x, pedestrians) {
        out << info.path() + QDir::separator() + info.baseName() << "\t"
            << "0\t" << x << "\t"
            << 0 + PATCH_HEIGHT << '\t' << x + PATCH_WIDTH
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
    tweaks::load(argc, argv);

    if (argc <= 1) {
        qCritical() << "Usage:" << argv[0] << "[tweaks] <model> [--label] <files_or_directories>";
        return 1;
    }

    struct model* pedestrianModel = load_model(argv[1]);
    if (0 == pedestrianModel) {
        qCritical("Can't load %s", argv[1]);
        return 1;
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

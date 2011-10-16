#include <QtCore>
#include <cstdio>

#include "idlparser.h"

int main(int argc, char* argv[])
{
    if (argc == 3) {
        if (NULL == freopen(argv[2], "rt", stdin)) {
            qDebug("Can't open '%s'", argv[2]);
            return 1;
        }
        --argc;
    }

    if (argc != 2) {
        qDebug() << "Usage:" << argv[0] << " <markup file> [detection results]";
        return 1;
    }

    IdlParser markup(argv[1]);
    QTextStream istream(stdin);
    IdlParser results(istream);

    int recall_found = 0, recall_all = 0;
    foreach (QString id, markup.images()) {
        const IdlNode node = markup.node(id);
        const IdlNode found = results.node(id);

        foreach (int right_loc, node.pedestrians()) {
            if (found.isPedestrian(right_loc))
                ++recall_found;
            ++recall_all;
        }
    }

    int precision_right = 0, precision_all = 0;
    foreach (QString id, results.images()) {
        const IdlNode right = markup.node(id);
        const IdlNode found = results.node(id);

        foreach (int found_loc, found.pedestrians()) {
            if (right.isPedestrian(found_loc))
                ++precision_right;
            ++precision_all;
        }
    }

    double recall = (double) recall_found / recall_all;
    double precision = (double) precision_right / precision_all;
    double f_score = 2 * precision * recall / (precision + recall);

    printf("Recall:\t\t%lf %%\n", 100 * recall);
    printf("Precision:\t%lf %%\n", 100 * precision);
    printf("F score:\t%lf %%\n", 100 * f_score);

    return 0;
}

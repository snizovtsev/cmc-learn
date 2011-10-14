#include "tweaks.h"

#include <QtCore>

namespace tweaks {
    double blur_sigma = 1.5;
    double nonlinear_L = 0.3;
    double svm_C = 1.0;
    uint learn_steps = 5;

    void load(int &argc, char** &argv)
    {
        int i;
        for (i = 1; i < argc-1; i += 2) {
            if (strcmp(argv[i], "--sigma") == 0) {
                blur_sigma = QString(argv[i+1]).toDouble();
            } else if (strcmp(argv[i], "-L") == 0) {
                nonlinear_L = QString(argv[i+1]).toDouble();
            } else if (strcmp(argv[i], "-C") == 0) {
                svm_C = QString(argv[i+1]).toDouble();
            } else if (strcmp(argv[i], "--learn-steps") == 0) {
                svm_C = QString(argv[i+1]).toUInt();
            } else {
                break;
            }
        }

        argv[i-1] = argv[0];
        argc -= i-1;
        argv += i-1;
    }
}

#ifndef TWEAKS_H
#define TWEAKS_H

#include <QString>

const int PATCH_WIDTH = 80;
const int PATCH_HEIGHT = 200;
const int CELL_SIZE = 10;
const int CELL_ROWS = PATCH_HEIGHT / CELL_SIZE;
const int CELL_COLUMNS = PATCH_WIDTH / CELL_SIZE;

namespace tweaks {
    extern double blur_sigma;
    extern double nonlinear_L;
    extern double svm_C;

    void load(int &argc, char** &argv);
}

#endif // TWEAKS_H

#include <QPainter>

#include <hog.h>
#include <linear.h>

int main(int, char* argv[])
{
    struct model* pedestrianModel = load_model(argv[1]);
    if (0 == pedestrianModel)
        qCritical("Can't load model");

    QImage image(argv[2]);
    OrientedGradients g(image);

    QPainter painter(&image);
    painter.setPen(QPen(Qt::green));

    for (int x = 0; x + PATCH_WIDTH < g.xvec.width(); x += 5) {
        struct feature_node* descr = makeDescriptor(x, 0, g.xvec, g.yvec);

        if (predict(pedestrianModel, descr)) {
            painter.drawRect(x, 0, PATCH_WIDTH, PATCH_HEIGHT);
        }

        free(descr);
    }
    painter.end();

    free_and_destroy_model(&pedestrianModel);
    image.save("out.png");
    return 0;
}

#ifndef UTIL_H
#define UTIL_H

#include <QVector>

template <typename T>
class CircularBuffer: public QVector<T> {
    typedef QVector<T> super;
    int head;
public:
    explicit CircularBuffer(int size): QVector<T>(size), head(0) { }

    void shift(T value) {
        super::operator[](head++) = value;
        if (head == super::size())
            head = 0;
    }

    T& operator [](int index) {
        return super::operator[]((head + index) % super::size());
    }

    const T& operator [](int index) const {
        return super::operator[]((head + index) % super::size());
    }
};

QVector<qreal> diffKernel(qreal sigma);
QVector<qreal> gaussKernel(qreal sigma);

#define MALLOC(type, count) \
    static_cast <type*> (malloc(sizeof(type) * count))

#endif // UTIL_H

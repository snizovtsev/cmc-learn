#include "qimagef.h"
#include "util.h"

#include <QColor>
#include <algorithm>

QImageF::QImageF(const QImage &source)
    : m_width(source.width()),
      m_height(source.height()),
      m_data_len(m_width * m_height),
      m_data(new qreal[m_data_len]),
      m_cacheKey(source.cacheKey())
{
    qreal* data = m_data;

    for (int line = 0; line < m_height; ++line) {
        const QRgb* rgb = reinterpret_cast<const QRgb*> (source.scanLine(line));
        for (int col = 0; col < m_width; ++col, ++rgb)
            *data++ = qBlue(*rgb) / 256.0;
    }
}

QImageF::~QImageF()
{
    delete[] m_data;
}

QImageF::QImageF(const QImageF &other)
{
    m_width = other.m_width;
    m_height = other.m_height;
    m_data_len = other.m_data_len;

    m_data = new qreal[m_data_len];
    std::copy(other.m_data, other.m_data + m_data_len, m_data);

    m_cacheKey = other.cacheKey();
}

inline qreal& QImageF::pixel(int x, int y) const
{
    x = abs(x);
    y = abs(y);
    if (x >= m_width)
        x = m_width - x % m_width - 1;
    if (y >= m_height)
        y = m_height - y % m_height - 1;
    return m_data[y * m_width + x];
}

template <class T>
T operator *(const CircularBuffer<T> &window, const QVector<T> filter)
{
    Q_ASSERT(window.size() == filter.size());
    T res = 0;
    for (int i = 0; i < filter.size(); ++i)
        res += window[i] * filter[i];
    return res;
}

void QImageF::rowFilter(const QVector<qreal> &filter)
{
    CircularBuffer <qreal> window(filter.size());
    int n = filter.size() / 2;

    for (int y = 0; y < m_height; ++y) {
        for (int x = -n; x < n; ++x)
            window.shift(pixel(x, y));

        for (int x = 0; x < m_width; ++x) {
            window.shift(pixel(x + n - 1, y));
            pixel(x, y) = window * filter;
        }
    }
}

void QImageF::columnFilter(const QVector<qreal> &filter)
{
    CircularBuffer <qreal> window(filter.size());
    int n = filter.size() / 2;

    for (int x = 0; x < m_width; ++x) {
        for (int y = -n; y < n; ++y)
            window.shift(pixel(x, y));

        for (int y = 0; y < m_height; ++y) {
            window.shift(pixel(x, y + n - 1));
            pixel(x, y) = window * filter;
        }
    }
}

QImageF::operator QImage() const
{
    QImage result(m_width, m_height, QImage::Format_ARGB32);

    for (int y = 0; y < m_height; ++y) {
        QRgb* ptr = reinterpret_cast<QRgb*> (result.scanLine(y));
        for (int x = 0; x < m_width; ++x) {
            int gray = (int) (m_data[x + y * m_width] * 256);
            *ptr++ = qRgb(gray, gray, gray);
        }
    }

    return result;
}

#ifndef QIMAGEF_H
#define QIMAGEF_H

#include <QVector>
#include <QImage>

class QImageF
{
public:
    QImageF(const QImage& source);
    QImageF(const QImageF& other);
    ~QImageF();

    qreal* data() { return m_data; }
    const qreal* data() const { return m_data; }
    qint64 cacheKey() const { return m_cacheKey; }

    int width() { return m_width; }
    int height() { return m_height; }
    int data_len() { return m_data_len; }

    qreal& pixel(int x, int y) const;

    void rowFilter(const QVector<qreal> &filter);
    void columnFilter(const QVector<qreal> &filter);

    operator QImage() const;
private:
    int m_width;
    int m_height;
    int m_data_len;
    qreal* m_data;
    qint64 m_cacheKey;

    QImage& operator = (const QImageF& other);
    QImageF() { }
};

#endif // QIMAGEF_H

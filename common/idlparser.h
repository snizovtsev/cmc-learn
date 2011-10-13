#ifndef IDLPARSER_H
#define IDLPARSER_H

#include <QtCore>

class IdlNode {
public:
    IdlNode();

    void addPedestrian(int loc) { m_loc.append(loc); }
    bool isPedestrian(int loc) const;
    bool intersects(int loc) const;

    const QList<int>& pedestrians() const { return m_loc; }
private:
    QList <int> m_loc;
};

class IdlParser
{
public:
    explicit IdlParser(const char* fileName);
    explicit IdlParser(QTextStream& stream) { construct(stream); }

    const QStringList& images() const { return m_images; }
    const IdlNode node(QString id) const { return m_node[id]; }
    QString path() const { return m_path; }
private:
    QStringList m_images;
    QMap<QString, IdlNode> m_node;
    QString m_path;

    void construct(QTextStream &stream);
};

#endif // IDLPARSER_H

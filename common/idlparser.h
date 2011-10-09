#ifndef IDLPARSER_H
#define IDLPARSER_H

#include <QtCore>

class IdlNode {
public:
    IdlNode();

    void addPedestrian(int loc) { m_loc.append(loc); }
    bool exactlyPedestrian(int loc) const { return m_loc.contains(loc); }
    bool isPedestrian(int loc) const;

private:
    QList <int> m_loc;
};

class IdlParser
{
public:
    explicit IdlParser(QTextStream& stream);

    const QStringList& images() const { return m_images; }
    const IdlNode node(QString id) const { return m_node[id]; }
private:
    QStringList m_images;
    QMap<QString, IdlNode> m_node;
};

#endif // IDLPARSER_H

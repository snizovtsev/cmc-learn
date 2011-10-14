#include "idlparser.h"
#include "hog.h"

IdlNode::IdlNode() { }

bool IdlNode::isPedestrian(int loc) const
{
    foreach (int ped, m_loc) {
        if (loc >= ped - PATCH_WIDTH / 2 && loc <= ped + PATCH_WIDTH / 2)
            return true;
    }
    return false;
}

bool IdlNode::intersects(int loc) const
{
    foreach (int ped, m_loc) {
        if (loc >= ped - PATCH_WIDTH && loc <= ped + PATCH_WIDTH)
            return true;
    }
    return false;
}

IdlParser::IdlParser(const char* fileName)
{
    QFile file(fileName);
    m_path = QFileInfo(file).path();

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("Can't open \"%s\": %s",
                  fileName, qPrintable(file.errorString()));
        exit(EXIT_FAILURE);
    }

    QTextStream stream(&file);
    construct(stream);
}

void IdlParser::construct(QTextStream &stream)
{
    while (!(stream.skipWhiteSpace(), stream.atEnd())) {
        QString id;
        int top, left, bottom, right;

        stream >> id >> top >> left >> bottom >> right;
        Q_ASSERT(top == 0);
        Q_ASSERT(bottom = 200);
        Q_ASSERT(right - left == PATCH_WIDTH);

        m_images.append(id);
        m_node[id].addPedestrian(left);
    }
}

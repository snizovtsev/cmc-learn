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

IdlParser::IdlParser(QTextStream& stream)
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

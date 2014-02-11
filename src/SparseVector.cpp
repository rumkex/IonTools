#include "SparseVector.h"
#include <ostream>

std::ostream& operator<<(std::ostream& out, SparseVector &v)
{
    out << "( ";
    for(auto it = v.begin(); it != v.end(); it++)
        out << (*it).first << ":" << (*it).second << " ";
    out << ")";
    return out;
}

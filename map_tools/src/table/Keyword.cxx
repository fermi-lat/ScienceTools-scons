/** @file KeyWord.cxx

*/
#include "table/Keyword.h"


using namespace table;

Keyword & Keyword::operator=(const Keyword &right)
{
    if (this != &right) copy(right);
    return *this;
}


void Keyword::copy (const Keyword& right)
{
    m_name    = right.m_name;
    m_type    = right.m_type;
    m_comment = right.m_comment;
}


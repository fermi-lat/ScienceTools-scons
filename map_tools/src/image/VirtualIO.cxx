/** @file VirtualIO.cxx
    @brief Implementaion of VirtualIO methods

    @author Toby Burnett
    Code orginally written by Riener Rohlfs

     $Header$
*/
#include "VirtualIO.h"


VirtualIO::VirtualIO( IOElement* element)
  : m_element(element)
{}

// dtor
VirtualIO::~VirtualIO() {}

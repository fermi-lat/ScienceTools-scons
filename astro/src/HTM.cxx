/** @file HTM.cxx
@brief implement HTM methods

@author T. Burnett (based on copyright code by Peter Z. Kunszt) 
$Header$
*/

#include "astro/HTM.h"
#include <algorithm>
#include <stdexcept>

using namespace astro;

HTM::HTM(int maxlevel)
: m_maxid( 2*size(maxlevel)-1 ) 
{
    using astro::SkyDir;
    // create vertices of the octahedron: order is the same as for htm
    SkyDir v[]= {        
        SkyDir(Hep3Vector( 0, 0, 1)), // 0
            SkyDir(Hep3Vector( 1, 0, 0)), // 1
            SkyDir(Hep3Vector( 0, 1, 0)), // 2
            SkyDir(Hep3Vector(-1, 0, 0)), // 3
            SkyDir(Hep3Vector( 0,-1, 0)), // 4
            SkyDir(Hep3Vector( 0, 0,-1))  // 5
    };

    // then make a quad tree of nodes for each one, again same order
    newNode(v[1],v[5],v[2], 8);  // S0
    newNode(v[2],v[5],v[3], 9);  // S1
    newNode(v[3],v[5],v[4],10);  // S2
    newNode(v[4],v[5],v[1],11);  // S3
    newNode(v[1],v[0],v[4],12);  // N0
    newNode(v[4],v[0],v[3],13);  // N1
    newNode(v[3],v[0],v[2],14);  // N2
    newNode(v[2],v[0],v[1],15);  // N3

    // finally sort by id
    std::sort(m_nodes.begin(), m_nodes.end());
}

HTM::NodeList::const_iterator HTM::begin(int level)const {
    return level>0? m_nodes.begin()+start(level) : m_nodes.begin();
    //   return std::find(m_nodes.begin(), m_nodes.end(), size(level));
}

HTM::NodeList::const_iterator HTM::end(int level)const {
    size_t off = start(level)+size(level);
    return off> m_nodes.size()? m_nodes.end() : m_nodes.begin()+off;
    //        return std::find(m_nodes.begin(), m_nodes.end(), size(level+1));
}

/** @brief return node by id */
const HTM::Node& HTM::node( unsigned int id) const
{
    return m_nodes[index(id)];
}

/** @brief return  first child of */
HTM::NodeList::const_iterator HTM::child(const HTM::Node& node) const
{
    unsigned int child_id = 4*node.id();
    return m_nodes.begin()+index(child_id);
}


/** @brief number of elements at the level */
size_t HTM::size(int level) { return 8*(1<<2*level); }

/** @brief index of the start for a given level*/
size_t HTM::start(int level) { return 8*((1<<2*level)-1)/3; }

/** @brief index into the vector for a given id */
size_t HTM::index(unsigned int id) const {
    if( id<8 || id>m_maxid) throw std::out_of_range("HTM::index id out of range");
    unsigned int i = id / 32; // shift 5 bits
    int level = 0;
    while( i != 0 ) { ++level; i /= 4;}
    return start(level)+ id-size(level);
}


/** @brief solid angle enclosed by three directions 

code from htm's SpatialIndex::area.
@todo: is it really necessary to evaluate 8 trig functions?
*/ 
double HTM::area(const astro::SkyDir& v0,
                   const astro::SkyDir& v1,
                   const astro::SkyDir& v2)
{  
    double a = acos( v0() * v1())
        ,  b = acos( v1() * v2())
        ,  c = acos( v2() * v0())
        ,  s = (a + b + c)/2.0;

    double area = 4.0*atan(sqrt(tan(s/2.0)*
        tan((s-a)/2.0)*
        tan((s-b)/2.0)*
        tan((s-c)/2.0)));        
    return area;
}

void HTM::dump(std::ostream & out) const {
    for( NodeList::const_iterator it = m_nodes.begin();it!= m_nodes.end(); ++it){
        const Node& n = *it;
        out << n.id() << " " << n.area() << std::endl;
    }
}

void HTM::newNode(const astro::SkyDir& a,
        const astro::SkyDir& b,
        const astro::SkyDir& c, 
        unsigned int id)
    {
        using astro::SkyDir;
        m_nodes.push_back(Node(id, SkyDir(a()+b()+c()), area(a,b,c)));
        if( 4*id> m_maxid) return;
        // create list of mid-points by interpolation: each used 3 times below
        SkyDir w[]={
            SkyDir( b()+c() ),
                SkyDir( c()+a() ),
                SkyDir( a()+b() )};

            newNode(a,   w[2], w[1], 4*id);
            newNode(b,   w[0], w[2], 4*id+1);
            newNode(c,   w[1], w[0], 4*id+2);
            newNode(w[0],w[1], w[2], 4*id+3);
    }


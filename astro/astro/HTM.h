/** @file HTM.h
@brief Define the class HTM

@author T. Burnett (based on copyright code by Peter Z. Kunszt) 
$Header$
*/

#ifndef astro_HTM_h
#define astro_HTM_h

#include "astro/SkyDir.h"
#include "astro/SkyFunction.h"
#include <vector>
#include <iostream>


namespace astro {


/**
@class HTM
@brief Create a Hierarchical Triangle Mesh (HTM)

The HTM is a quad tree of spherical triangles. The tree starts
with 8 triangles defined by a regular octahedron. Each triangle is then
decomposed into 4 new triangles by determining the midpoints of each pair
of vertices. This is applied recusively to the desired depth.


@verbatim
.          /\
.         /  \
.        /____\
.       /\    /\
.      /  \  /  \
.     /____\/____\
@endverbatim
The result is a vector of nodes describing each triangle, sorted by id.
Access is via special begin() and end() functions, delimiting each level.

A nested class, HTM::Integrand, is provided to facilitate numeric integrals
over the sphere. A simple example using it is:

@verbatim
    int level = 8; // 10 is limit for memory-resident??
    astro::HTM h(level);
    const astro::SkyFunction& fun ; // from somewhere
    double integral = std::accumulate(h.begin(), h.end(), 0., astro::HTM::Integrand(fun));

@endverbatim
Note that the begin and end iterators will select any level from zero (8 nodes)
to the maximum generated, which can be used to test the accuracy.

About the level: the number of nodes is 2**(3+2*level):
@verbatim
  level     nodes
   0            8 
   1           32 
   2          128 
   3          512 
   4         2048 
   5         8192 
   6        32768 
   7       131072 
   8       524288 
   9      2097152 
  10      8388608
  @endverbatim

*/
class HTM {
public:

    /** @brief ctor with maximum level 
    */
    HTM(int maxlevel);

    /** @class HTM::Node
    @brief describe a triangular node
    */
    class Node {
    public:
        Node(unsigned int i, const astro::SkyDir& d, double a)
            : m_id(i), m_dir(d), m_area(a)
        {}
        /** @brief conversion operator is the id, for sorting */
        operator unsigned int()const{return m_id;}
        unsigned int id()const{return m_id;}
        /** @brief the central direction */
        const astro::SkyDir & dir() const{return m_dir;}
        const double area()const {return m_area;}

        /** @brief evaluate the diffential element of a function */
        double fdA(const astro::SkyFunction& fun)const{return area()*fun(dir());}
    private:
        unsigned int m_id;
        astro::SkyDir m_dir;
        double m_area;
    };

    /** @class Integrand
        @brief Functor that can be used with std::accumulate to 
        perform numeric integration.

    */
    class Integrand {  public:
        /** brief ctor saves reference to a SkyFunction    */
        Integrand(const SkyFunction& f): m_f(f){}
        /** brief function called by accumulate    */
        double operator()( double result, const HTM::Node& node){
            return result+ node.fdA(m_f);  }
        const SkyFunction& m_f;
    };


    typedef std::vector<Node> NodeList;
    typedef NodeList::const_iterator const_iterator;

    NodeList::const_iterator begin(int level=-1)const;

    NodeList::const_iterator end(int level=-1)const;

    /** @brief return node by id */
    const Node& node( unsigned int id) const;

    /** @brief return  first child of */
    NodeList::const_iterator child(const HTM::Node& node) const;


    /** @brief number of elements at the level */
    static size_t size(int level);
    /** @brief index of the start for a given level*/
    static size_t start(int level);

    /** @brief index into the vector for a given id */
    size_t index(unsigned int id) const;

    /** @brief solid angle enclosed by three directions 

        code from htm's SpatialIndex::area.
        @todo: is it really necessary to evaluate 8 trig functions?
    */ 
    double area(const astro::SkyDir& v0,
                   const astro::SkyDir& v1,
                   const astro::SkyDir& v2);

    void dump(std::ostream & out) const;

private:

    /** @brief create a new node, recursively until limit */
    void newNode(const astro::SkyDir& a,
        const astro::SkyDir& b,
        const astro::SkyDir& c, 
        unsigned int id);
   

    NodeList m_nodes;
    size_t m_level;
    size_t m_maxid;
};
}// namespace
#endif

/**
* @file Parameters.h
* @brief Tool Input Parameter Reader base class
*
* $Header$
*/
#ifndef MAP_TOOLS_PARAMETERS_H
#define MAP_TOOLS_PARAMETERS_H 

#include "hoops/hoops_prompt_group.h" // for hoops::ParPromptGroup

#include <string>

namespace map_tools {
/**
* @class Parameters
* @brief Input reader base class for tools
*
* It uses hoops to read parameters from the par file.
* The description of pil format is available at
* <a href="http://www-glast.slac.stanford.edu/sciencetools/userInterface/doc/pil.pdf">PIL user
* manual</a>.
*
* @author Toby Burnett [originally from Sandhia Bansall]
*
* $Header$
*/

    class Parameters : public hoops::ParPromptGroup
{
public:
    // Constructors
    Parameters( int argc, char *argv[]);
    ~Parameters(){};

    template< typename T>
     T getValue(const std::string & name){ return (*this)[name];}

     template <typename T>
   T getValue(const std::string & name, const T & default_value) {
      try {
         return getValue<T>(name);
      } catch (...) {
         return default_value;
      }
   }

    // Accessor Methods
    const std::string &inputFile() const   { return m_inFile; }
    const std::string &filter() const      { return m_filter; }
    const std::string &outputFile() const  { return m_outFile; }
    bool verboseMode()  const            { return m_verboseMode; }
    bool clobber()      const            { return m_clobber; }
    short chatter()     const            { return m_chatter; }
private:
    // Data Members
    std::string   m_inFile;
    std::string   m_outFile;
    std::string   m_filter;
    bool        m_verboseMode;
    bool        m_clobber;
    int         m_chatter;
};
} // namespace map_tools
#endif

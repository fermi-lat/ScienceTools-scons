/** \file AppParGroup.h
    \brief Standard way to handle hoops.
    \author James Peachey, HEASARC
*/
#ifndef st_app_AppParGroup_h
#define st_app_AppParGroup_h

#include <list>
#include <map>
#include <set>
#include <string>

#include "hoops/hoops_prompt_group.h"

namespace st_app {

  /** \class AppParGroup
      \brief Convenient encapsulation of the most common hoops use case.
  */
  class AppParGroup : public hoops::ParPromptGroup {
    public:
      typedef std::pair<std::string, std::string> ParValuePair;
      typedef std::list<ParValuePair> CaseList;

      /** \brief Create parameter group, using StApp to get the command line arguments.
          \param comp_name The required name of this application.
      */
      AppParGroup(const std::string & comp_name);

      /** \brief Copy constructor.
      */
      AppParGroup(const AppParGroup & group);

      /** \brief Virtual destructor.
      */
      virtual ~AppParGroup() throw() {}

      /** \brief Assignments.
          \param group The right hand side of the assignment.
      */
      virtual hoops::IParGroup & operator =(const AppParGroup & group);

      /** \brief Assignments.
          \param group The right hand side of the assignment.
      */
      virtual hoops::IParGroup & operator =(const hoops::IParGroup & group);

      virtual void Prompt();

      virtual void Prompt(const std::string & par_name);

      void suppressPrompts(bool suppress = true);

      /** \brief Returns whether or not the named parameter is being used as a switch.
          \param par_name The parameter in question.
      */
      bool isSwitch(const std::string & par_name) const;

      /** \brief Returns whether or not the named parameter is being used as a switch.
          \param par_name The parameter in question.
      */
      void getCase(const std::string & par_name, CaseList & case_cont) const;

      /** \brief Cause the parameter with the given name to be used as a switch.
          \param switch_name The name of the parameter being used as a switch.
      */
      void setSwitch(const std::string & switch_name);

      /** \brief Cause the parameter with the given name to be associated with the given case of the given switch.
          \param switch_name The name of the parameter being used as a switch.
          \param case_name The value of the switch parameter used to identify the case.
          \param par_name The name of the parameter being associated with the case.
      */
      void setCase(const std::string & switch_name, const std::string & case_name, const std::string & par_name);

    private:
      typedef std::set<std::string> Switch;
      typedef std::multimap<std::string, ParValuePair> Case;
      Switch m_switch;
      Case m_case;
      bool m_prompt_mode;
  };

}

#endif

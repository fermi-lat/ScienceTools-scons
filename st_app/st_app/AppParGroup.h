/** \file AppParGroup.h
    \brief Standard way to handle hoops.
    \author James Peachey, HEASARC
*/
#ifndef st_app_AppParGroup_h
#define st_app_AppParGroup_h

#include <string>

#include "hoops/hoops_prompt_group.h"

namespace st_app {

  /** \class AppParGroup
      \brief Convenient encapsulation of the most common hoops use case.
  */
  class AppParGroup : public hoops::ParPromptGroup {
    public:
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

    private:
      bool m_prompt_mode;
  };

}

#endif

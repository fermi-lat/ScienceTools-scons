/** \file GlastLatBinConfig.h
    \brief Helper class which uses standard sets of parameters to configure binners for standard applications.
    \author James Peachey, HEASARC
*/
#ifndef evtbin_GlastLatBinConfig_h
#define evtbin_GlastLatBinConfig_h

#include "evtbin/BinConfig.h"

namespace evtbin {

  class Binner;

  /** \class GlastLatBinConfig
      \brief Helper class which uses standard sets of parameters to configure binners for standard applications.
  */
  class GlastLatBinConfig : public BinConfig {
    public:
      static void load();

      virtual GlastLatBinConfig * clone() const;
  };

}

#endif

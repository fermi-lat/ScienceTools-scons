/** \file GlastGbmBinConfig.h
    \brief Helper class which uses standard sets of parameters to configure binners for standard applications.
    \author James Peachey, HEASARC
*/
#ifndef evtbin_GlastGbmBinConfig_h
#define evtbin_GlastGbmBinConfig_h

#include "evtbin/BinConfig.h"

namespace evtbin {

  class Binner;

  /** \class GlastGbmBinConfig
      \brief Helper class which uses standard sets of parameters to configure binners for standard applications.
  */
  class GlastGbmBinConfig : public BinConfig {
    public:
      static void load();

      virtual GlastGbmBinConfig * clone() const;

      virtual void energyParPrompt(st_app::AppParGroup & par_group) const;
  
      virtual Binner * createEnergyBinner(const st_app::AppParGroup & par_group) const;
  
      virtual Binner * createEbounds(const st_app::AppParGroup & par_group) const;

      virtual Gti * createGti(const st_app::AppParGroup & par_group) const;
  };

}

#endif

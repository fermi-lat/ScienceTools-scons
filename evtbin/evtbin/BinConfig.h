/** \file BinConfig.h
    \brief Helper class which uses standard sets of parameters to configure binners for standard applications.
    \author James Peachey, HEASARC
*/
#ifndef evtbin_BinConfig_h
#define evtbin_BinConfig_h

#include <string>

// Interactive parameter file access from st_app.
#include "st_app/AppParGroup.h"

namespace evtbin {

  class Binner;

  /** \class BinConfig
      \brief Helper class which uses standard sets of parameters to configure binners for standard applications.
  */
  class BinConfig {
    public:
      virtual void energyParPrompt(st_app::AppParGroup & par_group);
  
      virtual void spatialParPrompt(st_app::AppParGroup & par_group);
  
      virtual void timeParPrompt(st_app::AppParGroup & par_group);
  
      virtual Binner * createEnergyBinner(const st_app::AppParGroup & par_group) const;
  
      virtual Binner * createTimeBinner(const st_app::AppParGroup & par_group) const;

      virtual void parPrompt(st_app::AppParGroup & par_group, const std::string & alg, const std::string & in_field,
        const std::string & bin_begin, const std::string & bin_end, const std::string & bin_size, const std::string & num_bins,
        const std::string & bin_file);

      virtual Binner * createBinner(const st_app::AppParGroup & par_group, const std::string & alg,
        const std::string & in_field,  const std::string & bin_begin, const std::string & bin_end, const std::string & bin_size,
        const std::string & num_bins, const std::string & bin_file, const std::string & bin_ext, const std::string & start_field,
        const std::string & stop_field, const std::string & sn_ratio = "", const std::string & lc_emin = "",
        const std::string & lc_emax = "") const;
  
  };

}

#endif

/** \file BinConfig.h
    \brief Helper class which uses standard sets of parameters to configure binners for standard applications.
    \author James Peachey, HEASARC
*/
#ifndef evtbin_BinConfig_h
#define evtbin_BinConfig_h

#include <string>
#include <map>

// Interactive parameter file access from st_app.
#include "st_app/AppParGroup.h"

namespace evtbin {

  class Binner;
  class Gti;

  /** \class BinConfig
      \brief Helper class which uses standard sets of parameters to configure binners for standard applications.
  */
  class BinConfig {
    public:
      static void load();

      static BinConfig * create(const std::string & ev_file_name);

      virtual ~BinConfig();

      virtual BinConfig * clone() const = 0;

      virtual void energyParPrompt(st_app::AppParGroup & par_group) const;
  
      virtual void spatialParPrompt(st_app::AppParGroup & par_group) const;
  
      virtual void timeParPrompt(st_app::AppParGroup & par_group) const;
  
      virtual Binner * createEnergyBinner(const st_app::AppParGroup & par_group) const;
  
      virtual Binner * createTimeBinner(const st_app::AppParGroup & par_group) const;

      virtual Binner * createEbounds(const st_app::AppParGroup & par_group) const;

      virtual Gti * createGti(const st_app::AppParGroup & par_group) const;

      virtual void parPrompt(st_app::AppParGroup & par_group, const std::string & alg, const std::string & in_field,
        const std::string & bin_begin, const std::string & bin_end, const std::string & bin_size, const std::string & num_bins,
        const std::string & bin_file, const std::string & sn_ratio = "", const std::string & lc_emin = "",
        const std::string & lc_emax = "") const;

      virtual Binner * createBinner(const st_app::AppParGroup & par_group, const std::string & alg,
        const std::string & in_field,  const std::string & bin_begin, const std::string & bin_end, const std::string & bin_size,
        const std::string & num_bins, const std::string & bin_file, const std::string & bin_ext, const std::string & start_field,
        const std::string & stop_field, const std::string & sn_ratio = "", const std::string & lc_emin = "",
        const std::string & lc_emax = "") const;

    protected:
      typedef std::map<std::string, BinConfig *> ConfigCont;
      static ConfigCont s_config_cont;
  };

}

#endif

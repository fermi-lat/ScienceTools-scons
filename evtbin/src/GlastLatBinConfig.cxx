#include "GlastLatBinConfig.h"

namespace evtbin {

  void GlastLatBinConfig::load() {
    static GlastLatBinConfig s_prototype;
    s_config_cont.insert(std::make_pair("GLAST::LAT", &s_prototype));
  }

  GlastLatBinConfig * GlastLatBinConfig::clone() const {
    return new GlastLatBinConfig(*this);
  }

}

/** \file FitsExtensionData.h
    \brief FITS-specific subclass of IExtensionData.h. Uses the ExtensionData template and the FitsExtensionManager.
*/
#ifndef tip_FitsExtensionData_h
#define tip_FitsExtensionData_h

#include "FitsExtensionManager.h"
#include "tip/IExtensionData.h"
#include "tip/ExtensionData.h"

namespace tip {
  typedef ExtensionData<FitsExtensionManager> FitsExtensionData;
}

#endif

/** \file RootExtensionData.h
    \brief Root-specific subclass of IExtensionData.h. Uses the ExtensionData template and the RootExtensionManager.
*/
#ifndef tip_RootExtensionData_h
#define tip_RootExtensionData_h

#include "RootExtensionManager.h"
#include "tip/ExtensionData.h"

namespace tip {
  typedef ExtensionData<RootExtensionManager> RootExtensionData;
}

#endif

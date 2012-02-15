/**
 * @file FitsSolarProfile.h
 * @brief Access to solar intensity as a function of angle and energy
 * @author G. Johannesson
 *
 * $Header$
 */

#ifndef SolarSystemTools_FitsSolarProfile_h
#define SolarSystemTools_FitsSolarProfile_h

#include "SolarProfile.h"
#include <string>

namespace SolarSystemTools {

/**
 * @class FitsSolarProfile
 * @brief This class encapsulates the access to a solar profile, intensity as a
 * function of angle from the sun and energy.  Adds reading from a fits file
 * from Andy's IC program.
 *
 * @author G. Johannesson
 *
 * $Header$
 */

class FitsSolarProfile : public SolarProfile {

public:

	FitsSolarProfile(const std::string &filename);

};

} //namespace SolarSystemTools

#endif // SolarSystemTools_FitsSolarProfile_h

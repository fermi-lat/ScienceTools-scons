/**
 * @file Dgaus8.h
 * @brief Wrapper for dgaus8.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef st_facilities_Dgaus8_h
#define st_facilities_Dgaus8_h

namespace st_facilities {

class Dgaus8 {

public:

   typedef double (*D_fp)(double*);

   static double integrate(D_fp func, double xmin, double xmax, 
                           double error, long & ier);

};

} // namespace st_facilities

#endif // st_facilities_Dgaus8_h

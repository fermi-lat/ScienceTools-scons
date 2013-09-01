/**
 * @file   quantity.h
 * @brief  Declaration for the Quantity class and the 3 printing functions.
 * These functions should be replaced by dedicated GLAST functions.
 * One symbolic constant is defined, could be replaced by a const variable
 * (5 already exist for basic limits or conversion).
 * @author A. Sauvageon
 *
 * $Header $
 */

#ifndef catalogAccess_quant_h
#define catalogAccess_quant_h

// can compile without first three
//#include <exception>
//#include <string>
//#include <math.h>      //for cos() sin () constant M_PI
#include <limits>     //for std::numeric_limits
#include <iostream>   //for cout, cerr
#include <sstream>    //for ostringstream 
#include <vector>

//#define DEBUG_CAT
#define NO_SEL_CUT  1.79E308    // just below maximum double on 8 bytes
// could use Infinite function isinf() but is it compiling one every platform ?
namespace catalogAccess {

/**********************************************************************/
/*  GLOBAL DEFINITONS for catalogAccess                               */
/**********************************************************************/

static const double Min_Axis = 1./3600.; // just below 0.00028 degree
static const double Min_Prec = std::numeric_limits<double>::epsilon();
static const double NearZero = std::numeric_limits<double>::min();
static const unsigned long Max_Test = std::numeric_limits<unsigned long>::max();
static const double Angle_Conv = M_PI/180.;

enum { IS_OK = 1, IS_VOID = 0,
     IMPORT_BIS = -1, IMPORT_NEED = -2, BAD_CATNAME = -3, BAD_FILENAME = -4,
     BAD_FILETYPE = -5, BAD_FILELINE = -6, BAD_URL = -7, NO_RA_DEC = -8,
   BAD_ROW = -10, BAD_QUANT_NAME = -11, BAD_QUANT_TYPE = -12, NO_QUANT_ERR= -13,
   BAD_RA = -14, BAD_DEC = -15, BAD_ROT = -16, BAD_AXIS = -17,
 BAD_SEL_LIM = -20 };

void printErr(const std::string origin, const std::string text);
void printWarn(const std::string origin, const std::string text);
void printLog(const unsigned char level, const std::string text);

/**
 * @class Quantity
 *
 * @brief Provide only data members for the Catalog member: m_quantities.
 * Only 2 constructors and the destructor are implemented here.
 * @author A. Sauvageon
 *
 * $Header $
 */

class Quantity {

public:

  typedef enum {NUM=1, STRING=2, VECTOR=0} QuantityType;

  Quantity() {                  // Default constructor
    m_type=VECTOR;
    m_index=-1;
    m_isGeneric=false;
    m_toBeLoaded=true;
    m_lowerCut=NO_SEL_CUT; m_upperCut=NO_SEL_CUT;
    m_precision=10*Min_Prec;
    m_rejectNaN=true;
  }
  Quantity(std::string name, std::string comm, std::string ucd,
           QuantityType type, std::string unit, int index=-1,
           bool gene=false, bool load=true,
           double low=NO_SEL_CUT, double up=NO_SEL_CUT) :
    m_name(name), m_comment(comm), m_ucd(ucd), m_type(type),
    m_unit(unit), m_index(index), m_isGeneric(gene), m_toBeLoaded(load),
    m_lowerCut(low), m_upperCut(up) {}
                                // Constructor with arguments
  ~Quantity();                  // Destructor needed to free memory
  Quantity(const Quantity & );  // Copy constructor needed

/**********************************************************************/
  std::string m_name;           // The name of the quantity, e.g., "hr1"
  std::string m_comment;        // e.g. " hardness ratio 1-3keV/3-6keV"
  std::string m_ucd;
      // Unified contents descriptor (if available, "" otherwise)
      // follows either UCD1 standard or UCD1+ (still t.b.d.)
  std::string  m_format;        // format used in CDS database
  QuantityType m_type;
  std::string  m_unit;
      // The unit description, "1" if dimensionless, "" if string
  int m_index;
      // Where to find the quantity in the m_strings or m_numericals
      // catalog members array
  bool m_isGeneric;
      // True if the quantity is part of the set of quantities
      // common to all catalogs
  bool m_toBeLoaded;
      // True if data corresponding to this quantity must be loaded into memory
  std::string m_statError;
      // For numerical quantities: the name of the quantity which contains
      // the statistical error of this quantity;
      // empty if unavailable or for strings
  std::string m_sysError;
      // For numerical quantities: the name of the quantity which contains
      // the systematic error of this quantity;
      // empty if unavailable or for strings
  std::vector<std::string> m_vectorQs;
      // If type == VECTOR, this vector of strings contains the ordered list
      // of quantity names which constitute the vector elements.
      // All vector elements have to be numerical quantities.
      // If type!=VECTOR, vectorQs is empty.


  // selection criteria
  //-------------------

  std::vector<std::string> m_excludedS;
      // for numerical: empty
      // for string   : stores the list of values leading to exclusion of a row
  std::vector<std::string> m_necessaryS;
      // for numerical: empty
      // for string   : stores the list of values neccessary for row inclusion
  double m_lowerCut;  
      // for string   : undefined; default == 1E-99
      // for numerical: stores the minimum value neccessary for row inclusion
  double m_upperCut;
      // for string   : undefined; default == 1E99
      // for numerical: stores the maximum value necessary for row inclusion
  std::vector<double> m_excludedN;
     // for string   : undefined; default: empty
     // for numerical: stores the list of values leading to exclusion of a row
  std::vector<double> m_necessaryN;
     // for string   : empty; default: empty
     // for numerical: stores the list of values necessary for row inclusion

  double m_precision;  // test for equality used for m_excludedN, m_necessaryN
  bool   m_rejectNaN;  // if true the NaN values are not selected

}; // end class definition 


/**********************************************************************/
/*  DEFINING inline FUNCTION MEMBERS                                  */
/**********************************************************************/

// Destructor needed to free memory
inline Quantity::~Quantity() {

  #ifdef DEBUG_CAT
  std::cout << "!! DEBUG Quantity destructor on: "
            << m_name <<" (ucd="<< m_ucd <<")"<< std::endl;
  #endif
  m_vectorQs.clear();
  m_excludedS.clear();
  m_necessaryS.clear();
  m_excludedN.clear();
  m_necessaryN.clear();
}

} // namespace catalogAccess
#endif // catalogAccess_quant_h

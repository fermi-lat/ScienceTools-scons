#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "hoops/hoops.h"
#include "hoops/hoops_par.h"
#include "hoops/hoops_pil_factory.h"
#include "pil.h"

static int sLine;
static int sStatus = hoops::P_OK;
static const int ERROR_UNDETECTED = hoops::P_UNEXPECTED + 1;
static const int BAD_CONVERTED_VALUE = ERROR_UNDETECTED + 1;

int SetGlobalStatus (int status) {
  using namespace hoops;
  if (P_OK == sStatus) {
    if (P_OK == status) sStatus = ERROR_UNDETECTED;
    else sStatus = status;
  }
  return sStatus;
}

int main(int argc, char * argv[]) {
  using namespace hoops;
  const char *code[1024];
  std::string std_string;

  for (int ii = 0; ii < 1024; ++ii) code[ii] = "";
  code[P_OK] = "P_OK";
  code[P_ILLEGAL] = "P_ILLEGAL";
  code[P_OVERFLOW] = "P_OVERFLOW";
  code[P_UNDERFLOW] = "P_UNDERFLOW";
  code[P_BADSIZE] = "P_BADSIZE";
  code[P_PRECISION] = "P_PRECISION";
  code[P_SIGNEDNESS] = "P_SIGNEDNESS";
  code[P_STR_OVERFLOW] = "P_STR_OVERFLOW";
  code[P_STR_INVALID] = "P_STR_INVALID";
  code[P_STR_NULL] = "P_STR_NULL";
  code[P_UNEXPECTED] = "P_UNEXPECTED";
  code[ERROR_UNDETECTED] = "ERROR_UNDETECTED";
  code[BAD_CONVERTED_VALUE] = "BAD_CONVERTED_VALUE";
  code[PAR_INVALID_TYPE] = "PAR_INVALID_TYPE";
  code[PAR_ILLEGAL_CONVERSION] = "PAR_ILLLEGAL_CONVERSION";
  code[PAR_NOT_FOUND] = "PAR_NOT_FOUND";
  code[PAR_FILE_CORRUPT] = "PAR_FILE_CORRUPT";

  std::cout.precision(32);
  std::cerr.precision(32);
  try {
    // Test default constructor.
    std_string = "";
    try {
      Par par_default; std_string = par_default.Value(); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test Par par_default; std_string = par_default.Value() at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("")) {
      std::cerr << "ERROR: Test Par par_default; std_string = par_default.Value() at line " << sLine << " produced result \"" << std_string << "\", not \"" << """" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    // Test constructors for each par type.
    std_string = "";
    try {
      Par par_bool("par_bool", "b", "a", "yes"); std_string = par_bool.Value(); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test Par par_bool(\"par_bool\", \"b\", \"a\", \"yes\"); std_string = par_bool.Value() at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("true")) {
      std::cerr << "ERROR: Test Par par_bool(\"par_bool\", \"b\", \"a\", \"yes\"); std_string = par_bool.Value() at line " << sLine << " produced result \"" << std_string << "\", not \"" << "true" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    std_string = "";
    try {
      Par par_char_p("par_char_p", "s", "a", "Valid"); std_string = par_char_p.Value(); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test Par par_char_p(\"par_char_p\", \"s\", \"a\", \"Valid\"); std_string = par_char_p.Value() at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("Valid")) {
      std::cerr << "ERROR: Test Par par_char_p(\"par_char_p\", \"s\", \"a\", \"Valid\"); std_string = par_char_p.Value() at line " << sLine << " produced result \"" << std_string << "\", not \"" << "Valid" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    std_string = "";
    try {
      Par par_int("par_int", "i", "a", "-2000000000"); std_string = par_int.Value(); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test Par par_int(\"par_int\", \"i\", \"a\", \"-2000000000\"); std_string = par_int.Value() at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("-2000000000")) {
      std::cerr << "ERROR: Test Par par_int(\"par_int\", \"i\", \"a\", \"-2000000000\"); std_string = par_int.Value() at line " << sLine << " produced result \"" << std_string << "\", not \"" << "-2000000000" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    std_string = "";
    try {
      Par par_real("par_real", "r", "a", "-1.2345678e3"); std_string = par_real.Value(); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test Par par_real(\"par_real\", \"r\", \"a\", \"-1.2345678e3\"); std_string = par_real.Value() at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("-1234.5678")) {
      std::cerr << "ERROR: Test Par par_real(\"par_real\", \"r\", \"a\", \"-1.2345678e3\"); std_string = par_real.Value() at line " << sLine << " produced result \"" << std_string << "\", not \"" << "-1234.5678" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    // Test copy constructor.
    Par par_bool1("par_bool", "b", "a", "1"); std_string = "";
    try {
      Par par_bool2 = par_bool1; std_string = par_bool2.Value(); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test Par par_bool2 = par_bool1; std_string = par_bool2.Value() at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("true")) {
      std::cerr << "ERROR: Test Par par_bool2 = par_bool1; std_string = par_bool2.Value() at line " << sLine << " produced result \"" << std_string << "\", not \"" << "true" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    Par par_char_p1("par_char_p", "s", "a", "Valid"); std_string = "";
    try {
      Par par_char_p2 = par_char_p1; std_string = par_char_p2.Value(); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test Par par_char_p2 = par_char_p1; std_string = par_char_p2.Value() at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("Valid")) {
      std::cerr << "ERROR: Test Par par_char_p2 = par_char_p1; std_string = par_char_p2.Value() at line " << sLine << " produced result \"" << std_string << "\", not \"" << "Valid" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    Par par_int1("par_int", "i", "a", "-2000000000"); std_string = "";
    try {
      Par par_int2 = par_int1; std_string = par_int2.Value(); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test Par par_int2 = par_int1; std_string = par_int2.Value() at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("-2000000000")) {
      std::cerr << "ERROR: Test Par par_int2 = par_int1; std_string = par_int2.Value() at line " << sLine << " produced result \"" << std_string << "\", not \"" << "-2000000000" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    Par par_real1("par_real", "r", "a", "-1.2345678e3"); std_string = "";
    try {
      Par par_real2 = par_real1; std_string = par_real2.Value(); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test Par par_real2 = par_real1; std_string = par_real2.Value() at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("-1234.5678")) {
      std::cerr << "ERROR: Test Par par_real2 = par_real1; std_string = par_real2.Value() at line " << sLine << " produced result \"" << std_string << "\", not \"" << "-1234.5678" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    // Test "From" methods.
    try {
      par_bool1.From("0"); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_bool1.From(\"0\") at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (par_bool1.Value().compare("false")) {
      std::cerr << "ERROR: Test par_bool1.From(\"0\") at line " << sLine << " produced result \"" << par_bool1.Value() << "\", not \"" << "false" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    try {
      par_char_p1.From("Invalid"); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_char_p1.From(\"Invalid\") at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (par_char_p1.Value().compare("Invalid")) {
      std::cerr << "ERROR: Test par_char_p1.From(\"Invalid\") at line " << sLine << " produced result \"" << par_char_p1.Value() << "\", not \"" << "Invalid" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    try {
      par_int1.From("-1000000000"); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_int1.From(\"-1000000000\") at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (par_int1.Value().compare("-1000000000")) {
      std::cerr << "ERROR: Test par_int1.From(\"-1000000000\") at line " << sLine << " produced result \"" << par_int1.Value() << "\", not \"" << "-1000000000" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    try {
      par_real1.From("-8.7654321e3"); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_real1.From(\"-8.7654321e3\") at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (par_real1.Value().compare("-8765.4321")) {
      std::cerr << "ERROR: Test par_real1.From(\"-8.7654321e3\") at line " << sLine << " produced result \"" << par_real1.Value() << "\", not \"" << "-8765.4321" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    // Test assignment methods.
    Par par_bool3("par_bool", "b", "a", "yes");
    try {
      par_bool3 = par_bool1; sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_bool3 = par_bool1 at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (par_bool3.Value().compare("false")) {
      std::cerr << "ERROR: Test par_bool3 = par_bool1 at line " << sLine << " produced result \"" << par_bool3.Value() << "\", not \"" << "false" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    Par par_char_p3("par_char_p", "s", "a", "Valid");
    try {
      par_char_p3 = par_char_p1; sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_char_p3 = par_char_p1 at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (par_char_p3.Value().compare("Invalid")) {
      std::cerr << "ERROR: Test par_char_p3 = par_char_p1 at line " << sLine << " produced result \"" << par_char_p3.Value() << "\", not \"" << "Invalid" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    Par par_int3("par_int", "i", "a", "-2000000000");
    try {
      par_int3 = par_int1; sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_int3 = par_int1 at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (par_int3.Value().compare("-1000000000")) {
      std::cerr << "ERROR: Test par_int3 = par_int1 at line " << sLine << " produced result \"" << par_int3.Value() << "\", not \"" << "-1000000000" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    Par par_real3("par_real", "r", "a", "-1.2345678e3");
    try {
      par_real3 = par_real1; sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_real3 = par_real1 at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (par_real3.Value().compare("-8765.4321")) {
      std::cerr << "ERROR: Test par_real3 = par_real1 at line " << sLine << " produced result \"" << par_real3.Value() << "\", not \"" << "-8765.4321" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    // Test "To" methods.
    std_string = "";
    try {
      par_bool1.To(std_string); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_bool1.To(std_string) at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("false")) {
      std::cerr << "ERROR: Test par_bool1.To(std_string) at line " << sLine << " produced result \"" << std_string << "\", not \"" << "false" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    std_string = "";
    try {
      par_char_p1.To(std_string); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_char_p1.To(std_string) at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("Invalid")) {
      std::cerr << "ERROR: Test par_char_p1.To(std_string) at line " << sLine << " produced result \"" << std_string << "\", not \"" << "Invalid" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    std_string = "";
    try {
      par_int1.To(std_string); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_int1.To(std_string) at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("-1000000000")) {
      std::cerr << "ERROR: Test par_int1.To(std_string) at line " << sLine << " produced result \"" << std_string << "\", not \"" << "-1000000000" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    std_string = "";
    try {
      par_real1.To(std_string); sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_real1.To(std_string) at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (std_string.compare("-8765.4321")) {
      std::cerr << "ERROR: Test par_real1.To(std_string) at line " << sLine << " produced result \"" << std_string << "\", not \"" << "-8765.4321" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    // Test assignment methods from primitives. (Not exhaustive).
    par_bool3.From("0");
    try {
      par_bool3 = 1; sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_bool3 = 1 at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (par_bool3.Value().compare("true")) {
      std::cerr << "ERROR: Test par_bool3 = 1 at line " << sLine << " produced result \"" << par_bool3.Value() << "\", not \"" << "true" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    par_int3.From("0");
    try {
      par_int3 = -1.2345678e3; sLine = __LINE__;
      std::cerr << "ERROR: Test par_int3 = -1.2345678e3 at line " << sLine << " did not throw an exception." << std::endl;
      SetGlobalStatus(ERROR_UNDETECTED);
    } catch (const Hexception & x) {
      if (P_PRECISION != x.Code()) {
        std::cerr << "ERROR: Test par_int3 = -1.2345678e3 at line " << sLine << " threw exception " << code[x.Code()] << ", not " << code[P_PRECISION] << "." << std::endl;
        std::cerr << x.what() << std::endl;
        SetGlobalStatus(ERROR_UNDETECTED);
      }
    }
    if (par_int3.Value().compare("-1234")) {
      std::cerr << "ERROR: Test par_int3 = -1.2345678e3 at line " << sLine << " produced result \"" << par_int3.Value() << "\", not \"" << "-1234" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }
    par_real3.From("0");
    try {
      par_real3 = -2.e32; sLine = __LINE__;
    } catch (const Hexception & x) {
      std::cerr << "ERROR: Test par_real3 = -2.e32 at line " << sLine << " threw exception " << code[x.Code()] << "." << std::endl;
      std::cerr << x.what() << std::endl;
      SetGlobalStatus(x.Code());
    }
    if (par_real3.Value().compare("-2e+32")) {
      std::cerr << "ERROR: Test par_real3 = -2.e32 at line " << sLine << " produced result \"" << par_real3.Value() << "\", not \"" << "-2e+32" << "\"." << std::endl;
      SetGlobalStatus(P_UNEXPECTED);
    }

    IParFile * file = PILParFileFactory().NewIParFile(argv[0]);

    file->Load();

    GenParItor it;

    for (it = file->begin(); it != file->end(); ++it) {
      std::cout << ':' << *(*it) << ':' << std::endl;
    }

    IParPrompt * prompt = PILParPromptFactory().NewIParPrompt(argc, argv);

    prompt->Prompt("prompt");
    if (!(bool) prompt->Group()["prompt"]) PILOverrideQueryMode(PIL_QUERY_OVERRIDE);

    prompt->Prompt();

    std::string name;
    for (it = prompt->Group().begin(); it != prompt->Group().end(); ++it) {
      name = (*it)->Name();
      if (name.empty()) continue;
      if ((*it)->Value().compare(file->Group()[name].Value())) {
        std::cout << "Before prompting, parameter " << name <<
          " was :" << file->Group()[name].Value() << ":" << std::endl <<
          "After prompting, value is :" << (*it)->Value() << ":" << std::endl;
      }
    }

    // Copy prompted parameters back to the file object.
    file->Group() = prompt->Group();

    // Save prompted parameters.
    file->Save();

    delete prompt;
    delete file;

  } catch (const Hexception &x) {
    SetGlobalStatus(ERROR_UNDETECTED);
    if (0 <= x.Code() && 1024 > x.Code())
      std::cerr << "An unexpected Hexception " << code[x.Code()] << " was caught at the top level!" << std::endl;
    else
      std::cerr << "An unexpected Hexception " << x.Code() << " was caught at the top level!" << std::endl;
    std::cerr << x.what() << std::endl;
  } catch (...) {
    SetGlobalStatus(ERROR_UNDETECTED);
    std::cerr << "A completely unexpected exception was caught at the top level!" << std::endl;
  }

  // Final status check and report.
  if (P_OK == sStatus)
    std::cout << "Test succeeded." << std::endl;
  else if (0 <= sStatus && 1024 > sStatus)
    std::cerr << "Test failed with error " << code[sStatus] << std::endl;
  else
    std::cerr << "Test failed with error " << sStatus << std::endl;

  return sStatus;
}

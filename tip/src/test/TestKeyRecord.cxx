/** \file TestKeyRecord.cxx
    \brief Implementation of class to perform detailed testing of KeyRecord abstractions.
    \author James Peachey, HEASARC
*/
#include <memory>
#include <string>

#include "TestKeyRecord.h"
#include "fitsio.h"
#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

namespace tip {

  TestKeyRecord::~TestKeyRecord() throw() {}

  /** \brief Perform all detailed tests.
  */
  int TestKeyRecord::test(int status) {
    setStatus(status);

    std::auto_ptr<Table> table(IFileSvc::instance().editTable(getDataDir() + "a1.pha", "SPECTRUM", "#row > 0"));

    Header & hh(table->getHeader());

    // Test records.
    KeyRecord string_rec(formatStringRec("BOZO", "987.", "Bozo is a fine clown"));
    KeyRecord long_comment_string_rec(formatStringRec("BOZO", "987.",  "This comment is 50 characters long, very precisely"));
    KeyRecord num_rec(formatRec("BOZO", "987.", "Bozo is a fine clown"));
    KeyRecord blank_rec("BOZO                           / Bozo is a fine clown");

    try {
      hh["BOZO"].setRecord(string_rec);
      ReportExpected("setting key record did not fail");
    } catch (const std::exception & x) {
      ReportUnexpected("setting key record threw exception", x);
    }

    try {
      KeyRecord read_rec;
      hh["BOZO"].getRecord(read_rec);

      if (read_rec.get() == string_rec.get()) ReportExpected("key record read matched key record written");
      else ReportUnexpected("key record read was\n" + read_rec.get() + ",\nnot\n" + string_rec.get() + ".");
    } catch (const std::exception & x) {
      ReportUnexpected("getting key record threw exception", x);
    }

    // Assign a number to string key record.
    try {
      KeyRecord rec = string_rec;
      rec.setValue("1234567.");
      std::string correct_rec = formatStringRec("BOZO", "1234567.", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of string to string key record worked");
      else ReportUnexpected("after assignment of string to string key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of number to a string key record threw exception", x);
    }

    // Assign a boolean to string key record.
    try {
      KeyRecord rec = string_rec;
      rec.setValue("T");
      std::string correct_rec = formatStringRec("BOZO", "T", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of boolean to string key record worked");
      else ReportUnexpected("after assignment of number to string key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of number to a string key record threw exception", x);
    }

    // Assign a short string with long comment to the same ol' key record.
    try {
      KeyRecord rec = long_comment_string_rec;
      rec.setValue("OK");

      // Comment is 50 characters long.
      std::string correct_rec = formatStringRec("BOZO", "OK", "This comment is 50 characters long, very precisely");

      if (rec.get() == correct_rec) ReportExpected("assignment of short string, long comment to key record worked");
      else ReportUnexpected("after assignment of short string, long comment to key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of short string, long comment to a key record threw exception", x);
    }

    // Assign a medium string with long comment to the same ol' key record.
    try {
      KeyRecord rec = long_comment_string_rec;
      rec.setValue("Value > 10 < 20");
      std::string correct_rec = formatStringRec("BOZO", "Value > 10 < 20", "This comment is 50 characters long, very precisely");

      if (rec.get() == correct_rec) ReportExpected("assignment of medium string, long comment to key record worked");
      else ReportUnexpected("after assignment of medium string, long comment to key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of medium string, long comment to a key record threw exception", x);
    }

    // Assign a long string with long comment to the same ol' key record.
    try {
      KeyRecord rec = long_comment_string_rec;
      rec.setValue("Value > 20 < 60 bonafide characters!");
      std::string correct_rec = formatStringRec("BOZO", "Value > 20 < 60 bonafide characters!",
        "This comment is 50 characters long, very precisely");

      if (rec.get() == correct_rec) ReportExpected("assignment of long string, long comment to key record worked");
      else ReportUnexpected("after assignment of long string, long comment to key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of long string, long comment to a key record threw exception", x);
    }

    // Assign a short string with short comment to the same ol' key record.
    try {
      KeyRecord rec = string_rec;
      rec.setValue("OK");
      std::string correct_rec = formatStringRec("BOZO", "OK", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of short string, short comment to key record worked");
      else ReportUnexpected("after assignment of short string, short comment to key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of short string, short comment to a key record threw exception", x);
    }

    // Assign a medium string with short comment to the same ol' key record.
    try {
      KeyRecord rec = string_rec;
      rec.setValue("Value > 10 < 20");
      std::string correct_rec = formatStringRec("BOZO", "Value > 10 < 20", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of medium string, short comment to key record worked");
      else ReportUnexpected("after assignment of medium string, short comment to key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of medium string, short comment to a key record threw exception", x);
    }

    // Assign a long string with short comment to the same ol' key record.
    try {
      KeyRecord rec = string_rec;
      rec.setValue("Value > 20 < 60 bonafide characters!");
      std::string correct_rec = formatStringRec("BOZO", "Value > 20 < 60 bonafide characters!", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of long string, short comment to key record worked");
      else ReportUnexpected("after assignment of long string, short comment to key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of long string, short comment to a key record threw exception", x);
    }

    // Assign a number to existing numeric record.
    try {
      KeyRecord rec = num_rec;
      rec.setValue("1234567.");
      std::string correct_rec = formatRec("BOZO", "1234567.", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of number to numeric key record worked");
      else ReportUnexpected("after assignment of number to numeric key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of number to numeric key record threw exception", x);
    }

    // Assign a boolean to existing numeric record.
    try {
      KeyRecord rec = num_rec;
      rec.setValue("F");
      std::string correct_rec = formatRec("BOZO", "F", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of boolean to numeric key record worked");
      else ReportUnexpected("after assignment of boolean to numeric key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of boolean to numeric key record threw exception", x);
    }

    // Assign a string to existing numeric record.
    try {
      KeyRecord rec = num_rec;
      rec.setValue("OK");
      ReportUnexpected("assignment of string to numeric key record did not throw an exception");
    } catch (const std::exception & x) {
      ReportExpected("assignment of string to numeric key record threw exception", x);
    }

    // Assign a number to blank record.
    try {
      KeyRecord rec = blank_rec;
      rec.setValue("1234567.");
      std::string correct_rec = formatRec("BOZO", "1234567.", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of number to blank key record worked");
      else ReportUnexpected("after assignment of number to blank key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of number to blank key record threw exception", x);
    }

    // Assign a string to blank record.
    try {
      KeyRecord rec = blank_rec;
      rec.setValue("Not a number");
      std::string correct_rec = formatStringRec("BOZO", "Not a number", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of string to blank key record worked");
      else ReportUnexpected("after assignment of string to blank key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of string to blank key record threw exception", x);
    }

    // Assign a bool to blank record.
    try {
      KeyRecord rec = blank_rec;
      rec.setValue("F");
      std::string correct_rec = formatRec("BOZO", "F", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of boolean to blank key record worked");
      else ReportUnexpected("after assignment of boolean to blank key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of boolean to blank key record threw exception", x);
    }

    // Assign a primitive bool to blank record.
    try {
      KeyRecord rec = blank_rec;
      rec.setValue(true);
      std::string correct_rec = formatRec("BOZO", "T", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of primitive bool to blank key record worked");
      else ReportUnexpected("after assignment of primitive bool to blank key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of primitive bool to blank key record threw exception", x);
    }

    // Assign a primitive bool to numeric record.
    try {
      KeyRecord rec = num_rec;
      rec.setValue(true);
      std::string correct_rec = formatRec("BOZO", "T", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of primitive bool to numeric key record worked");
      else ReportUnexpected("after assignment of primitive bool to numeric key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of primitive bool to numeric key record threw exception", x);
    }

    // Assign a primitive bool to string record.
    try {
      KeyRecord rec = string_rec;
      rec.setValue(true);
      std::string correct_rec = formatStringRec("BOZO", "T", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("assignment of primitive bool to string key record worked");
      else ReportUnexpected("after assignment of primitive bool to string key record, record was\n" + rec.get() + ",\nnot\n" +
        correct_rec + ".");

    } catch (const std::exception & x) {
      ReportUnexpected("assignment of primitive bool to string key record threw exception", x);
    }

    // Create a record from name, value, comment.
    try {
      KeyRecord rec("BOZO", true, "Bozo is a fine clown");
      std::string correct_rec = formatRec("BOZO", "T", "Bozo is a fine clown");

      if (rec.get() == correct_rec) ReportExpected("creation of boolean key record from name, value, comment worked");
      else ReportUnexpected("creation of boolean key record from name, value, comment failed");

    } catch (const std::exception & x) {
      ReportUnexpected("creation of boolean key record from name, value, comment threw exception", x);
    }
    return getStatus();
  }

  std::string TestKeyRecord::formatRec(const std::string & name, const std::string & value, const std::string & comment) {
    std::ostringstream os;

    os.width(8);
    os << std::left << name << "= ";
    os.width(20);
    os << std::right << value << std::left << " / " << comment;

    return os.str();
  }

  std::string TestKeyRecord::formatStringRec(const std::string & name, const std::string & value, const std::string & comment) {
    std::ostringstream os;

    // Start with name at left edge, padded as needed.
    os.width(8);
    os << std::left << name << "= '";

    // Add value, again left justified, padded as needed for fixed format FITS standard keywords.
    os.width(8);
    os << value << "'";

    // Save result so far.
    std::string result = os.str();
    os.str("");

    // Combine comment with its delimiter.
    std::string com = " / " + comment;

    // Want comment to be padded by up to 10 characters on the left, respecting overall length of record.
    os.width(50);
    os << std::left << com;
    com = os.str();
    os.str("");

    os.width(80 - result.size());
    os << std::right << com;
    result += os.str();

    result = result.substr(0, FLEN_CARD - 1);

    int pos = result.find_last_not_of(" ");
    result.erase(pos + 1);

    return result;
  }

}

/**
    \mainpage tip package

    \author  James Peachey peachey@lheamail.gsfc.nasa.gov

    \section intro Introduction
    Provides a generic, iterator-based abstract interface to tabular data.
    Includes file-related abstractions which are independent of the low
    level data format (e.g. FITS or ROOT).

    <hr>
    \section info Information For Clients
    This section describes how client code can and should use the tip
    classes to gain access to tabular data.

\subsection read Reading Tabular Data (First Method)
The first step is for the client to create a data access object
representing the table:

\verbatim
  using namespace tip;
  Table * my_table = IFileSvc::get()::editTable("my_file.fits", "LAT_Event_Sum");
\endverbatim

Table is the name of the main class clients need to worry about.
The expression on the right ("IFileSvc::get()::editTable(...)")
calls a function which opens the given file, moves to the indicated
extension and returns a pointer to a Table object which may
subsequently be used to access the data. (Note to developers:
IFileSvc is a singleton abstract factory, and get() returns a
reference to this factory.)

Next, one might want to read keywords from the header of the table:

\verbatim
  Header & header = my_table->getHeader();
  double tstart;
  header["tstart"].get(tstart);
\endverbatim

The first line creates a reference to my_table's header object.
The second creates a local variable to hold the value of
the TSTART keyword. The third line causes the value of the
TSTART keyword in the table to be copied into the local tstart
variable. (Note to developers: Header::operator [] looks up the
keyword in the header's container and returns a Keyword object.
Keyword's get() method is templated and supported for all primitive
types.)

Another operation of interest is to read values from one or more
columns (or leaves in Root parlance):

\verbatim
  double ph_time_dbl;
  for (Table::Iterator itor = my_table->begin(); itor != my_table->end(); ++itor) {
    // Read the current value.
    itor->get("ph_time", ph_time_dbl);
    // Do something with the current value of ph_time_dbl here.
  }
\endverbatim

The first clause of the for loop declares an object (itor) which
acts as a pointer to a sequence of rows in the table. This
object is initialized to point to the first row of the table
by the second half of the expression ("= my_table->begin()").
The second clause of the for loop ("itor != my_table->end()")
causes the loop to terminate after processing the last row.
The last clause ("++itor") causes the iterator (itor) to go on
to the next value.

Inside the loop, the get method is called to fill the current
value of the ph_time column into the local ph_time_dbl variable.

\subsection read2 Reading Tabular Data (Second Method)
The method above is obviously straightforward. However, there
is a performance penalty associated with the body of the for
loop. Each time a value is filled into the local variable,
the column (or TLeaf) containing the data must be looked up
by name. For many applications this will not matter, but when
performance becomes an issue there is a more efficient way
of implementing the for loop:

\verbatim
  Table::Iterator itor = my_table->begin();
  Table::Scalar<double> & ph_time_ref = (*itor)["ph_time"];
  for (; itor != my_table->end(); ++itor) {
    // Do something with the current value of ph_time_ref here.
  }
\endverbatim

The first line creates an iterator pointing to the beginning
of the table, just like the previous example. The second line
creates a local object (ph_time_ref) which refers to the
iterator's ph_time field. The lookup of which column contains
the ph_time information thus occurs only once, outside the loop.
Inside the loop, each time ph_time_ref is referred to, it will
magically have the correct current value without needing to
search for it again.

\subsection using_values How To Use Tabular Values
In both examples above, the body of the loop did not
include any useful code. In the first case, it is manifestly
clear that one could compute (with built-in double precision
math operators) whatever one wished in the loop using the
ph_time_dbl variable. In the second example, one might
reasonably ask how to compute anything with ph_time_ref. The
answer is that ph_time_ref is an object of a type (class) which
behaves mathematically just like a built-in double type. So
in the for loop in the first example, if one had:

\verbatim
    // Assume corrected_time and offset are both of type double:
    corrected_time = ph_time_dbl - offset;
\endverbatim

one could replace that in the for loop in the second example
by: 

\verbatim
    // corrected_time and offset are both still type double:
    corrected_time = ph_time_ref - offset;
    //                       ^^^
\endverbatim

\subsection read_write Reading and Writing Tabular Data (First Method)
The Table class also supports write access to keywords
using a "set" method:

\verbatim
  using namespace tip;
  double offset = 100000.;
  Table * my_table = IFileSvc::get()::editTable("my_file.fits", "LAT_Event_Sum");

  Header & header = my_table->getHeader();
  double tstart;
  // Get current value:
  header["tstart"].get(tstart);

  // Subtract offset:
  tstart -= offset;

  // Write the modified value back into the table:
  header["tstart"].set(tstart);
\endverbatim

and columns, also using "set":

\verbatim
  double ph_time_dbl;
  for (Table::Iterator itor = my_table->begin(); itor != my_table->end(); ++itor) {
    // Read the current value.
    itor->get("ph_time", ph_time_dbl);

    // Subtract an offset.
    ph_time_dbl -= offset;

    // Write the corrected value:
    itor->set("ph_time", ph_time_dbl);
  }
\endverbatim

\subsection read_write2 Reading and Writing Tabular Data (Second Method)
The second method for reading above also supports writing:

    <hr>
    \section notes Release Notes
    \section requirements Requirements
    There shall be one or more abstractions which manage data files.
    Abstract interfaces shall be used for the API to decouple client
    code from the low level file format. Concrete implementations for FITS
    and ROOT format files shall be provided. However, because the FITS
    format is more restricted in the data structures it can sensibly
    store, the abstract interface shall conform conceptually to the
    FITS format. Restated, there shall be no methods in the abstract
    interface which provide data in a format which cannot easily be
    stored in FITS files. Essentially this means that the file interface
    will provide data in the form of table abstractions which will be
    described below. The purpose of these data file-related classes
    is to encapsulate all low level file access, including buffering,
    reading and writing data, selecting portions of data, etc.

    Abstractions shall also be designed which represent generic
    tabular data stored in a file. As with the file-related classes, abstract
    interfaces shall be used to allow client code transparently
    to access tabular data stored in a file for any supported low level
    file format. The abstract interface shall conform as closely as possible
    to Standard Template Library-style (STL-style) containers in that it
    will provide iterator-based row-oriented access to the table.
    Column-oriented access will also be supported, perhaps as a special
    case of row access in which the row contains a single field. For
    purposes of these abstractions, a table consists of a group of key
    value pairs representing FITS-style keywords, and a group of identical
    records containing one or more fields of arbitrary type and dimension.
    Access to each of these parts, that is, to the keywords (FITS header)
    and records (FITS data unit) shall function independently. Thus, if no key-value
    pairs are present (e.g. in the case of a ROOT file), the records
    in the table shall still be accessible. The fields in the records
    correspond to FITS table columns in the case of FITS files. A FITS
    image may be represented as a table with one field: the image.
    Additional image-specific abstractions may also be needed.

    For efficiency and convenience, it shall be possible to select
    subsets of data from the table-related classes. Client code will thus
    not be required to read the whole record in order to access a particular
    subset of fields.

    The concrete implementations for file-based tables (that is, concrete
    table implementations which are provided by the file-related classes,
    and which remain linked to their underlying files) shall support
    large files by providing transparent buffering as needed for efficient
    access and conservative memory usage. In this way it shall be possible
    for client code to iterate over an entire table of data, treating it
    as if it were all present in memory without explicitly managing
    this memory.

    The file-related interface shall also provide tables which
    are decoupled from the file after they are read. Clients shall be
    able to specify a given range of record (row) numbers, and receive back
    from the file interface a table implementation which was read from
    the table, but no longer relates to it. This is for two purposes.
    First, some tables may be iterated over many times, and the performance
    penalty for repeatedly reading the same information from the file
    may be unacceptable. Second, an algorithm may need to be able to modify
    a table which was read from a read-only file. In this case, it would
    be useful to have a memory-resident, modifiable table representing
    the data which was in the file.

    The file-related interface shall also support the creation of new
    files and the modification of existing files from table objects.
    This mode of access may be stream-like.

    \section plan Design Plan
    The design will be realized in a series of steps. The first step
    is to flesh out the parts of the interfaces which pertain
    to file-based tables, and implement these interfaces for the case
    of FITS files.

    \subsection table Table Class
    This is the heart of the package, and the main interface for table access.
    It contains static methods which can open or create any of the standard
    supported table types. Currently only read-only access to existing
    FITS files is supported. However it is envisioned that Root files will
    be supported, at least for reading. This class also defines Iterator,
    Record and Cell classes. The Iterator class is mostly self-explanatory.
    Dereferencing an Iterator object produces an object of type Record. A
    Record is a container of Cells. Currently it is an associative container,
    associating each Cell with its name. The Cell name corresponds to the field
    in the row, i.e. the column name in a FITS file. The Cell contains methods
    to read and write values to/from the underlying table. (Again currently,
    only reading is supported.) Just like a FITS table, a Cell can contain
    scalar data of a primitive type or character string nature, or vector data.
    Client code can request from a Cell that it read data in any of these
    forms, and if possible, the request will be satisfied.

    \subsection table Table::Scalar Class
    Templated class which serves as an adaptor for the Cell class. It may
    be templated on any primitive type or string type (currently only type
    double will work.) A Scalar<T> object binds itself to a Cell reference, and
    provides a conversion operator to type T, and assignment from type T. This
    allows client code to use a Scalar<T> object as if it were of type T, but the
    Scalar<T> object will read/write values from the table.

    \section jobOptions jobOptions

    \section todo To do list:
    1. 4/2/2004: Memory leak in IFileSvc::editTable. See IFileSvc.cxx: TODO 1.
    2. 4/2/2004: Access should also be possible by extension number as well as name.
    3. 4/2/2004: For read-only access, need const Table * IFileSvc::readTable() and
       typedef Table::ConstIterator begin() const etc.
    4. 4/2/2004: This document's examples include some mistakes! They should be redone
       based on current live sample code.
    5. 4/2/2004: In RootExtensionManager::open() there are calls to Root's
       dynamic loader, which should be eliminated if possible by a requirements pattern.
       See RootExtensionManager:cxx: TODO 5.
    6. 4/2/2004: Add method to Table to facilitate "browsing", e.g. getting a list of
       fields in the table (as distinct from whatever happens to be in a Record).
    7. 4/2/2004: Table's iterator has problems with random access. See Table.h: TODO 7.
    8. 4/2/2004: Use of Record within a functor used with a for_each is problematic.
       See src/test/test_tip_main.cxx: TODO 8.
    9. 4/2/2004: Bug in certain versions of cfitsio may prevent filtering syntax from
       working. See FitsExtensionManager.cxx: TODO 9.

*/

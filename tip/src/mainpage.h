/**
    \mainpage table package

    \author  James Peachey peachey@lheamail.gsfc.nasa.gov

    \section intro Introduction
    Provides a generic, iterator-based abstract interface to tabular data.
    Includes file-related abstractions which are independent of the low
    level data format (e.g. FITS or ROOT).

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
    the data which was in the file. Of course, this imposes the burden
    of managing the memory on the client code.

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

    \subsection table Ref Class
    Templated class which serves as an adaptor for the Cell class. It may
    be templated on any primitive type or string type (currently only type
    double will work.) A Ref<T> object binds itself to a Cell reference, and
    provides a conversion operator to type T, and assignment from type T. This
    allows client code to use a Ref<T> object as if it were of type T, but the
    Ref<T> object will read/write values from the table.

    \section jobOptions jobOptions
*/

/**
 * @file libStApiExports.h
 * @brief Header file to export classes with static symbols from DLLs on
 * Windows.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef st_facilities_libStApiExports_h
#define st_facilities_libStApiExports_h

// The following is based on header file code by Matt Langston
// appearing in various places, e.g., libhippo.h in HippoDraw,
// libApiExports.h in Gleam(?), etc.
//
// The following ifdef block is the standard way of exporting symbols
// (e.g. class definitions) from a DLL on Windows. The macro
// ST_DLL_EXPORTS is defined when source code is compiled to create
// the DLL, which causes SCIENCETOOLS_API to expand to
// __declspec(dllexport) so that symbols decorated with
// SCIENCETOOLS_API are exported from the DLL.
//
// Clients of our DLL don't have to do anything special to use it,
// since they won't define ST_DLL_EXPORTS, which means
// SCIENCETOOLS_API will expand to __declspec(dllimport) so that
// symbols decorated with SCIENCETOOLS_API are imported from the DLL.

#if (defined(_WIN32) && defined(_MSC_VER) && (_MSC_VER<1400))
# ifndef SCons
#   ifdef ST_DLL_EXPORTS
#    undef  SCIENCETOOLS_API
#    define SCIENCETOOLS_API //__declspec(dllexport)
#   else
#    undef  SCIENCETOOLS_API
#    define SCIENCETOOLS_API //__declspec(dllimport)
#   endif
# else
#  undef  SCIENCETOOLS_API
#  define SCIENCETOOLS_API //__declspec(dllimport)
# endif
#else
// The gcc compiler (i.e. the Linux/Unix compiler) exports the Universe
// of symbols from a shared library, meaning that we can't control the
// API of our shared libraries. We therefore just define the Symbol
// Export Macro to expand to nothing.
// Or if on Windows but using a static library (neither IMPORTS nor EXPORTS)
// then we don't need this goofy declspec stuff.
# undef  SCIENCETOOLS_API
# define SCIENCETOOLS_API
#endif

#endif // st_facilities_libStApiExports_h

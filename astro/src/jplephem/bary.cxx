#define BARY_CONST 1
#include "bary.h"
#undef BARY_CONST

/*-----------------------------------------------------------------------
 *
 *  openFFile finds one of the FITS system files and opens it for reading.
 *  fitsfile *openFFile (const char *name)
 *   name:     Name of the file
 *   
 *   return:   FITS file pointer, NULL if not found
 *
 *----------------------------------------------------------------------*/
fitsfile *openFFile (const char *name)
{
   char fileName[1024];
   fitsfile *FF = NULL;
   int error = 0;
#if 0 // first mod by T. Hierath
   const char* astro_root = ::getenv("ASTROROOT");
   sprintf(fileName, "%s/%s/%s", astro_root, "src/jplephem", name);
#else // second mod
      const char* extfiles_root = ::getenv("EXTFILESSYS");
   sprintf(fileName, "%s/%s/%s", extfiles_root, "jplephem", name);
#endif
   fits_open_file(&FF, fileName, READONLY, &error);
   if(FF == NULL)
      fprintf (stderr, "File %s could not be found in %s\n", fileName, "jplephem") ;
   

  return FF ;
}


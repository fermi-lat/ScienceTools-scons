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
   const char* astro_root = ::getenv("ASTROROOT");
   sprintf(fileName, "%s/%s/%s", astro_root, "src/jplephem", name);
   fits_open_file(&FF, fileName, READONLY, &error);
   if(FF == NULL)
      fprintf (stderr, "File %s could not be found in %s\n", fileName, "jplephem") ;
   

  return FF ;
}


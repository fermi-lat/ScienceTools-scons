#include "math.h"
#include "bary.h"

/* Tolerance of 1 millisecond in checking time boundaries */
static double time_tolerance = 1.e-3; /* in units of seconds */
/* Notes on the time tolerance
/*   Masaharu Hirayama, GSSC
/*   October 20th, 2009
/*
/* Rationale for 1-millisecond tolerance:
/* o The time duration of 1 millisecond is more than two orders of
/*   magnitude longer than the time resolution of Fermi time stamps (3-10
/*   microseconds). Therefore, if two time stamps are separate by more
/*   than 1 millisecond, one can safely assume that they represent two
/*   different points in time.
/* o The Fermi spacecraft travels approximately 25 nano-light-seconds in
/*   1 millisecond, and that can create a difference in a photon arrival
/*   time of 25 nanoseconds at most, which is more than 2 orders of
/*   magnitude shorter than the time resolution of Fermi time stamp (3-10
/*   microseconds). Therefore, it is highly unlikely that extrapolation
/*   of spacecraft positions at a boundary of FT2 coverage for 1
/*   millisecond will create a significant difference in geocentric or
/*   barycentric times.
/*
/* Related fact:
/* o The time duration of 25 nanoseconds is even shorter than (but of the
/*   same order of magnitude of) the computation precision guaranteed by
/*   the barycentering function ctatv.c (100 ns). This means that
/*   1-millisecond tolerance is as acceptable as use of (the current
/*   version of) ctatv.c.
 */

/* Compare two time intervals.
/* Note: This expression becomes true if interval "x" is earlier than interval "y",
/*       and false if otherwise.  Interval "x" is defined by a time range [x[0], x[1])
/*       for x[0] < x[1], or (x[1], x[0]] for other cases.
 */
#define is_less_than(x, y) (x[0] < y[0] && x[1] <= y[0] && x[0] <= y[1] && x[1] <= y[1])

/* Comparison function to be passed to "bsearch" function. */
static int compare_interval(const void * a, const void * b)
{
  double *a_ptr = (double *)a;
  double *b_ptr = (double *)b;

  if (is_less_than(a_ptr, b_ptr)) return -1;
  else if (is_less_than(b_ptr, a_ptr)) return 1;
  else return 0;
}

/* Compute vector inner product. */
static double inner_product(double vect_x[], double vect_y[])
{
  return vect_x[0]*vect_y[0] + vect_x[1]*vect_y[1] + vect_x[2]*vect_y[2];
}

/* Compute vector outer product. */
static void outer_product(double vect_x[], double vect_y[], double vect_z[])
{
  vect_z[0] = vect_x[1]*vect_y[2] - vect_x[2]*vect_y[1];
  vect_z[1] = vect_x[2]*vect_y[0] - vect_x[0]*vect_y[2];
  vect_z[2] = vect_x[0]*vect_y[1] - vect_x[1]*vect_y[0];
}

/* Structure to hold the spacecraft file information. */
typedef struct {
  fitsfile *fits_ptr;
  long num_rows;
  int colnum_scposn;
  double *sctime_array;
  long sctime_array_size;
  char * filename;
  char * extname;
  int open_count;
} GlastScData;

static GlastScData ** ScDataTable = NULL;
static const int NMAXFILES = 300; /* Same as in fitio2.h */

typedef struct {
  GlastScData * scdata;
  int status;
} GlastScFile;

int glastscorbit_getstatus(GlastScFile * scfile) {
  if (scfile) return scfile->status;
  else return 0;
}

int glastscorbit_outofrange(GlastScFile * scfile) {
  if (scfile) return (scfile->status < 0);
  else return 0;
}

void glastscorbit_clearerr(GlastScFile * scfile) {
  if (scfile) scfile->status = 0;
}

static int close_scfile(GlastScFile * scfile)
{
  int close_status = 0;

  /* Do nothing if no spacecraft file information is available. */
  if (NULL == scfile) return 0;
  if (NULL == scfile->scdata) {
    /* Override the existing error with closing status. */
    scfile->status = 0;
    return scfile->status;
  }

  /* Close the opened file. */
  if (NULL != scfile->scdata->fits_ptr) {
    fits_close_file(scfile->scdata->fits_ptr, &close_status);
    scfile->scdata->fits_ptr = NULL;
  }
  scfile->scdata->num_rows = 0;
  scfile->scdata->colnum_scposn = 0;

  /* Free the allocated memory space for "START" column. */
  free(scfile->scdata->sctime_array);
  scfile->scdata->sctime_array = NULL;
  scfile->scdata->sctime_array_size = 0;

  /* Free the allocated memory space for names. */
  free(scfile->scdata->filename);
  scfile->scdata->filename = NULL;
  free(scfile->scdata->extname);
  scfile->scdata->extname = NULL;

  /* Free memory space for this pointer. */
  free(scfile->scdata);
  scfile->scdata = NULL;

  /* Override the existing error with closing status. */
  scfile->status = close_status;

  /* Return closing status. */
  return close_status;
}

/* Close the spacecraft file and clean up the initialized items. */
/* TODO: Describe function argument and return value. */
int glastscorbit_close(GlastScFile * scfile)
{
  GlastScData * scdata = NULL;
  int close_status = 0;
  int ii = 0;

  /* Do nothing if no spacecraft file information is available. */
  if (NULL == scfile) return 0;

  /* Remove spacecraft data if necessary. */
  scdata = scfile->scdata;
  if (scdata) {
    /* Decrement the file open counter. */
    scdata->open_count--;

    /* Destroy this pointer if the file open counter is zero. */
    if (scdata->open_count > 0) {
      /* Strip access to the spacecraft data. */
      scfile->scdata = NULL;

    } else {
      /* Remove this pointer from the pointer table. */
      if (ScDataTable) {
        for (ii = 0; ii < NMAXFILES; ++ii) {
          if (ScDataTable[ii] == scdata) ScDataTable[ii] = NULL;
        }
      }

      /* Close spacecraft file and free all the allocated memory spaces. */
      close_status = close_scfile(scfile);
    }
  }

  /* Override the existing status with closing status. */
  scfile->status = close_status;

  /* Remove the spacecraft file pointer. */
  free(scfile);

  /* Return closing status. */
  return close_status;
}

/* Open the given spacecraft file and initialize global variables. */
GlastScFile * glastscorbit_open(char *filename, char *extname)
{
  GlastScFile * scfile = NULL;
  GlastScData * scdata = NULL;
  int colnum_start = 0;
  int open_status = 0;
  int ii = 0;

  /* Create an object to return, and initialize the contents. */
  scfile = malloc(sizeof(GlastScFile));
  if (NULL == scfile) return scfile;
  scfile->scdata = NULL;
  scfile->status = 0;

  /* Check the pointer arguments. */
  if (NULL == filename || NULL == extname) {
    scfile->status = FILE_NOT_OPENED;
    return scfile;
  }

  /* Check whether the file is already open. */
  if (ScDataTable) {
    /* Look for an opened file with the same filename and the same extension name. */
    for (ii = 0; ii < NMAXFILES; ++ii) {
      scdata = ScDataTable[ii];
      if (scdata && !strcmp(filename, scdata->filename) && !strcmp(extname, scdata->extname)) {
        scdata->open_count++;
        scfile->scdata = scdata;
        return scfile;
      }
    }
  } else {
    /* Initialize the pointer table for opened spacecraft files. */
    ScDataTable = malloc(sizeof(GlastScFile *) * NMAXFILES);
    if (NULL == ScDataTable) {
      scfile->status = MEMORY_ALLOCATION;
      return scfile;
    }
    for (ii = 0; ii < NMAXFILES; ++ii) ScDataTable[ii] = NULL;
  }

  /* Allocate memory space for spacecraft file information, and initialize the members. */
  scfile->scdata = malloc(sizeof(GlastScData));
  if (NULL == scfile->scdata) {
    scfile->status = MEMORY_ALLOCATION;
    return scfile;
  }
  scdata = scfile->scdata;
  scdata->fits_ptr = NULL;
  scdata->num_rows = 0;
  scdata->colnum_scposn = 0;
  scdata->sctime_array = NULL;
  scdata->sctime_array_size = 0;
  scdata->filename = NULL;
  scdata->extname = NULL;
  scdata->open_count = 0;

  /* Allocate memory space for filename and extension name, and keep their copies. */
  scdata->filename = malloc(sizeof(char) * (strlen(filename) + 1));
  scdata->extname = malloc(sizeof(char) * (strlen(extname) + 1));
  if (scdata->filename && scdata->extname) {
    strcpy(scdata->filename, filename);
    strcpy(scdata->extname, extname);
  } else {
    open_status = MEMORY_ALLOCATION;
  }

  /* Open the given file, move to the spacecraft data, and read table information. */
  fits_open_file(&(scdata->fits_ptr), filename, 0, &open_status);
  fits_movnam_hdu(scdata->fits_ptr, ANY_HDU, extname, 0, &open_status);
  fits_get_num_rows(scdata->fits_ptr, &(scdata->num_rows), &open_status);
  fits_get_colnum(scdata->fits_ptr, CASEINSEN, "START", &colnum_start, &open_status);
  fits_get_colnum(scdata->fits_ptr, CASEINSEN, "SC_POSITION", &(scdata->colnum_scposn), &open_status);

  /* Require two rows at the minimum, for interpolation to work. */
  if (0 == open_status && scdata->num_rows < 2) {
    /* Return BAD_ROW_NUM because it would read the second row
       which does not exist in this case. */
    open_status = BAD_ROW_NUM;
  }

  /* Allocate memory space to cache "START" column. */
  if (0 == open_status) {
    scdata->sctime_array = malloc(sizeof(double) * scdata->num_rows);
    if (NULL == scdata->sctime_array) {
      scdata->sctime_array_size = 0;
      open_status = MEMORY_ALLOCATION;
    } else {
      scdata->sctime_array_size = scdata->num_rows;
    }
  }

  /* Read "START" column. */
  fits_read_col(scdata->fits_ptr, TDOUBLE, colnum_start, 1, 1, scdata->num_rows, 0, scdata->sctime_array, 0, &open_status);

  /* Register the opened file to the pointer table. */
  if (0 == open_status) {
    /* Find an open space in the pointer table. */
    for (ii = 0; ii < NMAXFILES; ++ii) if (NULL == ScDataTable[ii]) break;
    if (ii < NMAXFILES) {
      /* Add this file to the pointer table, and start the open counnter. */
      ScDataTable[ii] = scdata;
      scdata->open_count = 1;
    } else {
      open_status = MEMORY_ALLOCATION;
    }
  }

  /* Finally check errors in opening file. */
  if (open_status) {
    /* Close spacecraft file and free all the allocated memory spaces. */
    close_scfile(scfile);

    /* Ignore an error in closing file, and report the error in opening it. */
    scfile->status = open_status;
  }

  /* Return the successfully opened spacecraft file. */
  return scfile;
}

/* Read spacecraft positions from file and returns interpolated position. */
int glastscorbit_calcpos(GlastScFile * scfile, double t, double intposn[3])
{
  GlastScData * scdata = NULL;
  double evtime_array[2];
  double *sctime_ptr = NULL;
  long scrow1 = 0;
  long scrow2 = 0;
  double sctime1 = 0.;
  double sctime2 = 0.;
  double scposn1[3] = { 0., 0., 0. };
  double scposn2[3] = { 0., 0., 0. };
  double fract = 0.;
  int ii = 0;

  /* Clear the previously stored values. */
  for (ii = 0; ii < 3; ++ii) intposn[ii] = 0.0;

  /* Do nothing if no spacecraft file information is available. */
  if (NULL == scfile) return FILE_NOT_OPENED;
  if (NULL == scfile->scdata) {
    /* Override the existing error with this status. */
    scfile->status = FILE_NOT_OPENED;
    return scfile->status;
  }

  /* Do nothing if an error occurred previously on this file. */
  if (scfile->status) return scfile->status;

  /* Copy the pointer to the spacecraft data for better readability. */
  scdata = scfile->scdata;

  /* Find two neighboring rows from which the spacecraft position at
     the given time will be computed. */
  if (fabs(t - scdata->sctime_array[0]) <= time_tolerance) {
    /* In this case, the given time is close enough to the time in the
       first row within the given tolerance.  So, use the first two
       rows for the computation. */
    scrow1 = 1;
    scrow2 = 2;
    sctime1 = scdata->sctime_array[0];
    sctime2 = scdata->sctime_array[1];

  } else if (fabs(t - scdata->sctime_array[scdata->num_rows-1]) <= time_tolerance) {
    /* In this case, the given time is close enough to the time in the
       final row within the given tolerance.  So, use the penultimate
       row and the final row for the computation. */
    scrow1 = scdata->num_rows - 1;
    scrow2 = scdata->num_rows;
    sctime1 = scdata->sctime_array[scdata->num_rows - 2];
    sctime2 = scdata->sctime_array[scdata->num_rows - 1];

  } else {
    evtime_array[0] = evtime_array[1] = t;
    sctime_ptr = (double *)bsearch(evtime_array, scdata->sctime_array, scdata->num_rows - 1, sizeof(double), compare_interval);
    if (NULL == sctime_ptr) {
      /* In this case, the given time is out of bounds. */
      scfile->status = -2;
      return scfile->status;
    }

    /* In this case, the given time is between the first and the
       final rows, so use the row returned by bsearch and the next
       row for the computation. */
    scrow1 = sctime_ptr - scdata->sctime_array + 1;
    scrow2 = scrow1 + 1;
    sctime1 = sctime_ptr[0];
    sctime2 = sctime_ptr[1];
  }

  /* Read "SC_POSITION" column in the two rows found above. */
  fits_read_col(scdata->fits_ptr, TDOUBLE, scdata->colnum_scposn, scrow1, 1, 3, 0, scposn1, 0, &(scfile->status));
  fits_read_col(scdata->fits_ptr, TDOUBLE, scdata->colnum_scposn, scrow2, 1, 3, 0, scposn2, 0, &(scfile->status));

  /* Return if an error occurs while reading "SC_POSITION" columns. */
  if (scfile->status) return scfile->status;

  /* Interpolate. */
  fract = (t - sctime1) / (sctime2 - sctime1);
  {
    double length1, length2, length12, intlength;
    double vector12[3], vectprod_out[3];
    
    /* Linear interpolation for vector length. */
    length1 = sqrt(inner_product(scposn1, scposn1));
    length2 = sqrt(inner_product(scposn2, scposn2));
    intlength = length1 + fract*(length2 - length1);

    /* Compute a base vector on the orbital plane (vector12). */
    outer_product(scposn1, scposn2, vectprod_out);
    outer_product(vectprod_out, scposn1, vector12);
    length12 = sqrt(inner_product(vector12, vector12));

    /* Check vectors scposn1 and scposn2. */
    if ((length1 == 0.0) && (length2 == 0.0)) {
      /* both vectors are null */
      for (ii = 0; ii < 3; ++ii) intposn[ii] = 0.0;

    } else if (length1 == 0.0) {
      /* scposn1 is null, but scposn2 is not */
      for (ii = 0; ii < 3; ++ii) {
	intposn[ii] = scposn2[ii] / length2 * intlength;
      }

    } else if ((length2 == 0.0) || (length12 == 0.0)) {
      /* left:  scposn2 is null, but scposn1 is not */
      /* right: either vector is not null, but they are parallel */
      for (ii = 0; ii < 3; ++ii) {
	intposn[ii] = scposn1[ii] / length1 * intlength;
      }

    } else { /* Both has a non-zero length, and they are not parallel. */
      double inttheta, factor_cos, factor_sin;
      /* Linear interpolation for orbital phase. */
      inttheta = fract * acos(inner_product(scposn1, scposn2)
			      / length1 / length2);
      factor_cos = cos(inttheta);
      factor_sin = sin(inttheta);
      for (ii = 0; ii < 3; ++ii) {
	intposn[ii] = intlength * (scposn1[ii] / length1 * factor_cos 
				   + vector12[ii] / length12 * factor_sin);
      }
    }
  }

  /* Return the computed spacecraft position. */
  return scfile->status;
}

/* Wrapper function to read spacecraft positions from file and returns interpolated position,
/* for backward compatibility.
 */
double * glastscorbit(char *filename, double t, int *oerror)
{
  static double intposn[3] = {0., 0., 0.};
  static double dummy_scposn[3] = {0., 0., 0.};
  static char savefile[256] = " ";
  static GlastScFile * scfile = NULL;

  /* Override error status to preserve the original behavior. */
  *oerror = 0;
  glastscorbit_clearerr(scfile);

  /* Check whether a new file is given. */
  if (strcmp(savefile, filename)) {
    /* Close the previously opened spacecraft file, if any. */
    glastscorbit_close(scfile);

    /* Open file and prepare for reading the spacecraft position. */
    scfile = glastscorbit_open(filename, "SC_DATA");

    /* Copy file opening status. */
    *oerror = scfile->status;

    /* Refresh the saved file name. */
    if (0 == *oerror) {
      strcpy(savefile, filename);
    } else {
      strcpy(savefile, " ");
      return dummy_scposn;
    }
  }

  /* Compute and return the spacecraft position at the given time. */
  *oerror = glastscorbit_calcpos(scfile, t, intposn);
  return intposn;
}

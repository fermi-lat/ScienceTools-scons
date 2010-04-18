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
} GlastScFile;

/* Close the spacecraft file and clean up the initialized items. */
void glastscorbit_close(GlastScFile * scptr, int *oerror)
{
  /* Clear the previously stored value. */
  *oerror = 0;

  /* Do nothing if no spacecraft file information is available. */
  if (NULL == scptr) return;

  /* Close the opened file and reset global variables. */
  if (NULL != scptr->fits_ptr) {
    fits_close_file(scptr->fits_ptr, oerror);
    scptr->fits_ptr = NULL;
  }
  scptr->num_rows = 0;
  scptr->colnum_scposn = 0;

  /* Free the allocated memory space and reset global variables. */
  free(scptr->sctime_array);
  scptr->sctime_array = NULL;
  scptr->sctime_array_size = 0;

  /* Free memory space for the spacecraft file information. */
  free(scptr);
}

/* Open the given spacecraft file and initialize global variables. */
GlastScFile * glastscorbit_open(char *filename, char *extname, int *oerror)
{
  GlastScFile * scptr = NULL;
  int colnum_start = 0;

  /* Clear the previously stored value. */
  *oerror = 0;

  /* Allocate memory space for spacecraft file information, and initialize the members. */
  scptr = malloc(sizeof(GlastScFile));
  if (NULL == scptr) {
    *oerror = MEMORY_ALLOCATION;
    return scptr;
  } else {
    scptr->fits_ptr = NULL;
    scptr->num_rows = 0;
    scptr->colnum_scposn = 0;
    scptr->sctime_array = NULL;
    scptr->sctime_array_size = 0;
  }

  /* Open the given file, move to the spacecraft data, and read table information. */
  fits_open_file(&(scptr->fits_ptr), filename, 0, oerror);
  fits_movnam_hdu(scptr->fits_ptr, ANY_HDU, extname, 0, oerror);
  fits_get_num_rows(scptr->fits_ptr, &(scptr->num_rows), oerror);
  fits_get_colnum(scptr->fits_ptr, CASEINSEN, "START", &colnum_start, oerror);
  fits_get_colnum(scptr->fits_ptr, CASEINSEN, "SC_POSITION", &(scptr->colnum_scposn), oerror);

  /* Require two rows at the minimum, for interpolation to work. */
  if (0 == *oerror && scptr->num_rows < 2) {
    /* Return BAD_ROW_NUM because it would read the second row
       which does not exist in this case. */
    *oerror = BAD_ROW_NUM;
  }

  /* Allocate memory space to cache "START" column. */
  if (0 == *oerror) {
    scptr->sctime_array = malloc(sizeof(double) * scptr->num_rows);
    if (NULL == scptr->sctime_array) {
      scptr->sctime_array_size = 0;
      *oerror = MEMORY_ALLOCATION;
    } else {
      scptr->sctime_array_size = scptr->num_rows;
    }
  }

  /* Read "START" column. */
  fits_read_col(scptr->fits_ptr, TDOUBLE, colnum_start, 1, 1, scptr->num_rows, 0, scptr->sctime_array, 0, oerror);

  /* Close file on error(s) */
  if (*oerror) {
    int close_status = 0; /* Ignore an error in closing file */
    fits_close_file(scptr->fits_ptr, &close_status);
    scptr->fits_ptr = NULL;
  }

  /* Return the spacecraft file information. */
  return scptr;
}

/* Read spacecraft positions from file and returns interpolated position. */
double *glastscorbit_calcpos(GlastScFile * scptr, double t, int *oerror)
{
  static double intposn[3];
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

  /* Do nothing if no spacecraft file information is available. */
  if (NULL == scptr) return;

  /* Clear the previously stored values. */
  *oerror = 0;
  for (ii = 0; ii < 3; ++ii) intposn[ii] = 0.0;

  /* Find two neighboring rows from which the spacecraft position at
     the given time will be computed. */
  if (fabs(t - scptr->sctime_array[0]) <= time_tolerance) {
    /* In this case, the given time is close enough to the time in the
       first row within the given tolerance.  So, use the first two
       rows for the computation. */
    scrow1 = 1;
    scrow2 = 2;
    sctime1 = scptr->sctime_array[0];
    sctime2 = scptr->sctime_array[1];

  } else if (fabs(t - scptr->sctime_array[scptr->num_rows-1]) <= time_tolerance) {
    /* In this case, the given time is close enough to the time in the
       final row within the given tolerance.  So, use the penultimate
       row and the final row for the computation. */
    scrow1 = scptr->num_rows - 1;
    scrow2 = scptr->num_rows;
    sctime1 = scptr->sctime_array[scptr->num_rows - 2];
    sctime2 = scptr->sctime_array[scptr->num_rows - 1];

  } else {
    evtime_array[0] = evtime_array[1] = t;
    sctime_ptr = (double *)bsearch(evtime_array, scptr->sctime_array, scptr->num_rows - 1, sizeof(double), compare_interval);

    if (NULL == sctime_ptr) {
      /* In this case, the given time is out of bounds. */
      *oerror = -2;
      return intposn;

    } else {
      /* In this case, the given time is between the first and the
         final rows, so use the row returned by bsearch and the next
         row for the computation. */
      scrow1 = sctime_ptr - scptr->sctime_array + 1;
      scrow2 = scrow1 + 1;
      sctime1 = sctime_ptr[0];
      sctime2 = sctime_ptr[1];
    }
  }

  /* Read "SC_POSITION" column in the two rows found above. */
  fits_read_col(scptr->fits_ptr, TDOUBLE, scptr->colnum_scposn, scrow1, 1, 3, 0, scposn1, 0, oerror);
  fits_read_col(scptr->fits_ptr, TDOUBLE, scptr->colnum_scposn, scrow2, 1, 3, 0, scposn2, 0, oerror);

  /* Return if an error occurs while reading "SC_POSITION" columns. */
  if (*oerror) return intposn;

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
  return intposn;
}

/* Wrapper function to read spacecraft positions from file and returns interpolated position,
/* for backward compatibility.
 */
double *glastscorbit(char *filename, double t, int *oerror)
{
  static char savefile[256] = " ";
  static double dummy_scposn[3] = {0., 0., 0.};
  static GlastScFile * scptr = NULL;

  /* Check whether a new file is given. */
  if (strcmp(savefile, filename)) {
    /* Close the previously opened spacecraft file, if any. */
    glastscorbit_close(scptr, oerror);

    /* Open file and prepare for reading the spacecraft position. */
    scptr = glastscorbit_open(filename, "SC_DATA", oerror);

    /* Refresh the saved file name. */
    if (0 == *oerror) {
      strcpy(savefile, filename);
    } else {
      strcpy(savefile, " ");
      return dummy_scposn;
    }
  }

  /* Compute and return the spacecraft position at the given time. */
  return glastscorbit_calcpos(scptr, t, oerror);
}

#include <cmath>
#include <string>
#include <vector>
#include <valarray>
#include <iostream>

#include "fitsio.h"

#include "astro/SkyDir.h"

#include "latResponse/../src/AeffGlast25.h"

#include "Likelihood/Table.h"

void writeExposureFile(const std::string &filename, 
                       const std::vector<double> &glon,
                       const std::vector<double> &glat,
                       const std::vector<double> &energies,
                       const std::valarray<double> &exposure);

void fitsReportError(FILE *stream, int status);

int main(int argc, char *argv[]) {

// The GLAST25 effective area:
   std::string aeff_file;
   const char * latResponsePath = ::getenv("LATRESPONSEROOT");
   if (!latResponsePath) {
      std::cerr << "latResponse path not found.  Aborting." << std::endl;
      exit(0);
   } else {
      aeff_file = std::string(latResponsePath) + "/data/CALDB/aeff_lat.fits";
   }
   latResponse::AeffGlast25 aeff(aeff_file, latResponse::Glast25::Combined);

// The range of the exposure map in Galactic coordinates:
   double lmin, lmax, bmin, bmax;
   std::string scDataFile;

// Parse the command-line arguments.
   if ( argc == 1 || (argc != 2 && argc != 6) ) {
      std::cerr << "usage: exposureMap ScData_file "
                << "[lmin lmax bmin bmax]" << std::endl;
      return 0;
   } else if (argc >= 2) {
      scDataFile = std::string(argv[1]);
      if (argc == 2) {
         lmin = -180;
         lmax = 180;
         bmin = -90;
         bmax = 90;
      } else {
         lmin = static_cast<double>(::atof(argv[2]));
         lmax = static_cast<double>(::atof(argv[3]));
         bmin = static_cast<double>(::atof(argv[4]));
         bmax = static_cast<double>(::atof(argv[5]));
      }
   }

// Map positions in Galactic coordinates:
//   double degstep = 0.5;  // use a half-degree step size
   double degstep = 1.;  // use a degree step size

   std::vector<double> glon;
   int nglon = int( (lmax - lmin)/degstep );
   for (int i=0; i < nglon; i++) {
      glon.push_back(degstep*i + lmin);
   }

   std::vector<double> glat;
   int nglat = int( (bmax - bmin)/degstep );
   for (int i=0; i < nglat; i++) {
      glat.push_back(degstep*i + bmin);
   }

// Prepare unit vectors of source positions.
   std::vector<astro::SkyDir> srcDirs;
   std::vector<double>::const_iterator glonIt = glon.begin();
   for (int i=0; glonIt != glon.end(); glonIt++, i++) {
      std::vector<double>::const_iterator glatIt = glat.begin();
      for (int j=0; glatIt != glat.end(); glatIt++, j++) {
         srcDirs.push_back( astro::SkyDir(*glonIt, *glatIt, 
                                          astro::SkyDir::GALACTIC) );
      }
   }

// Assume a standard set of energy bands.
   double emin = 30.;
   double emax = 1e5;
   int nee = 10;
   double estep = log(emax/emin)/(nee-1);
   std::vector<double> energies;
   for (int i=0; i < nee; i++) {
      energies.push_back( emin*exp(estep*i) );
   }

// Create the valarray to contain the exposure data.
   std::valarray<double> exposure(glon.size()*glat.size()*energies.size());

// Read in the spacecraft data. (Use Likelihood::Table for now.)
   Likelihood::Table scData;

   int hdu = 2;   
   scData.add_columns("time SC_x0 SC_x1 SC_x2 SC_x SC_y SC_z");
   scData.read_FITS_table(scDataFile, hdu);

   int ntimes = scData[0].dim;
   double *times = scData[0].val;

// These are spacecraft axis directions in Equatorial coordinates.
   double *SC_x0 = scData[1].val;
   double *SC_x1 = scData[2].val;
   double *SC_x2 = scData[3].val;

   double *SC_x = scData[4].val;
   double *SC_y = scData[5].val;
   double *SC_z = scData[6].val;

   astro::SkyDir zaxis;
   astro::SkyDir xaxis;

// Loop over time intervals.
   ntimes = 100;
   for (int it=1; it < ntimes; it++) {
      if (it % (ntimes/20) == 0) std::cerr << ".";
      double dt = times[it] - times[it-1];
      zaxis = astro::SkyDir( Hep3Vector(SC_x[it], SC_y[it], SC_z[it]) );
      xaxis = astro::SkyDir( Hep3Vector(SC_x0[it], SC_x1[it], SC_x2[it]) );

      int indx = 0;
      int src_indx = 0;
// Loop over longitude coordinate.
      for (unsigned int i=0; i < glon.size(); i++) {
// Loop over latitude coordinate.
         for (unsigned int j=0; j < glat.size(); j++) {
            src_indx++;

// Apply the hard-wired maximum inclination for GLAST25 response functions.
            if (srcDirs[src_indx].difference(zaxis)*180./M_PI < 70.) {

// Loop over enerigies.
               std::vector<double>::const_iterator energyIt = energies.begin();
               for (int k=0; energyIt != energies.end(); energyIt++, k++) {
                  indx = glon.size()*(glat.size()*k + j) + i;
                  exposure[indx] += dt*aeff(*energyIt, srcDirs[src_indx], 
                                            zaxis, xaxis);
               } // energyIt
            } // if (srcDir.difference(...))
         } // glatIt
      } // glonIt
   } // it < ntimes

   std::cerr << "!" << std::endl;

// Write out the FITS image file.
   std::string filename("exposureMap.fits");
   writeExposureFile(filename, glon, glat, energies, exposure);
}

void writeExposureFile(const std::string &filename, 
                       const std::vector<double> &glon,
                       const std::vector<double> &glat,
                       const std::vector<double> &energies,
                       const std::valarray<double> &exposure) {

   fitsfile *fptr;
   int status = 0;
   
// Always overwrite an existing file.
   remove(filename.c_str());
   fits_create_file(&fptr, filename.c_str(), &status);
   fitsReportError(stderr, status);

   int bitpix = DOUBLE_IMG;
   long naxis = 3;
   long naxes[] = {glon.size(), glat.size(), energies.size()};
   fits_create_img(fptr, bitpix, naxis, naxes, &status);
   fitsReportError(stderr, status);

// Write the exposure map data.
   long group = 0;
   long dim1 = glon.size();
   long dim2 = glat.size();

// Repack exposure into a C array.
   double *exp_array = new double[glon.size()*glat.size()*energies.size()];
   for (unsigned int i = 0; i < exposure.size(); i++) {
      exp_array[i] = exposure[i];
   }
   fits_write_3d_dbl(fptr, group, dim1, dim2, 
                     glon.size(), glat.size(), energies.size(),
                     exp_array, &status);
   delete[] exp_array;
   fitsReportError(stderr, status);

// Write some keywords.
   double l0 = glon[0];
   fits_update_key(fptr, TDOUBLE, "CRVAL1", &l0, 
                   "longitude of reference pixel", &status);
   fitsReportError(stderr, status);
   double b0 = glat[0];
   fits_update_key(fptr, TDOUBLE, "CRVAL2", &b0, 
                   "latitude of reference pixel", &status);
   fitsReportError(stderr, status);
   
   double lstep = glon[1] - glon[0];
   fits_update_key(fptr, TDOUBLE, "CDELT1", &lstep, 
                   "longitude step at ref. pixel", &status);
   fitsReportError(stderr, status);
   double bstep = glat[1] - glat[0];
   fits_update_key(fptr, TDOUBLE, "CDELT2", &bstep, 
                   "latitude step at ref. pixel", &status);
   fitsReportError(stderr, status);
   
   float crpix1 = lstep/2.;
   fits_update_key(fptr, TFLOAT, "CRPIX1", &crpix1, 
                   "reference pixel for longitude coordinate", &status);
   fitsReportError(stderr, status);
   float crpix2 = bstep/2.;
   fits_update_key(fptr, TFLOAT, "CRPIX2", &crpix2, 
                   "reference pixel for latitude coordinate", &status);
   fitsReportError(stderr, status);
   
   char *ctype1 = "GLON-CAR";
   fits_update_key(fptr, TSTRING, "CTYPE1", ctype1, 
                   "right ascension", &status);
   fitsReportError(stderr, status);
   char *ctype2 = "GLAT-CAR";
   fits_update_key(fptr, TSTRING, "CTYPE2", ctype2, 
                   "declination", &status);
   fitsReportError(stderr, status);
   
// Write the energy array as a binary table.
   int nrows = energies.size();
   int tfields = 1;
   char *ttype[] = {"Energy"};
   char *tform[] = {"1D"};
   char *tunit[] = {"MeV"};
   char extname[] = "True Photon Energy Array";
   
   int firstrow  = 1;
   int firstelem = 1;
   
   fits_create_tbl(fptr, BINARY_TBL, nrows, tfields, ttype, tform,
                   tunit, extname, &status);
   fitsReportError(stderr, status);
   
   fits_write_col(fptr, TDOUBLE, 1, firstrow, firstelem, nrows, 
                  &energies[0], &status);
   fitsReportError(stderr, status);
   
   fits_close_file(fptr, &status);
   fitsReportError(stderr, status);
   
   return;
}

void fitsReportError(FILE *stream, int status) {
   fits_report_error(stream, status);
   if (status != 0) {
      throw std::string("writeExposureFile: cfitsio error.");
   }
}


#ifndef READPARFILE_H
#define READPARFILE_H

#include "hoops/hoops.h"
#include "hoops/hoops_par.h"
#include "hoops/hoops_prompt_group.h"
#include "hoops/hoops_exception.h"
const int maxSize = 256;

class pgwPars{
   public:
   	pgwPars(){};
	int readParFile(int argc, char** argv);
   private:
   	int scanParVect(char *buf,int nele, double * result);
	
   public:
   	char nome_file_in[maxSize];
	char nome_file_external_bkg[maxSize];
        char output_prefix[maxSize];
	char circ_square[80];
	int N_iterations, N_scale;
	double n_sigma;
	double median_box;
	double kappa;
	double min_pix;
	char fitsio_choice[80];
	char recursive_choice[80];
	char bgk_ch[80];
	double *scala;
	double *otpix;
	int border;
	int far,enable;
	int yes;
	int bgk_cho;
	int verbose;
	double *SN_ratio;
};

#endif

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "pgwave/readParFile.h"

const int maxNscale = 31;


int pgwPars::readParFile(int argc, char** argv)
{
  
  int stat=1;
  try {
	hoops::ParPromptGroup pars(argc, argv);
	pars.Prompt("input_file");
	strcpy(nome_file_in,pars["input_file"]);
	//std::cout<<nome_file_in<<std::endl;
	
	pars.Prompt("bgk_choise");        
	strcpy(bgk_ch,pars["bgk_choise"]);
	bgk_cho = strcmp(bgk_ch, "y");
	N_iterations=1;
	if(!bgk_cho){
		pars.Prompt("input_bgk_file");
		strcpy(nome_file_external_bkg,pars["input_bgk_file"]);
	}
	pars.Prompt("circ_square");                
	strcpy(circ_square,pars["circ_square"]);
	SN_ratio = new double[N_iterations];
	SN_ratio[0]=0.;
	pars.Prompt("N_scale");        
	N_scale=pars["N_scale"];
	scala = new double[N_scale];
	pars.Prompt("scala");        
	//scala=pars["scala"];
	char *tmp=new char(80);
	strcpy(tmp,pars["scala"]);
	scanParVect(tmp,N_scale,scala);
	otpix = new double[N_scale];
	pars.Prompt("otpix");
	strcpy(tmp,pars["otpix"]);
	scanParVect(tmp,N_scale,otpix);
	if(bgk_cho!=0){
		pars.Prompt("n_sigma");
		n_sigma=pars["n_sigma"];
		pars.Prompt("median_box");
		median_box = pars["median_box"];
	}
	pars.Prompt("kappa");
	kappa=pars["kappa"];
	pars.Prompt("min_pix");
	min_pix=pars["min_pix"];
	far=5;
	pars.Prompt("border_size");
	far=pars["border_size"];
	pars.Prompt("fitsio_choice");
	strcpy(fitsio_choice,pars["fitsio_choice"]);
	enable=1;
	if(N_scale>1){
		pars.Prompt("recursive_choice");
		strcpy(tmp,pars["recursive_choice"]);
		enable = strcmp(tmp, "y");
	}
	verbose=0;
	pars.Prompt("verbose_level");
	verbose=pars["verbose_level"];
	
	pars.Save();
	
     } catch (hoops::Hexception & x) {
    	std::cerr << "Error " << x.Code() << ": " << x.Msg() << std::endl;
    	stat = 0;
     } catch (std::exception & x) {
    	std::cerr << "Error " << ": " << x.what() << std::endl;
    	stat = 0;
     } catch (...) {
    	std::cerr << "Unknown Error " << std::endl;
    	stat = 0;
     }
	border = strcmp(circ_square, "c");
	yes =strcmp(fitsio_choice, "y");

	return stat;
}

int pgwPars::scanParVect(char *buf,int nele, double * result)
{
	double ival;
	int i, nbytes;
	for (i = 0; i<nele; i++)
    	{
      		if (1 != sscanf(buf, "%lg%n", &ival, &nbytes)) 
			return 0;
      		if (NULL != scala) {
			result[i] = ival;
			//std::cout<< result[i]<<"\n";
		}
      		buf=(buf+nbytes);
	}
	return 1;
}

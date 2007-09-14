/*
 *  utile.cpp
 *  di_servizio
 *
 *  Created by Gabriele on 20/10/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "pgwave/utile.h"
#include "pgwave/filtri.h"
//#include <fftw/fftw3.h>
//#include "astro/SkyFunction.h"
//#include "astro/SkyProj.h"
#include <iostream>
#include <string>
#include <vector>

#include <cmath>
#include <iostream>
#include <pil.h>
#include <pil_error.h>

using namespace std;


inline double myrand(double max)
{
	return max*(((double)rand())/RAND_MAX);
}

double exponential_distribution( double rate )
{
	return -(log(1-myrand(1-1e-12)))/rate;
}

double power_distribution( double Emin, double alpha )
{
	return Emin*pow(myrand(1),1./(1.-alpha));
}


void detect_sources::fill_coor(double xvalue, double yvalue, double dvalue, double wtvalue,
							   double magvalue, double bkgvalue, double SNRvalue,double Sigvar,double merr,double bkerr)
{
	x.push_back(xvalue);
	y.push_back(yvalue);
	d.push_back(dvalue);
	wt.push_back(wtvalue);
	mag.push_back(magvalue);
	bkg.push_back(bkgvalue);
	SNR.push_back(SNRvalue);
	signif.push_back(Sigvar);
	magerr.push_back(merr);
	bkgerr.push_back(bkerr);
	return;
}


void detect_sources::remove(int i)
{
	vector<double>::iterator px = x.begin();
	vector<double>::iterator py = y.begin();
	vector<double>::iterator pd = d.begin();
	vector<double>::iterator pwt = wt.begin();
	vector<double>::iterator pmag = mag.begin();
	vector<double>::iterator pbkg = bkg.begin();
	vector<double>::iterator pmagerr = magerr.begin();
	vector<double>::iterator pbkgerr = bkgerr.begin();
	vector<double>::iterator pSNR = SNR.begin();
	vector<double>::iterator pSignif =signif.begin();
	px += i;
	py += i;
	pd += i;
	pwt += i;
	pmag += i;
	pbkg += i;
	pmagerr += i;
	pbkgerr += i;
	pSNR += i;
	pSignif+=i;
	x.erase(px);
	y.erase(py);
	d.erase(pd);
	wt.erase(pwt);
	mag.erase(pmag);
	bkg.erase(pbkg);
	magerr.erase(pmagerr);
	bkgerr.erase(pbkgerr);
	SNR.erase(pSNR);
	signif.erase(pSignif);
	return;
}

void detect_sources::set_clear()
{
	x.clear();
	y.clear();
	d.clear();
	wt.clear();
	mag.clear();
	bkg.clear();
	magerr.clear();
	bkgerr.clear();
	SNR.clear();
	signif.clear();
	return;
}

void detect_sources::print_scale(int step)
{
	char nome_file_scale[80];
	sprintf(nome_file_scale, "partial_scale%i.reg", step+1);
	ofstream outfile(nome_file_scale);
	if (!outfile.is_open()) {
		cout << "Errore nell'apertura del file di output\n";
		return;
	}
	for (int i = 0; i < int(x.size()); i++) {
		outfile << "image;box point (" << (x[i]+1) << ", " << (y[i]+1) << ")\n";
	}
	outfile.close();
	
}

void detect_sources::print_to_file(fitsfile *fptr, char *filename)
{
//	astro::SkyProj* mwcs; 
	std::string fil(filename);
	std::string temp=fil.substr(0,fil.find("."));
	std::string detsource=temp+".reg";
	std::string detout=temp+".list";
	//cout <<detsource.c_str()<<endl;
	//cout <<detout.c_str()<<endl;
	//ofstream outfile("detected_sources.reg");
	ofstream outfile(detsource.c_str());
	if (!outfile.is_open()) {
		cout << "Errore nell'apertura del file di output\n";
		return;
	}
	for (int i = 0; i < int(x.size()); i++) {
//		outfile << "image;boxcircle point (" << (x[i]+1) << ", " << (y[i]+1) << ")  # text={"<< (i+1) <<"}"<<char(10);
		outfile << "image;point (" << (x[i]+1) << ", " << (y[i]+1) << ")  # color=red point=cross text={"<< (i+1) <<"}"<<char(10);

	}
	outfile.close();
	
	//ofstream outlist("output_list.reg");
	ofstream outlist(detout.c_str());
	if (!outlist.is_open()) {
		cout << "Unable to open the output file\n";
		return;
	}
	int status = 0;
	double xrefval, yrefval, xrefpix, yrefpix, xinc, yinc, rot, RA, DEC;
	char coordtype[10];
	char list_file[258];
	
//	mwcs = new astro::SkyProj(fil);
	if (fits_open_file(&fptr, filename, READONLY, &status))
		printerror(status);
	
	if(fits_read_img_coord(fptr, &xrefval, &yrefval, &xrefpix, &yrefpix, &xinc, &yinc, &rot, coordtype, &status))
		printerror(status);
	//outlist <<"#"<<filename<<endl;
	/*if(!mwcs->isGalactic()){
		outlist << "#[ID]   [X]	   [Y]     [RA]         [DEC]         [SNR]     [K-signf]  [Counts]  [SigC]  [BKG]  [SigBkg]" << endl;
	}else*/
{
		outlist << "#[ID]   [X]	   [Y]     [RA]           [DEC]          [SNR]     [K-signf]  [Counts]  [SigC]  [BKG]  [SigBkg]" << endl;
	}
	for(int k=0; k<int(x.size()); k++){
		//cout<<x[k]<<" "<<y[k]<<endl;
		fits_pix_to_world(double(x[k]+1), double(y[k]+1), xrefval, yrefval, xrefpix, yrefpix, xinc, yinc, rot, coordtype, &RA, &DEC, &status);
		if(status == 0){
		//pair<double, double> rr=mwcs->pix2sph(double(x[k]+1),double(y[k]+1));
			sprintf(list_file, "%4d  %6.1f   %6.1f   %10.3f   %10.3f   %10.3f   %10.3f   %6d  %6d  %6d  %6d\n",k+1, x[k], y[k],
				RA, DEC, SNR[k],signif[k],int(mag[k]+0.5), int(magerr[k]+0.5),int(bkg[k]+0.5),int(bkgerr[k]+0.5));
			outlist << list_file;
		
		}else{
			cout << "\nBAD WCS PARAMETERS in the header input file '" << filename << "': 'output_list.reg' is empty!" << endl;
			break;
		}
	}
	outlist.close();
	cout << "\n Detected sources DS9 regions written on file: \t" << detsource.c_str() <<endl;
	cout << "\n Detected sources list written on file: \t" << detout.c_str() <<endl;
	return;
}

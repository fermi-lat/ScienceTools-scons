/************************************************************
*	
*  function_util.cpp
*  Wave2D_7Feb
*
*  Created by Gabriele Discepoli on 7/02/06.
*  Copyright 2006 __Physics dept. University of Perugia__. All rights reserved.
*
************************************************************/
#include "pgwave/function_util.h"
#include <iostream>
#include "pgwave/filtri.h"
#include <cmath>
//#include <pil.h>
//#include <pil_error.h>
//#include "PGWave2D/MyImage.h"
//#include "PGWave2D/MyImageAnalisys.h"


using namespace std;

/*bool readParam_string( const char name[], char* result )
{
	int pilerr = PILGetString(name,result);
	if ( pilerr < 0 ) {
		cout << "Error reading string parameter " << name << ": " << PIL_err_handler(pilerr) << endl;
		return false;
	}
	return true;
}

bool readParam_int( const char name[], int* result )
{
	int pilerr = PILGetInt(name,result);
	if ( pilerr < 0 ) {
		cout << "Error reading int parameter " << name << ": " << PIL_err_handler(pilerr) << endl;
		return false;
	}
	return true;
}

bool readParam_double( const char name[], double* result )
{
	int pilerr = PILGetReal(name,result);
	if ( pilerr < 0 ) {
		cout << "Error reading int parameter " << name << ": " << PIL_err_handler(pilerr) << endl;
		return false;
	}
	return true;
}

bool readParam_double_vector( const char name[], int len, double* result )
{
	int pilerr = PILGetRealVector(name,len,result);
	if ( pilerr < 0 ) {
		cout << "Error reading int parameter " << name << ": " << PIL_err_handler(pilerr) << endl;
		return false;
	}
	return true;
}
*/
void printerror( int status)
{
    /*****************************************************/
    /* Print out cfitsio error messages and exit program */
    /*****************************************************/
	
	
    if (status)
    {
		fits_report_error(stderr, status); /* print error report */
		
		exit( status );    /* terminate the program, returning error status */
    }
    return;
}


// Utility per aprire, scrivere e chiudere immagini fits
void fitsio_all_in_one(fitsfile *fptr, char *filename, char *filename_header, array2d<double>& image, long *nax, int print)
{
	int status = 0;
	long primo_pixel = 1;
	const long int xbin = *nax, ybin = *(nax+1);
	long nelements = xbin*ybin;
	array2d<double> service(ybin,xbin);
	for(int i=0; i<xbin; i++){
		for(int j=0; j<ybin; j++)
			service(j,i) = image(i,j);
	}
	
	CMyImage *exmach = new CMyImage;
	
	remove(filename);
	
	if (print == 0) {
		exmach->CopyFITSFile(filename_header, filename);
		
		if (fits_open_file(&fptr, filename, READWRITE, &status))
			printerror(status);
		
		if ( fits_write_img(fptr, TDOUBLE, primo_pixel, nelements, &service(0,0), &status) )
			printerror( status );
		
		if ( fits_close_file(fptr, &status) )                /* close the file */
			printerror( status );
	}
	delete exmach;
}

// Utility per aprire, scrivere e chiudere immagini fits
void fitsio_all_in_one(fitsfile *fptr, char *filename, array2d<double>& image, long *nax, int print)
{
	int status = 0;
	long primo_pixel = 1;
	long naxis = 2;
	int bitpix = DOUBLE_IMG;
	const long int xbin = *nax, ybin = *(nax+1);
	long nelements = xbin*ybin;
	array2d<double> service(ybin,xbin);
	for(int i=0; i<xbin; i++){
		for(int j=0; j<ybin; j++)
			service(j,i) = image(i,j);
	}
	remove(filename);
	if (print == 0) {
		if (fits_create_file(&fptr, filename, &status)) /* create new FITS file */
			printerror( status ); 
		
		if ( fits_create_img(fptr,  bitpix, naxis, nax, &status) )
			printerror( status );     
		
		if (fits_open_file(&fptr, filename, READWRITE, &status))
			printerror(status);
		
		if ( fits_write_img(fptr, TDOUBLE, primo_pixel, nelements, &service(0,0), &status) )
			printerror( status );
		
		if ( fits_close_file(fptr, &status) )                /* close the file */
			printerror( status );
	}
	return;
}
// Utility per aprire, scrivere e chiudere immagini fits
void fitsio_all_in_one(fitsfile *fptr, char *filename, char *filename_header, array3d<double>& image, long *nax, int print)
{
	int status = 0;
	long primo_pixel = 1;
	long int xbin=*nax, ybin=*(nax+1), zbin=*(nax+2);
	long nelements = xbin*ybin*zbin;
	array3d<double> service(zbin,ybin,xbin);
	for(int i=0; i<xbin; i++){
		for(int j=0; j<ybin; j++){
			for(int k=0; k<zbin; k++)
				service(k,j,i) = image(i,j,k);
		}
	}
	
	CMyImage *exmach = new CMyImage;
	
	remove(filename);
	
	if (print == 0) {
		exmach->CopyFITSFile(filename_header, filename);
		/* NON FUNZIONA!
		if(fits_update_key(fptr, TLONG, "NAXIS3", &zbin, 0, &status))
			printerror(status);
		*/
		if (fits_open_file(&fptr, filename, READWRITE, &status))
			printerror(status);
		
		if ( fits_write_img(fptr, TDOUBLE, primo_pixel, nelements, &service(0,0,0), &status) )
			printerror( status );
		
		if ( fits_close_file(fptr, &status) )                /* close the file */
			printerror( status );
	}
	delete exmach;
}

void fitsio_all_in_one(fitsfile *fptr, char *filename, array3d<double>& image, long *nax, int print)
{
	int status = 0;
	long primo_pixel = 1;
	long naxis = 3;
	int bitpix = DOUBLE_IMG;
	const long int xbin=*nax, ybin=*(nax+1), zbin=*(nax+2);
	long nelements = xbin*ybin*zbin;
	array3d<double> service(zbin,ybin,xbin);
	for(int i=0; i<xbin; i++){
		for(int j=0; j<ybin; j++){
			for(int k=0; k<zbin; k++)
				service(k,j,i) = image(i,j,k);
		}
	}
	remove(filename);
	if (print == 0) {
		if (fits_create_file(&fptr, filename, &status)) /* create new FITS file */
			printerror( status ); 
		
		if ( fits_create_img(fptr,  bitpix, naxis, nax, &status) )
			printerror( status );     
		
		if (fits_open_file(&fptr, filename, READWRITE, &status))
			printerror(status);
		
		if ( fits_write_img(fptr, TDOUBLE, primo_pixel, nelements, &service(0,0,0), &status) )
			printerror( status );
		
		if ( fits_close_file(fptr, &status) )                /* close the file */
			printerror( status );
	}
	return;
}

void print_copy(array2d<double>& in, array3d<double>& print, int step, long *nax)
{
	const long int xbin = *nax, ybin = *(nax+1);
	for(int i=0; i<xbin; i++){
		for(int j=0; j<ybin; j++)
			print(i,j,step) = in(i,j);
	}
	
	return;
}
void cp_input(array2d<double>& service, array2d<double>& reas_image, long *nax)
{
	const long int xbin = *nax, ybin = *(nax+1);
	for(int i=0; i<xbin; i++){
		for(int j=0; j<ybin; j++)
			reas_image(i,j) = service(j,i);
	}
	return;
}

void find(char *nome_file, MyImageAnalisys *im, int min_num, int side_box)
{
	CMyImage *a =new CMyImage;
	a->OpenFITSFile(nome_file);
	a->ReadFITS();
	a->CloseFITSFile();
	im->SetImage(a->GetBuffer(),a->GetWidth(),a->GetHeight());
	im->SetLowGoodDat(1);
	im->SetHighGoodDat(59000.);
	im->SetMinNumPixOverThres(min_num);
	im->SegImage(side_box);
	
	cout<<" done.\n\n\t-> Number of identified candidate-sources = "<< im->GetNumStarFound() << "\n" << endl;
	delete a;
	
	return;
}

void find_stat(char *nome_file, MyImageAnalisys *im, fitsfile *fptr_in, int print)
{
	/*** questo box deve essere dell'ordine della PSF (4 gradi), ergo deve dipendere dal binnaggio
	per cui se un pixel = 0.25¡ => side_box = 8.0, se 1pipxel = 0.5¡ => side_box = 4.0
	Inoltre la PSF scende dai 4¡ fino ai 2¡ ad alte energie, per cui le cui dimensioni del box
	dovranno diminuire al salire dell'energia. Non e' necessario una lex continua, ma bastano
	un due/tre "else if" per suddividere lo spettro in altrettante bande
	***/
	double pix_bin, side_box,r1,r2;
	const char keyword[] = {"CDELT1"};
	CMyImage *a =new CMyImage;
	a->OpenFITSFile(nome_file);
	a->ReadFITS();
	a->GetKeyValue(keyword, pix_bin);
	if(pix_bin==0){
		side_box = 6.;
		r1=4;
		r2=8;
	}
	else{
		side_box = (2.0/fabs(pix_bin));
		r1=4./fabs(pix_bin);
		r2=7./fabs(pix_bin);
	}
	a->CloseFITSFile();
	im->SetImage(a->GetBuffer(),a->GetWidth(),a->GetHeight());
	if(print){cout << "\t [id] \t(X,Y)  \t\t[SNR]   \t[bkg]    \t[signal]\n" << endl;}
	for(int k= 0; k < int(im->GetNumStarFound()); k++){
		//cout <<im->centro[k].xpeak << " " << im->centro[k].ypeak <<" "<<
		//im->LocalBackground(im->centro[k],r1,r2);
		im->StarCenter(im->centro[k], side_box);
		if(print){
			cout << im->centro[k].id << " \t" << im->centro[k].x << " " << im->centro[k].y << "  \t"
			<< im->centro[k].theta << "\t" <<  im->centro[k].bkg << "\t" << im->centro[k].mag << endl;
		}
	}
}

void remove_twin(detect_sources *good_list, double dist)
{
	int dim = good_list->xdimension();
	vector<int> bad_list;
	//dist=6;
	int ii, jj;
	for(ii = 0; ii < dim; ii++) {
		for(jj = 0; jj < ii; jj++) {
			if ((pow(good_list->getx(ii) - good_list->getx(jj), 2) + pow(good_list->gety(ii) - good_list->gety(jj), 2)) <=dist*dist){
				//cout<<"ELIMINATA: "<< good_list->getx(ii)<<"-"<< good_list->getx(jj)<<" "<<good_list->gety(ii)<<" - "<< good_list->gety(jj)<<endl;
				bad_list.push_back(ii);
				break;
			}
		}
	}
	ii = int(bad_list.size());
	while (ii > 0){
		good_list->remove(bad_list[ii-1]);
		ii--;
	}
	dim =good_list->xdimension();
	return;
}

void erase_source(detect_sources *ogg, array2d<double>& input, double dist, long *nax, int print)
{
	int i, j, count;
	double xtest, ytest, dtest, wttest, magtest, bkgtest, SNRtest,signi;
	double magterr, bkgterr,sumx,sumy,a,b,theta;
	detect_sources *good=new detect_sources;
	for (i=0; i<(ogg->xdimension()-1); i++) {
			count=0;//sumx=0;sumy=0;
			xtest=ogg->getx(i);
			ytest=ogg->gety(i);
			dtest=ogg->getd(i);
			wttest=ogg->getwt(i);
			magtest=ogg->getmag(i);
			bkgtest=ogg->getbkg(i);
			magterr=ogg->getmagerr(i);
			bkgterr=ogg->getbkgerr(i);
			SNRtest = ogg->getSNR(i);
			signi=ogg->getSignif(i);
			a=ogg->getA(i);
			b=ogg->getB(i);
			theta=ogg->getTHETA(i);
			for(j=(i+1); j<ogg->xdimension(); j++) {
					if ((pow(ogg->getx(i) - ogg->getx(j), 2) + pow(ogg->gety(i) - ogg->gety(j), 2)) < dist*dist) {
						count++;
						if ((ogg->getd(j))< (dtest)) { //getd
							if ((ogg->getwt(j))> (wttest)) 
								wttest=ogg->getwt(j);
							//sumx+=ogg->getx(j);
							//sumy+=ogg->gety(j);
							xtest=ogg->getx(j);
							ytest=ogg->gety(j);
							dtest=ogg->getd(j);
							//wttest=ogg->getwt(j);
							magtest=ogg->getmag(j);
							bkgtest=ogg->getbkg(j);
							SNRtest = ogg->getSNR(j);
							signi=ogg->getSignif(j);
							magterr=ogg->getmagerr(j);
							bkgterr=ogg->getbkgerr(j);
							a=ogg->getA(j);
							b=ogg->getB(j);
							theta=ogg->getTHETA(j);

						}
					}
			}
			if (count!=0){
				//xtest=sumx/count; ytest=sumy/count;
				good->fill_coor(xtest, ytest, dtest, wttest,
				magtest, bkgtest, SNRtest,signi,magterr,
				bkgterr,a,b,theta);
			}
		}
	remove_twin(good, dist);

	//simul_bright(good, input, nax, print);
	
	delete good;
	return;
}

void erase_source(detect_sources *ogg, double dist)
{
	int i, j;
	double xtest, ytest, dtest, wttest, magtest, bkgtest, SNRtest,signi;
	double magterr, bkgterr,a,b,theta;
	detect_sources *good=new detect_sources;
	for (i=0; i<ogg->xdimension(); i++) {
		xtest=ogg->getx(i);
		ytest=ogg->gety(i);
		dtest=ogg->getd(i);
		wttest= ogg->getwt(i);
		magtest=ogg->getmag(i);
		bkgtest=ogg->getbkg(i);
		SNRtest = ogg->getSNR(i);
		signi=ogg->getSignif(i);
		magterr=ogg->getmagerr(i);
		bkgterr=ogg->getbkgerr(i);
		a=ogg->getA(i);
		b=ogg->getB(i);
		theta=ogg->getTHETA(i);
		for(j=(i+1); j<ogg->xdimension(); j++) {
			if ((pow(ogg->getx(i) - ogg->getx(j), 2) + pow(ogg->gety(i) - ogg->gety(j), 2)) < dist*dist) {
				if ((ogg->getd(j))< (dtest)) { //getd
					//if ((ogg->getwt(j))> (wttest)) { 
					if ((ogg->getwt(j))> (wttest)) 
								wttest=ogg->getwt(j);
					xtest=ogg->getx(j);
					ytest=ogg->gety(j);
					dtest=ogg->getd(j);
					//wttest=ogg->getwt(j);
					magtest=ogg->getmag(j);
					bkgtest=ogg->getbkg(j);
					SNRtest = ogg->getSNR(j);
					signi=ogg->getSignif(j);
					magterr=ogg->getmagerr(j);
					bkgterr=ogg->getbkgerr(j);
					a=ogg->getA(j);
					b=ogg->getB(j);
					theta=ogg->getTHETA(i);
				}
			}
		}
		good->fill_coor(xtest, ytest, dtest, wttest, magtest, bkgtest,
		SNRtest,signi,magterr, bkgterr,a,b,theta);
	}
	remove_twin(good, dist);
	ogg->set_clear();
	for (i=0; i<good->xdimension(); i++){
		ogg->fill_coor(good->getx(i), good->gety(i), good->getd(i), good->getwt(i),
					   good->getmag(i), good->getbkg(i),
good->getSNR(i),good->getSignif(i),good->getmagerr(i),
good->getbkgerr(i),good->getA(i),good->getB(i),0.);
	}
	
	delete good;
	return;
}

void simul_bright(detect_sources *erase_list, array2d<double>& input, long *nax, int print)
{
	int i, j, num= erase_list->xdimension();
	const long int xbin = *nax, ybin = *(nax+1);
	array2d<double> empty_field(xbin, ybin);
	
	for(i=0; i<xbin; i++){
		for(j=0; j<ybin; j++){
			empty_field(i,j) = 0;
		}
	}
	
	/**********    RIEMPIMENTO empty_field CON GAUSSIANA ******/
	
	for (i=0; i<num; i++)
		fill_gauss(erase_list, i, empty_field, nax);
		
	chi_quadro(erase_list, input, empty_field, nax, print);
	for(i=0; i<xbin; i++) {
		for(j=0; j<ybin; j++)
			input(i,j) = input(i,j) - empty_field(i,j);
	}
	
	return;
}

void chi_quadro(detect_sources *list, array2d<double>& input, array2d<double>& simul, long *nax, int print)
{
	int i,j;
	const long int xbin = *nax, ybin = *(nax+1);
	array2d<double> clean(xbin, ybin);
	for(i=0; i<xbin; i++){
		for(j=0; j<ybin; j++)
			clean(i,j) = 0;
	}
	for(i=0; i< list->xdimension(); i++){
		if (print == 0) {
			cout << "[" << i+1 << "]\t(" << list->getx(i) << ", " << list->gety(i)
				<< ") ->\twt: " << list->getwt(i) << "\t ->\tSNR: " << list->getSNR(i)<<"\tK-Signif: "<<list->getSignif(i);}
		back_sub(input, clean, list->getx(i), list->gety(i), list->getd(i), nax);
		test(clean, simul, list->getx(i), list->gety(i), list->getd(i), nax, print);
	}
	fitsfile *pt_clean = new fitsfile;
	fitsfile *pt_simul = new fitsfile;
	char nome_file_clean[] = {"clean.fits"};
	char nome_file_simul[] = {"simul.fits"};
	fitsio_all_in_one(pt_clean, nome_file_clean, clean, nax, print);
	fitsio_all_in_one(pt_simul, nome_file_simul, simul, nax, print);
	delete pt_clean;
	delete pt_simul;
	return;
}

void back_sub(array2d<double>& input, array2d<double>& clean, double x, double y, double sigma, long *nax)
{
	double average, sum_pixel = 0;
	int n_pixel = 0;
	const long int xbin = *nax, ybin = *(nax+1);
	double radius = 2.*sigma;
	int imin = MAX(0, int(x-(radius+2)));
	int imax = MIN(xbin-1,int(x+(radius+2)));
	int jmin = MAX(0, int(y-(radius+2)));
	int jmax = MIN(ybin-1,int(y+(radius+2)));
	for( int i = imin; i < imax; i++ ) {
		for(int j = jmin; j < jmax; j++ ) {
			if ( (x-i)*(x-i) + (y-j)*(y-j) >= radius*radius ) {
				sum_pixel += input(i,j);
				n_pixel++;
			}
		}
	}
	average = sum_pixel/n_pixel;
	for( int i = imin; i < imax; i++ ) {
		for(int j = jmin; j < jmax; j++ ) {
			clean(i,j) = input(i,j) - average;
		}
	}
	return;
}

void test(array2d<double>& clean, array2d<double>& simul, double x, double y, double sigma, long *nax, int print)
{
	double radius = 2.*sigma;
	const long int xbin = *nax, ybin = *(nax+1);
	double result, sum = 0;
	int numb = 0;
	int imin = MAX(0, int(x-(radius+2)));
	int imax = MIN(xbin-1, int(x+(radius+2)));
	int jmin = MAX(0, int(y-(radius+2)));
	int jmax = MIN(ybin-1, int(y+(radius+2)));
	for( int i = imin; i < imax; i++ ) {
		for(int j = jmin; j < jmax; j++ ) {
			if ( (x-i)*(x-i) + (y-j)*(y-j) <= radius*radius ) {
				if(simul(i,j) != 0){
					sum += pow((clean(i,j) - simul(i,j)), 2)/fabs(clean(i,j));
					numb++;
				}
			}
		}
	}
	result = sum/double(numb);
	if (print == 0) {cout << "\tChiSquare: " << sum << "\tPixel number: " << numb << "\tReducedChiSquare: " << result <<endl;}
	return;
}

void fill_gauss(detect_sources *erase, int hh, array2d<double>& empty, long *nax)
{
	int x0 = (int)erase->getx(hh);
	int y0 = (int)erase->gety(hh);
	const long int xbin = *nax, ybin = *(nax+1);
	double sigma = erase->getd(hh)/sqrt(3.);
	double sigma2 = sigma*sigma;
/***   N_mult fa da coefficiente per il numero di eventi atteso e quindi normalmente deve essere pari ad 1, ma essendo
		N_events solo una stima, N_mult puo' anche essere settato ad un valore differente,
		ma non troppo, da 1 stesso (e comunque < 2.0) ****/
	double N_mult = 1.0;
	int N_events = int(N_mult*erase->getwt(hh) +1);
	
	double arg1, arg2;
	double k_sig = 4.0;
	if (sigma <= 3) {k_sig = 8.0;}
	int side = int(k_sig*sigma);
	int side2 = side*side;
	int imin = MAX(0, int(x0- side));
	int imax = MIN(xbin-1, int(x0+side));
	int jmin = MAX(0, int(y0- side));
	int jmax = MIN(ybin-1, int(y0+side));
	for( int i = imin; i < imax; i++ ) {
		for(int j = jmin; j < jmax; j++ ) {
			if ( (x0-i)*(x0-i) + (y0-j)*(y0-j) <= side2 ) {
				arg1= - 0.5*((x0-i)*(x0-i) + (y0-j)*(y0-j));
				arg2 = 5.*exp(arg1/(sigma2))/(sigma2) + exp(arg1/(4*sigma2))/(sigma2);
				// Il "+" serve per tenere conto di eventuali conteggi precedenti, dovuti a sorgenti vicine
				empty(i,j) += N_events*arg2/(18*M_PI);
			}
		}
	}
	return;
}

void erase_border(detect_sources *ogg, int flag, long *nax, int far)
{
	int n;
	const long int xbin = *nax, ybin = *(nax+1);
	double x_0 = double(xbin)/2.0;
	double y_0 = double(ybin)/2.0;
	//int far = 5;		// sources have to be 15 pixel far from the image border.
	double max = x_0 - far;
	int N_source = ogg->xdimension();
	if (flag == 0){
		for (n=(N_source -1); n>=0; n--){
			if ((pow(ogg->getx(n) - x_0, 2) + pow(ogg->gety(n) - y_0, 2)) > max*max)
				ogg->remove(n);
		}
	}
	else{
		for (n=(N_source -1); n>=0; n--){
			x_0 = ogg->getx(n);
			y_0 = ogg->gety(n);
			if ((x_0 < far) || (x_0 > (xbin-far)) || (y_0 < far) || (y_0 > (ybin - far)))
				ogg->remove(n);
		}
	}
		
	return;
}

void SNR_cut(MyImageAnalisys *im, detect_sources *ogg, double SNR, double scale, double min_pix, array2d<double>& signi)
{
	double signif=0;
	for (int ii = 0; ii < int(im->GetNumStarFound()); ii++) {
		if((im->centro[ii].theta >=SNR) && (im->centro[ii].mag>=(im->centro[ii].magerr))){
		
signif=signi(static_cast<int>(im->centro[ii].xpeak),static_cast<int>(im->centro[ii].ypeak));
			ogg->fill_coor(im->centro[ii].xpeak,im->centro[ii].ypeak, scale, im->centro[ii].peak,
				im->centro[ii].mag,
im->centro[ii].bkg,im->centro[ii].theta,signif,im->centro[ii].magerr,im->centro[ii].bkgsig,im->centro[ii].A,im->centro[ii].B,0.);
			//cout<<"signi--->"<<signi(static_cast<int>(im->centro[ii].xpeak),static_cast<int>(im->centro[ii].ypeak))<<endl;
		}
		else
			cout<<"deleted"<<endl;
	}
	erase_source(ogg, min_pix);
}

int m_num_fit(int s)
{
	int m;
	double cost = 3.5028864;
		double a1 = 0.2325885;
		double a2 = 0.6336217;
		double a3 = -1.487e-2;
		double a4 = 1.2117e-4;
	m = int(cost + a1*s + a2*s*s + a3*s*s*s + a4*s*s*s*s);
	return m;
}

void select(detect_sources *list, detect_sources *flag, double min_pix, int print, int enable, int N_scale, double scala[])
{
	int ii, jj;
	int output=0;
	if (output) {
		cout <<  "Candidate-sources:\n" << endl;
		cout << " id \t x " <<"\t y " <<"\t wt " <<"\t SNR " << "\t Signal " << endl;
	}
	for (int step = 0; step<N_scale; step++) {
		
		if (output) {
			cout << "\nScale [" << 1+step << "] of [" << N_scale << "]\t**" << scala[step] << " channels**\n";
			if ( list[step].xdimension() != 0){
				for (ii = 0; ii < list[step].xdimension(); ii++){
					cout << " " << ii+1 << "\t " << list[step].getx(ii) << "\t " << list[step].gety(ii)
					<< "\t " << list[step].getwt(ii) << "\t " << list[step].getSNR(ii)
					<< "\t " << list[step].getmag(ii) << endl;
				}
			}
			else
				cout << "\t*** It did not find any source in that MH-scale. ***" << endl;
		}
		if (step == 0 && enable != 0)
		{
			for (ii = 0; ii < list[step].xdimension(); ii++) {
				flag->fill_coor(list[step].getx(ii), list[step].gety(ii), list[step].getd(ii), list[step].getwt(ii),
							
list[step].getmag(ii), list[step].getbkg(ii),
list[step].getSNR(ii),list[step].getSignif(ii),list[step].getmagerr(ii),
list[step].getbkgerr(ii),list[step].getA(ii),list[step].getB(ii),0.);
			}
		}
		if ((step > 0) && (list[step].xdimension() != 0)) {
			for (ii = 0; ii < list[step].xdimension(); ii++) {
				if (enable == 0) {
					for (jj = 0; jj < list[step-1].xdimension(); jj++){
						if ((pow(list[step].getx(ii) - list[step-1].getx(jj), 2) + pow(list[step].gety(ii) - list[step-1].gety(jj), 2)) <= (min_pix*min_pix)){
							flag->fill_coor(list[step].getx(ii), list[step].gety(ii), list[step].getd(ii), list[step].getwt(ii),
							list[step].getmag(ii), list[step].getbkg(ii),list[step].getSNR(ii),list[step].getSignif(ii),list[step].getmagerr(ii),list[step].getbkgerr(ii),list[step].getA(ii),list[step].getB(ii),0.);
						}
					}
				}
				else {
					flag->fill_coor(list[step].getx(ii), list[step].gety(ii), list[step].getd(ii), list[step].getwt(ii),
							
list[step].getmag(ii),
list[step].getbkg(ii),list[step].getSNR(ii),list[step].getSignif(ii),list[step].getmagerr(ii),list[step].getbkgerr(ii),list[step].getA(ii),list[step].getB(ii),0.);
				}
			}
		}
	}
	
}






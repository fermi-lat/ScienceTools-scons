/************************************************************
 *												
 *  Wave2D_7Feb
 *
 *  Created by Gabriele Discepoli & Gino Tosti on 7/02/06.
 *  Copyright 2006 __Physics dept. University of Perugia__. All rights reserved.
 *
 ************************************************************/
#ifdef WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4700)
#endif
#include "pgwave/filtri.h"
#include "pgwave/function_util.h"
#include "pil.h"
#include "pil_error.h"
#include <string>
#include <cstdio>
#include <sstream>
#include <cstdlib>
#include <vector>

using namespace std;

const char nome_input_file[] = "input_file";
const char nome_bgk[]="bgk_choise";
const char nome_bgk_file[] = "input_bgk_file";
const char nome_circ_square[] = "circ_square";
const char nome_n_iterations[] = "N_iterations";
const char nome_iterazione[] = "SN_ratio";
const char nome_n_scale[] = "N_scale";
const char nome_scala[] = "scala";
const char nome_otpix[] = "otpix";
const char nome_n_sigma[] = "n_sigma";
const char nome_median_box[] = "median_box";
const char nome_kappa[] = "kappa";
const char nome_min_pix[] = "min_pix";
const char nome_fitsio_choice[] = "fitsio_choice";
const char nome_recursive_choice[] = "recursive_choice";
const char nome_verbose_level[]="verbose_level";
const char nome_border_size[]="border_size";

const int maxSize = PIL_LINESIZE+1;
const int maxNscale = 31;

int main ( int argc, char *argv[] )
{
	/*cout << "\n\n\t\t*********************************************************************\n"
		<< "\t\t***			WAVELET ANALYSIS			  ***\n"
		<< "\t\t***	-- Programm for point-like source detection --		  ***\n"
		<< "\t\t*********************************************************************\n\n";
	cout.flush();*/
	char nome_file_in[maxSize];
	char nome_file_external_bkg[maxSize];
	char circ_square[80];
	int N_iterations, N_scale=1;
	// Kappa: numero di sigma della distribuzione gaussiana, corrispondente quindi al grado di confidenza
	double n_sigma;
	double median_box;
	double kappa;
	double min_pix;
	char fitsio_choice[80];
	char recursive_choice[80];
	char bgk_ch[80];
	int pilerr;
	int enable=1;
	
	int far = 5;
	PILSetModuleName("pgwave2D");
	//if ( argc == 1 ) {
		pilerr = PILInit( argc, argv );
	/*} else {
		pilerr = PILInit( 1, argv+1 );
	}*/
	if ( pilerr < 0 ) {
		cout << "PILInit failed: " << PIL_err_handler(pilerr) << endl;
		exit(pilerr);
	}
	
	bool success = true;
	int verbose=0;
	success &= readParam_string( nome_input_file, nome_file_in);
	success &= readParam_string( nome_bgk, bgk_ch);
	int bgk_cho = strcmp(bgk_ch, "y");
	//cout<<bgk_ch<<endl;
	//cout<<bgk_cho<<endl;
	N_iterations=1;
	if(!bgk_cho)
		success &= readParam_string( nome_bgk_file,nome_file_external_bkg);
	success &= readParam_string( nome_circ_square, circ_square);
	//success &= readParam_int( nome_n_iterations, &N_iterations );
	double *SN_ratio = new double[N_iterations];
	SN_ratio[0]=0.;
	//success &= readParam_double_vector(nome_iterazione , N_iterations, SN_ratio);
	success &= readParam_int( nome_n_scale, &N_scale );
	double *scala = new double[N_scale];
	success &= readParam_double_vector( nome_scala, N_scale, scala );
	double *otpix = new double[N_scale];
	success &= readParam_double_vector( nome_otpix, N_scale, otpix );
	if(bgk_cho!=0){
		success &= readParam_double( nome_n_sigma, &n_sigma );
		success &= readParam_double( nome_median_box, &median_box );
	}
	
	success &= readParam_double( nome_kappa, &kappa );
	success &= readParam_double( nome_min_pix, &min_pix );
	success &= readParam_int( nome_border_size, &far );
	success &= readParam_string( nome_fitsio_choice, fitsio_choice);
	if(N_scale>1){
	success &= readParam_string( nome_recursive_choice, recursive_choice);
	enable = strcmp(recursive_choice, "y");
	}
	success &= readParam_int( nome_verbose_level, &verbose );
	PILClose(PIL_OK);
	
	if ( !success ) {
		cout << "One or more required parameters could not be read (fatal)." << endl;
		return -1;
	}
	
	int border = strcmp(circ_square, "c");
	int yes =strcmp(fitsio_choice, "y");
	
	int step, iter, aa;
	long naxes[2];
	int anynull, nfound, ii, jj;
	int ms_box, m_num;
	int status = 0;
	fitsfile *pt_file_in= new fitsfile;
	
	if (fits_open_file(&pt_file_in, nome_file_in, READONLY, &status)){
		printerror(status);
		return 0;
	}
	double PixScale=1;	
	char comm[40];
	if (fits_read_keys_lng(pt_file_in, "NAXIS", 1, 2, naxes, &nfound, &status))
		printerror(status);
	if (fits_read_key_dbl(pt_file_in,"CDELT2",&PixScale, comm, &status))
		printerror(status);
	cout<<"pixScale="<<PixScale<<endl;
	const long int bin_x = naxes[0], bin_y = naxes[1];
	long nax3[3]={naxes[0], naxes[1], N_scale};
	/*if(naxes[0]!=naxes[1]){
		cout<<"Sorry, This version of the program can work only on squared images\n";
		return 0;
	}*/
	double nullval;
	long primo_pixel, n_pixel = bin_x*bin_y;
	cout << "\n1. Initialization and dynamic memory allocation. Execution in progress...\n";
	
	MyImageAnalisys *obj =new MyImageAnalisys[N_scale];
	
	detect_sources *list = new detect_sources[N_scale];
	detect_sources *flagged = new detect_sources;
	
	array2d<double> input_image(bin_x, bin_y);
	array2d<double> read_service(bin_y, bin_x);
	array2d<double> output_image(bin_x, bin_y);
	array2d<double> gauss_filtered(bin_x, bin_y);
	array2d<double> median_filtered(bin_x, bin_y); //_x
	array2d<double> significat(bin_x, bin_y); //_x
	//external
	array2d<double> external_bkg(bin_x, bin_y);
	array2d<double> external_filtered(bin_x, bin_y);
	array2d<double> threshold_map(bin_x, bin_y);
	array2d<double> BGW_map(bin_x, bin_y);
	array2d<double> BGWI_map(bin_x, bin_y);
	array2d<double> OTM_image(bin_x, bin_y);
	array3d<double> gauss_output(bin_x, bin_y, N_scale);
	array3d<double> median_output(bin_x, bin_y, N_scale);
	array3d<double> WT_output(bin_x, bin_y, N_scale);
	array3d<double> threshold_output(bin_x, bin_y, N_scale);
	array3d<double> significat_output(bin_x, bin_y, N_scale);
	array2d<fftw_complex> transformed_input( bin_x, bin_y/2+1 );
	array2d<fftw_complex> gauss_conv( bin_x, bin_y/2+1 );
	array2d<fftw_complex> median_transformed(bin_x, bin_y/2+1);
	array2d<fftw_complex> median_convoluted(bin_x, bin_y/2+1);
	array2d<fftw_complex> WTI_image(bin_x, bin_y/2+1);
	//external
	array2d<fftw_complex> external_convoluted(bin_x, bin_y/2+1);
	array2d<fftw_complex> external_transformed(bin_x, bin_y/2+1);
	
	fftw_plan input_plan;
	fftw_plan back_input_plan;
	fftw_plan back_WTI_image;
	fftw_plan median_plan;
	fftw_plan back_median_plan;
	//external
	fftw_plan external_bgk_plan;
	fftw_plan back_external_bgk_plan;

	input_plan = fftw_plan_dft_r2c_2d(bin_x, bin_y, input_image.array, transformed_input.array, FFTW_MEASURE);
	back_input_plan = fftw_plan_dft_c2r_2d(bin_x, bin_y, gauss_conv.array, gauss_filtered.array, FFTW_MEASURE);
	median_plan = fftw_plan_dft_r2c_2d(bin_x, bin_y, median_filtered.array, median_transformed.array, FFTW_MEASURE);
	back_median_plan = fftw_plan_dft_c2r_2d(bin_x, bin_y, median_convoluted.array,  BGWI_map.array, FFTW_MEASURE);
	back_WTI_image = fftw_plan_dft_c2r_2d(bin_x, bin_y, WTI_image.array, output_image.array, FFTW_MEASURE);
	//external
	external_bgk_plan = fftw_plan_dft_r2c_2d(bin_x, bin_y, external_bkg.array, external_transformed.array, FFTW_MEASURE);
	back_external_bgk_plan = fftw_plan_dft_c2r_2d(bin_x, bin_y, external_convoluted.array,  BGW_map.array, FFTW_MEASURE);
	//back_external_bgk_plan = fftw_plan_dft_c2r_2d(bin_x, bin_y, external_convoluted.array,  external_filtered.array, FFTW_MEASURE);
	
	fitsfile *pt_file_copyin = new fitsfile;
	fitsfile *pt_file_gauss = new fitsfile;
	fitsfile *pt_file_median = new fitsfile; 
	fitsfile *pt_file_threshold = new fitsfile;
	fitsfile *pt_file_significat = new fitsfile;
	fitsfile *pt_file_out= new fitsfile; 
	fitsfile *pt_file_OTM[maxNscale];
	fitsfile *pt_file_external_bkg =new fitsfile;
	
	// Files di output finale
	char nome_file_copyin[] = {"copyinput_map.fits"};
	char nome_file_out[80];
	char nome_file_gauss[80];
	char nome_file_median[80];							
	char nome_file_threshold[80];
	char nome_file_OTM[80];
	char nome_file_WT[80];
	char nome_file_detected[80];
	char nome_file_significat[80];
	primo_pixel = 1;                               /* first pixel to write      */
	nullval = 0;
	//external
	if(!bgk_cho){
		if (fits_open_file(&pt_file_external_bkg, nome_file_external_bkg, READONLY, &status)){
			printerror(status);
			return 1;
		}
	}
	cout << "\n2. External Image Loading... ";
				
	if ( fits_read_img(pt_file_in, TDOUBLE, primo_pixel, n_pixel, &nullval, &read_service(0,0), &anynull, &status) ){
		printerror(status);
		return 1;
	}
	cp_input(read_service, input_image, naxes);
	//external
	if(!bgk_cho){
		if ( fits_read_img(pt_file_external_bkg, TDOUBLE, primo_pixel, n_pixel, &nullval, &read_service(0,0), &anynull, &status) ){
			printerror(status);
			return 1;
		}
		    cp_input(read_service, external_bkg, naxes);

		//external
		if (fits_close_file(pt_file_external_bkg, &status)){
			printerror(status);
			return 1;
		}
	}
	if (fits_close_file(pt_file_in, &status)){
		printerror(status);
		return 1;
	}
	//cp_input(read_service, input_image, naxes);
	cout << "done.\n";
	for (iter = 0; iter < N_iterations; iter++)
	{
		cout << "\n" << 3 + iter << ". *** SCAN (" << iter+1 << ") of (" << N_iterations << ") ***\n";
		
		fftw_execute(input_plan);
		if(bgk_cho!=0){
			if(iter>=1)
				n_sigma*=2.5;
			cout<<"\t\tA. Gaussian Filter... ";
			filtro_gaussiano(transformed_input, gauss_conv, n_sigma, bin_x, bin_y);
			fftw_execute(back_input_plan);
		}
		
		for (step = 0; step < N_scale; step++) {
			//scala[step]=scala[step]/PixScale;
			//double area=scala[step]*(scala[step]-1);
			//cout<<"AREA="<<area<<endl;
			cout << "Scan (" << 1+iter << ") of (" << N_iterations << ")\n\t" << 3+iter << "." << 1+step
				<< ". Scale (" << 1+step << ") of (" << N_scale << ") with MH scale-factor set to ** "
					<< scala[step] << " channels **\n";
			/*if(int(scala[step])<2 && kappa <3.)
				kappa=3.0;*/
			if(!bgk_cho){
				fftw_execute(external_bgk_plan);
				wavelet_filter(external_transformed, external_convoluted, scala[step], bin_x, bin_y);
				fftw_execute(back_external_bgk_plan);

				/*cout << "done.\n\t\tB. Median Filter... ";
				int mmm=int(median_box*scala[step]);//int(median_box*scala[step])
				median_filter_2d(external_filtered, median_filtered, mmm, bin_x, bin_y);
				
				fftw_execute(median_plan);
				wavelet_filter(median_transformed, median_convoluted, scala[step], bin_x, bin_y);
				fftw_execute(back_median_plan);*/

			}else{
				/*cout<<"\t\tA. Gaussian Filter... ";
				filtro_gaussiano(transformed_input, gauss_conv, n_sigma*scala[step], bin_x, bin_y);
				fftw_execute(back_input_plan);*/
				
				cout << "done.\n\t\tB. Median Filter... ";
				int mmm=int(median_box*scala[step]);//int(median_box*sqrt(scala[step]*scala[step]+n_sigma*n_sigma));//int(median_box*scala[step])
				median_filter_2d(gauss_filtered, median_filtered, mmm, bin_x, bin_y);
				
				fftw_execute(median_plan);
				wavelet_filter(median_transformed, median_convoluted, scala[step], bin_x, bin_y);
				fftw_execute(back_median_plan);
			}
			
			cout << "done.\n\t\tC. WAVELET TRANSFORM MAP building (WT of the input image)...";
			
			wavelet_filter(transformed_input, WTI_image, scala[step], bin_x, bin_y);
			
			fftw_execute(back_WTI_image);
			cout << "done.\n\t\tD. THRESHOLD and OVER-THRESHOLD MAP building... ";
			
			//External
			if(!bgk_cho){
				//naxes[0]=bin_y; naxes[1]=bin_x;
				threshold_map_2d(external_bkg, threshold_map, output_image, significat, kappa, scala[step], naxes, iter, step, yes);
			}else{
				threshold_map_2d(median_filtered, threshold_map, output_image, significat, kappa, scala[step], naxes, iter, step, yes);
			}
			//naxes[0]=bin_x; naxes[1]=bin_y;
			for (ii = 0; ii < bin_x; ii++) {
				for (jj = 0; jj < bin_y; jj++) {
					if (output_image(ii, jj) >threshold_map(ii, jj) && (output_image(ii, jj) >= BGW_map(ii, jj)))
						OTM_image(ii, jj) =  output_image(ii, jj);
					else
						OTM_image(ii, jj) = 0;
				}
			}
			char nome_file_bkg[128];
			sprintf(nome_file_OTM, "OTM_scan%i_scale%i.fits", iter+1, step+1);
			sprintf(nome_file_bkg, "BKG_scan%i_scale%i.fits", iter+1, step+1);
			sprintf(nome_file_WT, "WT_scan%i_scale%i.fits", iter+1, step+1);
			fitsfile *ff = new fitsfile;
			fitsio_all_in_one(ff, nome_file_WT, nome_file_in,output_image , naxes, 0);
			fitsio_all_in_one(pt_file_OTM[step], nome_file_OTM, nome_file_in,OTM_image , naxes, 0);
			/*fitsfile *fff = new fitsfile;
			if(bgk_cho!=0)
				fitsio_all_in_one(fff, nome_file_bkg, nome_file_in, BGWI_map, naxes, 0);
			else
				fitsio_all_in_one(fff, nome_file_bkg, nome_file_in, BGW_map, naxes, 0);
			delete fff;*/
			cout << "done.\n\t\tE. Candidate-sources search...";
			ms_box = int(scala[step]*sqrt(2.)+0.5);
			
			m_num= otpix[step];
			
			
				//m_num = 10;//int(scala[step]*scala[step]*2);
			/*else if(ms_box >2 && ms_box<4){
				ms_box = 5;	
				//m_num = 14;//int(scala[step]*scala[step]*2);
			}else{
				ms_box = ms_box+1;
				//m_num=MAX(12,ms_box*(ms_box-1));//m_num_fit(ms_box);
			}*/
			
			sprintf(nome_file_detected, "Detected_scan%i_scale%i", iter+1, step+1);
			obj[step].SetDetectedOutfile(nome_file_detected);
			//ms_box=5;
			find(nome_file_OTM, &obj[step], m_num, ms_box);
			find_stat(nome_file_in, &obj[step], pt_file_in, verbose);
			SNR_cut(&obj[step], &list[step], SN_ratio[iter], scala[step], min_pix,significat);
			if(bgk_cho!=0){
				print_copy(gauss_filtered, gauss_output, step, nax3);
				print_copy(median_filtered, median_output, step, nax3);
			}
			print_copy(output_image, WT_output, step, nax3);
			print_copy(threshold_map, threshold_output, step, nax3);
			print_copy(significat, significat_output, step, nax3);
		}
		if (yes == 0){
			cout << "\n\n************************************\n\t" << 3+iter << "." << 1+step << ". SCANNING RESULTS "
			<< 1+iter << " of " << N_iterations << "\n************************************" << endl;
		}
		//if (N_scale>1)
			select(list, flagged, min_pix, yes, enable, N_scale, scala);
		
		if(iter==0){
			cout << "\n\t" << 3+iter << "." << 1+step << "Selection of sources found in two consecutive scales\n" << endl;
			erase_source(flagged, input_image, min_pix, naxes, verbose);
			fitsio_all_in_one(pt_file_copyin, nome_file_copyin, nome_file_in, input_image, naxes, yes);
		}
		for (step = 0; step < N_scale; step++)
			list[step].set_clear();
		if(bgk_cho!=0){
			sprintf(nome_file_gauss, "GaussF_scan%i.fits", iter+1);
			fitsio_all_in_one(pt_file_gauss, nome_file_gauss, gauss_output, nax3, yes);
			sprintf(nome_file_median, "MF_scan%i_.fits", iter+1);
			fitsio_all_in_one(pt_file_median, nome_file_median, median_output, nax3, yes);
		}

		sprintf(nome_file_out, "WTF_scan%i_.fits", iter+1);
		fitsio_all_in_one(pt_file_out, nome_file_out, WT_output, nax3, yes);
		sprintf(nome_file_threshold, "ThresholdMap_scan%i.fits", iter+1);
		fitsio_all_in_one(pt_file_threshold, nome_file_threshold, threshold_output, nax3, yes);
		sprintf(nome_file_significat, "SignificatOK_scan%i.fits", iter+1);
		fitsio_all_in_one(pt_file_significat, nome_file_significat, significat_output, nax3, yes);

	}
	cout << "\n" << 3+iter << ". Removal of double detections and border-sources.";
	erase_border(flagged, border, naxes, far);
	erase_source(flagged, min_pix);
	cout << "\n\n****************************************************************************\n";
	cout <<  4+iter  << ". DETECTED SOURCES: " << flagged->xdimension() << "\n" << endl;
	if(verbose){
	for (aa =0; aa < flagged->xdimension(); aa++)
		cout << 1+aa << ". [x]: " << flagged->getx(aa) << "\t\t[y]: " << flagged->gety(aa) << "\t\t[SNR]: "
			<< flagged->getSNR(aa) <<"\t\tK-Signif: "<<flagged->getSignif(aa)<< "\t\t[Signal]: " << flagged->getmag(aa) <<"\t\t[SigC]: " << flagged->getmagerr(aa) << "\t\t[bkg]: "
			<< flagged->getbkg(aa) <<"\t\t[SigBkg]: " << flagged->getbkgerr(aa) << endl;
		cout << "\n****************************************************************************\n";
	}
	flagged->print_to_file(pt_file_in, nome_file_in);
	cout << "\n" << 5+ iter << ". Clean.\n";
	delete []obj;
	delete []list;
	delete flagged;
	fftw_destroy_plan(input_plan);
	fftw_destroy_plan(back_input_plan);
	fftw_destroy_plan(back_WTI_image);
	fftw_destroy_plan(median_plan);
	fftw_destroy_plan(back_median_plan);
	fftw_destroy_plan(external_bgk_plan);
	fftw_destroy_plan(back_external_bgk_plan);
	cout << "\n" <<  6+iter << ". Images ready, esecution ended.\n\n";
				
	/***************  --  Acknowledgment: Don Moreno ®  --  **************/			
	//PILClose(PIL_OK);
	return 0;
}


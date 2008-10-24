/************************************************************
 *												
 *  Wave2D
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
#include <string>
#include <cstdio>
#include <sstream>
#include <cstdlib>
#include <vector>
#include "pgwave/readParFile.h"

using namespace std;

const int maxNscale = 31;


int main ( int argc, char *argv[] )
{
	/*cout << "\n\n\t\t*********************************************************************\n"
		<< "\t\t***			WAVELET ANALYSIS			  ***\n"
		<< "\t\t***	-- Programm for point-like source detection --		  ***\n"
		<< "\t\t*********************************************************************\n\n";
	cout.flush();*/
	pgwPars pars;
	if(!pars.readParFile(argc,argv))
		exit(1);
		
	
	int step, iter, aa;
	long naxes[2];
	int anynull, nfound, ii, jj;
	int ms_box, m_num;
	
	int status = 0;
	fitsfile *pt_file_in= new fitsfile;
	
	if (fits_open_file(&pt_file_in, pars.nome_file_in, READONLY, &status)){
		printerror(status);
		return 0;
	}
	double PixScale=1;	
	char comm[40];
	if (fits_read_keys_lng(pt_file_in, "NAXIS", 1, 2, naxes, &nfound, &status))
		printerror(status);
	if (fits_read_key_dbl(pt_file_in,"CDELT2",&PixScale, comm, &status))
		printerror(status);
	//cout<<"pixScale="<<PixScale<<endl;
	const long int bin_x = naxes[0], bin_y = naxes[1];
	long nax3[3]={naxes[0], naxes[1], pars.N_scale};

	double nullval;
	long primo_pixel, n_pixel = bin_x*bin_y;
	cout << "\n1. Initialization and dynamic memory allocation. Execution in progress...\n";
	
	MyImageAnalisys *obj =new MyImageAnalisys[pars.N_scale];
	
	detect_sources *list = new detect_sources[pars.N_scale];
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
	array3d<double> gauss_output(bin_x, bin_y, pars.N_scale);
	array3d<double> median_output(bin_x, bin_y, pars.N_scale);
	array3d<double> WT_output(bin_x, bin_y, pars.N_scale);
	array3d<double> threshold_output(bin_x, bin_y, pars.N_scale);
	array3d<double> significat_output(bin_x, bin_y, pars.N_scale);
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
	char nome_file_out[256];
	char nome_file_gauss[256];
	char nome_file_median[256];							
	char nome_file_threshold[256];
	char nome_file_OTM[256];
	char nome_file_WT[256];
	char nome_file_detected[256];
	char nome_file_significat[256];
	primo_pixel = 1;                               /* first pixel to write      */
	nullval = 0;
	//external
	if(!pars.bgk_cho){
		if (fits_open_file(&pt_file_external_bkg, pars.nome_file_external_bkg, READONLY, &status)){
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
	if(!pars.bgk_cho){
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
	for (iter = 0; iter < pars.N_iterations; iter++)
	{
		cout << "\n" << 3 + iter << ". *** SCAN (" << iter+1 << ") of (" << pars.N_iterations << ") ***\n";
		
		fftw_execute(input_plan);
		if(pars.bgk_cho!=0){
			if(iter>=1)
				pars.n_sigma*=2.5;
			cout<<"\t\tA. Gaussian Filter... ";
			filtro_gaussiano(transformed_input, gauss_conv, pars.n_sigma, bin_x, bin_y);
			fftw_execute(back_input_plan);
		}
		
		for (step = 0; step < pars.N_scale; step++) {
			//scala[step]=scala[step]/PixScale;
			//double area=scala[step]*(scala[step]-1);
			//cout<<"AREA="<<area<<endl;
			cout << "Scan (" << 1+iter << ") of (" << pars.N_iterations << ")\n\t" << 3+iter << "." << 1+step
				<< ". Scale (" << 1+step << ") of (" << pars.N_scale << ") with MH scale-factor set to ** "
					<< pars.scala[step] << " channels **\n";
			/*if(int(scala[step])<2 && kappa <3.)
				kappa=3.0;*/
			if(!pars.bgk_cho){
				fftw_execute(external_bgk_plan);
				wavelet_filter(external_transformed,
				external_convoluted, pars.scala[step], bin_x, bin_y);
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
				int mmm=int(pars.median_box*pars.scala[step]);//int(median_box*sqrt(scala[step]*scala[step]+n_sigma*n_sigma));//int(median_box*scala[step])
				median_filter_2d(gauss_filtered, median_filtered, mmm, bin_x, bin_y);
				
				fftw_execute(median_plan);
				wavelet_filter(median_transformed,median_convoluted, pars.scala[step], bin_x, bin_y);
				fftw_execute(back_median_plan);
			}
			
			cout << "done.\n\t\tC. WAVELET TRANSFORM MAP building (WT of the input image)...";
			
			wavelet_filter(transformed_input, WTI_image, pars.scala[step], bin_x, bin_y);
			
			fftw_execute(back_WTI_image);
			cout << "done.\n\t\tD. THRESHOLD and OVER-THRESHOLD MAP building... ";
			
			//External
			if(!pars.bgk_cho){
				//naxes[0]=bin_y; naxes[1]=bin_x;
				threshold_map_2d(external_bkg, threshold_map,output_image, significat, pars.kappa,pars.scala[step], naxes, iter, step, pars.yes);
			}else{
				threshold_map_2d(median_filtered, threshold_map,output_image, significat, pars.kappa,pars.scala[step], naxes, iter, step, pars.yes);
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
			sprintf(nome_file_OTM, "%s_OTM_scan%i_scale%i.fits",pars.output_prefix, iter+1, step+1);
			sprintf(nome_file_bkg, "%s_BKG_scan%i_scale%i.fits",pars.output_prefix, iter+1, step+1);
			sprintf(nome_file_WT, "%s_WT_scan%i_scale%i.fits", pars.output_prefix,iter+1, step+1);
			fitsfile *ff = new fitsfile;
			fitsio_all_in_one(ff, nome_file_WT, pars.nome_file_in,output_image , naxes, 0);
			fitsio_all_in_one(pt_file_OTM[step], nome_file_OTM,pars.nome_file_in,OTM_image , naxes, 0);
			/*fitsfile *fff = new fitsfile;
			if(pars.bgk_cho!=0)
				fitsio_all_in_one(fff, nome_file_bkg, nome_file_in, BGWI_map, naxes, 0);
			else
				fitsio_all_in_one(fff, nome_file_bkg, nome_file_in, BGW_map, naxes, 0);
			delete fff;*/
			cout << "done.\n\t\tE. Candidate-sources search...";
			ms_box = int(pars.scala[step]*sqrt(2.)+0.5);
			
			m_num= int(pars.otpix[step]);
			
			
				//m_num = 10;//int(scala[step]*scala[step]*2);
			/*else if(ms_box >2 && ms_box<4){
				ms_box = 5;	
				//m_num = 14;//int(scala[step]*scala[step]*2);
			}else{
				ms_box = ms_box+1;
				//m_num=MAX(12,ms_box*(ms_box-1));//m_num_fit(ms_box);
			}*/
			
			sprintf(nome_file_detected, "%s_Detected_scan%i_scale%i",pars.output_prefix, iter+1, step+1);
			obj[step].SetDetectedOutfile(nome_file_detected);
			//ms_box=5;
			find(nome_file_OTM, &obj[step], m_num, ms_box);
			find_stat(pars.nome_file_in, &obj[step], pt_file_in, pars.verbose);
			SNR_cut(&obj[step], &list[step], pars.SN_ratio[iter],pars.scala[step], pars.min_pix,significat);
			if(pars.bgk_cho!=0){
				print_copy(gauss_filtered, gauss_output, step, nax3);
				print_copy(median_filtered, median_output, step, nax3);
			}
			print_copy(output_image, WT_output, step, nax3);
			print_copy(threshold_map, threshold_output, step, nax3);
			print_copy(significat, significat_output, step, nax3);
		}
		if (pars.yes == 0){
			cout << "\n\n************************************\n\t" << 3+iter << "." << 1+step << ". SCANNING RESULTS "
			<< 1+iter << " of " << pars.N_iterations << "\n************************************" << endl;
		}
		//if (pars.N_scale>1)
			select(list, flagged, pars.min_pix, pars.yes, pars.enable, pars.N_scale, pars.scala);
		
		if(iter==0){
			cout << "\n\t" << 3+iter << "." << 1+step << "Selection of sources found in two consecutive scales\n" << endl;
			erase_source(flagged, input_image, pars.min_pix, naxes, pars.verbose);
			fitsio_all_in_one(pt_file_copyin, nome_file_copyin, pars.nome_file_in, input_image, naxes, pars.yes);
		}
		for (step = 0; step < pars.N_scale; step++)
			list[step].set_clear();
		if(pars.bgk_cho!=0){
			sprintf(nome_file_gauss, "%s_GaussF_scan%i.fits", pars.output_prefix,iter+1);
			fitsio_all_in_one(pt_file_gauss, nome_file_gauss, gauss_output, nax3, pars.yes);
			sprintf(nome_file_median, "%s_MF_scan%i_.fits",pars.output_prefix, iter+1);
			fitsio_all_in_one(pt_file_median, nome_file_median, median_output, nax3, pars.yes);
		}

		sprintf(nome_file_out, "%s_WTF_scan%i_.fits",pars.output_prefix, iter+1);
		fitsio_all_in_one(pt_file_out, nome_file_out, WT_output, nax3, pars.yes);
		sprintf(nome_file_threshold, "%s_ThresholdMap_scan%i.fits",pars.output_prefix, iter+1);
		fitsio_all_in_one(pt_file_threshold, nome_file_threshold, threshold_output, nax3, pars.yes);
		sprintf(nome_file_significat, "%s_SignificatOK_scan%i.fits",pars.output_prefix, iter+1);
		fitsio_all_in_one(pt_file_significat, nome_file_significat, significat_output, nax3, pars.yes);

	}
	cout << "\n" << 3+iter << ". Removal of double detections and border-sources.";
	erase_border(flagged, pars.border, naxes, pars.far);
	erase_source(flagged, pars.min_pix);
	cout << "\n\n****************************************************************************\n";
	cout <<  4+iter  << ". DETECTED SOURCES: " << flagged->xdimension() << "\n" << endl;
	if(pars.verbose){
	for (aa =0; aa < flagged->xdimension(); aa++)
		cout << 1+aa << ". [x]: " << flagged->getx(aa) << "\t\t[y]: " << flagged->gety(aa) << "\t\t[SNR]: "
			<< flagged->getSNR(aa) <<"\t\tK-Signif: "<<flagged->getSignif(aa)<< "\t\t[Signal]: " << flagged->getmag(aa) <<"\t\t[SigC]: " << flagged->getmagerr(aa) << "\t\t[bkg]: "
			<< flagged->getbkg(aa) <<"\t\t[SigBkg]: " << flagged->getbkgerr(aa) << endl;
		cout << "\n****************************************************************************\n";
	}
	flagged->print_to_file(pt_file_in, pars.nome_file_in,pars.output_prefix);
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


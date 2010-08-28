// MyImageAnalisys.h: interface for the MyImageAnalisys class.
//
// Author: Gino Tosti 
// Date  : 13 August 2003 (started) 
// Version 0.1
//
// Classe derivata adattando il codice della tesi di Fabio Roncella
//////////////////////////////////////////////////////////////////////

#if !defined MYIMAGEANALISYS_H
#define MYIMAGEANALISYS_H

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <vector>
using namespace std;
#define UINT unsigned int
#define WORD    unsigned short int
#define LPWORD	WORD*

const int MAXSTAR=10000;		//	Massimo numero stelle considerate da FIND



typedef struct cielo
{						//	Parametri sky determination (v. Naylor p.342)
	float skymn;		//	Mean sky value
	float skymed;		//	Median sky value
	float skymod;		//	Modal sky value
	float skysig;		//	Deviazione standard
	float skyskw;		//	Skewness
	int	  skynpx;
	float skymax;
	float skymin;
}SKY;


typedef struct center{	//	Centroide stella
	int id;
	float x;
	float y;
	float peak;
	float bkg;
	float bkgsig;
	int nbkgpix;
	float fwhmX;
	float fwhmY;
	float xmin;
	float xmax;
	float ymin;
	float ymax;
	float dx;
	float dy;
	float area;
	int	  flag;
	float xpeak;
	float ypeak;
	float elong;
	float ellip;
	float theta;
	float A;
	float B;
	float cxx;
	float cyy;
	float cxy;
	float mag;
	float magerr;
}CENTRO;


//	La struttura SORTSTAR è definita in 'ImagePro.h'.


typedef struct magni {  
	       	//CENTRO centro;
			//SKY cielo;
			double apmag;	//	Magnitudine (aperture 1-12)
			float magerr;	//	Errori relativi
			float fwhm;			//	Full Width at Half Maximum
			float sigfwhm;		//	Deviazione standard relativa
			char name[15];
			int nap;
			float FIL;
			float EFIL;
			int defaperad;
}MAG_APER;


class MyImageAnalisys  
{
public:
	int CleanDetectedObj();
	int Deblend(CENTRO cen,int nthre);
	int FindStobieBackground(int w,int h);
	int Segmentation();
	int Segmentation_1(int box);
	int CleanBox(int ix,int iy,int fx,int fy,float val);
	int CleanHotPixels();
	int SaveFitsImage(const char *filename);
	int SaveFitsImage(int *b,const char *filename);
	int SaveFitsImage(float *b,const char *filename);
	int SaveBackImage(const char *filename);
	int FilterImage(int filtertype=0,int size=3);
	int SetPix(int x,int y,float val);
	float GetPix(int x,int y);
	float GetBack(int x,int y);
	float GetBackSig(int x,int y);
	MyImageAnalisys();
	virtual ~MyImageAnalisys();


	    int WriteSpectra(center cen);
	    void CheckBorder(center &cen, int* bmap);
	    int Overlap(center cen, center cend);
    int SetImage(float *ima, short nc,short nr);
	
	// function usate by FindBackgroud()
	int BackgroundStat(float *sky, long int nsky);
	int BackgroundStat(vector<float> & sky1,SKY &smod);

	// Background evaluation Function 
	int FindBackground(long int max);
	int LocalBackground(center& cen, double r1, double r2);
	int LocalBackground(center& cen){};

	int SkyVectorStat(vector<float>& sky1,SKY &smod);
	//Aperture Photom
	int ApPhot();
	int SegImage(int box);
	
	// FindStar
	int FindStar();
	int FindSpectrum();
	void PrintBackground();
	void PrintParameters();
	void PrintStarCatalog();
	void SaveStarCatalog(const char *outfilename);
	// Set/Get Parameter Functions 
	void SetDetectedOutfile(const char *ff){m_detectfile=ff;};
	void SetCcdGain(const double g){m_opt01=float(g);};	 //	READ NOISE (ADU) 
	void SetCcdRon (const double r){m_opt00=float(r);};	 //	GAIN (e-/ADU) 
	void SetLowGoodDat(const double r){m_opt02=float(r);}; //	LOW GOOD DATUM (in sigmas)
	void SetHighGoodDat(const double r){m_opt03=float(r);};//HIGH GOOD DATUM (in ADU)
	void SetTrialFwhm(const double r){m_opt04 =float(r);};         //FWHM OF OBJECT
	void SetDetectionThres(const double r){m_opt05 =float(r);};    //	THRESHOLD (in sigmas)
	void SetLowSharpCutoff (const double r){m_opt06 =float(r);};   //LOW SHARPNESS CUTOFF
	void SetHighSharpCutoff (const double r){m_opt07=float(r);};   //HIGH SHARPNESS CUTOFF
	void SetLowRoundCutoff (const double r){m_opt08 =float(r);};   //LOW ROUNDNESS CUTOFF
	void SetHighRoundCutoff (const double r){m_opt09 =float(r);};  //HIGH ROUNDNESS CUTOFF
	void SetMaxBox(const unsigned int r){m_maxbox =r;};
	void SetMaxBoxXY(const unsigned int r,const unsigned int r1){m_maxboxX =r;
																m_maxboxY =r1;};
	void SetTrialFwhmXY(const double r,const double r1){m_fwhmX =float(r);
														m_fwhmY =float(r1);};         //FWHM OF OBJECT
														   
	void SetLowerSkyLevel(const unsigned int r){m_minsky=r;} //(ADU)
	void SetMaxNumSkyPix(const unsigned int r){m_maxsky=r;}  //	Numero massimo di pixel per la valutazione del cielo
	void SetNunAper(UINT r)	{m_maxap=r;}
	void SetMinNumPixOverThres(int r){m_minumpx=r;};
	float GetCcdGain()         {return m_opt01;};
	float GetCcdRon ()         {return m_opt00;};
	float GetLowGoodDat()      {return m_opt02;};
	float GetHighGoodDat()     {return m_opt03;};
	float GetTrialFwhm()       {return m_opt04 ;}; 
	float GetDetectionThres()  {return m_opt05 ;}; 
	float GetLowSharpCutoff () {return m_opt06 ;}; 
	float GetHighSharpCutoff (){return m_opt07;};  
	float GetLowRoundCutoff () {return m_opt08 ;}; 
	float GetHighRoundCutoff (){return m_opt09 ;}; 
	unsigned int GetMaxBox()   {return m_maxbox;};
	unsigned int GetLowerSkyLevel(){return m_minsky;};
	unsigned int GetMaxNumSkyPix(){return m_maxsky;} ;
	unsigned int GetNumStarFound(){return m_pnstar;};
	void SaveStarCatalogShort(const char *outfilename);
	// Set General Parameter Functions
	void SetExposureTime(const double e){m_expt=float(e);};
	void SetMagZeroPoint(const double r){m_magzero=float(r);};
	int StarCenter(center& cen, double side_box);
	int SegImage();

													   /*	int FindStar(float *hhh, WORD maxbox,WORD maxsky,LPWORD nstar, CENTRO *centro);
	int AperPhot (float *hhh,WORD maxsky, float tempo, WORD nstar, MAG_APER *stella);
	void riporta(char frase[]);
	int Scrivi_Header_File_FOT();
	int Scrivi_Val_File_FOT(MAG_APER *stella);
	int Scrivi_Val_File_USERFOT(char *nomefile, MAG_APER *stella, int rad);
	int Scrivi_Header_File_USERFOT(char *nomefile);*/
	
	CENTRO centro[MAXSTAR];	//	aggiunto per FindStar

private:
	int StarCenter(center& cen);
	int Fwhm(center& cen);
	int PeakRange(center& cen);
	int PeakRange(center& cen,float limit);

	int		m_ncol;
	int		m_nrow;	
	float   *m_imabuffer;
	float   *m_backbuffer;
	float   *m_backsigbuffer;
	float	m_opt00;
	float	m_opt01;
	float	m_opt02;
	float	m_opt03;
	unsigned int	m_minsky;
	unsigned int	m_maxsky;
	float	m_opt04;
	float	m_opt05;
	float	m_opt06;
	float	m_opt07;
	float	m_opt08;
	float	m_opt09;
	unsigned int	m_MAXSTAR;
	unsigned int	m_maxbox;
	unsigned int	m_maxboxX;
	unsigned int	m_maxboxY;
	float	m_par01;
	float	m_par02;
	float	m_par03;
	float	m_par04;
	float	m_par05;
	float	m_par06;
	float	m_par07;
	float	m_par08;
	float	m_par09;
	float	m_par10;
	float	m_par11;
	float	m_par12;
	float	m_par13;
	float	m_par14;
	float   m_fwhmX;
	float   m_fwhmY;
	int		m_minumpx;
	UINT	m_maxiter;
	UINT	m_rigbox;
	UINT	m_colbox;
	UINT	m_numbrill;
	UINT	m_psfmodel;
	UINT	m_maxap;
	double	m_iniguess1;
	double	m_iniguess2;
	string m_detectfile;
	SKY m_smod;				//	struttura SKY con parametri background (float)
	WORD nstar;				//	aggiunto per FindStar (era LPWORD nstar)
	//SORTSTAR sortstar[MAXSTAR+1];	//	vettore di elementi del tipo struttura SORTSTAR.
			//	C'è +1 perché uso gli elementi dall'indice 1 in poi invece che da 0 (per
			//	darlo in pasto a una routine delle 'Recipes'); sennò nel caso FIND trovasse 
			//	MAXSTAR stelle mi perderei l'ultima (benché nel SORT non sia poi un gran 
			//	problema).
	MAG_APER *stella;		//	Usato per APERPHOT.
	WORD global_minsky;
	WORD global_maxsky;
	WORD global_maxbox;
	float global_opt[20];

	//	modificati da dialog APFOT:
	float global_par[15];
	//	modificati da dialog FITMARQ:

	WORD global_maxiter;
	WORD global_rigbox;
	WORD global_colbox;
	WORD global_numbrill;
	double global_iniguess1;
	double global_iniguess2;
	char global_flagpsf;
	int global_psfmodel;

	//	modificati da dialog SYSPAR:
	WORD global_pixwidth;
	WORD global_pixheight;
	double global_focallenght;
	int m_background;
	float m_expt;
	float m_magzero;
	WORD m_pnstar;
	float m_lobad;
};

#endif 


// MyImage.h: interface for the CMyImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined MYIMAGE_H
#define MYIMAGE_H

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "fitsio.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <string>


using namespace std;

typedef vector<string> FITSHEADERCARD;

#define MAXFILENAME 256

class MyRGB {
public:
	unsigned char  red,
		       green,
		       blue;
};

class CelPos{
public:
	double lat, lon;
};

class ImgPos{
public:
	double x, y;
};

class CMyImage  
{
public:
	
	
	CMyImage();
	// arithmetic operations

	// with constants
	CMyImage& operator+=(float val);
	CMyImage& operator-=(float val);
	CMyImage& operator*=(float val);
	CMyImage& operator/=(float val);
	// with images
	CMyImage& operator+=(const CMyImage& rhs);
	CMyImage& operator-=(const CMyImage& rhs);
	CMyImage& operator*=(const CMyImage& rhs);
	CMyImage& operator/=(const CMyImage& rhs);
   
   	float GetPixel(int x, int y);
	int SetPixel(int x, int y, float value);
  	void CreateImage(long int ncol,long int nrow);	
	CMyImage& GetSubIma( int x,  int y, const int w, const int h);
	CMyImage& ExctractRossSpectrum( int x,  int y, const int w, const int h);
	unsigned char * GetByteIma(int flag=1,int flag2=0);
	
	int GetImageSlice(int n);
	float GetMax();
	float GetMin();
    double GetMean(){return m_mean;};
	double GetVar(){return m_var;};
	double GetStd(){return m_std;};
	double GetTotal(){return m_total;};
    void AddCol(vector<double>& vec, int rin,int rfin,int cinn,int cfin);
    void ExtractCol(vector<double>& vec,int col);
	int GetHeight();
	int GetWidth();
  	char * GetFname() {return fname;};
  	int  GetWCSFlag() {return Wcs;};
	virtual ~CMyImage();

public:
	int CloseFITSFile();
	int OpenFITSFile(const char *filename);
   	int ReadFITS();
   	int WriteFloatFITSImage(char * filename);
	int CopyFITSFile(const char *infile,const char *outfile);
    int ReadFITSHDUKey(const char *incard, char *value);
	int UpdateFITSHDUKey(const char *infile, const char *incard, const char *value);
	long int GetNaxesVal(int n);
	CMyImage& MosaicX(CMyImage& b);
	FITSHEADERCARD  GetFITSHeader();
	int WriteFITS(float *ima, long int ncol, long int nrow);
	int SetWCSInfo(const double xrfp, const double yrfp, const double xrfv, const 
	double yrfv, const double xi, const double yi, const char* xcoortype ,const 
	char* ycoortype );
	ImgPos WorldPix(double lon, double lat);
	CelPos PixWorld(double x,double y);
	int GetNAxis();
	
	
	MyRGB lut[256];
	void SetBuffer(float *ima, long int ncol,long int nrow);
   	void SetBuffer(unsigned short *ima, long int ncol,long int nrow);
protected:
	void LutRgb();
	void GrayLut();
	void SpectrumLut();
	void StatIma();
	

public:
	int GetKeyValue(const char * key, char* val);
	int GetKeyValue(const char * key, int& val);
	int GetKeyValue(const char * key, double& val);

	int Convert2Float(double *img);
	int SetHighImaCut(float val);
	int SetLowImageCut(float val);
	float * GetBuffer();
  	void GetBuffer(unsigned short *ima);
   	void GetBuffer(unsigned short *ima,const int x, const int y, const int w, const int h);
	int WriteWCSKeys();
  /** No descriptions */
   	CMyImage ( CMyImage& );
	

protected:
	void InitBuffer();
	long npixels;
	int bitpix;
	int nc;
	int nr;
	float datamin;
        float datamax;
	double m_mean;
	double m_var;
	double m_std;
	double m_total;
	float *buffer;
	char fname[MAXFILENAME];
	FITSHEADERCARD FitsHeader;
	int naxis;
	long int *nnaxes;
	fitsfile *infptr;
	fitsfile *outfptr;
	double xrefval;
	double yrefval;
	double xrefpix;
	double yrefpix;
	double xinc;
	double yinc;
	char coordtype[10];
	double rot;
	CelPos pos;
	ImgPos ipos;
	string xcotype;
	string ycotype;
   	float average;
   	float stdev;
	int Wcs;
};

#endif 


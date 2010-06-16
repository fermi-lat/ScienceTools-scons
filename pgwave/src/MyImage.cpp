// MyImage.cpp: implementation of the CMyImage class.
//
//////////////////////////////////////////////////////////////////////

#include "pgwave/MyImage.h"

#include <cstring>
#include <fstream>
#define FNAN 1e-40F

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyImage::CMyImage()
{
	nc=0;
	nr=0;
	//InitBuffer();
	buffer=NULL;
	npixels=0;
	naxis=2;
	nnaxes=0;
	datamin  = 1.0E30F;
	datamax  = -1.0E30F;
	m_mean=0.;
	m_var=0.;
	m_std=0.;
	m_total=0.;
	Wcs=0;
}

CMyImage::~CMyImage()
{
	if(buffer!=NULL)
			delete []buffer;
	//delete []fname;
}

void CMyImage::SetBuffer(float *ima, long int ncol,long int nrow)
{
	nc=(int)ncol;
	nr=(int)nrow;
	npixels=nc*nr;
	buffer=new float[nr*nc];
	memcpy(buffer,ima,sizeof(float)*nc*nr);

	StatIma();
}

void CMyImage::SetBuffer(unsigned short *ima, long int ncol,long int nrow)
{
	nc=(int)ncol;
	nr=(int)nrow;
	npixels=nc*nr;
	if(buffer!=NULL)
		delete []buffer;
	buffer=new float[nr*nc];
	for(long int i=0; i<npixels;i++)
		buffer[i]=(float)ima[i];

	StatIma();
}


void CMyImage::CreateImage(long int ncol, long int nrow)
{
	nc=(int)ncol;
	nr=(int)nrow;
	npixels=nc*nr;
	buffer=new float[nr*nc];
	InitBuffer();
}

float CMyImage::GetPixel(int x, int y)
{
	if(x<nc && x>=0 && y>=0 && y<nr)
		return *(buffer+nc*y+x);
	else
	    return -99; 
}

int CMyImage::SetPixel(int x, int y, float value)
{
	if(x<nc && x>=0 && y>=0 && y<nr)
		*(buffer+nc*y+x) = value;
	else
	    return -99;
	return 0;
}


CMyImage& CMyImage::operator+=(float val)
{
	long numPixels = (long)nc*nr;
	float* pPix = buffer;
	for (long i=0; i<numPixels; i++) {
		if (*pPix != FNAN) {
			*pPix += val;
		}
		pPix++;
	}
	
	// old meta data is no longer valid

	return *this;
}

CMyImage& CMyImage::operator-=(float val)
{
	long numPixels = (long)nc*nr;
	float* pPix = buffer;
	for (long i=0; i<numPixels; i++) {
		if (*pPix != FNAN) {
			*pPix -= val;
		}
		pPix++;
	}
	
	// old meta data is no longer valid

	return *this;
}

CMyImage& CMyImage::operator*=(float val)
{
	long numPixels = (long)nc*nr;
	float* pPix = buffer;
	for (long i=0; i<numPixels; i++) {
		if (*pPix != FNAN) {
			*pPix *= val;
		}
		pPix++;
	}
	
	// old meta data is no longer valid

	return *this;
}

CMyImage& CMyImage::operator/=(float val)
{
	long numPixels = (long)nc*nr;
	float* pPix = buffer;
	for (long i=0; i<numPixels; i++) {
		if (*pPix != FNAN) {
			*pPix /= val;
		}
		pPix++;
	}
	
	// old meta data is no longer valid

	return *this;
}

CMyImage& CMyImage::operator+=(const CMyImage& rhs)
{
	// they must be the same size
	assert(nc == rhs.nc);
	assert(nr == rhs.nr);
	long numPixels = (long)nc*nr;
	float* pPix = buffer;
	const float* pRhs = rhs.buffer;
	for (long i=0; i<numPixels; i++) {
		if (*pPix != FNAN &&
			*pRhs != FNAN) {
			*pPix += *pRhs;
		} else {
			*pPix = FNAN;
		}
		pPix++;
		pRhs++;
	}
	
	// old meta data is no longer valid

	return *this;
}

CMyImage& CMyImage::operator-=(const CMyImage& rhs)
{
	// they must be the same size
	assert(nc == rhs.nc);
	assert(nr == rhs.nr);
	long numPixels = (long)nc*nr;
	float* pPix = buffer;
	const float* pRhs = rhs.buffer;
	for (long i=0; i<numPixels; i++) {
		if (*pPix != FNAN &&
			*pRhs != FNAN) {
			*pPix -= *pRhs;
		} else {
			*pPix = FNAN;
		}
		pPix++;
		pRhs++;
	}
	
	// old meta data is no longer valid

	return *this;
}

CMyImage& CMyImage::operator*=(const CMyImage& rhs)
{
	// they must be the same size
	assert(nc == rhs.nc);
	assert(nr == rhs.nr);
	long numPixels = (long)nc*nr;
	float* pPix = buffer;
	const float* pRhs = rhs.buffer;
	for (long i=0; i<numPixels; i++) {
		if (*pPix != FNAN &&
			*pRhs != FNAN) {
			*pPix *= *pRhs;
		} else {
			*pPix = FNAN;
		}
		pPix++;
		pRhs++;
	}
	
	return *this;
}




int CMyImage::ReadFITS()
{
	int status=0,  anynull;
	long int fpixel1;
	float  nullval=0;
	int datatype=0;

	fpixel1   = 1L;
	nullval  = 0;                /* don't check for null values in the image */
	if(bitpix==FLOAT_IMG)
	   datatype=TFLOAT;
	if(bitpix==SHORT_IMG)
	   datatype=TSHORT;

	/*if ( fits_open_file(&infptr, fname, READONLY, &status) )
       		 return status;*/
	InitBuffer();
	
	if ( fits_read_img(infptr,TFLOAT, fpixel1, npixels, &nullval,
              buffer, &anynull, &status) ){
		  			 return status;
			 }
	//CloseFITSFile();
    //cout<<"letta"<<endl;
	StatIma();
	return 1;
}

int CMyImage::WriteFITS(float *ima, long int ncol, long int nrow) 
{ 
	buffer=new float[nrow*ncol]; 
	memcpy(buffer,ima,sizeof(float)*nrow*ncol); 
 
 	int status; 
	long  fpixel; 
 
	/* initialize FITS image parameters */ 
	bitpix   =  FLOAT_IMG; 
	long int naxes[2]; 
	naxes[0]=ncol; 
	naxes[1]=nrow; 
                                  
    	remove("pippo");               /* Delete old file if it already exists */ 
 
    	status = 0;         /* initialize status before calling fitsio routines */ 
 
    	if (fits_create_file(&outfptr, "pippo", &status)) /* create new FITS file */ 
		return ( status );           /* call printerror if error occurs */                   
	//cout<<outfptr<<endl;

	if (fits_create_img(outfptr, bitpix, naxis, naxes, &status) ) 
		return ( status );           
 	//cout<<status<<endl;

	/* initialize the values in the image with a linear ramp function */ 
    	fpixel = 1;                               /* first pixel to write      */ 
 
	/* write the array of unsigned integers to the FITS file */ 
    	if ( fits_write_img(outfptr, TFLOAT, fpixel, npixels, buffer, &status) ) 
        	return( status ); 
	//cout<<status<<endl;

	if ( fits_close_file(outfptr, &status) )                /* close the file */ 
		return( status );            
 	//cout<<status<<endl;

    return 0; 
}


int CMyImage::GetWidth()
{
	return nc;	
}

int CMyImage::GetHeight()
{
	return nr;
}


float CMyImage::GetMin()
{

    return datamin;
}

float CMyImage::GetMax()
{
	return datamax;
}

void CMyImage::StatIma()
{

	long ii;
	datamin  = 1.0E30F;
        datamax  = -1.0E30F;
        double tot2=0;
	for (ii = 0; ii < npixels; ii++)  {
        if ( buffer[ii] < datamin )
            datamin = buffer[ii];

        if ( buffer[ii] > datamax )
            datamax = buffer[ii];
	m_total+=double(buffer[ii]);
	tot2+=double(buffer[ii]*buffer[ii]);
	
      }
	m_mean=m_total/double(npixels);
	m_var=(double(npixels)*tot2 -m_total*m_total)/double(npixels*npixels);
	m_std=sqrt(m_var);
}




int CMyImage::GetImageSlice(int n)
{
		//fitsfile *infptr;       /* pointer to the FITS file, defined in fitsio.h */
    int status,  anynull;
	long int fpixel[4]={1,1,1,1};
	
    float  nullval=0;
	int datatype=0;
	
    status = 0;
	
	

    /* read the NAXIS1 and NAXIS2 keyword to get image size */
   //cout<<naxis<<" "<<naxes[0]<<endl;
	/*if ( fits_open_file(&infptr, fname,READONLY,&status))
		 return status;*/

   if(bitpix==FLOAT_IMG)
	   datatype=TFLOAT;
   if(bitpix==DOUBLE_IMG)
	   datatype=TDOUBLE;

	//int nimg=(int)naxes[2];
	InitBuffer();
    nullval  = 0;                /* don't check for null values in the image */
    fpixel[2]=(long int)n;
      if ( fits_read_pix(infptr, TFLOAT, fpixel, npixels, &nullval,
                  buffer, &anynull, &status) ){
         //AfxMessageBox("ERROR!!!");
			 return status;
			 }
	//CloseFITSFile();
	StatIma();
	return 1;
}

//DEL float CMyImage::GetAxesStartVal(int n)
//DEL {
//DEL 	assert(n<=naxis);
//DEL 	return crval[n-1];
//DEL }

//DEL float CMyImage::GetAxesInc(int n)
//DEL {
//DEL 	assert(n<=naxis);
//DEL 	return crdel[n-1];
//DEL }

/* 
	Return Image axes
*/

int CMyImage::GetNAxis()
{
	return naxis;
}

const unsigned int ncolors = 256;
const unsigned int MAXLUTVALUE = 256;
unsigned char * CMyImage::GetByteIma(int flag,int pal)
{
	long int nbyte=nc*nr*3,k=0;
	unsigned char *byima = new unsigned char[nbyte];
	unsigned char val;
	StatIma();
	if(pal==0)
		GrayLut();
	else if(pal==1)
		SpectrumLut();	   
	else if(pal==2)
		LutRgb();
	for (long int i=0;i<nc;i++)
		for(long int j=0;j<nr;j++){	
			switch(flag){
		case 1:
			val =(unsigned char)( ncolors*(log10( 1.+65000.*fabs( (GetPixel(i,j)-datamin)/(datamax-datamin)))/(log10(65001.))));
			break;
		case 2:
			val = (unsigned char)(ncolors*sqrt((GetPixel(i,j)-datamin)/(datamax-datamin)));
			break;
		default:
			val = (unsigned char)(ncolors*((GetPixel(i,j)-datamin)/(datamax-datamin)));
			break;
		}


	
		 byima[3*(j*nc+i)]=lut[val].red;
		 byima[3*(j*nc+i)+1]=lut[val].green;
		 byima[3*(j*nc+i)+2]=lut[val].blue;
		
	}

	return byima;
}



void CMyImage::LutRgb(){

  unsigned char c, step;
  int i;

  c = 0;
  step = (unsigned char)((ncolors - 1) / 1.0);
  
  for(i=0; i<ncolors; i++){
    if ( c < ncolors )
      lut[i].blue = c;
    else {
      c = 0;
      lut[i].blue = c;
    }
    c += step;
  }
  
  step = (unsigned char)((double)(ncolors - 1) / 3.0 + 0.5);
  c = 0 ;
  for(i=0; i<ncolors; i++){
    if ( c < ncolors )
      lut[i].green = c;
    else {
      c = 0;
      lut[i].green = c;
    }
    c += step;
  }
  step = (unsigned char)((double)(ncolors - 1) / 5.0 +0.5);
  c=0;
  for(i=0; i<ncolors; i++){
    if ( c < ncolors )
      lut[i].red = c;
    else {
      c = 0;
      lut[i].red= c;
    }
    c += step;
  }
}



FITSHEADERCARD  CMyImage::GetFITSHeader()
{
	//fitsfile *infptr;
	int status = 0,ii;
    	int nkeys=1;
	string pippo("");
   if(FitsHeader.size()>0)
       FitsHeader.clear();
	char card[FLEN_CARD];
       /*if ( fits_open_file(&infptr, fname, READONLY, &status) ){
           FitsHeader.push_back(pippo);
	   return FitsHeader;
       }*/
	fits_get_hdrspace(infptr,&nkeys,NULL,&status);

	for(ii=1; ii<=nkeys;ii++){
		fits_read_record(infptr,ii,card,&status);
		pippo=card;
		FitsHeader.push_back(pippo);
		pippo.erase();

	}

   /* if ( fits_close_file(infptr, &status) ){
         return FitsHeader;
			 }*/
	return FitsHeader;
}

void CMyImage::SpectrumLut()
{

  int 		i;
  double 	pi_over_4, f1, f2, aa, bb, wavelength, s, delta;
  const double S1=2.;
  const double S2=7.;
  /* Compute some necessary values */
  pi_over_4 = atan(1.0);	/* for later */
  aa = (2. * S1 - S2) / (S1 * S2);
  bb =      (S2 - S1) / (S1 * S2);
  delta = 1.0 / (ncolors - 1.0);
  
  /* Go thru each color of the spectrum and load with the proper values */
  for (i=0; i<ncolors; i++) {

    /* Compute the distance along a contour in RGB space */
    wavelength = i * delta;
    s = wavelength / (aa * wavelength + bb);
    
    if (s <= 0.0) {		/* Black floor */
      lut[i].red = 0;
      lut[i].green = 0;
      lut[i].blue = 0;
      
    } 
    else if (s <= 1.0) {	/* Black to red */
      lut[i].red = (unsigned char)(s * MAXLUTVALUE) ;
      lut[i].green = 0;
      lut[i].blue = 0;
    } 
    else if (s <= 2.0) {	/* Red to yellow */
      lut[i].red = (unsigned char)MAXLUTVALUE ;
      lut[i].green = (unsigned char)((s - 1.0) * MAXLUTVALUE) ;
      lut[i].blue = 0;
    } 
    else if (s <= 3.0) {	/* Yellow to green */
      lut[i].red = (unsigned char)(MAXLUTVALUE  - ((s - 2.0) * MAXLUTVALUE ));
      lut[i].green = (unsigned char)MAXLUTVALUE ;
      lut[i].blue = 0;
    } 
    else if (s <= 4.0) {	/* Green to cyan */
      f1 = (s - 3.0) * pi_over_4;
      lut[i].red = 0;
      lut[i].green = (unsigned char)(cos(f1) * MAXLUTVALUE) ;
      lut[i].blue = (unsigned char)(sin(f1) * MAXLUTVALUE) ;
    } 
    else if (s <= 5.0) {	/* Cyan to blue */
      f1 = (s - 3.0) * pi_over_4;	/* Yes, s-3, not s-4 here */
      lut[i].red = 0;
      lut[i].green = (unsigned char)(cos(f1) * MAXLUTVALUE) ;
      lut[i].blue =(unsigned char)(sin(f1) * MAXLUTVALUE) ;
    } 
    else if (s <= 6.0) {	/* Blue to magenta */
      f1 = (s - 5.0) * pi_over_4;
      lut[i].red = (unsigned char)(sin(f1) * MAXLUTVALUE) ;
      lut[i].green = 0;
      lut[i].blue = (unsigned char)(cos(f1) * MAXLUTVALUE) ;
    } 
    else if (s <= 7.0) {	/* Magenta to white */
      f1 = s - 6.0;
      f2 = (f1 + (1.0-f1)/sqrt(2.0));
      lut[i].red = (unsigned char)(f2 * MAXLUTVALUE) ;
      lut[i].green = (unsigned char)(f1 * MAXLUTVALUE) ;
      lut[i].blue = (unsigned char)(f2 * MAXLUTVALUE) ;
    } 
    else {			/* Saturate to white */
      lut[i].red = (unsigned char)MAXLUTVALUE ;
      lut[i].green = (unsigned char)MAXLUTVALUE ;
      lut[i].blue = (unsigned char)MAXLUTVALUE ;
    }

  } /* end for each color in spectrum */

}

void CMyImage::GrayLut()
{
	for (int i=0; i<ncolors; i++) {
	  lut[i].red = (unsigned char)i;
      lut[i].green = (unsigned char)i ;
      lut[i].blue = (unsigned char)i ;
	}
}

CMyImage& CMyImage::GetSubIma(const int x, const int y, const int w, const int h)
{
	CMyImage *rsh = new CMyImage;
	//ofstream out("pippo");
	float *ima =new float[w*h];
	assert(w<=nc);
	assert(h<=nr);
	int k=0;
	assert (x>=0 && y >=0);
	for (int i=x;i<(x+w);i++){
		for (int j=y;j<(y+h);j++){
				ima[k]=GetPixel(i,j);
				//out<<GetPixel(i,j)<<" "<<ima[k]<<" ";
		k++;
		}
	}

	rsh->SetBuffer( ima, (long int)w,(long int)h);
	//out.close();
	return *rsh;

}

CMyImage& CMyImage::ExctractRossSpectrum( int x,  int y, const int w, const int h)
{
	CMyImage *rsh = new CMyImage;
	//ofstream out("pippo");
        int xw=(x+w);
        int yh=(y+h);
        if(x<=0) x=0;
        if(y<=0) y=0;
        if(xw<=0) xw=0;
        if(yh<=0) yh=0;
        if(xw>nc) xw=nc;
        if(yh>nr) yh=nr;
	float *ima =new float[w*h];
	int k=0;
	for (int i=x;i<(xw);i++){
		for (int j=y;j<(yh);j++){
				ima[k]=GetPixel(j,i);
				//out<<GetPixel(i,j)<<" "<<ima[k]<<" ";
                                if(ima[k]<0.){
                                  ima[k]=0.;
				}
		k++;
		}
	}

	rsh->SetBuffer( ima, (long int)h,(long int)w);
	//out.close();
	return *rsh;

}
void CMyImage::AddCol(vector<double>& vec, int rin,int rfin,int cinn,int cfin)
{
        int k=0;
        double sum=0;
        if(rin<=0) rin=0;
        if(rfin>nr) rfin=nr;
        if(cinn<=0) rin=0;
        if(cfin>nc) cfin=nc;
	for (int i=rin;i<rfin;i++){
                sum=0.;
		//cout<<i<<" ";
		for (int j=cinn;j<cfin;j++){
				sum+=(double)GetPixel(j,i);				
			//cout<<" "<<GetPixel(j,i)<<" ";	 
		}
          //cout<<"So="<<sum<<endl;
          vec.push_back(sum);
          k++;
	}
}

void CMyImage::ExtractCol(vector<double>& vec,int col)
{
        double sum=0.;
        if(col<=0) col=0;
        if(col>nc) col=nc;
	for (int i=0;i<nr;i++){
		
	 sum=(double)GetPixel(col,i);						 
         vec.push_back(sum);
	}
}

void CMyImage::GetBuffer(unsigned short *ima,const int x, const int y, const int w, const int h)
{ 
	assert(w<=nc);
	assert(h<=nr);
	int k=0;
	assert (x>=0 && y >=0 && (x+w)<=nc && (y+h)<=nr);
	for (int i=x;i<(x+w);i++){
		for (int j=y;j<(y+h);j++){
				ima[k]=(unsigned short )GetPixel(j,i);				
				k++;
		}
	}
}


int CMyImage::WriteFloatFITSImage(char * filename)
{
	//fitsfile *fptr;       /* pointer to the FITS file, defined in fitsio.h */
    int status;
    long  fpixel;
	//float exposure;
    /* initialize FITS image parameters */
    bitpix   =  FLOAT_IMG   ; /* */
	long int naxes[2];
	naxes[0]=nc;
	naxes[1]=nr;
                                 
    remove(filename);               /* Delete old file if it already exists */

    status = 0;         /* initialize status before calling fitsio routines */

    if (fits_create_file(&outfptr, filename, &status)) /* create new FITS file */
         return ( status );           /* call printerror if error occurs */                  

    if ( fits_create_img(outfptr,  bitpix, naxis, naxes, &status) )
         return ( status );          

    /* initialize the values in the image with a linear ramp function */
    fpixel = 1;                               /* first pixel to write      */

    /* write the array of unsigned integers to the FITS file */
    if ( fits_write_img(outfptr, TFLOAT, fpixel, npixels, buffer, &status) )
        return( status );

    /* write another optional keyword to the header 
    Note that the ADDRESS of the value is passed in the routine 
    exposure = 1500.;
    if ( fits_update_key(outfptr, TFLOAT, "EXPOSURE", &exposure,
         "Total Exposure Time", &status) )
         return( status );           */

    if ( fits_close_file(outfptr, &status) )                /* close the file */
         return( status );           

    return 0;
}

CMyImage& CMyImage::MosaicX(CMyImage& b)
{
	
	CMyImage *rsh = new CMyImage;

	int bnc=b.GetWidth(), bnr=b.GetHeight();
	long int xnc=bnc+nc, xnr=(nr>bnr)?nr:bnr;
	long int pp=xnc*xnr;
	long int li, lj;
	float *ima =new float[xnc*xnr];
	for(li=0; li<pp;li++)
		ima[li]=0.0;

	for(li=0;li<xnr;li++)
		for(lj=0;lj<xnc;lj++){
			if(li<nr && li<bnr && lj<nc)
				ima[li*xnc+lj]=GetPixel(li,lj);
			else if(li>bnr && li<nr && lj<nc)
				ima[li*xnc+lj]=GetPixel(li,lj);
			else if(li<nr && li<bnr && lj>=nc)
				ima[li*xnc+lj]=b.GetPixel(li,lj-nc);
			//else if(li>=bnr && li<nr && lj>=nc)
				//ima[li*xnc+lj]=b.GetPixel(li,lj-nc);
	}
	rsh->SetBuffer( ima, xnc,xnr);
	//delete ima;
	return *rsh;
}

long int CMyImage::GetNaxesVal(int n)
{
	if(nnaxes!=NULL)
		return nnaxes[n];
	return 0;
}

void CMyImage::InitBuffer()
{
	if(buffer!=NULL) delete []buffer;
	buffer=new float[npixels];
	for(long int i=0; i<npixels;i++)
		buffer[i]=0.0;
}

int CMyImage::OpenFITSFile(const char *filename)
{
	int status;
    long int naxes[10];
	
	int maxdim=10;
    
    //cout<<filename<<endl,
	strcpy(fname,filename);
    status = 0;
	
    if ( fits_open_file(&infptr, filename, READONLY, &status) ){
         
			 return status;
			 }
	if ( fits_get_img_param(infptr,maxdim,&bitpix,&naxis,naxes, &status) ){
		   return status;
         
	}
	nnaxes=new long int[naxis];
	memcpy(nnaxes,naxes,naxis*sizeof(long int));  
    nc=naxes[0];
	nr=naxes[1];
    npixels  = naxes[0] * naxes[1];         /* number of pixels in the image */
    //buffer=new float[npixels];
	InitBuffer();
	//cout<<nc;
	if( fits_read_img_coord(infptr,&xrefval,&yrefval,&xrefpix,&yrefpix,&xinc,&yinc,&rot,coordtype,&status))
		return status;
	//CloseFITSFile();
	return 1;
}

int CMyImage::CloseFITSFile()
{
	int status;
	if ( fits_close_file(infptr, &status) ){
         //AfxMessageBox("ERROR!!!");
			 return status;
			 }
		return 1;

}

CelPos CMyImage::PixWorld(double x, double y)
{
	int status=0;
	 fits_pix_to_world(x,y,
		 xrefval,yrefval,xrefpix,
		 yrefpix,xinc,yinc,rot,coordtype,&pos.lon,&pos.lat,&status);
	 pos.lon=fmod(pos.lon,360.);
	 if(pos.lon<0.)pos.lon+=360;
	return pos;
}

ImgPos CMyImage::WorldPix(double lon, double lat)
{
	int status=0;
	 fits_world_to_pix(lon,lat,
		 xrefval,yrefval,xrefpix,
		 yrefpix,xinc,yinc,rot,coordtype,&ipos.x,&ipos.y,&status);
	return ipos;
}

int CMyImage::SetWCSInfo(const double xrfp, const double yrfp, const double xrfv, const double yrfv, const double xi, const double yi, const char *xcoortype, const char *ycoortype)
{
	xrefpix=xrfp;
	yrefpix=yrfp;
	xrefval=xrfv;
	yrefval=yrfv;
	xinc=xi;
	yinc=yi;
	xcotype=xcoortype;
	ycotype=ycoortype;
	Wcs=1;
	return 0;
}

int CMyImage::WriteWCSKeys()
{
	double temp=0.0;
	int status=0;
	if(Wcs){

 if ( fits_update_key(outfptr, TSTRING, "CTYPE1",(void*)xcotype.c_str(),
         "Longitude - projection", &status) )
         return( status );
 if ( fits_update_key(outfptr, TSTRING, "CTYPE2",(void*)ycotype.c_str(),
         "Latitude - projection", &status) )
         return( status );

  if ( fits_update_key(outfptr, TDOUBLE, "CRPIX1", &xrefpix,
         "Reference pixel X", &status) )
         return( status );

  if ( fits_update_key(outfptr, TDOUBLE, "CRPIX2", &yrefpix,
         "Reference pixel Y", &status) )
         return( status );

  if ( fits_update_key(outfptr, TDOUBLE, "CRVAL1", &xrefval,
         "Longitude ref value (deg)", &status) )
         return( status );

  if ( fits_update_key(outfptr, TDOUBLE, "CRVAL2", &yrefval,
         "Latitude ref value", &status) )
         return( status );
  if ( fits_update_key(outfptr, TDOUBLE, "CDELT1", &xinc,
         "Displacement X (deg/pixel)", &status) )
         return( status );

  if ( fits_update_key(outfptr, TDOUBLE, "CDELT2", &yinc,
         "Displacement Y (deg/pixel)", &status) )
         return( status );
    if ( fits_update_key(outfptr, TDOUBLE, "CD1_1", &xinc,
         "Transformation matrix element", &status) )
         return( status );
  
  if ( fits_update_key(outfptr, TDOUBLE, "CD1_2", &temp,
         "Transformation matrix element", &status) )
         return( status );
  if ( fits_update_key(outfptr, TDOUBLE, "CD2_1", &temp,
         "Transformation matrix element", &status) )
         return( status );
  if ( fits_update_key(outfptr, TDOUBLE, "CD2_2", &yinc,
         "Transformation matrix element", &status) )
         return( status );

  if ( fits_update_key(outfptr, TSTRING, "FILENAME", outfptr->Fptr->filename,
         "Original file name", &status) )
         return( status ); 
	}
	return 1;
}


float * CMyImage::GetBuffer()
{
	return buffer;
}


void CMyImage::GetBuffer(unsigned short *ima)
{
	for(long int i=0; i<npixels;i++)
		ima[i]=(unsigned short)buffer[i];
}

int CMyImage::SetLowImageCut(float val)
{
	if(val>=datamin){
	for(long int i=0; i<npixels;i++)
		if(buffer[i]<=val)buffer[i]=val;
	}
	return 1;
}

int CMyImage::SetHighImaCut(float val)
{
	if(val<=datamax){
	for(long int i=0; i<npixels;i++)
		if(buffer[i]>=val)buffer[i]=val;
	}
	return 1;

}

int CMyImage::Convert2Float(double *img)
{

	for(long int i=0; i<npixels;i++)
		buffer[i]=(float)(1.*img[i]);
	return 1;
}

/////////////// FITS UTILITY ////////////////////

int CMyImage::CopyFITSFile(const char *infile,const char *outfile)
{
	 fitsfile *inptr, *outptr;   /* FITS file pointers defined in fitsio.h */
   	 int status = 0, ii = 1;       /* status must always be initialized = 0  */
	  /* Open the input file */
    	if ( !fits_open_file(&inptr, infile, READONLY, &status) )
    	{
     	 /* Create the output file */
     	 if ( !fits_create_file(&outptr, outfile, &status) )
      		{
        	/* Copy every HDU until we get an error */
        	while( !fits_movabs_hdu(inptr, ii++, NULL, &status) )
          	fits_copy_hdu(inptr, outptr, 0, &status);
 
        	/* Reset status after normal error */
        	if (status == END_OF_FILE) status = 0;

        	fits_close_file(outptr,  &status);
      	}
      	fits_close_file(inptr, &status);
    	}

    	/* if error occured, print out error message */
    	if (status) fits_report_error(stderr, status);
    	return(status);
}

int CMyImage::ReadFITSHDUKey( const char *incard, char *value)
{
   // fitsfile *inptr;         /* FITS file pointer, defined in fitsio.h */
    char card[FLEN_CARD];
    char comment[FLEN_COMMENT],oldvalue[FLEN_VALUE];
    int status = 0;   /*  CFITSIO status value MUST be initialized to zero!  */
    //int iomode;
    /*iomode = READONLY;
    if (!fits_open_file(&inptr, infile, iomode, &status))
    {*/
      if (fits_read_card(infptr,(char *)incard, card, &status))
      {
        printf("Keyword does not exist\n");
        card[0] = '\0';
        comment[0] = '\0';
        status = 0;  /* reset status after error */
		return 1;
      }else{
	  fits_parse_value(card, oldvalue, comment, &status);
	  //printf("%s \n", oldvalue);
	  strcpy(value,oldvalue);
      }
     /*fits_close_file(infptr, &status);
     //}
    if (status) {
	fits_report_error(stderr, status);
    }*/
     return 0;
}

int CMyImage::UpdateFITSHDUKey(const char *infile, const char *incard, const char *value)
{
    fitsfile *inptr;         /* FITS file pointer, defined in fitsio.h */
    char card[FLEN_CARD], newcard[FLEN_CARD];
    char oldvalue[FLEN_VALUE], comment[FLEN_COMMENT];
    int status = 0;   /*  CFITSIO status value MUST be initialized to zero!  */
    int iomode, keytype;
    iomode = READWRITE;
    if (!fits_open_file(&inptr, infile, iomode, &status))
    {
      if (fits_read_card(inptr,(char *)incard, card, &status))
      {
        printf("Keyword does not exist\n");
        card[0] = '\0';
        comment[0] = '\0';
        status = 0;  /* reset status after error */
	return 1;
      }else{
          /* check if this is a protected keyword that must not be changed */
          if (*card && fits_get_keyclass(card) == TYP_STRUC_KEY){
            printf("Protected keyword cannot be modified.\n");
          }else{
            /* get the comment string */
            if (*card)
		fits_parse_value(card, oldvalue, comment, &status);

            /* construct template for new keyword */
            strcpy(newcard, incard);     /* copy keyword name */
            strcat(newcard, " = ");       /* '=' value delimiter */
            strcat(newcard, value);     /* new value */
            if (*comment) {
              strcat(newcard, " / ");  /* comment delimiter */
              strcat(newcard, comment);     /* append the comment */
            }

            /* reformat the keyword string to conform to FITS rules */
            fits_parse_template(newcard, card, &keytype, &status);

            /* overwrite the keyword with the new value */
            fits_update_card(inptr, (char *)incard, card, &status);

            printf("Keyword has been changed to:\n");
            printf("%s\n",card);
	}
       }
     fits_close_file(inptr, &status);
     }
      if (status) 
	fits_report_error(stderr, status);
     return 0;
}

/** No descriptions */
CMyImage::CMyImage ( CMyImage& im){
	nc=im.GetWidth();
	nr=im.GetHeight();
	//InitBuffer();
	buffer=im.GetBuffer();
	npixels=nc*nr;
	naxis=im.GetNAxis();
	nnaxes=0;
	datamin  = im.GetMin();
	datamax  = im.GetMax();
	strcpy(fname,im.GetFname());
	Wcs=0;
}

int CMyImage::GetKeyValue(const char *key, char *val)
{
	if(ReadFITSHDUKey(key,val)){
		return 1;
	}
	return 0;
}

int CMyImage::GetKeyValue(const char *key, int& val)
{
	char valu[30];
	if(ReadFITSHDUKey(key,valu)){
		val=0;
		return 1;
	}
	val=atoi(valu);
	return 0;
}

int CMyImage::GetKeyValue(const char *key, double& val)
{
	char valu[30];
	if(ReadFITSHDUKey(key,valu)){
		val=0.;
		return 1;
	}
	val=atof(valu);
	return 0;
}



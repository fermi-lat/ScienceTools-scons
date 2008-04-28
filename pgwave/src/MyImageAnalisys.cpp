// MyImageAnalisys.cpp: implementation of the MyImageAnalisys class
// G.Tosti - 2003
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef WIN32
#pragma warning(disable:4244)
#endif

#include <fitsio.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <iomanip>
#include "pgwave/MyMatrix.h"
#include "pgwave/MyImageAnalisys.h"

int sign1(float);
float amin1(float,float);
float amax1(float,float);
int amin1(int x,int y);
int amax1(int x,int y);

template <class T>
void SortStarList(T &s,int nstar)
{
  int i=0, j=0;
  for(i=0;i<nstar;i++){
    for(j=0;j<nstar-i;j++)
      if(s[j].x>s[j+1].x)
         swap(s[j] ,s[j+1]);
  }
}

double gaussian_average_2d( float* image, int xmax, int ymax, int x, int y, double sigma )
{
	double val = 0.0;
	double sum = 0.0;
	int imin = amax1(0, x-sigma);
	int imax = amin1(xmax-1,x+sigma);
	int jmin = amax1(0, y-sigma);
	int jmax = amin1(ymax-1,y+sigma);
	for( int i = imin; i <=imax; i++ ) {
		for(int j = jmin; j <= jmax; j++ ) {
			double r2 = (i-x)*(i-x)+(j-y)*(j-y);
			double g = exp(-0.5*r2/(sigma*sigma));
			sum += g;
			val += image[i+j*xmax]*g;
		}
	}
	return val/sum;
}

int gaussian_filter(float *in, float *out, int xmax,int ymax,double sigma )
{
		int i=0,j=0;
		for( i = 0; i < ymax; i++ ) {
			for( j = 0; j < xmax; j++ ) {
			out[i*xmax+j] = gaussian_average_2d( in,xmax,ymax,j, i, sigma );
			}
		}
		
	return 1;
}

MyImageAnalisys::MyImageAnalisys()
{
	m_opt00 = 1.65F;	//	READ NOISE (ADU) 		
	m_opt01 = 1.F;		//	GAIN (e-/ADU) 				
	m_opt02 =  1.5F;		//	LOW GOOD DATUM (in sigmas)   
	m_opt03 =  60000.F;	//	HIGH GOOD DATUM (in ADU)		
	m_minsky = 10.;		//	Limite inferiore per sky level (era a 1000)
	m_maxsky = 10000;   //	Numero massimo di pixel per la valutazione del cielo
						//	(e non limite superiore per sky level!!!)
	m_fwhmY=m_fwhmX=m_opt04 = 5.F;		//FWHM OF OBJECT
	m_opt05 = 10.F;	//	THRESHOLD (in sigmas)
	m_opt06 = .1F;		//LOW SHARPNESS CUTOFF			 
	m_opt07 = 1.F;		//HIGH SHARPNESS CUTOFF         
	m_opt08 = -1.2F;		//LOW ROUNDNESS CUTOFF			 
	m_opt09 = 1.0F;		//HIGH ROUNDNESS CUTOFF         
	m_MAXSTAR = MAXSTAR;				
	m_maxboxX=m_maxboxY=m_maxbox = 10;

	global_minsky = 10;
	global_maxsky = 10000;
	global_maxbox = 10;
	global_opt[0] = 1.65F;	//	READ NOISE (ADU)
	global_opt[1] = 1.F;	//	GAIN (e-/ADU) 
	global_opt[2] = 1.F;	//	LOW GOOD DATUM (in sigmas)
	global_opt[3] = 55000.F;//	HIGH GOOD DATUM (in ADU)
	global_opt[4] = 5.F;	//	FWHM OF OBJECT
	global_opt[5] = 3.F;	//	THRESHOLD (in sigmas)
	global_opt[6] = .0F;	//	LOW SHARPNESS CUTOFF
	global_opt[7] = 1.F;	//	HIGH SHARPNESS CUTOFF 
	global_opt[8] = -1.F;	//	LOW ROUNDNESS CUTOFF
	global_opt[9] = 1.F;	//	HIGH ROUNDNESS CUTOFF 

	m_par01 = global_par[1] = 2.5F;	//radius of aperture 1 (attenzione agli indici)
	m_par02 = global_par[2] = 3.0F;	//radius of aperture 2
	m_par03 = global_par[3] = 4.0F;	//radius of aperture 3
	m_par04 = global_par[4] = 5.0F;	//radius of aperture 4
	m_par05 = global_par[5] = 6.0F;	//radius of aperture 5
	m_par06 = global_par[6] = 7.0F;	//radius of aperture 6
	m_par07 = global_par[7] = 8.0F;	//radius of aperture 7
	m_par08 = global_par[8] = 9.0F;	//radius of aperture 8
	m_par09 = global_par[9] = 10.0F;	//radius of aperture 9
	m_par10 = global_par[10] = 11.0F;//	radius of aperture 10
	m_par11 = global_par[11] = 0.0F;//	radius of aperture 11
	m_par12 = global_par[12] = 0.0F;//	radius of aperture 12
	m_par13 = global_par[13] = 13.0F;//	inner sky radius (era 10.0F)
	m_par14 = global_par[14] = 18.0F;//	outer sky radius (era 16.0F)

		//	modificati da dialog FITMARQ:
	global_maxiter = 100;	//	limite n iterazioni per una stella
	global_rigbox = 11;		//	righe box utilizzato per il fit
	global_colbox = 11;		//	colonne box utilizzato per il fit
	global_numbrill = 5;	//	numero stelle brillanti fit preliminare
	global_iniguess1 = 2.0;	//	guess iniziale per 1 parametro forma
	global_iniguess2 = 2.0;	//	guess iniziale per 2 parametro forma
	global_flagpsf = '5';	//	flag per indicare modello psf da usare
	global_psfmodel = 1;	//	corrispondente flag

	//	modificati da dialog SYSPAR:
	global_pixwidth = 13;	//	dimensione x di un pixel del CCD in um
	global_pixheight = 13;	//	dimensione x di un pixel del CCD in um
	global_focallenght = 4800.;	//	focale del telescopio in mm
	m_ncol=1024;
	m_nrow=1024;
	m_imabuffer=NULL;
	m_backbuffer=NULL;
	m_backsigbuffer=NULL;
	m_background=0;
	m_smod.skymn=0.0;
	m_smod.skymed=0.0;
	m_smod.skymod=0.0;
	m_smod.skysig=0.0;
	m_smod.skyskw=0.0;
	m_smod.skynpx=10000;
	m_smod.skymax=0.0;
	m_smod.skymin=0.0;
	m_expt=0.0;
	m_magzero=0.0;
	m_pnstar=0;
	m_minumpx=12;
	stella =new MAG_APER[MAXSTAR];
	m_detectfile="pippo";
}

MyImageAnalisys::~MyImageAnalisys()
{
	if(m_imabuffer!=NULL)
		delete []m_imabuffer;
	if(m_backbuffer!=NULL)
		delete []m_backbuffer;
	if(m_backsigbuffer!=NULL)
		delete []m_backsigbuffer;
}

int MyImageAnalisys::FindBackground(long int max)
{
	//	max; valor massimo del NUMERO dei pixels dove valutare il cielo.
	//	FSKY restituisce '0' (cioè FALSE) se qualcosa non funziona.

	long int	istep,lx,irow,i,ifirst,j;
	long int	n;
	float		*s;
	size_t		larg,numb;

	
		//	locali; vedi successiva inizializzazione con variabili
		//	membro del documento corrente.
	larg=sizeof(float);
	s = new float [max+1];
	istep = m_ncol*m_nrow/max + 1;
	lx = 0;
	ifirst = 0;
	n = 0;
	irow=0;
	do{
		irow = irow+1;
		ifirst = ifirst+1;
		if(ifirst>istep){
			ifirst=ifirst-istep;
		}
		i = ifirst-1;
		j = irow-1;
		do{	
			if(m_imabuffer[j*m_ncol+i] > m_opt03){
			//if((*(m_imabuffer+j*m_ncol+i)) > (m_opt03)
			//if((*(m_imabuffer+j*m_ncol+i)) > opt[3])
				i = i+1;
			}else{
				
				s[n] = m_imabuffer[j*m_ncol+i];
				//cout<<s[n]<<endl;
				n = n+1;
				
				if (n==max){
					i=m_ncol+1;
					irow=m_nrow;
				}else{
					i=i+istep;
				}
			}
		}while (i < m_ncol);
	}while (irow < m_nrow);
	numb=(size_t)n-1;
	//cout<<numb<<endl;
	//cout<<s[0]<<" "<<s[numb]<<endl;
	//qsort(s,numb,larg,rutina);
	sort(&s[0],&s[numb+1],less<float>());
	//cout<<s[0]<<" "<<s[numb]<<endl;
	if(int(BackgroundStat(s,(numb+1)))==1)
	{
		cout<<"Find Background error!!"<<endl;
		return (0);
		//	se 'mmm' restituisce '1' vuol dire che qualcosa non ha funzionato.
	}

	//printf("\n skymod = %6.3f    \n skysig = %6.3f    ",m_smod.skymod,m_smod.skysig);
	m_background=1;
	delete []s;
	return (1);
}




//	copiato qui da 'utilita.cpp'
//
int MyImageAnalisys::BackgroundStat(float *sky, long int nsky)
/* float *skymn, float *skymed, 
					 float *skymod, float *sigma, float *skew)
*/
{
	long int	minimm,maximm=0,niter,istep,jstep,redo,
				i,maxit=30,a,b;
	//float *skymn, *skymed, *skymod, *sigma, *skew
	float		cut,cut1,cut2,delta,skymid,r,x,center,side;
	double		sum,sumsq;


	//	se 'mmm' restituisce '1' vuol dire che qualcosa non ha funzionato.
	//cout<<nsky<<endl;
	//cout<<sky[0]<<" "<<sky[nsky]<<endl;
	m_smod.skynpx=nsky;
	m_smod.skymax=sky[nsky-1];
	m_smod.skymin=sky[0];
	if(nsky<=0)
		return(1);				//	goto nnzz;
	skymid=0.5F*(*(sky+(nsky+1)/2)+*(sky+(nsky/2)+1));
	sum=0.0;
	sumsq=0.0;
	cut1=amin1(*(sky+nsky-1)-skymid,(m_opt03-skymid));
	//cut1=amin1(*(sky+nsky-1)-skymid,opt[3]-skymid);
	cut1=amin1(skymid-*(sky+1),cut1);
	cut2=skymid+cut1;
	cut1=skymid-cut1;
	minimm=0;
	for(i=1;i<=nsky;i++)
	{
		 if (*(sky+i)<cut1)
		 {
				minimm=i;
				goto uzuz;		//	continue;
		 }
		 if (*(sky+i)>cut2)
				goto uzdz;		//	break;
		 delta=*(sky+i)-skymid;
		 sum=sum+(double)delta;
		 sumsq=sumsq+(double)(delta*delta);
		 maximm=i;
uzuz:	;						//
	}

uzdz:						
	m_smod.skymed=0.5F*(*(sky+(minimm+maximm+1)/2)+*(sky+((minimm+maximm)/2)+1));
	m_smod.skymn =(float)(sum/(double)(maximm-minimm));
	m_smod.skysig=(float)(sqrt(sumsq/(double)(maximm-minimm)-(double)((m_smod.skymn )*(m_smod.skymn ))));
	m_smod.skymn =m_smod.skymn +skymid;
	m_smod.skymod=m_smod.skymn ;
	if (m_smod.skymed<m_smod.skymn )
		m_smod.skymod=3.F*(m_smod.skymed)-2.F*(m_smod.skymn );

	niter=0;
dzzz:   ;
	niter=niter+1;
	if((niter>maxit)||((maximm-minimm)<(long int)m_minsky))	//	if((niter>maxit))//||((maximm-minimm)<minsky))
	//if((niter>maxit)||((maximm-minimm)<minsky))	//	if((niter>maxit))//||((maximm-minimm)<minsky))
		goto nnzz;
	r=(float)log10((double)(maximm-minimm));
	r=amax1(2.F,(-0.1042F*r+1.1695F)*r+0.8895F);
	cut=r*(m_smod.skysig)+0.5F*(float)fabs((double)(m_smod.skymn -m_smod.skymod));
	cut=amax1(1.5F,cut);
	cut1=m_smod.skymod-cut;
	cut2=m_smod.skymod+cut;
	redo=0;
	istep=sign1(cut1-*(sky+minimm+1));
	jstep=(istep+1)/2;
	if(istep>0)
		goto dudz;
duzz:   ;
	if((istep<0)&&(minimm<=0))
		goto ducz;
	if((*(sky+minimm)<=cut1)&&(*(sky+minimm+1)>=cut1))
		goto ducz;
dudz:   ;
	delta=*(sky+minimm+jstep)-skymid;
	sum=sum-(double)istep*(double)delta;
	sumsq=sumsq-(double)istep*(double)(delta*delta);
	minimm=minimm+istep;
	redo=1;
	goto duzz;
ducz:	;
	istep=sign1(cut2-*(sky+maximm));
	jstep=(istep+1)/2;
	if(istep<0)
		goto dddz;
ddzz:	;
	if((istep>0)&&(maximm>=nsky))
		goto ddcz;
	if((*(sky+maximm)<=cut2)&&(*(sky+maximm+1)>=cut2))
		goto ddcz;
dddz:   ;
	delta=*(sky+maximm+jstep)-skymid;
	sum=sum+(double)istep*(double)delta;
	sumsq=sumsq+(double)istep*(double)(delta*delta);
	maximm=maximm+istep;
	redo=1;
	goto ddzz;
ddcz:   ;
	m_smod.skymn =(float)((sum)/((double)(maximm-minimm)));
	m_smod.skysig=(float)(sqrt(sumsq/(double)(maximm-minimm)-(double)((m_smod.skymn )*(m_smod.skymn ))));
	m_smod.skymn =m_smod.skymn +skymid;
	m_smod.skymed=0.0F;
	x=0.0F;
	center=(float)(minimm+1+maximm)/2.F;
	side=(float)((int)(0.05*(float)(maximm-minimm)))/2.F+0.25F;
	a=(int)(center-side);
	b=(int)(center+side);
	for(i=a;i<=b;i++)
	{
		m_smod.skymed=*(sky+i)+m_smod.skymed;
		x=x+1;
	}
	m_smod.skymed=(m_smod.skymed)/x;
	m_smod.skymod=m_smod.skymn ;
	if (m_smod.skymed<m_smod.skymn )
		m_smod.skymod=3.F*(m_smod.skymed)-2.F*(m_smod.skymn );
	if(redo==1)
		goto dzzz;
	m_smod.skyskw=(m_smod.skymn -m_smod.skymod)/amax1(1.F,m_smod.skysig);
	m_smod.skynpx=nsky=maximm-minimm;
	return(0);

nnzz:   ;
	m_smod.skysig=-1.0F;
	m_smod.skyskw=0.0F;
	return(1);	// return(0) era sbagliato
}

int MyImageAnalisys::BackgroundStat(vector<float>& sky1,SKY &smod)
/* float *skymn, float *skymed, 
					 float *skymod, float *sigma, float *skew)
*/
{
	long int	minimm,maximm=0,niter,istep,jstep,redo,
				i,maxit=30,a,b;
	//float *skymn, *skymed, *skymod, *sigma, *skew
	float		cut,cut1,cut2,delta,skymid,r,x,center,side;
	double		sum,sumsq;


	//	se 'mmm' restituisce '1' vuol dire che qualcosa non ha funzionato.
	//cout<<nsky<<endl;
	//cout<<sky[0]<<" "<<sky[nsky]<<endl;
	smod.skysig=-1.0F;
	smod.skyskw=0.0F;
	smod.skymed=0.;
	smod.skymod=0.;
	smod.skymn=0. ;
	float *sky=new float[sky1.size()+1];
	for(int ii=1;ii<(int)sky1.size()+1;ii++)
		sky[ii]=sky1[ii-1];
	long int nsky=long(sky1.size());

	smod.skynpx=nsky;
	smod.skymax=sky[nsky];
	smod.skymin=sky[1];
	//cout<<smod.skymin<<" "<<smod.skymax<<endl;
	if(nsky<=0)
		return(1);				//	goto nnzz;
	skymid=0.5F*(*(sky+(nsky+1)/2)+*(sky+(nsky/2)+1));
	sum=0.0;
	sumsq=0.0;
	cut1=amin1(*(sky+nsky-1)-skymid,(m_opt03-skymid));
	//cut1=amin1(*(sky+nsky-1)-skymid,opt[3]-skymid);
	cut1=amin1(skymid-*(sky+1),cut1);
	cut2=skymid+cut1;
	cut1=skymid-cut1;
	minimm=0;
	for(i=1;i<=nsky;i++)
	{
		 if (*(sky+i)<cut1)
		 {
				minimm=i;
				goto uzuz;		//	continue;
		 }
		 if (*(sky+i)>cut2)
				goto uzdz;		//	break;
		 delta=*(sky+i)-skymid;
		 sum=sum+(double)delta;
		 sumsq=sumsq+(double)(delta*delta);
		 maximm=i;
uzuz:	;						//
	}

uzdz:						
	smod.skymed=0.5F*(*(sky+(minimm+maximm+1)/2)+*(sky+((minimm+maximm)/2)+1));
	smod.skymn =(float)(sum/(double)(maximm-minimm));
	smod.skysig=(float)(sqrt(sumsq/(double)(maximm-minimm)-(double)((smod.skymn )*(smod.skymn ))));
	smod.skymn =smod.skymn +skymid;
	smod.skymod=smod.skymn ;
	//cout<<" Media="<<smod.skymn<<endl;
	//cout<<" MediAN"<<smod.skymed<<endl;
	if (smod.skymed<smod.skymn )
		smod.skymod=3.F*(smod.skymed)-2.F*(smod.skymn );
	//cout<<" Mede"<<smod.skymod<<endl;
	niter=0;
dzzz:   ;
	niter=niter+1;
	if((niter>maxit)||((maximm-minimm)<(long int)m_minsky))	//	if((niter>maxit))//||((maximm-minimm)<minsky))
	//if((niter>maxit)||((maximm-minimm)<minsky))	//	if((niter>maxit))//||((maximm-minimm)<minsky))
		goto nnzz;
	r=(float)log10((double)(maximm-minimm));
	r=amax1(2.F,(-0.1042F*r+1.1695F)*r+0.8895F);
	cut=r*(smod.skysig)+0.5F*(float)fabs((double)(smod.skymn -smod.skymod));
	cut=amax1(1.5F,cut);
	cut1=smod.skymod-cut;
	cut2=smod.skymod+cut;
	redo=0;
	istep=sign1(cut1-*(sky+minimm+1));
	jstep=(istep+1)/2;
	if(istep>0)
		goto dudz;
duzz:   ;
	if((istep<0)&&(minimm<=0))
		goto ducz;
	if((*(sky+minimm)<=cut1)&&(*(sky+minimm+1)>=cut1))
		goto ducz;
dudz:   ;
	delta=*(sky+minimm+jstep)-skymid;
	sum=sum-(double)istep*(double)delta;
	sumsq=sumsq-(double)istep*(double)(delta*delta);
	minimm=minimm+istep;
	redo=1;
	goto duzz;
ducz:	;
	istep=sign1(cut2-*(sky+maximm));
	jstep=(istep+1)/2;
	if(istep<0)
		goto dddz;
ddzz:	;
	if((istep>0)&&(maximm>=nsky))
		goto ddcz;
	if((*(sky+maximm)<=cut2)&&(*(sky+maximm+1)>=cut2))
		goto ddcz;
dddz:   ;
	delta=*(sky+maximm+jstep)-skymid;
	sum=sum+(double)istep*(double)delta;
	sumsq=sumsq+(double)istep*(double)(delta*delta);
	maximm=maximm+istep;
	redo=1;
	goto ddzz;
ddcz:   ;
	smod.skymn =(float)((sum)/((double)(maximm-minimm)));
	smod.skysig=(float)(sqrt(sumsq/(double)(maximm-minimm)-(double)((smod.skymn )*(smod.skymn ))));
	smod.skymn =smod.skymn +skymid;
	smod.skymed=0.0F;
	x=0.0F;
	center=(float)(minimm+1+maximm)/2.F;
	side=(float)((int)(0.05*(float)(maximm-minimm)))/2.F+0.25F;
	a=(int)(center-side);
	b=(int)(center+side);
	for(i=a;i<=b;i++)
	{
		smod.skymed=*(sky+i)+smod.skymed;
		x=x+1;
	}
	smod.skymed=(smod.skymed)/x;
	smod.skymod=smod.skymn ;
	if (smod.skymed<smod.skymn )
		smod.skymod=3.F*(smod.skymed)-2.F*(smod.skymn );
	if(redo==1)
		goto dzzz;
	smod.skyskw=(smod.skymn -smod.skymod)/amax1(1.F,smod.skysig);
	smod.skynpx=nsky=maximm-minimm;
	delete []sky;
	return(0);

nnzz:   ;
	smod.skysig=-1.0F;
	smod.skyskw=0.0F;
	smod.skymed=0.;
	smod.skymod=0.;
	smod.skymn=0. ;
	delete []sky;
	return(1);	// return(0) era sbagliato
}



int MyImageAnalisys::FindStar()
//int FindStar(float *hhh, WORD maxbox,WORD maxsky,LPWORD nstar, CENTRO *centro)
{
	float	pixels,radius,sigsq,rsq,relerr,skylvl,temp,
			hmin,p,datum,height,denom,sgop,readns,phpadu,
			round,sharp,lobad,
			sumg,sumgsq,sumgd,sumd,sg,sgsq,sgd,sd,wt,hx,hy,
			dgdx,sdgdx,sdgdxs,sddgdx,sgdgdx,
			xcen,ycen,dx,dy;
	float	*g,*skip;
	int		nhalf,nbox,middle,lastcl,lastro,jsq,
			i,j,n,ix,iy,jx,jy;
	//ofstream out("sources.reg");
//	SKY m_smod;

	//	variabili locali; usate spesso; più comodo così

	g = new float[(m_maxbox*m_maxbox)+1];
		if(g == NULL)
			return (0);

	skip = new float[(m_maxbox*m_maxbox)+1];
		if(skip == NULL)
			return (0);

	radius = amax1(2.001F,0.637F*(m_opt04));
	nhalf = int(amin1((m_maxbox-1)/2.,radius));
	nbox = 2*nhalf + 1;
	middle = nhalf;
	lastro = m_nrow - nhalf;
	lastcl = m_ncol - nhalf;
	sigsq=(m_opt04/2.35482F)*(m_opt04/2.35482F);
	radius = radius*radius;

	sumg = 0.0F;
	sumgsq = 0.0F;
	pixels = 0.0F;
	for (j=0;j<nbox;j++)
	{
		 jsq = (j-middle)*(j-middle);
		 for (i=0;i<nbox;i++)
		 {
				rsq = (float)((i-middle)*(i-middle)+jsq);
				*(g+j*m_maxbox+i)=(float)(exp((double)(-0.5F*rsq/sigsq)));
				if (rsq <= radius)
				{
						*(skip+j*m_maxbox+i) = 0.F;
						sumg = sumg + *(g+j*m_maxbox+i);
						sumgsq=sumgsq + (*(g+j*m_maxbox+i))*(*(g+j*m_maxbox+i));
						pixels = pixels + 1.0F;
				}
				else
				{
						*(skip+j*m_maxbox+i) = 1.F;
				}
		 }
	}
	if(pixels==0.F)
	{
		 //printf("\n ***** pixels = 0");
		 pixels=1.F;
	}
	denom=sumgsq-(sumg*sumg)/pixels;
	sgop=sumg/pixels;
	relerr=1.0F/denom;
	relerr=(float)(sqrt((double)relerr));

	*(skip+middle*m_maxbox+middle)=1.F;
	pixels = pixels-1.0F;

	//	Nella matrice g (m_maxbox x m_maxbox) abbiamo messo un profilo 
	//	gaussiano con FWHM data. I valori significativi nei calcoli 
	//	seguenti sono contraddistinti da un valore nullo nella 
	//	matrice skip e il numero di questi pixel significativi è 
	//	indicato nella variabile "pixel".

	if(!FindBackground(m_maxsky))
	{
		//riporta(" MAXFIND: sky non trovato");
		return (0);
	}
	else
	{
		//gotoxy(1,4);
		//printf("\nerrore relativo=%4.2f",relerr);
		
		readns = m_opt00*m_opt00;
		phpadu = m_opt01;
		hmin = (float)sqrt((double)(readns+amax1(0.0F,m_smod.skymod)/phpadu));
		m_lobad=lobad = m_smod.skymod-((m_opt02)*hmin);
		hmin = relerr*((m_opt05)*hmin);
		readns = (float)sqrt((double)readns);
		//cout<<"hmin="<<hmin<<" lobad="<<lobad<<endl;
		m_pnstar=0;	//era *nstar=0;

		for(jy=middle;jy<lastro;jy++)
		{
			for(jx=middle;jx<lastcl;jx++)
			{
				datum= GetPix(jx, jy);//*(hhh+jy*m_ncol+jx);
				if(datum>(m_smod.skymod+hmin))
				{
					for(iy=jy-nhalf;iy<=jy+nhalf;iy++)
					{
						for(ix=jx-nhalf;ix<=jx+nhalf;ix++)
						{
							i=middle+(ix-jx);
							j=middle+(iy-jy);
							if(*(skip+j*m_maxbox+i) == 0.F)
							{
								if(datum<GetPix(ix, iy)) //*(hhh+iy*m_ncol+ix)
								{
									goto ttzz;
								}
							}
						}
					}      	
					// fine controllo massimo locale

					*(skip+middle*m_maxbox+middle)=0.F;
					pixels = pixels+1.0F;

		// 	occorre considerare anche il pixel centrale per la convoluzione

					sgd=0.F;
					sd=0.F;
					sgsq=sumgsq;
					sg=sumg;
					p=pixels;
					for(iy=jy-nhalf;iy<=jy+nhalf;iy++)
					{
						for(ix=jx-nhalf;ix<=jx+nhalf;ix++)
						{
							i=middle+(ix-jx);
							j=middle+(iy-jy);
							if(*(skip+j*m_maxbox+i) == 0)
							{
								datum=GetPix(ix, iy);//*(hhh+iy*m_ncol+ix);
								if((datum>=lobad)&&(datum<=(m_opt03)))
								{
									sgd=sgd+datum*(*(g+j*m_maxbox+i));
									sd=sd+datum;
								}
								else
								{
									sgsq=sgsq-(*(g+j*m_maxbox+i))*(*(g+j*m_maxbox+i));
									sg=sg-(*(g+j*m_maxbox+i));
									p=p-1.F;
								}
							}
						}
					}
					if (p>1.5)
					{
						if(p<pixels)
						{
							sgsq=sgsq-(sg*sg)/p;
							if(sgsq!=0.)
								sgd=(sgd-sg*sd/p)/sgsq;
							else
								sgd=0.F;
						}
						else
							sgd=(sgd-sgop*sd)/denom;
					}
					else
						sgd=0.0F;

					height=sgd;
					*(skip+middle*m_maxbox+middle)=1.F;
					pixels = pixels-1.0F;

		//	Si risetta il pixel centrale del profilo gaussiano come prima

					if(height<hmin)
					{
						goto ttzz;
					}		
					// fine controllo convoluzione

					sharp=0.F;
					datum=GetPix(jx, jy);//*(hhh+jy*m_ncol+jx);
					if((datum>lobad)&&(datum<(m_opt03)))
					{
						p=0.F;
						for(iy=jy-nhalf;iy<=jy+nhalf;iy++)
						{
							for(ix=jx-nhalf;ix<=jx+nhalf;ix++)
							{
								i=middle+(ix-jx);
								j=middle+(iy-jy);
								temp=0.0F;
								if(*(skip+j*m_maxbox+i)==0.F)
								{
									datum=GetPix(ix, iy);//*(hhh+iy*m_ncol+ix);
									if((datum>=lobad)&&(datum<=(m_opt03)))
									{
										temp=temp+(datum-m_smod.skymod);
										p=p+1.F;
									}
								}
								sharp=sharp+temp;
							}
						}
						sharp=(GetPix(jx, jy)-m_smod.skymod-sharp/p)/height;//(*(hhh+jy*m_ncol+jx))
						if((sharp<(m_opt06))||(sharp>(m_opt07)))
						{
							goto ttzz;
						}
					}			
					// fine controllo profilo

					//	inizio calcolo centroide

					sumgd=0.0F;
					sumgsq=0.0F;
					sumg=0.0F;
					sumd=0.0F;
					sdgdx=0.0F;
					sdgdxs=0.0F;
					sddgdx=0.0F;
					sgdgdx=0.0F;
					p=0.0F;
					n=0;
					for(ix=jx-nhalf;ix<=jx+nhalf;ix++)
					{
						i=middle+(ix-jx);
						sg=0.0F;
						sd=0.0F;
						for(iy=jy-nhalf;iy<=jy+nhalf;iy++)
						{
							j=middle+(iy-jy);
							wt=(float)(middle-abs(j-middle));
							datum=GetPix(ix,iy);//(*(hhh+jy*m_ncol+jx));//*(hhh+iy*m_ncol+ix);
							if((datum>=lobad)&&(datum<=(m_opt03)))
							{
								sd=sd+(datum-m_smod.skymod)*wt;
								sg=sg+wt*(*(g+j*m_maxbox+i));
							}
						}
						if(sg>0.0)
						{
							wt=(float)(middle-abs(i-middle));
							sumgd=sumgd+wt*sg*sd;
							sumgsq=sumgsq+wt*sg*sg;
							sumg=sumg+wt*sg;
							sumd=sumd+wt*sd;
							p=p+wt;
							n=n+1;
							dgdx=sg*(middle-i);
							sdgdxs=sdgdxs+wt*dgdx*dgdx;
							sdgdx=sdgdx+wt*dgdx;
							sddgdx=sddgdx+wt*sd*dgdx;
							sgdgdx=sgdgdx+wt*sg*dgdx;
						}
					}
					if(n<=2)
					{
						goto ttzz;
					}

					hx=(sumgd-sumg*sumd/p)/(sumgsq-(sumg*sumg)/p);
					if(hx<=0.0)
					{
						goto ttzz;
					}
					skylvl=(sumd-hx*sumg)/p;
					dx=(sgdgdx-(sddgdx-sdgdx*(hx*sumg+skylvl*p)))/(hx*sdgdxs/sigsq);
					xcen=jx+dx/(1.F+(float)fabs((double)dx));

					sumgd=0.0F;
					sumgsq=0.0F;
					sumg=0.0F;
					sumd=0.0F;
					sdgdx=0.0F;
					sdgdxs=0.0F;
					sddgdx=0.0F;
					sgdgdx=0.0F;
					p=0.0F;
					n=0;
					for(iy=jy-nhalf;iy<=jy+nhalf;iy++)
					{
						j=middle+(iy-jy);
						sg=0.0F;
						sd=0.0F;
						for(ix=jx-nhalf;ix<=jx+nhalf;ix++)
						{
							i=middle+(ix-jx);
							wt=(float)(middle-abs(i-middle));
							datum=GetPix(ix,iy);//*(hhh+iy*m_ncol+ix);
							if((datum>=lobad)&&(datum<=(m_opt03)))
							{
								sd=sd+(datum-m_smod.skymod)*wt;
								sg=sg+wt*(*(g+j*m_maxbox+i));
							}
						}
						if(sg>0.0)
						{
							wt=(float)(middle-abs(j-middle));
							sumgd=sumgd+wt*sg*sd;
							sumgsq=sumgsq+wt*sg*sg;
							sumg=sumg+wt*sg;
							sumd=sumd+wt*sd;
							p=p+wt;
							dgdx=sg*(middle-j);
							sdgdxs=sdgdxs+wt*dgdx*dgdx;
							sdgdx=sdgdx+wt*dgdx;
							sddgdx=sddgdx+wt*sd*dgdx;
							sgdgdx=sgdgdx+wt*sg*dgdx;
							n=n+1;
						}
					}
					if(n<=2)
					{
						goto ttzz;
					}
					hy=(sumgd-sumg*sumd/p)/(sumgsq-(sumg*sumg)/p);
					if(hy<=0.0)
					{
						goto ttzz;
					}
					skylvl=(sumd-hy*sumg)/p;
					dy=(sgdgdx-(sddgdx-sdgdx*(hy*sumg+skylvl*p)))/(hy*sdgdxs/sigsq);
					ycen=jy+dy/(1.F+(float)fabs((double)dy));

					//	fine calcolo centroide

					round=2.F*(hx-hy)/(hx+hy);
					//cout<<"hx="<<hx<<" hy="<<hy<<" round="<<round<<endl;
					if((round<(m_opt08))||(round>(m_opt09)))
					{
						goto ttzz;
					}              
					// fine controllo round
					centro[m_pnstar].id=m_pnstar;
					centro[m_pnstar].peak=GetPix(int(xcen+0.5),int(ycen+0.5));
					height=-2.5F*(float)log10((double)(height/hmin));
					centro[m_pnstar].x=xcen;	//era centro[*nstar].x
					centro[m_pnstar].y=ycen;	//era centro[*nstar].y
					centro[m_pnstar].bkg=m_smod.skymod;
					centro[m_pnstar].bkgsig=m_smod.skysig;
					
					if(PeakRange(centro[m_pnstar])){
					 //cout<<"Xmin="<<centro[m_pnstar].xmin<<"Xmax="<<centro[m_pnstar].xmax<<endl;
					 //cout<<"Ymin="<<centro[m_pnstar].ymin<<"Ymax="<<centro[m_pnstar].ymax<<endl;
					 //cout<<centro[m_pnstar].id<<" "<<centro[m_pnstar].x<<" "<<centro[m_pnstar].y<<" "<<centro[m_pnstar].fwhmX <<" "<<centro[m_pnstar].fwhmY<<endl;
						if (LocalBackground(centro[m_pnstar],3,5)){
							//out<<centro[m_pnstar].x+1<<" "<<centro[m_pnstar].y+1<<endl;
					//cout<<centro[m_pnstar].x+1<<" "<<centro[m_pnstar].y+1<<endl;
					//PeakRange(centro[m_pnstar]);
							if (StarCenter(centro[m_pnstar])){							
								Fwhm(centro[m_pnstar]);
								m_pnstar=m_pnstar+1;
							}
						}
					}
							//era *nstar=*nstar+1;
					if((m_pnstar)==m_MAXSTAR)	//era if((*nstar)==MAXSTAR)
					{
						delete []g;
						delete []skip;
					//	out.close();
						//SortStarList(centro,m_pnstar);
						return (1);
					}

ttzz:   	;
				}
			}
		}
	}
	delete []g;
	delete []skip;
	//out.close();
	//SortStarList(centro,m_pnstar);
	return (1);
}


int MyImageAnalisys::FindSpectrum()
//int FindStar(float *hhh, WORD maxbox,WORD maxsky,LPWORD nstar, CENTRO *centro)
{
	float	pixels,radius,sigsq,rsq,relerr,skylvl,temp,
			hmin,p,datum,height,denom,sgop,readns,phpadu,
			round,sharp,lobad,
			sumg,sumgsq,sumgd,sumd,sg,sgsq,sgd,sd,wt,hx,hy,
			dgdx,sdgdx,sdgdxs,sddgdx,sgdgdx,
			xcen,ycen,dx,dy;
	float	*g,*skip;
	int		nhalf,nbox,middle,lastcl,lastro,jsq,
			i,j,n,ix,iy,jx,jy;
	ofstream out("pippo.reg");
//	SKY m_smod;

	//	variabili locali; usate spesso; più comodo così

	g = new float[(m_maxboxX*m_maxboxY)+1];
		if(g == NULL)
			return (0);

	skip = new float[(m_maxboxX*m_maxboxY)+1];
		if(skip == NULL)
			return (0);

	radius = amax1(2.001F,0.637F*(m_fwhmX));
	nhalf = int(amin1((m_maxboxX-1)/2.,radius));
	double radiusY = amax1(2.001F,0.637F*(m_fwhmY));
	double nhalfY = (double)amin1((float)(m_maxboxY-1)/2.,(float)radiusY);
	nbox = 2*nhalf +1;
	int nboxY=2*nhalfY +1;
	middle = nhalf;
	int middleY = nhalfY;
	lastro = m_nrow - nhalfY;
	lastcl = m_ncol - nhalf;
	sigsq=(m_fwhmX/2.35482F)*(m_fwhmX/2.35482F);
	double sigsqY=(m_fwhmY/2.35482F)*(m_fwhmY/2.35482F);
	radius = 1.;//radiusY*radiusY;

	sumg = 0.0F;
	sumgsq = 0.0F;
	pixels = 0.0F;
	for (j=0;j<nboxY;j++)
	{
		 jsq = (j-middleY)*(j-middleY)/sigsqY;
		 for (i=0;i<nbox;i++)
		 {
				rsq = (float)((i-middle)*(i-middle)/sigsq+jsq);
				*(g+j*m_maxboxX+i)=(float)(exp((double)(-0.5F*rsq)));
				if (rsq <= radius)
				{
						*(skip+j*m_maxboxX+i) = 0.F;
						sumg = sumg + *(g+j*m_maxboxX+i);
						sumgsq=sumgsq + (*(g+j*m_maxboxX+i))*(*(g+j*m_maxboxX+i));
						pixels = pixels + 1.0F;
				}
				else
				{
						*(skip+j*m_maxboxX+i) = 1.F;
				}
		 }
	}
	if(pixels==0.F)
	{
		 //printf("\n ***** pixels = 0");
		 pixels=1.F;
	}
	denom=sumgsq-(sumg*sumg)/pixels;
	sgop=sumg/pixels;
	relerr=1.0F/denom;
	relerr=(float)(sqrt((double)relerr));

	*(skip+middleY*m_maxboxX+middle)=1.F;
	pixels = pixels-1.0F;

	//	Nella matrice g (m_maxbox x m_maxbox) abbiamo messo un profilo 
	//	gaussiano con FWHM data. I valori significativi nei calcoli 
	//	seguenti sono contraddistinti da un valore nullo nella 
	//	matrice skip e il numero di questi pixel significativi è 
	//	indicato nella variabile "pixel".

	if(!FindBackground(m_maxsky))
	{
		//riporta(" MAXFIND: sky non trovato");
		return (0);
	}
	else
	{
		
		readns = m_opt00*m_opt00;
		phpadu = m_opt01;
		hmin = (float)sqrt((double)(readns+amax1(0.0F,m_smod.skymod)/phpadu));
		lobad = m_smod.skymod-((m_opt02)*hmin);
		hmin = relerr*((m_opt05)*hmin);
		readns = (float)sqrt((double)readns);
		//cout<<"hmin="<<hmin<<" lobad="<<lobad<<endl;
		m_pnstar=0;	//era *nstar=0;

		for(jy=middleY;jy<lastro;jy++)
		{
			for(jx=middle;jx<lastcl;jx++)
			{
				datum= GetPix(jx, jy);//*(hhh+jy*m_ncol+jx);
				if(datum>(m_smod.skymod+hmin))
				{
					for(iy=jy-nhalfY;iy<=jy+nhalfY;iy++)
					{
						for(ix=jx-nhalf;ix<=jx+nhalf;ix++)
						{
							i=middle+(ix-jx);
							j=middleY+(iy-jy);
							if(*(skip+j*m_maxboxX+i) == 0.F)
							{
								if(datum<GetPix(ix, iy)) //*(hhh+iy*m_ncol+ix)
								{
									goto ttzz;
								}
							}
						}
					}      	
					// fine controllo massimo locale

					*(skip+middleY*m_maxboxX+middle)=0.F;
					pixels = pixels+1.0F;

		// 	occorre considerare anche il pixel centrale per la convoluzione

					sgd=0.F;
					sd=0.F;
					sgsq=sumgsq;
					sg=sumg;
					p=pixels;
					for(iy=jy-nhalfY;iy<=jy+nhalfY;iy++)
					{
						for(ix=jx-nhalf;ix<=jx+nhalf;ix++)
						{
							i=middle+(ix-jx);
							j=middleY+(iy-jy);
							if(*(skip+j*m_maxboxX+i) == 0)
							{
								datum=GetPix(ix, iy);//*(hhh+iy*m_ncol+ix);
								if((datum>=lobad)&&(datum<=(m_opt03)))
								{
									sgd=sgd+datum*(*(g+j*m_maxboxX+i));
									sd=sd+datum;
								}
								else
								{
									sgsq=sgsq-(*(g+j*m_maxboxX+i))*(*(g+j*m_maxboxX+i));
									sg=sg-(*(g+j*m_maxboxX+i));
									p=p-1.F;
								}
							}
						}
					}
					if (p>1.5)
					{
						if(p<pixels)
						{
							sgsq=sgsq-(sg*sg)/p;
							if(sgsq!=0.)
								sgd=(sgd-sg*sd/p)/sgsq;
							else
								sgd=0.F;
						}
						else
							sgd=(sgd-sgop*sd)/denom;
					}
					else
						sgd=0.0F;

					height=sgd;
					*(skip+middleY*m_maxboxX+middle)=1.F;
					pixels = pixels-1.0F;

		//	Si risetta il pixel centrale del profilo gaussiano come prima

					if(height<hmin)
					{
						goto ttzz;
					}		
					// fine controllo convoluzione

					sharp=0.F;
					datum=GetPix(jx, jy);//*(hhh+jy*m_ncol+jx);
					if((datum>lobad)&&(datum<(m_opt03)))
					{
						p=0.F;
						for(iy=jy-nhalfY;iy<=jy+nhalfY;iy++)
						{
							for(ix=jx-nhalf;ix<=jx+nhalf;ix++)
							{
								i=middle+(ix-jx);
								j=middleY+(iy-jy);
								temp=0.0F;
								if(*(skip+j*m_maxboxX+i)==0.F)
								{
									datum=GetPix(ix, iy);//*(hhh+iy*m_ncol+ix);
									if((datum>=lobad)&&(datum<=(m_opt03)))
									{
										temp=temp+(datum-m_smod.skymod);
										p=p+1.F;
									}
								}
								sharp=sharp+temp;
							}
						}
						sharp=(GetPix(jx, jy)-m_smod.skymod-sharp/p)/height;//(*(hhh+jy*m_ncol+jx))
						/*if((sharp<(m_opt06))||(sharp>(m_opt07)))
						{
							goto ttzz;
						}*/
					}			
					// fine controllo profilo

					//	inizio calcolo centroide

					sumgd=0.0F;
					sumgsq=0.0F;
					sumg=0.0F;
					sumd=0.0F;
					sdgdx=0.0F;
					sdgdxs=0.0F;
					sddgdx=0.0F;
					sgdgdx=0.0F;
					p=0.0F;
					n=0;
					for(ix=jx-nhalf;ix<=jx+nhalf;ix++)
					{
						i=middle+(ix-jx);
						sg=0.0F;
						sd=0.0F;
						for(iy=jy-nhalfY;iy<=jy+nhalfY;iy++)
						{
							j=middleY+(iy-jy);
							wt=(float)(middleY-abs(j-middleY));
							datum=GetPix(ix,iy);//(*(hhh+jy*m_ncol+jx));//*(hhh+iy*m_ncol+ix);
							if((datum>=lobad)&&(datum<=(m_opt03)))
							{
								sd=sd+(datum-m_smod.skymod)*wt;
								sg=sg+wt*(*(g+j*m_maxboxX+i));
							}
						}
						if(sg>0.0)
						{
							wt=(float)(middle-abs(i-middle));
							sumgd=sumgd+wt*sg*sd;
							sumgsq=sumgsq+wt*sg*sg;
							sumg=sumg+wt*sg;
							sumd=sumd+wt*sd;
							p=p+wt;
							n=n+1;
							dgdx=sg*(middle-i);
							sdgdxs=sdgdxs+wt*dgdx*dgdx;
							sdgdx=sdgdx+wt*dgdx;
							sddgdx=sddgdx+wt*sd*dgdx;
							sgdgdx=sgdgdx+wt*sg*dgdx;
						}
					}
					if(n<=2)
					{
						goto ttzz;
					}

					hx=(sumgd-sumg*sumd/p)/(sumgsq-(sumg*sumg)/p);
					if(hx<=0.0)
					{
						goto ttzz;
					}
					skylvl=(sumd-hx*sumg)/p;
					dx=(sgdgdx-(sddgdx-sdgdx*(hx*sumg+skylvl*p)))/(hx*sdgdxs/sigsq);
					xcen=jx+dx/(1.F+(float)fabs((double)dx));

					sumgd=0.0F;
					sumgsq=0.0F;
					sumg=0.0F;
					sumd=0.0F;
					sdgdx=0.0F;
					sdgdxs=0.0F;
					sddgdx=0.0F;
					sgdgdx=0.0F;
					p=0.0F;
					n=0;
					for(iy=jy-nhalfY;iy<=jy+nhalfY;iy++)
					{
						j=middleY+(iy-jy);
						sg=0.0F;
						sd=0.0F;
						for(ix=jx-nhalf;ix<=jx+nhalf;ix++)
						{
							i=middle+(ix-jx);
							wt=(float)(middle-abs(i-middle));
							datum=GetPix(ix,iy);//*(hhh+iy*m_ncol+ix);
							if((datum>=lobad)&&(datum<=(m_opt03)))
							{
								sd=sd+(datum-m_smod.skymod)*wt;
								sg=sg+wt*(*(g+j*m_maxboxX+i));
							}
						}
						if(sg>0.0)
						{
							wt=(float)(middleY-abs(j-middleY));
							sumgd=sumgd+wt*sg*sd;
							sumgsq=sumgsq+wt*sg*sg;
							sumg=sumg+wt*sg;
							sumd=sumd+wt*sd;
							p=p+wt;
							dgdx=sg*(middleY-j);
							sdgdxs=sdgdxs+wt*dgdx*dgdx;
							sdgdx=sdgdx+wt*dgdx;
							sddgdx=sddgdx+wt*sd*dgdx;
							sgdgdx=sgdgdx+wt*sg*dgdx;
							n=n+1;
						}
					}
					if(n<=2)
					{
						goto ttzz;
					}
					hy=(sumgd-sumg*sumd/p)/(sumgsq-(sumg*sumg)/p);
					if(hy<=0.0)
					{
						goto ttzz;
					}
					skylvl=(sumd-hy*sumg)/p;
					dy=(sgdgdx-(sddgdx-sdgdx*(hy*sumg+skylvl*p)))/(hy*sdgdxs/sigsq);
					ycen=jy+dy/(1.F+(float)fabs((double)dy));

					//	fine calcolo centroide

					round=2.F*(hx-hy)/(hx+hy);
					//cout<<"hx="<<hx<<" hy="<<hy<<" round="<<round<<endl;
					/*if((round<(m_opt08))||(round>(m_opt09)))
					{
						goto ttzz;
					} */             
					// fine controllo round
					centro[m_pnstar].id=m_pnstar;
					centro[m_pnstar].peak=GetPix(int(xcen+0.5),int(ycen+0.5));
					height=-2.5F*(float)log10((double)(height/hmin));
					centro[m_pnstar].x=xcen;	//era centro[*nstar].x
					centro[m_pnstar].y=ycen;	//era centro[*nstar].y
					centro[m_pnstar].bkg=m_smod.skymod;
					centro[m_pnstar].bkgsig=m_smod.skysig;
					out<<centro[m_pnstar].x<<" "<<centro[m_pnstar].y<<endl;
					if(PeakRange(centro[m_pnstar])){
					 //cout<<"Xmin="<<centro[m_pnstar].xmin<<"Xmax="<<centro[m_pnstar].xmax<<endl;
					 //cout<<"Ymin="<<centro[m_pnstar].ymin<<"Ymax="<<centro[m_pnstar].ymax<<endl;
					 //cout<<centro[m_pnstar].id<<" "<<centro[m_pnstar].x<<" "<<centro[m_pnstar].y<<" "<<centro[m_pnstar].fwhmX <<" "<<centro[m_pnstar].fwhmY<<endl;
						//if((centro[m_pnstar].xmax-centro[m_pnstar].xmin)>m_fwhmX ||(centro[m_pnstar].ymax-centro[m_pnstar].ymin)>m_fwhmY){
							if (LocalBackground(centro[m_pnstar],3,5)){
							
					//PeakRange(centro[m_pnstar]);
								if(StarCenter(centro[m_pnstar])	){
									//Fwhm(centro[m_pnstar]);
									CleanBox(centro[m_pnstar].xmin, centro[m_pnstar].ymin, centro[m_pnstar].xmax, centro[m_pnstar].ymax, centro[m_pnstar].bkg);
									m_pnstar=m_pnstar+1;
								}
							
							}
						//}
					}
							//era *nstar=*nstar+1;
					if((m_pnstar)==m_MAXSTAR)	//era if((*nstar)==MAXSTAR)
					{
						delete []g;
						delete []skip;
						out.close();
						//SortStarList(centro,m_pnstar);
						return (1);
					}

ttzz:   	;
				}
			}
		}
	}
	delete []g;
	delete []skip;
	out.close();
	//SortStarList(centro,m_pnstar);
	return (1);
}


int MyImageAnalisys::SetImage(float *ima, short nc, short nr)
{
	long int i;
	m_ncol=nc;
	m_nrow=nr;
	long int npixels=(long int )nc*nr;
	m_imabuffer=new float[npixels];
	for(i=0;i<npixels;i++){
		m_imabuffer[i]=ima[i];
	}
	
	return 0;
	
}

float amin1(float x,float y)
{
	float minus;
	if(x<y)
		minus=x;
	else
		minus=y;
	return(minus);
}


float amax1(float x,float y)
{
	float max;
	if(x<y)
		max=y;
	else
		max=x;
	return(max);
}

int amin1(int x,int y)
{
	int minus;
	if(x<y)
		minus=x;
	else
		minus=y;
	return(minus);
}


int amax1(int x,int y)
{
	int max;
	if(x<y)
		max=y;
	else
		max=x;
	return(max);
}
int sign1(float x)
{
	int segno;

	if(fabs(x)>x)
		segno=-1;
	else
		segno=1;
	return(segno);
}

void MyImageAnalisys::PrintBackground()
{
	if(m_background){
		cout<<"Background Analisys results:"<<endl;
		cout<<"   Npixel    : "<<m_smod.skynpx <<endl;
		cout<<"   Max       : "<<m_smod.skymax <<endl;
		cout<<"   Min       : "<<m_smod.skymin <<endl;
		cout<<"   Average   : "<<m_smod.skymn <<endl;
		cout<<"   Median    : "<<m_smod.skymed <<endl;
		cout<<"   Mode      : "<<m_smod.skymod <<endl;
		cout<<"   Stddev    : "<<m_smod.skysig <<endl;
		cout<<"   Skew      : "<<m_smod.skyskw <<endl;		
	}
}

void MyImageAnalisys::PrintParameters()
{
	cout<<"READ NOISE (ADU)          : "<<m_opt01<<endl;
	cout<<"GAIN (e-/ADU)             : "<<m_opt00<<endl;
	cout<<"LOW GOOD DATUM (in sigmas): "<<m_opt02<<endl;
	cout<<"HIGH GOOD DATUM (in ADU)  : " <<m_opt03<<endl;
	cout<<"FWHM OF OBJECT(pixel)     : "<<m_opt04 <<endl;
	cout<<"THRESHOLD (in sigmas)     : "<<m_opt05 <<endl;
	cout<<"LOW SHARPNESS CUTOFF      : "<<m_opt06 <<endl;
	cout<<"HIGH SHARPNESS CUTOFF     : "<<m_opt07<<endl;
	cout<<"LOW ROUNDNESS CUTOFF      : "<<m_opt08 <<endl;
	cout<<"HIGH ROUNDNESS CUTOFF     : "<<m_opt09 <<endl;
}

void MyImageAnalisys::PrintStarCatalog()
{
	int i=0;
	for(i=m_pnstar-1;i>=0;i--){
	cout<<centro[i].id<<" "<<centro[i].x<<" "<<centro[i].y<<" ";
	cout<<centro[i].xmin<<" "<<centro[i].ymin<<" "<<centro[i].xmax<<" "<<centro[i].ymax<<endl;
	cout<<" "<<centro[i].A;
	cout<<" "<<centro[i].B;
	cout<<" "<<centro[i].theta<<endl;
	//cout<<"elong  = "<<cen.elong<<endl;
	//cout<<"ellip  = "<<cen.ellip<<endl;
	//cout<<"CXX    = "<<cen.cxx<<endl;
	//cout<<"CYY    = "<<cen.cyy<<endl;
	//cout<<"CXY    = "<<cen.cxy<<endl;
	//<<centro[m_pnstar].fwhmX <<" "<<centro[m_pnstar].fwhmY<<endl;
	}
}

int MyImageAnalisys::LocalBackground(center& cen, double r1,double r2)
{
	// calculates the base value around an object
	// which is deliminated by the bounds rect
	// this is done by getting the median value of
	// the pixels just outside of the bounds rect

	// the the number of pixels used is returned if not NULL
	vector<float> values(0);		// values seen in annulus
	//cout<<"r1="<<int(r)<<endl;
	int BOUNDS_THICKNESS=int(r1+r2);
//	int BOUNDS_THICKNESS=int(r2);
	float val =0.;
	for (int offset=int(r1); offset<=BOUNDS_THICKNESS; offset++) {
	//for (int offset=0; offset<=BOUNDS_THICKNESS; offset++) {
		// load all the valid pixels on the border into the values array
		// make sure we take the image bounds into account
		int x, y;
		int l = (cen.xpeak) - offset;
		int r = (cen.xpeak) + offset;
		int t = (cen.ypeak) - offset;
		int b = (cen.ypeak) + offset;

		int xtart = l;
		if (xtart < 0) {
			xtart = 0;
		}
		int xstop = r + 1;
		if (xstop > m_ncol) {
			xstop = m_ncol;
		}
		// the start and stop for y are 1 step smaller so we
		// don't count the corner pixels twice
		int ystart = t + 1;
		if (ystart < 0) {
			ystart = 0;
		}
		int ystop = b;
		if (ystop > m_nrow) {
			ystop = m_nrow;
		}
		if (t >= 0) {
			// get the pixels along the top
			for (x=xtart; x<xstop; x++) {
				val = GetPix(x, t);
				if (val >0.) {
					values.push_back(val);
				}
			}
		}
		if (b < m_nrow) {
			// get the pixels along the bottom
			for (x=xtart; x<xstop; x++) {
				val = GetPix(x, b);
				if (val >0.) {
					values.push_back(val);
				}
			}
		}
		if (l >= 0) {
			// get the pixels to the left
			for (y=ystart; y<ystop; y++) {
				val = GetPix(l, y);
				if (val >0.) {
					values.push_back(val);
				}
			}
		}
		if (r < m_ncol) {
			// get the pixels to the right
			for (y=ystart; y<ystop; y++) {
				val = GetPix (r, y);
				if (val >0.) {
					values.push_back(val);
				}
			}
		}
	}

	// sort the array if we have any valid pixel values
	float retVal = 0.;	
	SKY smod;// returned value
	if (values.size() > 0) {
		sort(values.begin(),values.end(),less<float>());
		if(!BackgroundStat(values,smod)){
		
		if(fabs(smod.skymod)>59000.)
			return 0;
		cen.bkg=smod.skymn;
		cen.bkgsig=smod.skysig;
		cen.nbkgpix=smod.skynpx;
		return (1);
		}else{
			return (0);
		}
	}
	return 0;
}



void MyImageAnalisys::SaveStarCatalog(const char *outfilename)
{
	char st[100];
	ofstream out(outfilename);
	ofstream ds("ds9.reg");
	ds<<"# Region file format : DS9 version 3.0"<<endl;
	ds<<"global color=green font=\"helvetica 10 normal\" select=1 edit=1 move=1 delete=1 include=1 fixed=0"<<endl;
	out.setf(ios::fixed|ios::left);
	out<<setprecision(3);
	int i=0;
	cout<<"id"<<"\t "<<"X(px)"<<"\t \t"<<"Y(px)"<<"\t "<<"MAG"<<"\t "<<"MAGERR"<<endl;
	cout<<endl;
	for(i=m_pnstar-1;i>=0;i--){
		if(centro[i].flag>0){
		out<<centro[i].id<<"\t "<<centro[i].x<<"\t "<<centro[i].y<<"\t "<<m_magzero+centro[i].mag<<"\t "<<centro[i].magerr<<"\t ";
		out<<centro[i].bkg<<"\t ";
		cout<<centro[i].id<<"\t "<<centro[i].x<<"\t "<<centro[i].y<<"\t "<<m_magzero+centro[i].mag<<"\t "<<centro[i].magerr<<endl;
		out<<centro[i].xmin<<"\t "<<centro[i].ymin<<"\t "<<centro[i].xmax<<"\t "<<centro[i].ymax;
		sprintf(st,"image;box(%d,%d,%d,%d,0)",int(centro[i].x),int(centro[i].y),int((centro[i].xmax-centro[i].xmin)),int((centro[i].ymax-centro[i].ymin)));
		//sprintf(st,"image;ellipse(%d,%d,%d,%d)	# text ={%d}",(int)centro[i].x,(int)centro[i].y,(int)(2*centro[i].A),(int)(2*centro[i].B)/*,(int)(-1*centro[i].theta)*/,(int)centro[i].id);
		ds<<st<<endl;
	out<<"\t"<<centro[i].A;
	out<<"\t"<<centro[i].B;
	out<<"\t"<<centro[i].theta<<endl;
	//out<<"\t"<<centro[i].elong;
	//out<<"\t"<<centro[i].ellip;
	//out<<"\t"<<centro[i].cxx;
	//out<<"\t"<<centro[i].cyy;
	//out<<"\t"<<centro[i].cxy;
	//out<<centro[i].fwhmX <<"\t"<<centro[i].fwhmY<<endl;
	/*if (!WriteSpectra(centro[i])){
	    cout<<"Not able to open ther spectra file.."<<endl;
	}*/

		}
	}
	out.close();
	ds.close();
}

void MyImageAnalisys::SaveStarCatalogShort(const char *outfilename)
{
	char st[100];
	//ofstream out(outfilename);
	ofstream ds("box.reg");
	ds<<"# Region file format : DS9 version 3.0"<<endl;
	ds<<"global color=green font=\"helvetica 10 normal\" select=1 edit=1 move=1 delete=1 include=1 fixed=0"<<endl;
	//out.setf(ios::fixed|ios::left);
	//out<<setprecision(3);
	int i=0;
	//cout<<"id"<<"\t "<<"X(px)"<<"\t \t"<<"Y(px)"<<endl;
	//cout<<endl;
	for(i=0;i<m_pnstar;i++){
		if(centro[i].flag>0){
		//out<<centro[i].id<<"\t "<<centro[i].x<<"\t "<<centro[i].y<<"\t ";
		//out<<centro[i].bkg<<"\t ";
		//cout<<centro[i].id<<"\t "<<centro[i].dx<<"\t "<<centro[i].dy<<endl;
		//out<<centro[i].xmin<<"\t "<<centro[i].ymin<<"\t "<<centro[i].xmax<<"\t "<<centro[i].ymax;
		sprintf(st,"image;box(%d,%d,%d,%d,0)# text ={%d}",int(centro[i].xpeak+1),int(centro[i].ypeak+1),int((centro[i].xmax-centro[i].xmin)),int((centro[i].ymax-centro[i].ymin)),centro[i].id);
		//sprintf(st,"image;ellipse(%d,%d,%d,%d)	# text ={%d}",(int)centro[i].x,(int)centro[i].y,(int)(2*centro[i].A),(int)(2*centro[i].B)/*,(int)(-1*centro[i].theta)*/,(int)centro[i].id);
		ds<<st<<endl;
	//out<<"\t"<<centro[i].A;
	//out<<"\t"<<centro[i].B;
	//out<<"\t"<<centro[i].theta<<endl;
	//out<<"\t"<<centro[i].elong;
	//out<<"\t"<<centro[i].ellip;
	//out<<"\t"<<centro[i].cxx;
	//out<<"\t"<<centro[i].cyy;
	//out<<"\t"<<centro[i].cxy;
	//out<<centro[i].fwhmX <<"\t"<<centro[i].fwhmY<<endl;
	/*if (!WriteSpectra(centro[i])){
	    cout<<"Not able to open ther spectra file.."<<endl;
	}*/

		}
	}
	//out.close();
	ds.close();
}

int MyImageAnalisys::Fwhm(center& cen)
{
	float peakVal = cen.peak;
    float limit = (peakVal+m_smod.skymod)/2;
	//float limit = m_smod.skymod+3.*m_smod.skysig;
	cen.fwhmX = 0.;
    cen.fwhmY = 0.;
	// guard against very poor stars
	if (limit >= peakVal) return (0);

    float left, right;
    float top, bottom;
    int x, y;
    int width = m_ncol;
    int height = m_nrow;
    float val;

    // work to the left
    x = cen.x-1; y = cen.y;
    while (x>=0 && (val = GetPix(x,y)) > limit) x--;
    if (x < 0) return (0);
	// logic above precludes divide by zero below
    left = x + (limit - val)/(GetPix(x+1,y) - val);

    // work to the right
    x = cen.x+1; y = cen.y;
    while (x<width && (val = GetPix(x,y)) > limit) x++;
    if (x >= width) return (0);
	// logic above precludes divide by zero below
    right = x - (limit - val)/(GetPix(x-1,y) - val);

    // work to the top
    x = cen.x; y = cen.y-1;
    while (y>=0 && (val = GetPix(x,y)) > limit) y--;
    if (y < 0) return (0);
    top = y + (limit - val)/(GetPix(x,y+1) - val);

    // work to the bottom
    x = cen.x; y = cen.y+1;
    while (y<height && (val = GetPix(x,y)) > limit) y++;
    if (y>=height) return (0);
    bottom = y - (limit - val)/(GetPix(x,y-1) - val);
    cen.fwhmX = right - left;
    cen.fwhmY = bottom - top;
    return (1);
}

int MyImageAnalisys::PeakRange(center& cen)
{
	float peakVal = cen.peak;
	float limit = (peakVal + cen.bkg)/2;
	//float limit = cen.bkg+2.5*cen.bkgsig;
	

	// guard against very poor stars
	if (limit >= peakVal) return (0);
    int x, y;
    int width = m_ncol;
    int height = m_nrow;
    float val;

    // work to the left
    x = cen.x-1; y = cen.y;
    while (x>=0 && (val = GetPix(x,y)) > limit) x--;
    if (x < 0) return (0);
	// logic above precludes divide by zero below
    cen.xmin = x + (limit - val)/(GetPix(x+1,y) - val);

    // work to the right
    x = cen.x+1; y = cen.y;
    while (x<width && (val = GetPix(x,y)) > limit) x++;
    if (x >= width) return (0);
	// logic above precludes divide by zero below
    cen.xmax = x - (limit - val)/(GetPix(x-1,y) - val);

    // work to the top
    x = cen.x; y = cen.y-1;
    while (y>=0 && (val = GetPix(x,y)) > limit) y--;
    if (y < 0) return (0);
    cen.ymin = y + (limit - val)/(GetPix(x,y+1) - val);

    // work to the bottom
    x = cen.x; y = cen.y+1;
    while (y<height && (val = GetPix(x,y)) > limit) y++;
    if (y>=height) return (0);
    cen.ymax = y - (limit - val)/(GetPix(x,y-1) - val);

    return (1);
}

int MyImageAnalisys::PeakRange(center& cen,float limit)
{
	float peakVal = cen.peak;
    //float limit = (peakVal + cen.bkg)/2;
	//float limit = cen.bkg+cen.bkgsig;
	
	//cout<<"limit="<<limit<<" peak="<<cen.peak<<endl;
	// guard against very poor stars
	if (limit >= peakVal) return (0);
    int x, y;
    int width = m_ncol;
    int height = m_nrow;
    float val;

    // work to the left
    x = cen.xpeak-1; y = cen.ypeak;
    while (x>=0 && (val = GetPix(x,y)-GetBack(x,y)) > limit) x--;
    if (x < 0) return (0);
	// logic above precludes divide by zero below
    cen.xmin = x + (limit - val)/((GetPix(x+1,y)-GetBack(x+1,y)) - val);

    // work to the right
    x = cen.xpeak+1; y = cen.ypeak;
    while (x<width && (val = GetPix(x,y)-GetBack(x,y)) > limit) x++;
    if (x >= width) return (0);
	// logic above precludes divide by zero below
    cen.xmax = x - (limit - val)/((GetPix(x-1,y)-GetBack(x-1,y)) - val);

    // work to the top
    x = cen.xpeak; y = cen.ypeak-1;
    while (y>=0 && (val = GetPix(x,y)-GetBack(x,y)) > limit) y--;
    if (y < 0) return (0);
    cen.ymin = y + (limit - val)/((GetPix(x,y+1)-GetBack(x,y+1)) - val);

    // work to the bottom
    x = cen.xpeak; y = cen.ypeak+1;
    while (y<height && (val = GetPix(x,y)-GetBack(x,y)) > limit) y++;
    if (y>=height) return (0);
    cen.ymax = y - (limit - val)/((GetPix(x,y-1)-GetBack(x,y-1)) - val);
	//cout<<"peakymax:"<<cen.ymax<<" pxmin:"<<cen.ymin<<endl;
    return (1);
}



int MyImageAnalisys::StarCenter(center& cen, double side_box)
{
	int ix=0,iy=0;
	//side_box=5;
	static int ns=1;

//bkg box
	int xmin1=int(cen.xpeak-2.*side_box+0.50000);
	int xmax1=int(cen.xpeak+2.*side_box+0.50000);
	int ymin1=int(cen.ypeak-2.*side_box+0.50000);
	int ymax1=int(cen.ypeak+2.*side_box+0.50000);

	if(xmin1<0)
		xmin1=0;
	if(xmax1>=m_ncol)
		xmax1=m_ncol-1;
	if(ymin1<0)
		ymin1=0;
	if(ymax1>=m_nrow)
		ymax1=m_nrow-1;
//

	side_box=amin1((xmax1-xmin1),(ymax1-ymin1))/4.;
	int xmin=int(cen.xpeak-side_box+0.50000);
	int xmax=int(cen.xpeak+side_box+0.50000);
	int ymin=int(cen.ypeak-side_box+0.50000);
	int ymax=int(cen.ypeak+side_box+0.50000);
	xmin1=amax1(int(cen.xpeak-2.*side_box+0.50000),0);
	xmax1=amin1(int(cen.xpeak+2.*side_box+0.50000),m_ncol-1);
	ymin1=amax1(int(cen.ypeak-2.*side_box+0.50000),0);
	ymax1=amin1(int(cen.ypeak+2.*side_box+0.50000),m_nrow-1);
	cen.xmax=xmax;
	cen.xmin=xmin;
	cen.ymax=ymax;
	cen.ymin=xmin;
	double sumI=0.;
	double sumIX=0.;
	double sumIY=0.;
	double sumIX2=0.;
	double sumIY2=0.;
	double sumIXY=0.;
	double sx=0,sy=0,sxy=0;
	double val=0.;
	double max=-1.;
	double x,y;
	double d=xmax-xmin;
	double b=xmax1-xmin1;
	double tt=0,bb=0,cc=0,qq=0,sb2=0,ss2=0,sq2=0,sc2=0,snr=0,ss;
	double alpha=1,beta=1;
	double bbb=0;
	for(iy=ymin1;iy<=ymax1;iy++){
		for(ix=xmin1;ix<=xmax1;ix++){
			bbb+=1.;
			val=GetPix(ix,iy);
			if(val>0)
				tt+=val;//-cen.bkg;//cen.bkg;
		}
	}
	//cout<<"tttt--> "<<tt<<" "<<xmin1<<" "<<endl;
	int area=0;
	if(xmin<0)
		xmin=0;
	if(xmax>=m_ncol)
		xmax=m_ncol-1;
	if(ymin<0)
		ymin=0;
	if(ymax>=m_nrow)
		ymax=m_nrow-1;
	for(iy=ymin;iy<=ymax;iy++){
		for(ix=xmin;ix<=xmax;ix++){
			val=GetPix(ix,iy);//-cen.bkg;//cen.bkg;
			x=ix-xmin;
			y=iy-ymin;
			area++;
			if(val>=0.){//cen.bkgsig
			//cout<<val<<" ";
				
				sumI+=(double)val;
				sumIX+=x*val;
				sumIY+=y*val;
				sumIXY+=(double)val*x*y;
				sumIX2+=(double)val*x*x;
				sumIY2+=(double)val*y*y;
			}

		}
	}
	cc=sumI;
//	
	b=bbb;
	d=area;
	
	double den=((alpha*b*b)/(d*d)-beta);
	double pd=(alpha-beta);
	double bd=(b*b-d*d)/(d*d);
	qq=tt-cc;
	double sc=1.+sqrt(cc+0.75);
	double sq=1.+sqrt(qq+0.75);
	bb=qq/(b*b-d*d)*(d*d);//bb=(pd*cc+alpha*qq)/den;
	//im->centro[k].bkg=bb
	ss=cc-bb;//ss=(cc*bd-qq)/den;
	
	if(bb>0.&& ss>0.){
		double sc=1.+sqrt(ss+0.75);//sc=1.+sqrt(cc+0.75);
		double sq=1.+sqrt(bb+0.75);//sq=1.+sqrt(qq+0.75);
		//sb2=(pd*pd*sc*sc+alpha*sq*sq)/(den*den);
		ss2=(sc*sc+sq*sq);//ss2=(sc*sc*bd*bd+sq*sq)/(den*den);
		snr=ss/sqrt(ss2);
	}else
		snr=0;
	x=xmin+sumIX/cc;
	y=ymin+sumIY/cc;
	sx=0,sy=0,sxy=0;
		/*for(iy=ymin;iy<=ymax;iy++){
			for(ix=xmin;ix<=xmax;ix++){
				sx+=(ix-x)*(ix-x);
				sy+=(iy-y)*(iy-y);
				sxy+=(ix-x)*(iy-y);
			}
		}*/
		//cout<<"tttt--> "<<x<<" "<<y<<" "<<sxy<<endl;
	sx=(sumIX2/(cc))-(sumIX/cc)*(sumIX/cc);
	sy=(sumIY2/(cc))-(sumIY/cc)*(sumIY/cc);
	sxy=(sumIXY/(cc))- sumIX*sumIY/(cc*cc);
		//cout<<"SX "<<sx<<" SY "<<sy<<" sxy"<<sxy<<endl;
	cen.A= sqrt((sx+sy+sqrt(pow(sx+sy,2.)+4.*(sx*sy-sxy*sxy)))/2.);
	cen.B= sqrt((sx+sy+sqrt(pow(sx+sy,2.)-4.*(sx*sy-sxy*sxy)))/2.);
	//cout<<ns<<") SX-->"<<sqrt(sx/cc)<<"SY->"<<sqrt(sy/cc)<<"SXY->"<<sxy<<" A->"<<cen.A<<" B->"<<cen.B<<endl;
	cen.x=x;
	cen.y=y;
	ns++;
		//cen.xpeak=x;
		//cen.ypeak=y;
	/*}else{
		//cout<<"gggggg-<< "<<qq<<" "<<bb<<" "<<ss<<" "<<b<<" "<<d<<endl;
		bb=0;
		ss=0;
		sb2=0;
		ss2=0;
		snr=0;
		x=0;
		y=0;
		sx=0,sy=0,sxy=0;
		x=xmin+sumIX/cc;
		y=ymin+sumIY/cc;
		sx=0,sy=0,sxy=0;
		for(iy=ymin;iy<=ymax;iy++){
			for(ix=xmin;ix<=xmax;ix++){
				sx+=(ix-x)*(ix-x);
				sy+=(iy-y)*(iy-y);
				sxy+=(ix-x)*(iy-y);
			}
		}
		sx=sumIX; (sx/(cc-1));
		sy=(sy/(cc-1));
		sxy=(sxy/(cc-1));
		cen.A= sqrt( (sx+sy+ sqrt(pow(sx+sy,2.)+4.*(sx*sy-sxy*sxy)) )/2.);
		cen.B= sqrt( (sx+sy+sqrt(pow(sx+sy,2.)-4.*(sx*sy-sxy*sxy)) )/2.);
		cout<<ns<<") SX-->"<<sqrt(sx/cc)<<"SY->"<<sqrt(sy/cc)<<"SXY->"<<sxy<<" A->"<<cen.A<<" B->"<<cen.B<<endl;

		//cout<<cc<<" "<<sumIX<<"SX--> "<<sx/sqrt(cc)<<" SY-> "<<sy/sqrt(cc)<<" XC->"<<x<<" YC-> "<<cen.xpeak<<endl;
		//cen.x=x;
		//cen.y=y;
		cen.x=cen.xpeak;
		cen.y=cen.ypeak;

	}*/
	//cout<<"SX--> "<<sx/sqrt(cc)<<" SY-> "<<sy/sqrt(cc)<<" XC-> "<<x<<" YC-> "<<cen.xpeak<<endl;
	cen.mag=ss;
	cen.bkg=bb;
	cen.bkgsig=sq;//sqrt(sq);//(sb2);
	cen.magerr=sc;//sqrt(ss2);
	cen.theta=snr;
	cen.dx=cen.xmax-cen.xmin;
	cen.dy=cen.ymax-cen.ymin;
//

	//if(area<m_minumpx){
	//	cen.flag=0;
		//cout<<cen.id<<"  ";
	//	return (0);
	//}

	/*cen.area=area;
	cen.mag=(sumI-area*cen.bkg);
	if(cen.mag>0.){
		cen.magerr =1. + sqrt(cen.mag + 0.75);
		//cen.magerr=1.0857*sqrt(area*cen.bkgsig*cen.bkgsig +sumI/m_opt01)/sumI;
		//cout<<"flux="<<sumI<<endl;
		//cout<<"mag="<<cen.bkg<<" "<<<<endl;
		//cout<<"err0"<<(1.0857*sqrt(area*cen.bkgsig*cen.bkgsig +sumI/m_opt01)/sumI)<<endl;
		cen.theta = cen.mag/sqrt(cen.magerr*cen.magerr + area*cen.bkgsig*cen.bkgsig);
	}else
		cen.theta =0.0;
	if(cen.flag!=2)
		cen.flag=1;
	cen.mag=sumI;
	cen.dx=cen.xmax-cen.xmin;
	cen.dy=cen.ymax-cen.ymin;
	x=sumIX/sumI;
	y=sumIY/sumI;
	cen.x=xmin+x; //standard FITS;
	cen.y=ymin+y;
	*/

	return 1;
}

int MyImageAnalisys::StarCenter(center& cen)
{
	int ix=0,iy=0;
	int xmin=int(cen.xmin+0.50000);
	int xmax=int(cen.xmax+0.50000);
	int ymin=int(cen.ymin+0.50000);
	int ymax=int(cen.ymax+0.50000);
	double sumI=0.;
	double sumIX=0.;
	double sumIY=0.;
	double sumIX2=0.;
	double sumIY2=0.;
	double sumIXY=0.;
	double val=0.;
	double max=-1.;
	double x,y;
	int area=0;
	for(iy=ymin;iy<=ymax;iy++){
		for(ix=xmin;ix<=xmax;ix++){
			val=GetPix(ix,iy);//-cen.bkg;//cen.bkg;
			x=ix-xmin;
			y=iy-ymin;
			//if(val>=cen.bkg){//cen.bkgsig
			
			//cout<<val<<" ";
			area++;
			sumI+=(double)val;
			sumIX+=(double)val*x;
			sumIY+=(double)val*y;
			sumIXY+=(double)val*x*y;
			sumIX2+=(double)val*x*x;
			sumIY2+=(double)val*y*y;
			/*if(val>max){
				cen.xpeak=ix;
				cen.ypeak=iy;
				cout<< ix <<" ";
				cout<< iy <<" ";
				max=cen.peak=float(val);
				cout << cen.peak << endl;
			}*/
			//}
		}
	}
//if(area<m_minumpx){
//	cen.flag=0;
//cout<<cen.id<<"  ";
//	return (0);
//}
cen.area=area;
cen.mag=sumI;
cen.magerr =1. + sqrt(sumI + 0.75);
//cen.magerr=1.0857*sqrt(area*cen.bkgsig*cen.bkgsig +sumI/m_opt01)/sumI;
//cout<<"flux="<<sumI<<endl;
//cout<<"mag="<<cen.bkg<<" "<<<<endl;
//cout<<"err0"<<(1.0857*sqrt(area*cen.bkgsig*cen.bkgsig +sumI/m_opt01)/sumI)<<endl;
cen.theta = (sumI - area*cen.bkg)/sqrt(cen.magerr*cen.magerr + area*cen.bkgsig*cen.bkgsig);

if(cen.flag!=2)
cen.flag=1;
cen.dx=cen.xmax-cen.xmin;
cen.dy=cen.ymax-cen.ymin;
x=sumIX/sumI;
y=sumIY/sumI;
cen.x=xmin+x; //standard FITS;
cen.y=ymin+y;


return 1;

double x2=(sumIX2/sumI)-x*x;
double y2=(sumIY2/sumI)-y*y;
double xy=(sumIXY/sumI)-x*y;
double temp=(x2-y2);
double theta,temp2;
if ((temp2=x2*y2-xy*xy)<0.00694)
{
	x2 += 0.0833333;
	y2 += 0.0833333;
	temp2 = x2*y2-xy*xy;
}

if ((fabs(temp) > 0.0))
theta = atan2(2.0 * xy,temp) / 2.0;
else
theta = 3.14/4.0;

cen.theta=theta*(180./3.1416);
double co=(x2+y2)/2.;
double co1=(temp)/2.;
co1=sqrt(co1*co1+xy*xy);
double A=sqrt(co+co1);
double B=0.;
if((co-co1)>0.)
B=sqrt(co-co1);
cen.A=A;
cen.B=B;
if(B>0.){
	cen.elong=float(A/B);
	cen.ellip=float(1-B/A);
}else{
	cen.elong=99.;
	cen.ellip=0.;
}
/*
	cout<<"Xpeak  = "<<cen.xpeak<<endl;
	cout<<"Ypeak  = "<<cen.ypeak<<endl;
	cout<<"PeakVal= "<<cen.peak<<endl;
	
	cout<<"A      = "<<cen.A<<endl;
	cout<<"B      = "<<cen.B<<endl;
	cout<<"theta  = "<<cen.theta<<endl;
	cout<<"elong  = "<<cen.elong<<endl;
	cout<<"ellip  = "<<cen.ellip<<endl;
	cout<<"CXX    = "<<cen.cxx<<endl;
	cout<<"CYY    = "<<cen.cyy<<endl;
	cout<<"CXY    = "<<cen.cxy<<endl;*/

cen.cxx=y2/temp2;
cen.cyy=x2/temp2;
cen.cxy=-2.*xy/temp2;


cen.xmin+=0.0;
cen.xmax+=0.0;
cen.ymin+=0.0;
cen.ymax+=0.0;
return 1;
}


float MyImageAnalisys::GetPix(int x, int y)
{
	if(x>=0 && x<m_ncol && y>=0 && y<m_nrow){
		return m_imabuffer[m_ncol*y+x];
	}else{
//		cout<<"pixels coordinates out of image!!"<<endl;
		return (-99.);
	}
}

float MyImageAnalisys::GetBack(int x, int y)
{
	if(x>=0 && x<m_ncol && y>=0 && y<m_nrow){
		return m_backbuffer[m_ncol*y+x];
	}else{
//	cout<<"pixels coordinates out of image!!"<<endl;
		return (-99.);
	}
}

float MyImageAnalisys::GetBackSig(int x, int y)
{
	if(x>=0 && x<m_ncol && y>=0 && y<m_nrow){
		return m_backsigbuffer[m_ncol*y+x];
	}else{
//		cout<<"pixels coordinates out of image!!"<<endl;
		return (-99.);
	}
}

int MyImageAnalisys::SetPix(int x, int y, float val)
{
	if(x>=0 && x<m_ncol && y>=0 && y<m_nrow){
		m_imabuffer[m_ncol*y+x]=val;
		return (1);
	}else{
		cout<<"pixels coordinates out of image!!"<<endl;
		return (0);
	}
}

int MyImageAnalisys::FilterImage(int filtertype, int size)
{
	
	vector<float> temp(9);
	int ix=1,iy=1,kx,ky,middle,k=0;
	middle=size/2;
	int median=(size*size)/2+1;
	//cout<<"middle="<<middle<<" med="<<median<<endl;
	for(iy=1;iy<m_nrow-1;iy++){
		for(ix=1;ix<m_ncol-1;ix++){
			k=0;
			//cout<<"oldval="<<GetPix(ix,iy)<<endl;
			for(ky=iy-middle;ky<=iy+middle;ky++){
				for(kx=ix-middle;kx<=ix+middle;kx++){
					temp[k]=(GetPix(kx,ky));
					//cout<<"k="<<k<<" "<<temp[k]<<" ";
					k++;
				}
			}
			sort(&temp[0],&temp[8],less<float>());
			SetPix(ix,iy,temp[median]);
			//cout<<"temp[0]="<<temp[0]<<" temp[8]="<<temp[8]<<endl;
			//cout<<"medianval="<<GetPix(ix,iy)<<endl;
		}
	}
	
	return 0;
}

int MyImageAnalisys::SaveFitsImage(const char *filename)
{

 	int status; 
	long  fpixel; 
	fitsfile *outfptr;
	/* initialize FITS image parameters */ 
	int bitpix   =  FLOAT_IMG; 
	long int naxes[2]; 
	naxes[0]=m_ncol; 
	naxes[1]=m_nrow; 
	long int npixels=m_ncol*m_nrow;
	int naxis=2;
                                  
    	remove(filename);               /* Delete old file if it already exists */ 
 
    	status = 0;         /* initialize status before calling fitsio routines */ 
 
    	if (fits_create_file(&outfptr, filename, &status)) /* create new FITS file */ 
		return ( status );           /* call printerror if error occurs */                   
	//cout<<outfptr<<endl;

	if (fits_create_img(outfptr, bitpix, naxis, naxes, &status) ) 
		return ( status );           
 	//cout<<status<<endl;

	/* initialize the values in the image with a linear ramp function */ 
    	fpixel = 1;                               /* first pixel to write      */ 
 
	/* write the array of unsigned integers to the FITS file */ 
    	if ( fits_write_img(outfptr, TFLOAT, fpixel, npixels, m_imabuffer, &status) ) 
        	return( status ); 
	//cout<<status<<endl;

	if ( fits_close_file(outfptr, &status) )                /* close the file */ 
		return( status );            
 	//cout<<status<<endl;

    return 0; 
}

int MyImageAnalisys::SaveFitsImage(int *b,const char *filename)
{

 	int status; 
	long  fpixel; 
	fitsfile *outfptr;
	/* initialize FITS image parameters */ 
	int bitpix   =  SHORT_IMG; 
	long int naxes[2]; 
	naxes[0]=m_ncol; 
	naxes[1]=m_nrow; 
	long int npixels=m_ncol*m_nrow;
	int naxis=2;
                                  
    	remove(filename);               /* Delete old file if it already exists */ 
 
    	status = 0;         /* initialize status before calling fitsio routines */ 
 
    	if (fits_create_file(&outfptr, filename, &status)) /* create new FITS file */ 
		return ( status );           /* call printerror if error occurs */                   
	//cout<<outfptr<<endl;

	if (fits_create_img(outfptr, bitpix, naxis, naxes, &status) ) 
		return ( status );           
 	//cout<<status<<endl;

	/* initialize the values in the image with a linear ramp function */ 
    	fpixel = 1;                               /* first pixel to write      */ 
 
	/* write the array of unsigned integers to the FITS file */ 
    	if ( fits_write_img(outfptr, TINT, fpixel, npixels,b, &status) ) 
        	return( status ); 
	//cout<<status<<endl;

	if ( fits_close_file(outfptr, &status) )                /* close the file */ 
		return( status );            
 	//cout<<status<<endl;

    return 0; 
}

int MyImageAnalisys::SaveFitsImage(float *b,const char *filename)
{

 	int status; 
	long  fpixel; 
	fitsfile *outfptr;
	/* initialize FITS image parameters */ 
	int bitpix   =  SHORT_IMG; 
	long int naxes[2]; 
	naxes[0]=m_ncol; 
	naxes[1]=m_nrow; 
	long int npixels=m_ncol*m_nrow;
	int naxis=2;
                                  
    	remove(filename);               /* Delete old file if it already exists */ 
 
    	status = 0;         /* initialize status before calling fitsio routines */ 
 
    	if (fits_create_file(&outfptr, filename, &status)) /* create new FITS file */ 
		return ( status );           /* call printerror if error occurs */                   
	//cout<<outfptr<<endl;

	if (fits_create_img(outfptr, bitpix, naxis, naxes, &status) ) 
		return ( status );           
 	//cout<<status<<endl;

	/* initialize the values in the image with a linear ramp function */ 
    	fpixel = 1;                               /* first pixel to write      */ 
 
	/* write the array of unsigned integers to the FITS file */ 
    	if ( fits_write_img(outfptr, TFLOAT, fpixel, npixels,b, &status) ) 
        	return( status ); 
	//cout<<status<<endl;

	if ( fits_close_file(outfptr, &status) )                /* close the file */ 
		return( status );            
 	//cout<<status<<endl;

    return 0; 
}

int MyImageAnalisys::SaveBackImage(const char *filename)
{

 	int status; 
	long  fpixel; 
	fitsfile *outfptr;
	/* initialize FITS image parameters */ 
	int bitpix   =  FLOAT_IMG; 
	long int naxes[2]; 
	naxes[0]=m_ncol; 
	naxes[1]=m_nrow; 
	long int npixels=m_ncol*m_nrow;
	int naxis=2;
                                  
    	remove(filename);               /* Delete old file if it already exists */ 
 
    	status = 0;         /* initialize status before calling fitsio routines */ 
 
    	if (fits_create_file(&outfptr, filename, &status)) /* create new FITS file */ 
		return ( status );           /* call printerror if error occurs */                   
	//cout<<outfptr<<endl;

	if (fits_create_img(outfptr, bitpix, naxis, naxes, &status) ) 
		return ( status );           
 	//cout<<status<<endl;

	/* initialize the values in the image with a linear ramp function */ 
    	fpixel = 1;                               /* first pixel to write      */ 
 
	/* write the array of unsigned integers to the FITS file */ 
    	if ( fits_write_img(outfptr, TFLOAT, fpixel, npixels, m_backbuffer, &status) ) 
        	return( status ); 
	//cout<<status<<endl;

	if ( fits_close_file(outfptr, &status) )                /* close the file */ 
		return( status );            
 	//cout<<status<<endl;

    return 0; 
}


int MyImageAnalisys::CleanHotPixels()
{
	//da modificare per tener conto del back locale
	vector<float> temp(9);
	int ix=1,iy=1,kx,ky,middle,k=0;
	middle=1;
	int median=5;
	float tresh=m_smod.skymn+5.*m_smod.skysig;
	float tresh1=m_smod.skymn+2.*m_smod.skysig;
	//cout<<"middle="<<middle<<" med="<<median<<endl;
	for(iy=1;iy<m_nrow-1;iy++){
		for(ix=1;ix<m_ncol-1;ix++){
			k=0;
			if((GetPix(ix,iy)-GetBack(ix,iy))>5*GetBackSig(ix,iy)){
			//cout<<"oldval="<<GetPix(ix,iy)<<endl;
			if((GetPix(ix-1,iy)-GetBack(ix-1,iy))<=tresh1 &&
			   (GetPix(ix+1,iy)-GetBack(ix+1,iy))<=tresh1 &&
			   (GetPix(ix,iy-1)-GetBack(ix,iy+1))<=tresh1 &&
			   (GetPix(ix,iy-1)-GetBack(ix,iy-1))<=tresh1){
			for(ky=iy-middle;ky<=iy+middle;ky++){
				for(kx=ix-middle;kx<=ix+middle;kx++){
					temp[k]=(GetPix(kx,ky));
					//cout<<"k="<<k<<" "<<temp[k]<<" ";
					k++;
				}
			}
			sort(&temp[0],&temp[8],less<float>());
			SetPix(ix,iy,temp[median]);
			//cout<<"medianval="<<GetPix(ix,iy)<<endl;
			}
			}
			//cout<<"temp[0]="<<temp[0]<<" temp[8]="<<temp[8]<<endl;
			//cout<<"medianval="<<GetPix(ix,iy)<<endl;
		}
	}
	
	return 0;
}

int MyImageAnalisys::CleanBox(int ix, int iy, int fx, int fy, float val)
{
	int iix=0,iiy=0;
	//cout<<"middle="<<middle<<" med="<<median<<endl;
	for(iiy=iy;iiy<fy;iiy++){
		for(iix=ix;iix<fx;iix++){
			if(GetPix(iix,iiy)>(val+m_smod.skysig))
			   SetPix(iix,iiy,val);
			//cout<<"medianval="<<GetPix(ix,iy)<<endl;
		}
	}
	
	return 0;
}

int MyImageAnalisys::Segmentation()
{
		int iy=0,ix=0,kx=0,ky=0,i=0,j=0,kky=0;
		CENTRO cen;
		int nipx=9;
		double val=0;
		m_pnstar=0;
		int end=2;
		int tot=12;
		int *bmap=new int[m_ncol*m_nrow];
		ofstream out("pippo.reg");
		float tresh1=m_smod.skymod+m_opt05*m_smod.skysig;
		//float tresh1=2167.91+m_opt05*14.524;

		for(iy=1;iy<m_nrow-1;iy++){
			for(ix=1;ix<m_ncol-1;ix++){
				if(GetPix(ix,iy)<=tresh1){
				  bmap[iy*m_nrow+ix]=0;
				}else{
				  bmap[iy*m_nrow+ix]=1;
				}
			}
		}
		double max=-1e30;
		for(iy=end;iy<m_nrow-end;iy++){
			for(ix=end;ix<m_ncol-end;ix++){
				if(bmap[iy*m_nrow+ix]==1){
					int temp=0.;
					for(ky=iy-end;ky<=iy+end && ky<m_nrow;ky++){
						for(kx=ix-end;kx<=ix+end;kx++){
							temp+=(bmap[ky*m_nrow+kx]);
							val=GetPix(kx,ky);
							if(val>max)
								max=val;
						}
					}
					if(temp==tot){
						cen.xmin=ix-1;
						cen.xmax=ix+1;
						cen.ymin=iy-1;
						cen.ymax=iy+1;
						kky=ky;kx=ix;
						nipx=temp;
						int y=iy-1;
						while(y<m_nrow && ((bmap[y*m_nrow+kx]==1)||(bmap[y*m_nrow+kx-1]==1)||(bmap[y*m_nrow+kx+1]==1))){
							val=GetPix(kx,y);
								if(val>max)
									max=val;
								y--;
								nipx++;
						}
						if(y<cen.ymin)
							cen.ymin=y;
						while(kky<m_nrow && ((bmap[ky*m_nrow+kx]==1)||(bmap[ky*m_nrow+kx-1]==1)||(bmap[ky*m_nrow+kx+1]==1))){
							val=GetPix(kx,kky);
								if(val>max)
									max=val;
							int x=ix-1;
							nipx++;
							while(bmap[ky*m_nrow+x]==1 && x>0 && abs(ix-x)<=int(3.*m_fwhmX+0.50000)){
								val=GetPix(x,ky);
								if(val>max)
									max=val;
								x--;
								nipx++;
							}
							 if(x<cen.xmin)cen.xmin=x;
							x=ix+1;
							while(bmap[ky*m_nrow+x]==1&& x<m_ncol && abs(ix-x)<=int(3.*m_fwhmX+0.50000)){
								val=GetPix(x,ky);
								if(val>max)
									max=val;
								x++;
								nipx++;
							}
							 if(x>cen.xmax)cen.xmax=x;
							cen.ymax=ky;
							ky++;
						}
						if(nipx>=12){
							centro[m_pnstar].peak=max;
							centro[m_pnstar].xmin=cen.xmin;
							centro[m_pnstar].xmax=cen.xmax;
							centro[m_pnstar].ymin=cen.ymin;
							centro[m_pnstar].ymax=cen.ymax;
							centro[m_pnstar].id=m_pnstar;
							LocalBackground(centro[m_pnstar],3,5);
							if(centro[m_pnstar].bkg>(m_smod.skymed+5.*m_smod.skysig)){
								centro[m_pnstar].bkg=m_smod.skymn;
								centro[m_pnstar].bkgsig=m_smod.skysig;

							}
							if(StarCenter(centro[m_pnstar])	){
									//Fwhm(centro[m_pnstar]);
									//CleanBox(centro[m_pnstar].xmin, centro[m_pnstar].ymin, centro[m_pnstar].xmax, centro[m_pnstar].ymax, centro[m_pnstar].bkg);
									m_pnstar=m_pnstar+1;
							}
							//cout<<"DY="<<cen.ymax<<" "<<cen.ymin<<endl;
							//cout<<"DX="<<cen.xmax<<" "<<cen.xmin<<endl;
							out<<cen.xmin+1<<" "<<cen.ymin+1<<endl;
						}
						for(i=cen.ymin;i<=cen.ymax;i++)
							for(j=cen.xmin;j<=cen.xmax;j++)
								bmap[i*m_nrow+j]=0;
					}
				}
			}
		}
	out.close();
	double dx=0,dy=0,dy1;
	/*SortStarList(centro,m_pnstar);
	
	
	for(i=0;i<m_pnstar-1;i++){
		
		dx=centro[i+1].x-centro[i].x;
		dy=centro[i+1].ymin-centro[i].ymax;
		dy1=centro[i+1].ymax-centro[i].ymin;
		//cout<<"i="<<i<<" "<<centro[i].id<<"-"<<" dx="<<dx<<" "<<dy<<" " <<dy1<<endl;

		if(fabs(dx)<=5. && (fabs(dy)<=7.|| fabs(dy1)<=7.)){
			cout<<"AA0 "<<centro[i].ymax<<" "<<centro[i].ymin<<endl;
			centro[i].xmin=amin1(centro[i+1].xmin,centro[i].xmin);
			centro[i].xmax=amax1(centro[i+1].xmax,centro[i].xmax);
			centro[i].ymax=amax1(centro[i+1].ymax,centro[i].ymax);
			centro[i].ymin=amin1(centro[i+1].ymin,centro[i].ymin);
			//cout<<"AA1 "<<centro[i+1].ymax<<" "<<centro[i+1].ymin<<endl;
			cout<<"AA2 "<<centro[i].ymax<<" "<<centro[i].ymin<<endl;
			j=i+1;
			//cout<<centro[i+1].id<<"-"<<centro[i].id<<" dx="<<dx<<" "<<centro[i+1].ymin-centro[i].ymax<<endl;
			//cout<<"AA"<<endl;
			
			while(j<m_pnstar){
				swap(centro[j],centro[j+1]);
				j++;
			}
			//cout<<centro[i+1].id<<"-"<<centro[i].id<<" dx="<<dx<<" "<<centro[i+1].ymin-centro[i].ymax<<endl;
			m_pnstar--;
			//SortStarList(centro,m_pnstar);
			i-=2;
		}
		
	}*/
	
	for(int jj=0;jj<m_pnstar-1;jj++){
		for(i=jj+1;i<m_pnstar;i++){
			if(centro[i].flag){
		dx=centro[i].x-centro[jj].x;
		double dy2=centro[i].y-centro[jj].y;
		//cout<<"dy2 "<<dy2<<endl;
		dy=centro[i].ymin-centro[jj].ymax;
		dy1=centro[i].ymax-centro[jj].ymin;
		//cout<<"i="<<i<<" "<<centro[i].id<<"-"<<" dx="<<dx<<" "<<dy<<" " <<dy1<<endl;
		if(((centro[jj].ymin<centro[i].ymin &&centro[jj].ymax>centro[i].ymax &&
			centro[jj].xmin<centro[i].xmin &&centro[jj].xmax>centro[i].xmax) ||
			(centro[jj].ymin>centro[i].ymin && centro[jj].ymax<centro[i].ymax && fabs(dx)<=5.)||
			(centro[jj].ymin<centro[i].ymin && centro[jj].ymax<centro[i].ymax && fabs(dx)<=5.)
			)&&(((fabs(dy)<=5.)|| (fabs(dy1)<=5.)||(fabs(dy2)<=5.)) && (fabs(dx)<=2.))){


		/*if((fabs(dy)<=5.)|| (fabs(dy1)<=5.) || (fabs(dy2)<=5.)){
			if(fabs(dx)<=5.){*/
			cout<<jj<<" O "<<centro[jj].ymax<<" "<<centro[jj].ymin<<endl;
			centro[jj].xmin=amin1(centro[i].xmin,centro[jj].xmin);
			centro[jj].xmax=amax1(centro[i].xmax,centro[jj].xmax);
			centro[jj].ymax=amax1(centro[i].ymax,centro[jj].ymax);
			centro[jj].ymin=amin1(centro[i].ymin,centro[jj].ymin);
			//cout<<"AA1 "<<centro[i+1].ymax<<" "<<centro[i+1].ymin<<endl;
			//cout<<jj<<" N "<<centro[jj].ymax<<" "<<centro[jj].ymin<<endl;
			//j=i+1;
			//cout<<centro[i+1].id<<"-"<<centro[i].id<<" dx="<<dx<<" "<<centro[i+1].ymin-centro[i].ymax<<endl;
			//cout<<"elimino star"<<centro[i].id<<endl;
			centro[i].flag=0;
			//while(j<m_pnstar){
				//swap(centro[i],centro[ns-1]);

			//	j++;
			//}
			//cout<<centro[i+1].id<<"-"<<centro[i].id<<" dx="<<dx<<" "<<centro[i+1].ymin-centro[i].ymax<<endl;
			//m_pnstar--;
			//SortStarList(centro,m_pnstar);
			//}
		}
			}
	}
	///	cout<<"BB"<<endl;
	}
	

	delete []bmap;
	return 0;
}

int MyImageAnalisys::Segmentation_1(int box)
{
		int iy=0,ix=0,kx=0,ky=0,i=0,j=0,kky=0;
		CENTRO cen;
		int nipx=box*box;
		double val=0;
		m_pnstar=0;
		int end=box/2;
		int tot=box*box;
		int *bmap=new int[m_ncol*m_nrow];
		ofstream out("pippo.reg");
		float tresh1;//=m_smod.skymn+m_opt05*m_smod.skysig;
		//float tresh1=2167.91+m_opt05*14.524;

		for(iy=end;iy<m_nrow-end;iy++){
			for(ix=end;ix<m_ncol-end;ix++){
				tresh1=m_opt05*GetBackSig(ix,iy);
				if((GetPix(ix,iy)-GetBack(ix,iy))<=tresh1){
				  bmap[iy*m_ncol+ix]=0;
				}else{
				  bmap[iy*m_ncol+ix]=1;
				}
			}
		}
	
		for(iy=end;iy<m_nrow-end;iy++){
			for(ix=end;ix<m_ncol-end;ix++){
				if(bmap[iy*m_nrow+ix]==1){
					int temp=0,xx=0,yy=0;
					double max=-1e30;
					for(ky=iy-end;ky<=iy+end && ky<m_nrow;ky++){
						for(kx=ix-end;kx<=ix+end;kx++){
							temp+=(bmap[ky*m_ncol+kx]);
							val=GetPix(kx,ky)-GetBack(kx,ky);
							if(val>max){
								max=val;
								xx=kx;
								yy=ky;
							}
						}
					}

//     if(temp==1) bmap[iy*m_nrow+ix]=0; //Elimina pixels non connessi

					if(temp==tot){
						cen.xmin=ix-end;
						cen.xmax=ix+end;
						cen.ymin=iy-end;
						cen.ymax=iy+end;
						cen.xpeak=xx;
						cen.ypeak=yy;
						kky=yy;kx=xx;
						nipx=temp;
						int y=yy;
						int x;
						int old_peak=xx;
						int x1;


   while(y>0 && ((bmap[y*m_ncol+kx]==1)||(bmap[y*m_ncol+kx-1]==1)||(bmap[y*m_ncol+kx+1]==1)))
                                                {
						 y--;		
						}
						kky=y+1;

while(kky<m_nrow && ((bmap[kky*m_ncol+kx]==1)||(bmap[kky*m_ncol+kx-1]==1)||(bmap[kky*m_nrow+kx+1]==1))){
		         /*	 if((val=GetPix(kx,kky)-GetBack(kx,kky))>max){
								//if(val>max){
									max=val;
									cen.xpeak=kx;
									cen.ypeak=kky;
			 //cout<<"1-XI="<<ix<<" XP="<<cen.xpeak<<endl;
								}					nipx++;*/
							
							old_peak=kx=x=int(cen.xpeak);
							x1=int(cen.xpeak)+1;

  while((bmap[kky*m_ncol+x]==1 && bmap[kky*m_ncol+x1]==1) && x>0 && x1<m_ncol)
{
       if((val=GetPix(x,kky)-GetBack(x,kky))>max/*=3.*GetBackSig(x,kky)*/){
									//if(val>max){		
	                                                                        max=val;
	    if(abs(old_peak-x)<2) {
		                           cen.xpeak=x;
					   old_peak=x;
	                                   cen.ypeak=kky;
	                                    }
				 //cout<<"2-XI="<<ix<<" XP="<<cen.xpeak<<endl;
									}
	if((val=GetPix(x1,kky)-GetBack(x1,kky))>max/*=3.*GetBackSig(x,kky)*/){
									//if(val>max){		
										max=val;
				 if(abs(old_peak-x1)<2)	 {
				                                  cen.xpeak=x1;
								  old_peak=x1;
								  cen.ypeak=kky;
				                                   }
			       //cout<<"2-XI="<<ix<<" XP="<<cen.xpeak<<endl;
									}
								if(x<cen.xmin)
									 cen.xmin=x;
								if(x1>cen.xmax)
									 cen.xmax=x1;
								//}
								//if(fabs((xx-x)-(x1-xx))>4)
								//	break;
								//else{
									nipx+=2;
	  	//bmap[kky*m_nrow-1+x]=bmap[kky*m_nrow+x]=bmap[kky*m_nrow+1+x]==0;
									bmap[kky*m_ncol+x]=5;
									bmap[kky*m_ncol+x1]=8;
			     	    /*if(bmap[kky*m_ncol+x]==1)*/ x--;
				   /*if(bmap[kky*m_ncol+x1]==1)*/ x1++;
								//}
}   //End while per ricerca simmetria
							/*x=xx+1;
       		while(bmap[kky*m_ncol+x]==1 && x<m_ncol && (x-xx)<=(xx-int(cen.xmin))){
				 if((val=GetPix(x,kky)-GetBack(x,kky))>max){
								//if(val>max){
									max=val;
									cen.xpeak=x;
									cen.ypeak=kky;
			     //cout<<"3-XI="<<ix<<" XP="<<cen.xpeak<<endl;
								}
								if(x>(int)cen.xmax)
									cen.xmax=x;
									nipx++;
		    //bmap[kky*m_nrow-1+x]=bmap[kky*m_nrow+x]=bmap[kky*m_nrow+1+x]==0;
									bmap[kky*m_ncol+x]=8;
								//}
								x++;
							}*/
							if(kky>(int)cen.ymax){
								cen.ymax=kky;
							}
	      //bmap[kky*m_nrow+kx-1]=bmap[kky*m_nrow+kx]=bmap[kky*m_nrow+kx+1]==0;
							bmap[kky*m_ncol+int(cen.xpeak)]=2;
							//}
							kky++;
						}
						if(nipx>(box*box*2)){				     
							cen.id=m_pnstar; 
							centro[m_pnstar].peak=max;
							centro[m_pnstar].xmin=cen.xmin;
							centro[m_pnstar].xmax=cen.xmax;
							centro[m_pnstar].ymin=cen.ymin;
							centro[m_pnstar].ymax=cen.ymax;
							centro[m_pnstar].id=m_pnstar;

							/*for(i=(int)centro[m_pnstar].ymin;i<=(int)centro[m_pnstar].ymax;i++)
								for(j=(int)centro[m_pnstar].xmin;j<=(int)centro[m_pnstar].xmax;j++)
										if(bmap[i*m_ncol+j]==1)
											bmap[i*m_ncol+j]=0;*/																					


							//LocalBackground(centro[m_pnstar]);
							/*if(centro[m_pnstar].bkg>(m_smod.skymed+5.*m_smod.skysig)){
								

							}*/
	centro[m_pnstar].bkg=(GetBack(cen.xmin,cen.ymin)+GetBack(cen.xmin,cen.ymax)+ GetBack(cen.xmax,cen.ymin)+GetBack(cen.xmax,cen.ymax))/4.;
	centro[m_pnstar].bkgsig=(GetBackSig(cen.xmin,cen.ymin)+GetBackSig(cen.xmin,cen.ymax)+GetBackSig(cen.xmax,cen.ymin)+GetBackSig(cen.xmax,cen.ymax))/4.;
							/*						
								double dxs;
								double dxd;
								double dx=cen.xmax-cen.xmin;
								double dyp;
								double dym;
								double dy=cen.ymax-cen.ymin;
								if(dx>15){
									centro[m_pnstar].flag=2;
									cout<<"Star\#"<<m_pnstar<<endl;
									
									for(i=(int)centro[m_pnstar].ymin+1;i<=(int)centro[m_pnstar].ymax;i++){
										for(j=(int)centro[m_pnstar].xmin+1;j<=(int)centro[m_pnstar].xmax;j++){
										 dxs=GetPix(j,i)-GetPix(j-1,i);
										 dxd=GetPix(j+1,i)-GetPix(j,i);
										 if(dxs<0 && dxd>0 && bmap[i*m_ncol+j]>=3)
											 bmap[i*m_ncol+j]=6;
											 //cout<<"x="<<j<<"/"<<i<<";"<<endl;
										}
									}
								}*/
					if(StarCenter(centro[m_pnstar])	){
								/*for(i=(int)centro[m_pnstar].ymin;i<=(int)centro[m_pnstar].ymax;i++)
									for(j=(int)centro[m_pnstar].xmin;j<=(int)centro[m_pnstar].xmax;j++)
										bmap[i*m_nrow+j]=0;*/
										m_pnstar=m_pnstar+1;
							}
							else
								//cout<<"FAILED"<<endl;
							out<<cen.xmin+1<<" "<<cen.ymin+1<<endl;
						}
						
					}
				}
			}
		}
	out.close();	

	double dx=0,dy=0,dy1;
	//Scan lista di oggetti per pulizia	
	for(int jj=0;jj<m_pnstar-1;jj++)
	  {
		for(i=jj+1;i<m_pnstar;i++)
		  {
			if(centro[i].flag)
			  {
		dx=centro[i].x-centro[jj].x;
		double dy2=centro[i].y-centro[jj].y;
		//cout<<"dy2 "<<dy2<<endl;
		dy=centro[i].ymin-centro[jj].ymax;
		dy1=centro[i].ymax-centro[jj].ymin;

		if(Overlap(centro[jj],centro[i])&&(centro[i].flag)&&(centro[jj].flag))
//		   if(1<0)
{		
                        cout<<jj<<" O "<<centro[jj].ymax<<" "<<centro[jj].ymin<<endl;
			centro[jj].xmin=amin1(centro[i].xmin,centro[jj].xmin);
			centro[jj].xmax=amax1(centro[i].xmax,centro[jj].xmax);
			centro[jj].ymax=amax1(centro[i].ymax,centro[jj].ymax);
			centro[jj].ymin=amin1(centro[i].ymin,centro[jj].ymin);
			//cout<<"AA1 "<<centro[i+1].ymax<<" "<<centro[i+1].ymin<<endl;
 	//cout<<jj<<" N "<<centro[jj].ymax<<" "<<centro[jj].ymin<<endl;
  //cout<<centro[i+1].id<<"-"<<centro[i].id<<" dx="<<dx<<" "<<centro[i+1].ymin-centro[i].ymax<<endl;
  		cout<<"elimino star"<<centro[i].id<<endl;
			centro[i].flag=0;
			StarCenter(centro[jj]);
}

			}
	         }
//out<<centro[jj].id<<" "<<centro[jj].xmin+1<<" "<<centro[jj].ymin+1<<" flag: "<<centro[jj].flag<<endl;
	}
	//Fine scan lista di oggetti

//Scan per estensione
	for( int jj1=0;jj1<m_pnstar-1;jj1++)
	{
	    if(centro[jj1].flag)
		CheckBorder(centro[jj1],bmap);
	    //StarCenter(centro[jj]);
	}
//END scan per estensione


/*	int jj;
	for(jj=0;jj<m_pnstar;jj++){
		//if(fabs(centro[jj].x-centro[jj].xpeak)>3.){
		//getchar();
		//cout<<"star "<< centro[jj].id<<endl;
		if(centro[jj].id==53)
			Deblend(centro[jj], 30);	
		
		//}
	}
*/	

	SaveFitsImage(bmap,"seg2.fits");
	
	delete []bmap;
	return 0;
}

int MyImageAnalisys::FindStobieBackground(int w, int h)
{
		fImage bg,mbg;
		vector<float> sky1,sky2;
		m_backbuffer=new float[m_nrow*m_ncol];
		m_backsigbuffer=new float[m_nrow*m_ncol];
		SKY smod;
		int i,j,ii,jj,k=0;
		while(m_ncol%w)
			w--;
		while(m_nrow%h)
			h--;
		int nfield=(m_ncol/w)*(m_nrow/h);
		bg.Resize(1,nfield+1);
		mbg.Resize(1,nfield+1);
		
		//cout<<w<<" "<<h<<" "<<nfield<<endl;
		for(i=0;i<m_nrow;i+=h){
			for(j=0;j<m_ncol;j+=w){
				//bg.FillPix(0.0);
				sky1.clear();
				for(ii=0;ii<h;ii+=16){
					for(jj=0;jj<w;jj+=16){
						double val=GetPix(j+jj,i+ii);
						if(val>0.)
						sky1.push_back(GetPix(j+jj,i+ii));
						//cout<<bg(ii,jj)<<" ";
					}
				}
				sort(sky1.begin(),sky1.end(),less<float>());
				//cout<<"lbkg"<<endl;
				if(!BackgroundStat(sky1,smod)){
				for(ii=0;ii<h;ii++){
					for(jj=0;jj<w;jj++){
					*(m_backbuffer+(i+ii)*m_ncol+(j+jj))=smod.skymod;
					*(m_backsigbuffer+(i+ii)*m_ncol+(j+jj))=smod.skysig;
												
					}
				}
				//cout<<"lbkg max="<<smod.skymax<<endl;
				//cout<<"lbkg min="<<smod.skymin<<endl;
				//bg.InitStat();
				//bg.Histogram(0.,65535.,255);
				//cout<<"Nfiled="<<k<<endl;
				//cout<<"xstart="<<j<<"xstop"<<j+w-1<<endl;
				//cout<<"ystart="<<i<<"ystop"<<i+h-1<<endl;
				//cout<<"Nfiled="<<k<<" "<<3.*bg.GetMedian()-2.*bg.GetMean()<<" "<<bg.GetStdev()<<endl;
				//mbg(0,k)=3.*bg.GetMedian()-2.*bg.GetMean();
				//cout<<"lbkg max="<<smod.skymax<<endl;
				//cout<<"lbkg min="<<smod.skymin<<endl;
				//cout<<"Npix="<<values.size()<<endl;
				//cout<<"mode="<<smod.skymn<<"stddev="<<smod.skymed<<endl;
				//mbg(0,k)=smod.skymod;
				//bg(0,k)=smod.skysig;
				//sky2.push_back(smod.skymod);
				//k++;
				}
			}
		}
		//mbg.InitStat();
		//bg.InitStat();
	/*	sort(sky2.begin(),sky2.end(),less<float>());
				if(!BackgroundStat(sky2,smod)){
					cout<<"final mode="<<smod.skymod<<"stddev="<<smod.skysig<<endl;

				}*/
		//cout<<"BG = "<<2.5*mbg.GetMedian()-1.5*mbg.GetMean()<<" "<<mbg.GetMean()<<" "<<mbg.GetStdev()<<endl;
		//cout<<"BG Sig= "<<2.5*bg.GetMedian()-1.5*bg.GetMean()<<" "<<bg.GetMean()<<" "<<bg.GetStdev()<<endl;

	return (0);
}

int MyImageAnalisys::Deblend(CENTRO cen, int nthre)
{
	int iix=0,iiy=0,k=0,ix=0,iy=0,kx,ky;
	//cout<<"middle="<<middle<<" med="<<median<<endl;
	int w=int(cen.xmax-cen.xmin+0.5);
	int h=int(cen.ymax-cen.ymin+0.5);

	vector<unsigned short> b(w*h);
	double thresh=cen.bkgsig;
	double threshstep=((GetPix(int(cen.x+0.5000),int(cen.y+0.5000)))-(cen.bkg))/nthre;
	int ymin=(int)(cen.ymin+0.5);
	int ymax=(int)(cen.ymax+0.5);
	int xmin=(int)(cen.xmin+0.5);
	int xmax=(int)(cen.xmax+0.5);
		
	double *xc=new double[nthre+1];
	//double xp,yp;
	//cout<<"thresh="<<thresh<<endl;
	cout<<"star: "<<cen.id<<endl;
	for(int i=0;i<nthre;i++){
		thresh=(cen.bkg)+(i+1)*threshstep;
		k=0;
		double sumI=0.;
		double sumIX=0.;
		double sumIY=0.;
		double area=0;
		double val;
	  for(iiy=ymin;iiy<cen.ymax;iiy++){
		for(iix=xmin;iix<xmax;iix++){
			if((val=(GetPix(iix,iiy)))>=thresh){
				b[k]=1;
			   //sumI+=GetPix(iix,iiy)-GetBack(iix,iiy);
			   //area++;
				
			   
			}
			else
				b[k]=0;
			//cout<<"medianval="<<GetPix(ix,iy)<<endl;
			k++;
		}
		
	  }
	  //cout<<"K="<<i<<endl;
	  //cout<<"area="<<area<<" Somma="<<sumI<<endl;
	  for(iy=1;iy<h-1;iy++){
		double sumI=0.;
		double sumIX=0.;
		double sumIY=0.;
		double area=0;
		double val=0.;
		int temp=0;
		for(ix=1;ix<w-1;ix++){
			if(b[iy*w+ix]==1){
				for(ky=iy-1;ky<=iy+1 && ky<h;ky++){
					for(kx=ix-1;kx<=ix+1;kx++){
							temp+=(b[ky*w+kx]);
					}
				}
			}
			if(temp==9){
				
				if(PeakRange(cen,(i+1)*threshstep)){
				//cout<<i<<" "<<cen.xmin<<","<<cen.ymin<<";; "<<endl;
			    //cout<<cen.x<<","<<cen.y<<endl;
				}
			}
			
			
		}
		
	  }
	}
	
	return 0;
}

int MyImageAnalisys::CleanDetectedObj()
{
	double dx=0,dy=0,dy1;
	int jj=0,i=0;
	for(jj=0;jj<m_pnstar-1;jj++){
		for(i=jj+1;i<m_pnstar;i++){
				if(centro[i].flag>0 && centro[jj].flag>0){
				if(centro[jj].ymin<=centro[i].ymin &&
				   centro[jj].ymax>=centro[i].ymax &&
				   centro[jj].xmin<=centro[i].xmin &&
				   centro[jj].xmax>=centro[i].xmax){
				   centro[i].flag=0;
     	   		   //cout<<"1-elimino star"<<centro[i].id<<endl;

				   i++;//continue;
				}

		dx=centro[i].x-centro[jj].x;
		double dy2=centro[i].y-centro[jj].y;
		//cout<<"dy2 "<<dy2<<endl;
		dy=centro[i].ymin-centro[jj].ymax;
		dy1=centro[i].ymax-centro[jj].ymin;
		//cout<<"i="<<i<<" "<<centro[i].id<<"-"<<" dx="<<dx<<" "<<dy<<" " <<dy1<<endl;
		if(((centro[jj].ymin<centro[i].ymin &&centro[jj].ymax>centro[i].ymax &&
			centro[jj].xmin<centro[i].xmin &&centro[jj].xmax>centro[i].xmax) ||
			(centro[jj].ymin>centro[i].ymin && centro[jj].ymax<centro[i].ymax && fabs(dx)<=3.)||
			(centro[jj].ymin<centro[i].ymin && centro[jj].ymax<centro[i].ymax && fabs(dx)<=3.)
			)&&((((dy)<=4.)|| ((dy1)<=4.)||(fabs(dy2)<=4.)) && (fabs(dx)<=3.))){


		
			//cout<<jj<<" O "<<centro[jj].ymax<<" "<<centro[jj].ymin<<endl;
			centro[jj].xmin=amin1(centro[i].xmin,centro[jj].xmin);
			centro[jj].xmax=amax1(centro[i].xmax,centro[jj].xmax);
			centro[jj].ymax=amax1(centro[i].ymax,centro[jj].ymax);
			centro[jj].ymin=amin1(centro[i].ymin,centro[jj].ymin);
			//cout<<"AA1 "<<centro[i+1].ymax<<" "<<centro[i+1].ymin<<endl;
			//cout<<jj<<" N "<<centro[jj].ymax<<" "<<centro[jj].ymin<<endl;
			//j=i+1;
			//cout<<centro[i+1].id<<"-"<<centro[i].id<<" dx="<<dx<<" "<<centro[i+1].ymin-centro[i].ymax<<endl;
			//cout<<"2-elimino star"<<centro[i].id<<endl;
			centro[i].flag=0;
			StarCenter(centro[jj]);
			//while(j<m_pnstar){
				//swap(centro[i],centro[ns-1]);

			//	j++;
			//}
			//cout<<centro[i+1].id<<"-"<<centro[i].id<<" dx="<<dx<<" "<<centro[i+1].ymin-centro[i].ymax<<endl;
			//m_pnstar--;
			//SortStarList(centro,m_pnstar);
			//}
		}
			}
	}
	///	cout<<"BB"<<endl;
	}
	return 0;
}

int MyImageAnalisys::Overlap(center cen, center cend)
{   
      /*Returning 1 if one object box has at least two points in the other object area*/
      /*An "overarea" is added to associate objects separated by less than two pixels*/
      /*Puo' essere migliorato. Se questa funzione torna 1 allora i due oggetti sono accorpati.*/
  const float vicino=3.;

  float xul,xuu,yul,yuu;
  float xdl,xdu,ydl,ydu;
  float xu,xd,yu,yd;
  int ch=0, chd=0;
  const int o=2.;             //Overarea
  xul=cen.xmin; xuu=cen.xmax; yul=cen.ymin; yuu=cen.ymax;
  xdl=cend.xmin; xdu=cend.xmax; ydl=cend.ymin; ydu=cend.ymax;
  xu=0.5*(cen.xmin+cen.xmax);
  yu=0.5*(cen.ymin+cen.ymax);
  xd=0.5*(cend.xmin+cend.xmax);
  yd=0.5*(cend.ymin+cend.ymax);
  
  if(xul>=(xdl-o) && xul<=(xdu+o) && yul>=(ydl-o) && yul<=(ydu+o)) ch++;
  if(xuu>=(xdl-o) && xuu<=(xdu+o) && yul>=(ydl-o) && yul<=(ydu+o)) ch++;
  if(xul>=(xdl-o) && xul<=(xdu+o) && yuu>=(ydl-o) && yuu<=(ydu+o)) ch++;
  if(xuu>=(xdl-o) && xuu<=(xdu+o) && yuu>=(ydl-o) && yuu<=(ydu+o)) ch++;

  if(ch>3) return 1;    //If at least two points are in the other box

  if(xdl>=(xul-o) && xdl<=(xuu+o) && ydl>=(yul-o) && ydl<=(yuu+o)) chd++;
  if(xdu>=(xul-o) && xdu<=(xuu+o) && ydl>=(yul-o) && ydl<=(yuu+o)) chd++;
  if(xdl>=(xul-o) && xdl<=(xuu+o) && ydu>=(yul-o) && ydu<=(yuu+o)) chd++;
  if(xdu>=(xul-o) && xdu<=(xuu+o) && ydu>=(yul-o) && ydu<=(yuu+o)) chd++;

  if(chd>3) return 1;   //If at least two points are in the other box

  /*Controlli aggiuntivi*/ 
  /*Esempio: se almeno un punto appartiene all'altro box controllare area, x e y*/
  if((fabs(xu-xd)<vicino) && (fabs(yu-yd)<vicino)) return 1; //centri vicini
  
  if((ch>1||chd>1)&&(fabs(xu-xd)<=3)) return 1;

  //if( ) return 1; //Aggiungere, in caso, altri controlli
  return 0;
}


void MyImageAnalisys::CheckBorder(center &cen, int* bmap)
{
  const float sigm=2.5;
  const float xlim=2.;
  const float ylim=10.;    //inutilizzato
  const float around_ypeak=3;

//float mean_bkg=(GetBack(cen.xmin,cen.ymin)+GetBack(cen.xmin,cen.ymax)+GetBack(cen.xmax,cen.ymin)+GetBack(cen.xmax,cen.ymax))/4.;
//float mean_bkgsig=(GetBackSig(cen.xmin,cen.ymin)+GetBackSig(cen.xmin,cen.ymax)+GetBackSig(cen.xmax,cen.ymin)+GetBackSig(cen.xmax,cen.ymax))/4.;

  int ovrl=0;
 int yyy=cen.ymin; int xxx=(cen.xmin)-1; //Scan of the object left side
 int vai=1;
 int num_up=0;
 while((yyy<m_nrow)&&(xxx<m_ncol)&&(xxx>0)&&(yyy>0)&&(vai))
   {
     for(yyy=cen.ymin; yyy<=cen.ymax && yyy<m_nrow ;yyy++)
       { 
	 if(  (GetPix(xxx,yyy)-GetBack(xxx,yyy)) > (sigm*GetBackSig(xxx,yyy)) ) 
  if((bmap[yyy*m_ncol+xxx]!=5)&&(bmap[yyy*m_ncol+xxx]!=8)&&(bmap[yyy*m_ncol+xxx]!=2)) 
{num_up++;bmap[yyy*m_ncol+xxx]=6;};
  if(bmap[yyy*m_ncol+xxx]==8) 
      ovrl++;
       }
if((((float)num_up*xlim)<(cen.ymax-cen.ymin))||(num_up<=1)) vai=0;//Less than (1/xlim) over "sigm" sigma
// cout<<"xx "<<xxx<<endl; 
     xxx--; num_up=0;
   }
cen.xmin=xxx+1; vai=1;

 yyy=cen.ymin; xxx=(cen.xmax)+1; //Scan of the object right side
 vai=1;
 num_up=0;
 while((yyy<m_nrow)&&(xxx<m_ncol)&&(xxx>0)&&(yyy>0)&&(vai))
   {
     for(yyy=cen.ymin; yyy<=cen.ymax && yyy<m_nrow ;yyy++)
       {
	 if(  (GetPix(xxx,yyy)-GetBack(xxx,yyy)) > (sigm*GetBackSig(xxx,yyy)) ) 
  if((bmap[yyy*m_ncol+xxx]!=5)&&(bmap[yyy*m_ncol+xxx]!=8)&&(bmap[yyy*m_ncol+xxx]!=2)) 
{num_up++;bmap[yyy*m_ncol+xxx]=6;};
       }
if((((float)num_up*xlim)<(cen.ymax-cen.ymin))||(num_up<=1)) vai=0;//Less than (1/xlim) over "sigm" sigma
     xxx++; num_up=0;
   }
cen.xmax=xxx-1; vai=1;

 yyy=1+(cen.ymax); xxx=cen.xmin; //Scan of the object top
 vai=1;
 num_up=0;
  while((yyy<m_nrow)&&(xxx<m_ncol)&&(xxx>0)&&(yyy>0)&&(vai))
   {
//     for(xxx=cen.xmin; xxx<=cen.xmax && xxx<m_ncol;xxx++)
     for(xxx=(cen.xpeak-around_ypeak); xxx<=(cen.xpeak+around_ypeak) && xxx<m_ncol;xxx++)
       {
	 if(  (GetPix(xxx,yyy)-GetBack(xxx,yyy)) > (sigm*GetBackSig(xxx,yyy)) ) 
  if((bmap[yyy*m_ncol+xxx]!=5)&&(bmap[yyy*m_ncol+xxx]!=8)&&(bmap[yyy*m_ncol+xxx]!=2)) 
  {num_up++;bmap[yyy*m_ncol+xxx]=6;};
       }
//if((((float)num_up*ylim)<(cen.xmax-cen.xmin))||(num_up<=1)) vai=0;
   if(num_up<1) vai=0;
      yyy++; 
       num_up=0;
  }
 cen.ymax=yyy-1; num_up=0; vai=1;

 yyy=(cen.ymin)-1; xxx=cen.xmin; //Scan of the object bottom
 vai=1;
 num_up=0;
 while((yyy<m_nrow)&&(xxx<m_ncol)&&(xxx>0)&&(yyy>0)&&(vai))
   {
//     for(xxx=cen.xmin; xxx<=cen.xmax && xxx<m_ncol;xxx++)
     for(xxx=(cen.xpeak-around_ypeak); xxx<=(cen.xpeak+around_ypeak)&& xxx<m_ncol;xxx++)
       {
	 if(  (GetPix(xxx,yyy)-GetBack(xxx,yyy)) > (sigm*GetBackSig(xxx,yyy)) ) 
 if((bmap[yyy*m_ncol+xxx]!=5)&&(bmap[yyy*m_ncol+xxx]!=8)&&(bmap[yyy*m_ncol+xxx]!=2))  
{num_up++;bmap[yyy*m_ncol+xxx]=6;};
       }
//if((((float)num_up*ylim)<(cen.xmax-cen.xmin))||(num_up<=1)) vai=0;//Less than (1/ylim) over "sigm" sigma
if(num_up<1) vai=0;//Less than (1/ylim) over "sigm" sigm
     yyy--; num_up=0;
   }
 cen.ymin=yyy+1; vai=1;

// cout<<" ID:"<<cen.id<<endl;
} 


int MyImageAnalisys::WriteSpectra(center cen)
{
    char nfil[40];
    ofstream fil;
    int k=0;
    double cnt_tot=0;
	sprintf(nfil,"./spettri/sp%04d.dat",cen.id);
	fil.open(nfil);
	if(fil.good())
	{
	for(int i=cen.y-50;i<=cen.y+50&&i>0&&i<m_nrow;i++,k++)
	{
	    float tot=0;
      
	    for(int j=cen.x-int((cen.dx)/2); j<=cen.x+int((cen.dx)/2)&&j>0&&j<m_ncol;j++)
		tot+=(GetPix(j,i)-GetBack(j,i));
	    fil<<(k)<<"\t"<<tot<<endl;
	    cnt_tot+=tot;
	}
	//cout<<cen.id<<": "<<-2.5*log10(cnt_tot)+25.<<endl;
	} else return 0;
	fil.close();
	return 1;
}


int MyImageAnalisys::SkyVectorStat(vector<float>& sky1,SKY &smod)
{
	
	if (sky1.size() > 0) {
		sort(sky1.begin(),sky1.end(),less<float>());
		if(!BackgroundStat(sky1,smod)){
		
		if(fabs(smod.skymod)>59000.)
			return 0;
		//cen.bkg=smod.skymod;
		//cen.bkgsig=smod.skysig;
		//cen.nbkgpix=smod.skynpx;
		return (1);
		}else{
			return (0);
		}
	}
	return 0;
}

int MyImageAnalisys::ApPhot()
{
	
	double	costante,apmag[15],area[15],error[4];

	float	magerr[15],par[15],
			FWHM, sigma,
			//skymod, skysig, skyskw, sigsq,
			skyvar,	datum, r, rsq, fractn, edge, dum,
			xc, yc, apmxsq, round,
			rinsq, rout, routsq, dysq, edge1, edge2;
	int		i, j, k, naper, lx, ly,
			istar, mx, my, nsky;
	
//	size_t	numb, larg=sizeof(s);
	nstar=m_pnstar;
	SKY smod;
	vector<float> s;

	par[1]= m_par01;		//	radius of aperture 1
	par[2]= m_par02;	//	radius of aperture 2
	par[3]= m_par03;		//	radius of aperture 3
	par[4]= m_par04;		//	radius of aperture 4
	par[5]= m_par05;		//	radius of aperture 5
	par[6]= m_par06;		//	radius of aperture 6
	par[7]= m_par07;		//	radius of aperture 7
	par[8]= m_par08;		//	radius of aperture 8
	par[9]= m_par09;		//	radius of aperture 9
	par[10]= m_par10;		//	radius of aperture 10
	par[11]= m_par11;		//	radius of aperture 11
	par[12]= m_par12;		//	radius of aperture 12
	par[13]= m_par13;		//	inner sky radius
	par[14]= m_par14;		//	outer sky radius

	double lobad=m_lobad;
	double hibad=m_opt03;
	costante = 2.5*log10((double)m_expt);

	naper=m_maxap;
	apmxsq=-1.0;
	for(i=1;i<=int(m_maxap);i++)
	{
		 if (par[i]<=0.0){
				naper=i-1;
				break;
		 }
		 apmxsq=amax1(apmxsq,((par[i]+0.5)*(par[i]+0.5)));
	}
	rinsq=amax1(par[m_maxap+1],0.0);
	rinsq=rinsq*rinsq;
	routsq = ((float)m_maxsky)/3.1415 + rinsq;
	dum = par[m_maxap+2]*par[m_maxap+2];
	if (dum>routsq)
	{
		round=(float)sqrt((double)routsq);
		return(1);
	}
	else
	{
		if (dum<=rinsq)
		{
			return(2);
		}
		else
		{
			rout = par[m_maxap+2];
			routsq = dum;
		}
	}
	double phpadu = m_opt01;
	double readns=m_opt00;
	readns=readns*readns;
	
	for (istar=1;istar<=nstar;istar++){
		xc=centro[istar-1].x;
		yc=centro[istar-1].y;
		//cout<<"Centro"<<xc<<" "<<yc<<endl;
		if((xc>m_ncol)||(yc>m_nrow)||(xc<0.0)||(yc<0.0))
		{
			//stella[istar-1].apmag=99.99;
			//stella[istar-1].magerr=9.99;
			continue;
		}
		lx = amax1(0,(int)(xc-rout));
		mx = amin1(m_ncol,(int)(xc+rout));
		ly = amax1(0,(int)(yc-rout));
		my = amin1(m_nrow,(int)(yc+rout));
		edge1=amin1(float(xc-0.5), ((float)m_ncol+0.5)-xc);
		edge2=amin1(float(yc-0.5), ((float)m_nrow+0.5)-yc);
		edge=amin1(edge1,edge2);
		
		for(i=1;i<=naper;i++)
		{
			 apmag[i] = 0.0;
			 if (edge<par[i]){
				apmag[i]=(-1.0E36);
			 }
			 area[i]=0.0;
		}
		nsky=0;
		j=ly;
		//cout<<"lx,ly "<<lx<<" "<<ly<<endl;
		do
		{
			dysq=((float)j-yc)*((float)j-yc);
			i=lx;
			do
			{
				rsq=dysq+((float)i-xc)*((float)i-xc);
				datum=GetPix(j,i);
				if((rsq<rinsq)||(rsq>routsq)||(nsky>int(m_maxsky))||(datum<lobad)||(datum>hibad)||rsq>apmxsq) {
						i=i+1;
						continue;
				}else{
					nsky=nsky+1;
					s.push_back(datum);
				 }
				
				r=(float)sqrt((double)rsq)-0.5;
				k=1;
				do{
					 if (r>par[k]){
						 k=k+1;
						 continue;
					 }
					 fractn=amin1(1.0,(par[k]-r));
					 fractn=amax1(0.0,fractn);
					 if ((datum<lobad)||(datum>hibad)){
						apmag[k]=(double)(-1.0E36);
						k++;
						continue;
					 }
						apmag[k] = apmag[k]+(double)(fractn*datum);
						area[k] = area[k]+(double)fractn;
						k++;
						
				} while(k<=naper);
				i++;
			 }while (i<=mx);
			 j=j+1;
		}while (j<=my);

		if (nsky<int(m_minsky))
		{
			return(3);
		}
		if(!SkyVectorStat(s,smod)){
			return(4);
		}
	
		skyvar=smod.skysig*smod.skysig;
	    //cout<<"datun "<<smod.skymod<<endl;
		double sigsq=skyvar/(double)(nsky);
		cout<<"skyvar= "<<skyvar<<endl;
		for(i=1;i<=naper;i++)
		{
			 if (smod.skysig<0.){
				apmag[i]=99.99F;
				magerr[i]=9.99F;
				FWHM=99.99F;
				sigma=9.99F;
				continue;
			 }
			 apmag[i]=apmag[i]-(double)(smod.skymod)*area[i];
			 //cout<<"naper="<<i<<"area "<<area[i]<<"  apmag[i]="<<apmag[i]<<endl;
			 if (apmag[i]<=0.0){
				apmag[i]=99.99F;
				magerr[i]=9.99F;
				FWHM=99.99F;
				sigma=9.99F;
				continue;
			 }
			 error[1]=area[i]*(double)skyvar;
			 error[2]=apmag[i]/(double)phpadu;
			 //cout<<"err1= "<<error[1]<<endl;
			 //cout<<"err2= "<<error[2]<<endl;
			 //error[3]=(double)sigsq*(area[i]*area[i]);

			 magerr[i]=(float)(sqrt(error[1]+error[2]/*+error[3]*/)/apmag[i]);
			 //cout<<"magerr= "<<magerr[i]<<endl;
			 magerr[i]=amin1(9.99F,1.0857*magerr[i]);
			 apmag[i]=costante-2.5*log10(apmag[i]);
			 if (apmag[i] > (double)99.99){
				apmag[i]=99.99F;
				magerr[i]=9.99F;
				FWHM=99.99F;
				sigma=9.99F;
			 }
			 //seeing(xc,yc,skymod,&FWHM,&sigma);
		}

		stella[istar-1].apmag=apmag[5];
		stella[istar-1].magerr=magerr[5];
		//cout<<istar-1<<" "<<stella[istar-1].apmag<<" "<<stella[istar-1].magerr<<endl;

	}
	return(0);

} 

int MyImageAnalisys::SegImage(int box)
{
	int iy=0,ix=0,kx=0,ky=0,i=0,j=0,kky=0;
	CENTRO cen;
	int nipx=0;
	double val=0;
	 m_pnstar=0;
	int end=box;
	float minval=0.;
	//int *bmap=new int[m_ncol*m_nrow];
	string outfile=m_detectfile+".reg";
	ofstream out(outfile.c_str()); 
	//float tresh1=2167.91+m_opt05*14.524;
	/*float*mybuf=new float[m_ncol*m_nrow];
	float xmi=GetPix(ix,iy),xma=GetPix(ix,iy);
	for(iy=0;iy<m_nrow;iy++){
			for(ix=0;ix<m_ncol;ix++){
				mybuf[iy*m_ncol+ix]=GetPix(ix,iy);
			}
	}*/
	double siggg=2.0;
	int tot=m_minumpx;
	//minval=xmi*1.1;

			//cout<<"Filtro"<<endl;
		//SaveFitsImage(mybuf,"seg1.fits");
		/*gaussian_filter(mybuf, m_imabuffer,m_ncol,m_nrow,siggg);
		SaveFitsImage("seg0.fits");*/
		//minval=10.;
		//tot=14;
	
	/*for(iy=end;iy<m_nrow-end;iy++){
			for(ix=end;ix<m_ncol-end;ix++){
				//tresh1=m_opt05*GetBackSig(ix,iy);
				if(GetPix(ix,iy)<minval){
				  bmap[iy*m_ncol+ix]=0;
				}else{
				  bmap[iy*m_ncol+ix]=1;
				}
			}
		}
		SaveFitsImage(bmap,"seg1.fits");*/
		for(iy=end;iy<m_nrow-end;iy++){
			for(ix=end;ix<m_ncol-end;ix++){
				int temp=0,xx=0,yy=0;
				double max=0.;
				//if(bmap[iy*m_ncol+ix]==1){
					if(GetPix(ix,iy)>minval){
						cen.xmin=ix;
						cen.xmax=ix;
						cen.ymin=iy;
						cen.ymax=iy;
					kx=ix+1; ky=iy;
//					while(kx<m_ncol && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
					while(kx<ix+box && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
						cen.xmax=kx;
						val=GetPix(kx,ky);
							if(val>max){
								max=val;
								xx=kx;
								yy=ky;
							}
						kx++;
						temp++;
					}
					kx=ix-1; ky=iy;
		//			while(kx>0 && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
					while(kx>ix-box && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){

						cen.xmin=kx;
						val=GetPix(kx,ky);
							if(val>max){
								max=val;
								xx=kx;
								yy=ky;
							}
						kx--;
						temp++;
					}
					kx=ix; ky=iy+1;
//					while(ky<m_nrow && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
					while(ky<iy+box && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
						cen.ymax=ky;
						val=GetPix(kx,ky);
							if(val>max){
								max=val;
								xx=kx;
								yy=ky;
							}
						ky++;
						temp++;
					}
					kx=ix; ky=iy-1;
	//				while(ky>0 && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
					while(ky>iy-box && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){	
					cen.ymin=ky;
						val=GetPix(kx,ky);
							if(val>max){
								max=val;
								xx=kx;
								yy=ky;
							}
						ky--;
						temp++;
					}
					kx=ix+1; ky=iy+1;
//					while(ky<m_nrow && kx<m_ncol && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
					while(kx<ix+box && ky<iy+box && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
						if(ky>cen.ymax)
							cen.ymax=ky;
						if(kx>cen.xmax)
							cen.xmax=kx;
						val=GetPix(kx,ky);
							if(val>max){
								max=val;
								xx=kx;
								yy=ky;
							}
						ky++;
						kx++;
						temp++;
					}
					kx=ix-1; ky=iy-1;
//					while(ky>=0 && kx>=0 && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
					while(kx<ix-box && ky<iy-box && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
						if(ky<cen.ymin)
							cen.ymin=ky;
						if(kx<cen.xmin)
							cen.xmin=kx;
						val=GetPix(kx,ky);
							if(val>max){
								max=val;
								xx=kx;
								yy=ky;
							}
						ky--;
						kx--;
						temp++;
					}
					kx=ix+1; ky=iy-1;
//					while(ky>=0 && kx<m_ncol && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
					while(kx<ix+box && ky<iy-box && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
					if(ky<cen.ymin)
							cen.ymin=ky;
						if(kx>cen.xmax)
							cen.xmax=kx;
						val=GetPix(kx,ky);
							if(val>max){
								max=val;
								xx=kx;
								yy=ky;
							}
						ky--;
						kx++;
						temp++;
					}
					kx=ix-1; ky=iy+1;
//					while(ky<m_nrow && kx>=0 && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){
					while(kx<ix-box && ky<iy+box && GetPix(kx,ky)>minval){//bmap[ky*m_ncol+kx]==1){

					if(ky>cen.ymax)
							cen.ymax=ky;
						if(kx<cen.xmin)
							cen.xmin=kx;
						val=GetPix(kx,ky);
							if(val>max){
								max=val;
								xx=kx;
								yy=ky;
							}
						ky++;
						kx--;
						temp++;
					}
				
					if(temp>=tot){
						
						cen.xpeak=xx;
						cen.ypeak=yy;
						nipx=temp;
						ky=yy;kx=xx;
							int dx=int((cen.xmax-cen.xmin)/2+1);
							int	dy=int((cen.ymax-cen.ymin)/2+1);
							double rat=dx/double(dy);
							if(rat>=1.1 || rat<=.9){
								dx=box;
								dy=box;
								continue;
							}
							/*else
								dy=dx;

						    if (dx>box){
								dx=box;
								dy=box;
							}*/

							/*centro[m_pnstar].xmin=cen.xmin;
							centro[m_pnstar].xmax=cen.xmax;
							centro[m_pnstar].ymin=cen.ymin;
							centro[m_pnstar].ymax=cen.ymax;*/
							centro[m_pnstar].xmin=cen.xpeak-dx;//-1;
							centro[m_pnstar].xmax=cen.xpeak+dx;//+1;
							centro[m_pnstar].ymin=cen.ypeak-dy;//-1;
							centro[m_pnstar].ymax=cen.ypeak+dy;//+1;
						//if(nipx>=m_minumpx){				     
							if(centro[m_pnstar].xmin<=0) centro[m_pnstar].xmin=0;
							if(centro[m_pnstar].xmax>=m_ncol) centro[m_pnstar].xmax=m_ncol-1;
							if(centro[m_pnstar].ymin<=0) centro[m_pnstar].ymin=0;
							if(centro[m_pnstar].ymax>=m_nrow) centro[m_pnstar].ymax=m_nrow-1;
							//cout<<cen.xpeak<<" "<<cen.ypeak<<"-- "<<dx<<" "<<dy<<endl;
							for(i=(int)(centro[m_pnstar].ymin);i<=(int)(centro[m_pnstar].ymax);i++){
								for(j=(int)(centro[m_pnstar].xmin);j<=(int)(centro[m_pnstar].xmax);j++){
										//bmap[i*m_ncol+j]=0;
										if(GetPix(j,i)>max){
											max=GetPix(j,i);
											cen.peak=max;
											cen.xpeak=j;
											cen.ypeak=i;
										}
										SetPix(j,i,0.);
								}
							}
							centro[m_pnstar].peak=cen.peak;
							centro[m_pnstar].xpeak=cen.xpeak;
							centro[m_pnstar].ypeak=cen.ypeak;
							
							centro[m_pnstar].id=m_pnstar;
							centro[m_pnstar].flag=1;
							centro[m_pnstar].bkg=0.;//(GetBack(cen.xmin,cen.ymin)+GetBack(cen.xmin,cen.ymax)+ GetBack(cen.xmax,cen.ymin)+GetBack(cen.xmax,cen.ymax))/4.;
							centro[m_pnstar].bkgsig=-1;//(GetBackSig(cen.xmin,cen.ymin)+GetBackSig(cen.xmin,cen.ymax)+GetBackSig(cen.xmax,cen.ymin)+GetBackSig(cen.xmax,cen.ymax))/4.;
							//out<<"image;box("<<centro[m_pnstar].xpeak+1<<","<<centro[m_pnstar].ypeak+1<<","<<2*dx<<","<<2*dy<<")"<<endl;
				
							m_pnstar=m_pnstar+1;
						//}
						
					}
				}
			}
		}
			
	
/*	double dx=0,dy=0,dy1;
	int jj=0;
	//Scan lista di oggetti per pulizia	
	for(jj=0;jj<m_pnstar-1;jj++){
		for(i=jj+1;i<m_pnstar;i++){
			if(centro[i].flag){
				//dx=centro[i].x-centro[jj].x;
				//double dy2=centro[i].xmax-centro[jj].xmin;
				//cout<<"dy2 "<<dy2<<endl;
				//dy=centro[i].ymin-centro[jj].ymax;
				//dy1=centro[i].ymax-centro[jj].ymin;

				if(Overlap(centro[jj],centro[i])&&(centro[i].flag)&&(centro[jj].flag)){		
					centro[jj].xmin=amin1(centro[i].xmin,centro[jj].xmin);
					centro[jj].xmax=amax1(centro[i].xmax,centro[jj].xmax);
					centro[jj].ymax=amax1(centro[i].ymax,centro[jj].ymax);
					centro[jj].ymin=amin1(centro[i].ymin,centro[jj].ymin);
			
 				  //cout<<jj<<" N "<<centro[jj].ymax<<" "<<centro[jj].ymin<<endl;
				  //cout<<centro[i+1].id<<"-"<<centro[i].id<<" dx="<<dx<<" "<<centro[i+1].ymin-centro[i].ymax<<endl;
  				  //cout<<"elimino star"<<centro[i].id<<endl;
					centro[i].flag=0;
				}

			}
	    }
	}
*/
	for(i=0;i<m_pnstar;i++){
		if(centro[i].flag){
					float dx=centro[i].xmax-centro[i].xmin;
					float dy=centro[i].ymax-centro[i].ymin;
					//out<<"image;box("<<centro[i].xpeak+1<<","<<centro[i].ypeak+1<<","<<dx<<","<<dy<<")"<<char(10);
					out<<"image;point("<<centro[i].xpeak+1.<<","<<centro[i].ypeak+1.<<") # point=cross color=red"<<char(10);

		}

	}
	out.close();
	//SaveFitsImage("seg2.fits");
	//delete []bmap;
	return 0;
}

int MyImageAnalisys::SegImage()
{
	int iy=0,ix=0,kx=0,ky=0,i=0,j=0,kky=0;
	CENTRO cen;
	int box = 6;
	int nipx=0 ;//box*box;
		double val=0;
		m_pnstar=0;
		int end=box/2;
		int tot=m_minumpx;
		int *bmap=new int[m_ncol*m_nrow];
		//ofstream out("pippo.reg");
		//float tresh1=2167.91+m_opt05*14.524;
		string outfile=m_detectfile+".reg";
		ofstream out(outfile.c_str()); 
		for(iy=end;iy<m_nrow-end;iy++){
			for(ix=end;ix<m_ncol-end;ix++){
				//tresh1=m_opt05*GetBackSig(ix,iy);
				if(GetPix(ix,iy)<=0.){
					bmap[iy*m_ncol+ix]=0;
				}else{
					bmap[iy*m_ncol+ix]=1;
				}
			}
		}
		
		for(iy=end;iy<m_nrow-end;iy++){
			for(ix=end;ix<m_ncol-end;ix++){
					int temp=0,xx=0,yy=0;
					double max=-1e30;
				if(bmap[iy*m_ncol+ix]==1){
					
					cen.xmin=ix;
					cen.xmax=ix;
					cen.ymin=iy;
					cen.ymax=iy;
					kx=ix+1; ky=iy;
					while(kx<m_ncol && bmap[ky*m_ncol+kx]==1){
						cen.xmax=kx;
						val=GetPix(kx,ky);
						if(val>max){
							max=val;
							xx=kx;
							yy=ky;
						}
						kx++;
						temp++;
					}
					kx=ix-1; ky=iy;
					while(kx>0 && bmap[ky*m_ncol+kx]==1){
						cen.xmin=kx;
						val=GetPix(kx,ky);
						if(val>max){
							max=val;
							xx=kx;
							yy=ky;
						}
						kx--;
						temp++;
					}
					kx=ix; ky=iy+1;
					while(ky<m_nrow && bmap[ky*m_ncol+kx]==1){
						cen.ymax=ky;
						val=GetPix(kx,ky);
						if(val>max){
							max=val;
							xx=kx;
							yy=ky;
						}
						ky++;
						temp++;
					}
					kx=ix; ky=iy-1;
					while(ky>0 && bmap[ky*m_ncol+kx]==1){
						cen.ymin=ky;
						val=GetPix(kx,ky);
						if(val>max){
							max=val;
							xx=kx;
							yy=ky;
						}
						ky--;
						temp++;
					}
					kx=ix+1; ky=iy+1;
					while(ky<m_nrow && kx<m_ncol && bmap[ky*m_ncol+kx]==1){
						if(ky>cen.ymax)
							cen.ymax=ky;
						if(kx>cen.xmax)
							cen.xmax=kx;
						val=GetPix(kx,ky);
						if(val>max){
							max=val;
							xx=kx;
							yy=ky;
						}
						ky++;
						kx++;
						temp++;
					}
					kx=ix-1; ky=iy-1;
					while(ky>0 && kx>0 && bmap[ky*m_ncol+kx]==1){
						if(ky<cen.ymin)
							cen.ymin=ky;
						if(kx<cen.xmin)
							cen.xmin=kx;
						val=GetPix(kx,ky);
						if(val>max){
							max=val;
							xx=kx;
							yy=ky;
						}
						ky--;
						kx--;
						temp++;
					}
					kx=ix+1; ky=iy-1;
					while(ky>0 && kx<m_ncol && bmap[ky*m_ncol+kx]==1){
						if(ky<cen.ymin)
							cen.ymin=ky;
						if(kx>cen.xmax)
							cen.xmax=kx;
						val=GetPix(kx,ky);
						if(val>max){
							max=val;
							xx=kx;
							yy=ky;
						}
						ky--;
						kx++;
						temp++;
					}
					kx=ix-1; ky=iy+1;
					while(ky<m_nrow && kx>0 && bmap[ky*m_ncol+kx]==1){
						if(ky>cen.ymax)
							cen.ymax=ky;
						if(kx<cen.xmin)
							cen.xmin=kx;
						val=GetPix(kx,ky);
						if(val>max){
							max=val;
							xx=kx;
							yy=ky;
						}
						ky++;
						kx--;
						temp++;
					}
					
					/*
					 for(ky=iy-end;ky<=iy+end && ky<m_nrow;ky++){
						 for(kx=ix-end;kx<=ix+end;kx++){
							 temp+=(bmap[ky*m_ncol+kx]);
							 val=GetPix(kx,ky);
							 if(val>max){
								 max=val;
								 xx=kx;
								 yy=ky;
							 }
						 }
					 }
					 */
					if(temp>=tot){
						/*cen.xmin=ix-end;
						cen.xmax=ix+end;
						cen.ymin=iy-end;
						cen.ymax=iy+end;*/
						cen.xpeak=xx;
						cen.ypeak=yy;
						nipx=temp;
						kky=yy;kx=xx;
						/*
						 int y=yy;
						 int x;
						 int old_peak=xx;
						 int x1;
						 while(y>0 && ((bmap[y*m_ncol+kx]==1)||
									   (bmap[y*m_ncol+kx-1]==1)||
									   (bmap[y*m_ncol+kx+1]==1))){
							 y--;                  
						 }
						 kky=y+1;
						 
						 while(kky<m_nrow && ((bmap[kky*m_ncol+kx]==1)||
											  (bmap[kky*m_ncol+kx-1]==1)||
											  (bmap[kky*m_nrow+kx+1]==1))){
							 old_peak=kx=x=int(cen.xpeak);
							 x1=int(cen.xpeak)+1;
							 
							 while((bmap[kky*m_ncol+x]==1 && bmap[kky*m_ncol+x1]==1)
								   && x>0 && x1<m_ncol){
								 if((val=GetPix(x,kky))>max){
									 max=val;
									 if(abs(old_peak-x)<2){
										 cen.xpeak=x;
										 old_peak=x;
										 cen.ypeak=kky;
									 }
								 }
								 if((val=GetPix(x1,kky))>max){
									 max=val;
									 if(abs(old_peak-x1)<2){
										 cen.xpeak=x1;
										 old_peak=x1;
										 cen.ypeak=kky;
									 }
								 }
								 if(x<cen.xmin)
									 cen.xmin=x;
								 if(x1>cen.xmax)
									 cen.xmax=x1;
								 nipx+=2;
								 bmap[kky*m_ncol+x]=5;
								 bmap[kky*m_ncol+x1]=8;
								 x--;
								 x1++;
							 }   //End while per ricerca simmetria
							 if(kky>(int)cen.ymax){
								 cen.ymax=kky;
							 }
							 bmap[kky*m_ncol+int(cen.xpeak)]=2;
							 kky++;
						 }*/
						//if(nipx>=m_minumpx){                                            
							//cen.id=m_pnstar;
							centro[m_pnstar].peak=max;
							centro[m_pnstar].xpeak=cen.xpeak;
							centro[m_pnstar].ypeak=cen.xpeak;
							centro[m_pnstar].xmin=cen.xmin;
							centro[m_pnstar].xmax=cen.xmax;
							centro[m_pnstar].ymin=cen.ymin;
							centro[m_pnstar].ymax=cen.ymax;
							centro[m_pnstar].id=m_pnstar;
							centro[m_pnstar].flag=1;
							centro[m_pnstar].bkg=0.;//(GetBack(cen.xmin,cen.ymin)+GetBack(cen.xmin,cen.ymax)+ GetBack(cen.xmax,cen.ymin)+GetBack(cen.xmax,cen.ymax))/4.;
								centro[m_pnstar].bkgsig=-1;//(GetBackSig(cen.xmin,cen.ymin)+GetBackSig(cen.xmin,cen.ymax)+GetBackSig(cen.xmax,cen.ymin)+GetBackSig(cen.xmax,cen.ymax))/4.;
									
									//out<<cen.xpeak+1<<" "<<cen.ypeak+1<<endl;
									for(i=(int)centro[m_pnstar].ymin;i<=(int)centro[m_pnstar].ymax;i++)
										for(j=(int)centro[m_pnstar].xmin;j<=(int)centro[m_pnstar].xmax;j++)
											bmap[i*m_ncol+j]=0;
									out<<"image;boxcircle point (" << (cen.xpeak+1) << ", " << (cen.ypeak+1) << ")  # text={"<< (m_pnstar+1) <<"}\r";
									m_pnstar=m_pnstar+1;
						//}
						
					}
				}
			}
		}
		
		out.close();
		double dx=0,dy=0,dy1;
		int jj=0;
		//Scan lista di oggetti per pulizia 
		for(jj=0;jj<m_pnstar-1;jj++){
			for(i=jj+1;i<m_pnstar;i++){
				if(centro[i].flag){
					dx=centro[i].x-centro[jj].x;
					double dy2=centro[i].y-centro[jj].y;
					//cout<<"dy2 "<<dy2<<endl;
					dy=centro[i].ymin-centro[jj].ymax;
					dy1=centro[i].ymax-centro[jj].ymin;
					
					if(Overlap(centro[jj],centro[i])&&(centro[i].flag)&&(centro[jj].flag)){               
						centro[jj].xmin=amin1(centro[i].xmin,centro[jj].xmin);
						centro[jj].xmax=amax1(centro[i].xmax,centro[jj].xmax);
						centro[jj].ymax=amax1(centro[i].ymax,centro[jj].ymax);
						centro[jj].ymin=amin1(centro[i].ymin,centro[jj].ymin);
						
						//cout<<jj<<" N "<<centro[jj].ymax<<" "<<centro[jj].ymin<<endl;
						//cout<<centro[i+1].id<<"-"<<centro[i].id<<" dx="<<dx<<" "<<centro[i+1].ymin-centro[i].ymax<<endl;
						//cout<<"elimino star"<<centro[i].id<<endl;
						centro[i].flag=0;
					}
					
				}
			}
		}
		
		
		
		delete []bmap;
		return 0;
}




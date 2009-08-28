// MyMatrix.h: interface for the MyMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined MYMATRIX_H
#define MYMATRIX_H

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
//#include "Shapes.h"
#include <cassert>
using namespace std;

template<class T>
class MyMatrix  
{
public:
        
  MyMatrix(unsigned int r=1,unsigned int c=1): nr(r),nc(c){
    si=nr*nc;
    v.resize(si);};
  virtual ~MyMatrix(){ /*delete v;*/};
  unsigned int size(){return si;};
  unsigned int columns(){return nc;};
  unsigned int rows(){return nr;};
  T operator ()(unsigned int i,unsigned int j) const {if(i<nr && j<nc)
      return v[i*nc+j];
    else return v[0];};
  T& operator ()(unsigned int i,unsigned int j)  {return v[i*nc+j];};
  void resize(unsigned int i,unsigned int j){
    nr=i; nc=j;
    //delete v;
    si=i*j;
    v.resize(si);
  };
  void Sort(vector<T>& s){
    s=v;
    sort(s.begin(),s.end());
  };
  // operator = (unsigned int i,unsigned int j,T va) {v[i*nc+j]=va;}; 
protected:
  vector<T> v;
  unsigned int nr,nc;
  unsigned int si;

};
typedef MyMatrix<int> iMatrix;
typedef MyMatrix<unsigned int> uMatrix;
typedef MyMatrix<unsigned short> usMatrix;
typedef MyMatrix<float> fMatrix;
typedef MyMatrix<double> dMatrix;


template<class T>
class Image 
{
public:
  Image(unsigned int r=1,unsigned int c=1){
    m.resize(r,c);
    moda=0.;
    mean=0.;
    variance=0.;
    stdev=0.;
    median=0.;
  };
  virtual ~Image(){/*if(histo!=NULL) delete histo;*/};
  void SetPix(unsigned int i,unsigned int j, T val){m(i,j)=val;};
        
  T GetPix(unsigned int i,unsigned int j){return m(i,j);};
  void FillPix(T val){unsigned int i=0,j=0; 
    for (i=0;i<m.rows();i++)
      for(j=0;j<m.columns();j++)
        m(i,j)=val;
  };
  void FillPix(T *val,unsigned int r,unsigned int c){
    unsigned int i=0,j=0; 
    m.resize(r,c);
                        
    for (i=0;i<m.rows();i++)
      for(j=0;j<m.columns();j++)
        m(i,j)=val[i*c+j];
                        
  };
  unsigned int GetColumns(){return m.columns();};
  unsigned int GetRows(){return m.rows();};
  T GetMax(){return ma;};
  T GetMin(){return mi;};
  unsigned int GetNBins(){return Nbin;};
  double GetMean(){return mean;};
  double GetVariance(){return variance;};
  double GetStdev(){return stdev;};
  double GetTotal(){return total;};
  double GetMedian(){
    vector<T> s;
    s.resize(m.rows()*m.columns());
    m.Sort(s);
    median=(double)s[int(s.size()/2.+0.5)];
    return median;
  };
  double GetModa(){return moda;};
  long int GetBin(unsigned int i){
    if(i>=0 && i<Nbin)
      return histo[i];
    else
      return (0);
  };
  void Histogram(unsigned int nbin=1){
    histo.resize(nbin);
    nhisto.resize(nbin);
    unsigned int i=0,j=0;
    /*for (i=0;i<nbin;i++)
      histo[i]=0;*/
                        
    unsigned int bin=0;
    Nbin=nbin;
    double tot=0;
    double step=(double)ceil((ma-mi)/double(nbin));
                        
    for (i=0;i<m.rows();i++)
      for(j=0;j<m.columns();j++){
        bin=(unsigned int)floor((m(i,j)-mi)/step);
        histo[bin]=histo[bin]+1;
      }
    for (i=0;i<nbin;i++){
      tot+=double(histo[i]);
                                
    }
    for (i=0;i<nbin;i++){
      nhisto[i]=double(histo[i])/tot;
      //cout<<nhisto[i]<<endl;
    }
                                
  };
  void Histogram(double minval,double maxval,double step){
    const unsigned int nbin= (unsigned int)((maxval-minval)/step +1);
    double tot=0;
    vector<double> vv(nbin);
    histo.resize(nbin);
    nhisto.resize(nbin);
    unsigned int i=0,j=0;
    double x=minval;
    for (i=0;i<nbin;i++){
      x+=step;
      vv[i]=x;
    }
    unsigned int bin=0;
                        
    /*Nbin=nbin;
                        
    double step=(double)ceil((ma-mi)/double(nbin));
    */
    for (i=0;i<m.rows();i++)
      for(j=0;j<m.columns();j++){
        bin=(unsigned int)floor((m(i,j)-minval)/step);
        histo[bin]=histo[bin]+1;
      }
    for (i=0;i<nbin;i++){
      tot+=double(histo[i]);
                                
    }
    for (i=0;i<nbin;i++){
      nhisto[i]=double(histo[i])/tot;
      //cout<<nhisto[i]<<endl;
    }
    const long int *p=max_element(histo.begin(),histo.end());
    // moda=(double)(p-histo.begin());
    moda=(double)(p - (long int *) histo.begin());
  };

  void Rebin(unsigned int nbin){
    //      InitHisto(nbin);
  };
  void InitStat(){
    unsigned int i=0,j=0; 
                        
    ma=mi=m(0,0);
    total=0.0;
    tot2=0.0;
    for (i=0;i<m.rows();i++)
      for(j=0;j<m.columns();j++){
        (m(i,j)>ma)?ma=m(i,j):ma=ma;
        (m(i,j)<mi)?mi=m(i,j):mi=mi;
        total+=double(m(i,j));
        tot2 +=double(m(i,j)*m(i,j));
      }
    mean=total/double(m.size());
    variance=double(tot2)/double(m.size())-mean*mean;
    stdev=sqrt(variance);
    //cout<<"stdev="<<m.size();
                        
  };

  void ExtractSubImage(Image<T> & im,unsigned int xi=1,unsigned int yi=1,
                       unsigned int xf=1,unsigned int yf=1) {
    assert(xi>=0);
    assert(yi>=0);
    assert(xf<m.columns());
    assert(xf<m.rows());
    unsigned int i=0,j=0;
    for (i=xi;i<xf;i++)
      for(j=yi;j<yf;j++)
        im.SetPix(i,j,m(i,j));
  };
  void GetRow(vector<T>& im,unsigned int r){
    im.resize(m.columns());
    unsigned int j=0;
    for(j=0;j<m.columns();j++)
      im[j]=m(r,j);
  };
  void GetColumn(vector<T>& im,unsigned int c){
    im.resize(m.rows());
    unsigned int j=0;
    for(j=0;j<m.rows();j++)
      im[j]=m(j,c);
  };
  void Resize(unsigned int i=1,unsigned int j=1){m.resize(i,j);};
  T& operator ()(unsigned int i,unsigned int j) {return m(i,j);};

private:
  MyMatrix<T> m;
  double mean;
  double variance;
  double stdev;
  double median;
  double moda;
  T ma;
  T mi;
  double total;
  double tot2;
  vector<long int> histo;
  vector<double> nhisto;
  unsigned int Nbin;
};





typedef Image<int> iImage;
typedef Image<unsigned int> uImage;
typedef Image<unsigned short> usImage;
typedef Image<float> fImage;
#endif 

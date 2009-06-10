#include "optimizers/Powell.h"
#include "optimizers/Exception.h"
#include <limits>
#include <cmath>

namespace optimizers {

  // Useful utilities

  inline void shft3(double &a, double &b, double &c, const double d) {
    a=b;
    b=c;
    c=d;
  }
  
  template<class T>
  inline void SWAP(T &a, T &b)
  {T dum=a; a=b; b=dum;}
  
  template<class T>
  inline const T SIGN(const T &a, const T &b)
  {return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}
  
  template<class T>
  inline const T MAX(const T &a, const T &b)
  {return b > a ? (b) : (a);}
  
  template<class T>
  inline const T SQR(const T &a) {return a*a;}
  
  // Values of the function along a line in parameter space.
  // Used in the line-search part of the algorithm.  
  double Powell::f1dim(double x) {
    int ncom = m_stat->getNumFreeParams();
    std::vector<double> xt(ncom);
    for (int j=0; j<ncom; j++) {
      xt[j] = m_pcom[j] + x * m_xicom[j];
    } 
    return this->value(xt);
  }

  // Function value at an arbitrary point
  double Powell::value(std::vector<double> &pval) {
    m_stat->setFreeParamValues(pval);
    return -m_stat->value();
  }

  // The minimizer.  It's just a wrapper for the function powell.
  int Powell::find_min_only(int verbose, double tol, int tolType) {
    return find_min(verbose, tol, tolType);
  }

  int Powell::find_min(int verbose, double tol, int tolType) {
    (void) (verbose);
    std::vector<double> p;
    m_stat->getFreeParamValues(p);
    int npar = p.size();
    std::vector<std::vector<double> > xi;
    for (int j=0; j < npar; j++) {
      xi.push_back(std::vector<double>(npar,0.));
      xi[j][j] = 1.;
    }
    int code = powell(p, xi, tol, tolType, m_iter, m_maxEval, m_fret, *this);
    setRetCode(code);
    return code;
  }

  // Printed summary
  std::ostream& Powell::put(std::ostream& s) const {
    s << "Powell performed " << m_iter << " iterations"  << std::endl;
    s << "and produced a final value of " << m_fret << std::endl;
    return s;
  }

  //  Brent's parabolic interpolation method for a 1-dim function
  double brent(const double ax, const double bx, const double cx, 
	       Powell &f,const double tol, double &xmin)
  {
    const int ITMAX=100;
    const double CGOLD=0.3819660;
    const double ZEPS=std::numeric_limits<double>::epsilon()*1.0e-3;
    int iter;
    double a,b,d=0.0,etemp,fu,fv,fw,fx;
    double p,q,r,tol1,u,v,w,x;
    double e=0.0;
    
    a=(ax < cx ? ax : cx);
    b=(ax > cx ? ax : cx);
    x=w=v=bx;
    fw=fv=fx=f.f1dim(x);
    for (iter=0;iter<ITMAX;iter++) {
      double xm=0.5*(a+b);
      double tol2=2.0*(tol1=tol*fabs(x)+ZEPS);
      if (fabs(x-xm) <= (tol2-0.5*(b-a))) {
	xmin=x;
	return fx;
      }
      if (fabs(e) > tol1) {
	r=(x-w)*(fx-fv);
	q=(x-v)*(fx-fw);
	p=(x-v)*q-(x-w)*r;
	q=2.0*(q-r);
	if (q > 0.0) p = -p;
	q=fabs(q);
	etemp=e;
	e=d;
	if (fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
	  d=CGOLD*(e=(x >= xm ? a-x : b-x));
	else {
	  d=p/q;
	  u=x+d;
	  if (u-a < tol2 || b-u < tol2)
	    d=SIGN(tol1,xm-x);
	}
      } else {
	d=CGOLD*(e=(x >= xm ? a-x : b-x));
      }
      u=(fabs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
      fu=f.f1dim(u);
      if (fu <= fx) {
	if (u >= x) a=x; else b=x;
	shft3(v,w,x,u);
	shft3(fv,fw,fx,fu);
      } else {
	if (u < x) a=u; else b=u;
	if (fu <= fw || w == x) {
	  v=w;
	  w=u;
	  fv=fw;
	  fw=fu;
	} else if (fu <= fv || v == x || v == w) {
	  v=u;
	  fv=fu;
	}
      }
    }
    throw Exception("Too many iterations in brent");
    xmin=x;
    return fx;
  }

  // Line search.  Bracket the minimum and call brent  
  void linmin(std::vector<double> &p, std::vector<double> &xi, 
		  double &fret, Powell &func)
  {
    int j;
    const double TOL=1.0e-8;
    double xx,xmin,fx,fb,fa,bx,ax;
    
    int n=p.size();
    func.set_pcom(p);
    func.set_xicom(xi);
    ax=0.0;
    xx=1.0;
    mnbrak(ax,xx,bx,fa,fx,fb,func);
    fret=brent(ax,xx,bx,func,TOL,xmin);
    for (j=0;j<n;j++) {
      xi[j] *= xmin;
      p[j] += xi[j];
    }
    func.set_pcom(p);
    func.set_xicom(xi);
  }

  // Bracket the min. of a 1-d function.
  void mnbrak(double &ax, double &bx, double &cx, double &fa, double &fb, 
	      double &fc, Powell &func)
  {
    const double GOLD=1.618034,GLIMIT=100.0,TINY=1.0e-20;
    double ulim,u,r,q,fu;
    
    fa=func.f1dim(ax);
    fb=func.f1dim(bx);
    if (fb > fa) {
      SWAP(ax,bx);
      SWAP(fb,fa);
    }
    cx=bx+GOLD*(bx-ax);
    fc=func.f1dim(cx);
    while (fb > fc) {
      r=(bx-ax)*(fb-fc);
      q=(bx-cx)*(fb-fa);
      u=bx-((bx-cx)*q-(bx-ax)*r)/
	(2.0*SIGN(MAX(fabs(q-r),TINY),q-r));
      ulim=bx+GLIMIT*(cx-bx);
      if ((bx-u)*(u-cx) > 0.0) {
	fu=func.f1dim(u);
	if (fu < fc) {
	  ax=bx;
	  bx=u;
	  fa=fb;
	  fb=fu;
	  return;
	} else if (fu > fb) {
	  cx=u;
	  fc=fu;
	  return;
	}
	u=cx+GOLD*(cx-bx);
	fu=func.f1dim(u);
      } else if ((cx-u)*(u-ulim) > 0.0) {
	fu=func.f1dim(u);
	if (fu < fc) {
	  shft3(bx,cx,u,u+GOLD*(cx-bx));
	  shft3(fb,fc,fu,func.f1dim(u));
	}
      } else if ((u-ulim)*(ulim-cx) >= 0.0) {
	u=ulim;
	fu=func.f1dim(u);
      } else {
	u=cx+GOLD*(cx-bx);
	fu=func.f1dim(u);
      }
      shft3(ax,bx,cx,u);
      shft3(fa,fb,fc,fu);
    }
  }

  // Powell's direction-set method, discarding the direction of
  // largest increase.  See Numerical Recipes.
  int powell(std::vector<double> &p, std::vector<std::vector<double> > &xi, 
	      const double ftol, int tolType, int &iter, int itmax, double &fret, 
	      Powell &func)
  {
    const double TINY=1.0e-25;
    int i,j,ibig;
    double del,fp,fptt,t;
    
    int n=p.size();
    std::vector<double> pt(n),ptt(n),xit(n);

    fret=func.value(p);
    for (j=0;j<n;j++) pt[j]=p[j];
    for (iter=0;;++iter) {
      fp=fret;
      ibig=0;
      del=0.0;
      for (i=0;i<n;i++) {
	for (j=0;j<n;j++) xit[j]=xi[j][i];
	fptt=fret;
	linmin(p,xit,fret,func);
	if (fptt-fret > del) {
	  del=fptt-fret;
	  ibig=i+1;
	}
      }
      double fcheck = 0.5 * (fabs(fp)+fabs(fret));
      if (tolType == ABSOLUTE) fcheck = 1.;
      if (fabs(fp-fret) <= ftol*fcheck+TINY) {
	return 0;
      }
      if (iter == itmax) {
        return 1;
      }
      for (j=0;j<n;j++) {
	ptt[j]=2.0*p[j]-pt[j];
	xit[j]=p[j]-pt[j];
	pt[j]=p[j];
      }
      fptt=func.value(ptt);
      if (fptt < fp) {
	t=2.0*(fp-2.0*fret+fptt)*SQR(fp-fret-del)-del*SQR(fp-fptt);
	if (t < 0.0) {
	  linmin(p,xit,fret,func);
	  for (j=0;j<n;j++) {
	    xi[j][ibig-1]=xi[j][n-1];
	    xi[j][n-1]=xit[j];
	  }
	}
      }
    }
  }
  
} // namespace 

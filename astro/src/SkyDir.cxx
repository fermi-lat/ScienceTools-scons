// $Header$

// Include files

#include "astro/SkyDir.h"

namespace astro {

/** @brief initialize from (ra, dec), or (l,b)
    @param param1 either ra or l, in degrees
    @param param2 either dec or b, in degrees
    @param inputType EQUATORIAL (default) or GALACTIC
    */
    SkyDir::SkyDir(double param1, double param2, CoordSystem inputType){
        if(inputType == GALACTIC){
            double  l = param1*M_PI/180;
            double  b = param2*M_PI/180;
            
            //here we construct the cartesian galactic vector
            Hep3Vector gamgal( cos(l)*cos(b) , sin(l)*cos(b) , sin(b) );
            
            //get the transformation matrix from galactic to celestial coordinates.
            HepRotation galToCel = s_celestialToGalactic.inverse();
            //and do the transform to get the cartesian celestial vector
            m_dir = galToCel*gamgal;
            
        }else if(inputType == CELESTIAL){
            double ra = param1*M_PI/180;
            double dec = param2*M_PI/180;
            
            //here we construct the cartesian celestial vector
            m_dir = Hep3Vector( cos(ra)*cos(dec), sin(ra)*cos(dec) , sin(dec) );        
            
        }else{
            //improper coordinate system declaration - default things and say so.
            throw("Improper coordinate System declaration in SkyDir" );
            
            m_dir = Hep3Vector(0,0,1);
        }

        initProjection();
    }
    
    /** @brief initialize from direction
    */
    SkyDir::SkyDir(Hep3Vector dir, CoordSystem inputType)
        : m_dir(dir.unit())
    {
        if(inputType!=EQUATORIAL){
            m_dir = s_celestialToGalactic.inverse() * m_dir;
        }
        initProjection();
    }
    
    HepRotation SkyDir::s_celestialToGalactic = HepRotation().rotateZ(-282.8592*M_PI/180).rotateX(-62.8717*M_PI/180).rotateZ(32.93224*M_PI/180);
    
    void  SkyDir::setGalCoordsFromDir(double & l, double & b) const{
        
        //do the transform to get the galactic celestial vector
        Hep3Vector pointingin(s_celestialToGalactic*m_dir);
        
        // pointingin is the galactic cartesian pointing vector,
        //where yhat points at the galactic origin.
        // we want to make this into l and b now.
        l = atan2(pointingin.y(), pointingin.x())*180/M_PI;
        b = asin(pointingin.z())*180/M_PI;
        
    }
    
    
    double SkyDir::l ()const{
        double xl, xb; setGalCoordsFromDir(xl,xb); return xl;
    }
    
    double SkyDir::b ()const{
        double xl, xb; setGalCoordsFromDir(xl,xb); return xb;
    }
    
    double SkyDir::ra ()const{
        double ra=atan2(m_dir.y(), m_dir.x())*180/M_PI;    
        //fold RA into the range (0,360)
        while(ra < 0) ra+=360.;
        while(ra > 360) ra -= 360.;
        return ra;
    }
    
    double SkyDir::dec ()const{
        return asin(m_dir.z())*180/M_PI;
    }
    
    /*
    *  This routine will convert galactic longitude and latitude values ("l" and "b")
    *  into equivalent cartesian coordinates.  The values
    *  will be in internal Aitoff units where "l" is in the range [-2, 2]
    *  and "b" is in the range [-1, 1].
    */
    std::pair<double,double> SkyDir::hammerAitoff()const{
        double l = this->l();
        double b = this->b();
        float lover2, den;
        float radl, radb;
        
        //Force "l" to be in the range (-180 <= l <= +180) and
        //"b" to be in the range (-90 <= b <= +90).    
        while (l < -180.0) l += 360;
        while (l >  180.0) l -= 360;
        while (b <  -90.0) b += 180;
        while (b >   90.0) b -= 180;

        // Convert l and b to radians.
        double RadPerDeg = M_PI/180.;
        radl = l * RadPerDeg;
        radb = b * RadPerDeg;

        lover2 = radl / 2.0;
        den = sqrt(1.0 + (cos(radb) * cos(lover2)));
        l = 2.0 * cos(radb) * sin(lover2) / den;
        b = sin(radb) / den;
        // "x" is now in the range [-2, 2] and "y" in the range [-1, 1]. 

        return std::make_pair<double,double>(l,b);
    }

        //        ref_ra      point of reference for the
        //        ref_dec     projection (projection center)
        //        projType    projection type
        //        myRef_x     coordinates of the projection center
        //        myRef_y           in the projection coordinate system
        //        myScale_x   x scale of the projection coordinate system (1/degrees) at the projection center
        //        myScale_y   y scale of the projection coordinate
        //                                          system (1/degrees) at the projection center
        //        rot         rotation of the projection coordinate system
        //                          0 = x-axis parallel to lines of equal DEC
        //                          M_PI = x-axis parallel to lines of equal RA
        //        use_lb      If set to true, the projection gets the current galactic
        //                    coordinates instead of ra/dec.  Also it is implicit that 
        //                    ref_ra and ref_dec now refer to the projection center's 
        //                    coordinates in terms of l, b instead of ra, dec.
        void SkyDir::setProjection(const float ref_ra, const float ref_dec,
                const ProjType projType, const float myRef_x, const float myRef_y, 
                const float myScale_x, const float myScale_y, const float rot,
                const bool use_lb)
        {
                m_refRA = ref_ra;
                m_refDEC = ref_dec;
                m_projType = projType;
                m_refX = myRef_x;
                m_refY = myRef_y;
                m_scaleX = myScale_x;
                m_scaleY = myScale_y;
                m_rot = rot;
                m_project_lb = use_lb;
        }

        void SkyDir::initProjection(void)
        {
            m_refRA = 0.0;
            m_refDEC = 0.0;
            m_projType = TAN;
            m_refX = 0.0;
            m_refY = 0.0;
            m_scaleX = float(2./180.);
            m_scaleY = float(1./90.);
            m_rot = 0.0;
            m_project_lb = false;
        }

    std::pair<double,double> SkyDir::project() const
    {
       // Based on ffxypx(), Copyright (C) 1994 Associated Universities, Inc. 
       // Washington DC, USA.
       // Since this is going to be quite heavily used, we don't wrap it
       // but copy the source code from wcsutil.c in the HEADAS software.
       // Modifications by DP to use input in Radian.

      float point_ra;
      float point_dec;
      double point_x;
      double point_y;
      float dx, dy, dz, coss, sins, dt, da, dd, sint, point_ra_m;
      float l, m, geo1, geo2, geo3, sinr, cosr, cos0, sin0;
      float deps=float(1.0e-5), twopi=float(2.*M_PI);
      //int   i;

      //  Check whether projection should use l,b coords instead of ra, dec.
       if(m_project_lb)
       {
          point_ra = this->l()*M_PI/180.;       
          point_dec = this->b()*M_PI/180.;
       } 
       else
       {
          point_ra = this->ra()*M_PI/180.;
          point_dec = this->dec()*M_PI/180.;
       }

       // wrap-around tests 
       dt = (point_ra - m_refRA);
       point_ra_m = point_ra; 
       if (dt >  M_PI) point_ra_m -= twopi;
       if (dt < -M_PI) point_ra_m += twopi;

       // default values - linear 
       dx = point_ra_m - m_refRA;
       dy = point_dec - m_refDEC;
       //  dz = 0.0; 
       //  Correct for rotation 
       cosr = cos (m_rot);
       sinr = sin (m_rot);
       dz = dx*cosr + dy*sinr;
       dy = dy*cosr - dx*sinr;
       dx = dz;
       // check axis increments - bail out if either 0 *
       if ((m_scaleX==0.0) || (m_scaleY==0.0)) {
          point_x=0.0; 
          point_y=0.0; 
          throw("Improper projection axis scaling.");
       }
       // convert to pixels  
       point_x = dx / m_scaleX / (M_PI/180.) + m_refX;
       point_y = dy / m_scaleY / (M_PI/180.) + m_refY;

       if (m_projType==CAR) return std::make_pair<double,double>(point_x,point_y);  // done if linear 

       // Non linear position

       // compute direction cosine 
       coss = cos (point_dec);
       sins = sin (point_dec);
       cos0 = cos (m_refDEC);
       sin0 = sin (m_refDEC);
       l = sin(point_ra_m-m_refRA) * coss;
       sint = sins * sin0 + coss * cos0 * cos(point_ra_m-m_refRA);

       // process by case  
       switch (m_projType) {
                case SIN:  
                   if (sint<0.0) throw("Angle too large for projection.");
                   m = sins * cos(m_refDEC) - coss * sin(m_refDEC) * cos(point_ra_m-m_refRA);
                   break;
                case TAN:  
                   if (sint<=0.0) throw("Angle too large for projection.");
                   if( cos0<0.001 ) {
                      // Do a first order expansion around pole 
                      m = (coss * cos(point_ra_m-m_refRA)) / (sins * sin0);
                      m = (-m + cos0 * (1.0 + m*m)) / sin0;
                   } else {
                      m = ( sins/sint - sin0 ) / cos0;
                   }
                   if( fabs(sin(m_refRA)) < 0.3 ) {
                      l  = coss*sin(point_ra_m)/sint - cos0*sin(m_refRA) + m*sin(m_refRA)*sin0;
                      l /= cos(m_refRA);
                   } else {
                      l  = coss*cos(point_ra_m)/sint - cos0*cos(m_refRA) + m*cos(m_refRA)*sin0;
                      l /= -sin(m_refRA);
                   }
                   break;
                case ARC:
                   m = sins * sin(m_refDEC) + coss * cos(m_refDEC) * cos(point_ra_m-m_refRA);
                   if (m<-1.0) m = -1.0;
                   if (m>1.0) m = 1.0;
                   m = acos (m);
                   if (m!=0) 
                      m = m / sin(m);
                   else
                      m = 1.0;
                   l = l * m;
                   m = (sins * cos(m_refDEC) - coss * sin(m_refDEC) * cos(point_ra_m-m_refRA)) * m;
                   break;
                case NCP: 
                   if (m_refDEC==0.0) 
                      throw("Angle too large for projection.  Can't project equator.");  // can't stand the equator 
                   else
                      m = (cos(m_refDEC) - coss * cos(point_ra_m-m_refRA)) / sin(m_refDEC);
                   break;
                case GLS:
                   dt = point_ra_m - m_refRA;
                   if (fabs(point_dec)>twopi/4.0) throw("Angle too large for projection.");
                   if (fabs(m_refDEC)>twopi/4.0) throw("Angle too large for projection.");
                   m = point_dec - m_refDEC;
                   l = dt * coss;
                   break;
                case MER:
                   dt = m_scaleY * cosr + m_scaleX * sinr;
                   if (dt==0.0) dt = 1.0;
                   dy = m_refDEC/2.0 + 45.0 * (M_PI/180.);
                   dx = dy + dt / 2.0 * (M_PI/180.);
                   dy = log (tan (dy));
                   dx = log (tan (dx));
                   geo2 = dt * (M_PI/180.) / (dx - dy);
                   geo3 = geo2 * dy;
                   geo1 = cos (m_refDEC);
                   if (geo1<=0.0) geo1 = 1.0;
                   dt = point_ra_m - m_refRA;
                   l = geo1 * dt;
                   dt = point_dec / 2.0 + twopi / 8.0;
                   dt = tan (dt);
                   if (dt<deps) throw("Out of range");
                   m = geo2 * log (dt) - geo3;
                   break;
                case AIT:
                   da = (point_ra_m - m_refRA) / 2.0;
                   if (fabs(da)>twopi/4.0) throw("Angle too large for projection.");
                   dt = m_scaleY*cosr + m_scaleX*sinr;
                   if (dt==0.0) dt = 1.0;
                   dt = dt * (M_PI/180.);
                   dy = m_refDEC;
                   dx = sin(dy+dt)/sqrt((1.0+cos(dy+dt))/2.0) -
                      sin(dy)/sqrt((1.0+cos(dy))/2.0);
                   if (dx==0.0) dx = 1.0;
                   geo2 = dt / dx;
                   dt = m_scaleX*cosr - m_scaleY* sinr;
                   if (dt==0.0) dt = 1.0;
                   dt = dt * (M_PI/180.);
                   dx = 2.0 * cos(dy) * sin(dt/2.0);
                   if (dx==0.0) dx = 1.0;
                   geo1 = dt * sqrt((1.0+cos(dy)*cos(dt/2.0))/2.0) / dx;
                   geo3 = geo2 * sin(dy) / sqrt((1.0+cos(dy))/2.0);
                   dt = sqrt ((1.0 + cos(point_dec) * cos(da))/2.0);
                   if (fabs(dt)<deps) throw("Out of range");
                   l = 2.0 * geo1 * cos(point_dec) * sin(da) / dt;
                   m = geo2 * sin(point_dec) / dt - geo3;
                   break;
                case STG:
                   da = point_ra_m - m_refRA;
                   if (fabs(point_dec)>twopi/4.0) throw("Angle too large for projection.");
                   dd = 1.0 + sins * sin(m_refDEC) + coss * cos(m_refDEC) * cos(da);
                   if (fabs(dd)<deps) throw("Angle too large for projection.");
                   dd = 2.0 / dd;
                   l = l * dd;
                   m = dd * (sins * cos(m_refDEC) - coss * sin(m_refDEC) * cos(da));
                   break;

                default:
                   // fall through to here on error 
                   throw("Improper coordinate projection declaration in SkyDir::project" );

       }  // end switch 

       // to degrees 
       dx = l / (M_PI/180.);
       dy = m / (M_PI/180.);
       //  Correct for rotation 
       dz = dx*cosr + dy*sinr;
       dy = dy*cosr - dx*sinr;
       dx = dz;
       // convert to projection coordinate system  
       point_x = dx / m_scaleX + m_refX;
       point_y = dy / m_scaleY + m_refY;
       return std::make_pair<double,double>(point_x,point_y);
    }  

    double SkyDir::difference(const SkyDir& other)const
    {
       // TODO: make this computationally efficient, avoid sqrt and asin at least for small angles
       return 2.*asin(0.5*(m_dir-other.dir()).mag());
    }

    
    
} //namespace astro

/** @file SkyDir.cxx
    @brief implementation of the class SkyDir

   $Header$
*/

// Include files

#include "astro/SkyDir.h"

using namespace astro;
#include <string>
#include <exception>

bool  SkyDir::s_project_lb=false;  // If true, project uses l, b coords instead of RA, DEC
float SkyDir::s_refRA=0;  // Projection Center RA (radians)
float SkyDir::s_refDEC=0; // Projection Center DEC (radians)
SkyDir::ProjType SkyDir::s_projType=SkyDir::AIT; // Projection Type.  Valid values are CAR, SIN, 
// TAN, ARC, NCP, GLS, MER, AIT, STG
float SkyDir::s_refX=180.5; // Projection Output Center X
float SkyDir::s_refY=90.5; // Projection Output Center Y
float SkyDir::s_scaleX=-1.0; // Projection X Scaling 1/degrees
float SkyDir::s_scaleY=1.0; // Projection Y Scaling 1/degrees
float SkyDir::s_rot=0;;   // Projection Rotation Angle
namespace{
        static double DEGTORAD=M_PI/180.;
}

class SkyDir::Exception : public std::exception 
    {
    public:
        Exception() {}
        Exception(std::string errorString) 
            : m_what(errorString)
        {}

        virtual ~Exception() throw() {}
        virtual const char *what() const throw() {return m_what.c_str();}
    protected:
        std::string m_what;
    };


/** @brief initialize from (ra, dec), or (l,b)
@param param1 either ra or l, in degrees
@param param2 either dec or b, in degrees
@param inputType EQUATORIAL (default) or GALACTIC or PROJECTION
*/
SkyDir::SkyDir(double param1, double param2, CoordSystem inputType){
    if(inputType == GALACTIC){
        double  l = param1*M_PI/180;
        double  b = param2*M_PI/180;

        //here we construct the cartesian galactic vector
        Hep3Vector gamgal( cos(l)*cos(b) , sin(l)*cos(b) , sin(b) );

        //get the transformation matrix from galactic to celestial coordinates.
        HepRotation galToCel = s_equatorialToGalactic.inverse();
        //and do the transform to get the cartesian celestial vector
        m_dir = galToCel*gamgal;

    }else if(inputType == EQUATORIAL){
        double ra = param1*M_PI/180;
        double dec = param2*M_PI/180;

        //here we construct the cartesian celestial vector
        m_dir = Hep3Vector( cos(ra)*cos(dec), sin(ra)*cos(dec) , sin(dec) );        
    }else if(inputType == PROJECTION){
        double ra_rad, dec_rad;
        inverseProjection(param1, param2, &ra_rad, &dec_rad);

        Hep3Vector t = Hep3Vector( cos(ra_rad)*cos(dec_rad), sin(ra_rad)*cos(dec_rad) , sin(dec_rad) );        
        if( !s_project_lb){
            m_dir = t;
        }else{
            m_dir = s_equatorialToGalactic.inverse()* t;
        }

        
    }else{
        //improper coordinate system declaration - default things and say so.
        throw("Improper coordinate System declaration in SkyDir" );

        m_dir = Hep3Vector(0,0,1);
    }

}

/** @brief initialize from direction
*/
SkyDir::SkyDir(Hep3Vector dir, CoordSystem inputType)
: m_dir(dir.unit())
{
    if(inputType!=EQUATORIAL){
        m_dir = s_equatorialToGalactic.inverse() * m_dir;
    }
}

HepRotation SkyDir::s_equatorialToGalactic = HepRotation().rotateZ(-282.8592*M_PI/180).rotateX(-62.8717*M_PI/180).rotateZ(32.93224*M_PI/180);

void  SkyDir::setGalCoordsFromDir(double & l, double & b) const{

    //do the transform to get the galactic celestial vector
    Hep3Vector pointingin(s_equatorialToGalactic*m_dir);

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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
void SkyDir::setProjection( float ref_ra,  float ref_dec,
                            ProjType projType, 
                            float myRef_x,  float myRef_y, 
                            float myScale_x,  float myScale_y, 
                            float rot,
                            bool use_lb)
{
    s_refRA = ref_ra;
    s_refDEC = ref_dec;
    s_projType = projType;
    s_refX = myRef_x;
    s_refY = myRef_y;
    s_scaleX = myScale_x;
    s_scaleY = myScale_y;
    s_rot = rot;
    s_project_lb = use_lb;
}

void SkyDir::setProjection( float ref_ra,  float ref_dec,
            const std::string& projName,  float myRef_x,  float myRef_y, 
            float myScale_x,  float myScale_y,  float rot,
            bool use_lb)
{
    const char * names[]={
    "CAR", "SIN", "TAN", "ARC", "NCP", "GLS", "MER", "AIT", "STG"};

    ProjType type=BAD;

    for( int i = 0; i< sizeof(names)/sizeof(void*); ++i){
        if( projName != std::string(names[i])) continue;
            type=static_cast<ProjType>(i); break;
    }
    if( type==BAD) throw Exception(std::string("Unrecognized SkyDir projection type: ")+projName);
    setProjection(ref_ra, ref_dec, type, myRef_x, myRef_y, myScale_x, myScale_y, rot, use_lb);

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
    if(s_project_lb)
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
    dt = (point_ra - s_refRA);
    point_ra_m = point_ra; 
    if (dt >  M_PI) point_ra_m -= twopi;
    if (dt < -M_PI) point_ra_m += twopi;

    // default values - linear 
    dx = point_ra_m - s_refRA;
    dy = point_dec - s_refDEC;
    //  dz = 0.0; 
    //  Correct for rotation 
    cosr = cos (s_rot);
    sinr = sin (s_rot);
    dz = dx*cosr + dy*sinr;
    dy = dy*cosr - dx*sinr;
    dx = dz;
    // check axis increments - bail out if either 0 *
    if ((s_scaleX==0.0) || (s_scaleY==0.0)) {
        point_x=0.0; 
        point_y=0.0; 
        throw("Improper projection axis scaling.");
    }
    // convert to pixels  
    point_x = dx / s_scaleX / (M_PI/180.) + s_refX;
    point_y = dy / s_scaleY / (M_PI/180.) + s_refY;

    if (s_projType==CAR) return std::make_pair<double,double>(point_x,point_y);  // done if linear 

    // Non linear position

    // compute direction cosine 
    coss = cos (point_dec);
    sins = sin (point_dec);
    cos0 = cos (s_refDEC);
    sin0 = sin (s_refDEC);
    l = sin(point_ra_m-s_refRA) * coss;
    sint = sins * sin0 + coss * cos0 * cos(point_ra_m-s_refRA);

    // process by case  
    switch (s_projType) {
        case SIN:  
            if (sint<0.0) throw("Angle too large for projection.");
            m = sins * cos(s_refDEC) - coss * sin(s_refDEC) * cos(point_ra_m-s_refRA);
            break;
        case TAN:  
            if (sint<=0.0) throw("Angle too large for projection.");
            if( cos0<0.001 ) {
                // Do a first order expansion around pole 
                m = (coss * cos(point_ra_m-s_refRA)) / (sins * sin0);
                m = (-m + cos0 * (1.0 + m*m)) / sin0;
            } else {
                m = ( sins/sint - sin0 ) / cos0;
            }
            if( fabs(sin(s_refRA)) < 0.3 ) {
                l  = coss*sin(point_ra_m)/sint - cos0*sin(s_refRA) + m*sin(s_refRA)*sin0;
                l /= cos(s_refRA);
            } else {
                l  = coss*cos(point_ra_m)/sint - cos0*cos(s_refRA) + m*cos(s_refRA)*sin0;
                l /= -sin(s_refRA);
            }
            break;
        case ARC:
            m = sins * sin(s_refDEC) + coss * cos(s_refDEC) * cos(point_ra_m-s_refRA);
            if (m<-1.0) m = -1.0;
            if (m>1.0) m = 1.0;
            m = acos (m);
            if (m!=0) 
                m = m / sin(m);
            else
                m = 1.0;
            l = l * m;
            m = (sins * cos(s_refDEC) - coss * sin(s_refDEC) * cos(point_ra_m-s_refRA)) * m;
            break;
        case NCP: 
            if (s_refDEC==0.0) 
                throw("Angle too large for projection.  Can't project equator.");  // can't stand the equator 
            else
                m = (cos(s_refDEC) - coss * cos(point_ra_m-s_refRA)) / sin(s_refDEC);
            break;
        case GLS:
            dt = point_ra_m - s_refRA;
            if (fabs(point_dec)>twopi/4.0) throw("Angle too large for projection.");
            if (fabs(s_refDEC)>twopi/4.0) throw("Angle too large for projection.");
            m = point_dec - s_refDEC;
            l = dt * coss;
            break;
        case MER:
            dt = s_scaleY * cosr + s_scaleX * sinr;
            if (dt==0.0) dt = 1.0;
            dy = s_refDEC/2.0 + 45.0 * (M_PI/180.);
            dx = dy + dt / 2.0 * (M_PI/180.);
            dy = log (tan (dy));
            dx = log (tan (dx));
            geo2 = dt * (M_PI/180.) / (dx - dy);
            geo3 = geo2 * dy;
            geo1 = cos (s_refDEC);
            if (geo1<=0.0) geo1 = 1.0;
            dt = point_ra_m - s_refRA;
            l = geo1 * dt;
            dt = point_dec / 2.0 + twopi / 8.0;
            dt = tan (dt);
            if (dt<deps) throw("Out of range");
            m = geo2 * log (dt) - geo3;
            break;
        case AIT:
            da = (point_ra_m - s_refRA) / 2.0;
            if (fabs(da)>twopi/4.0) throw("Angle too large for projection.");
            dt = s_scaleY*cosr + s_scaleX*sinr;
            if (dt==0.0) dt = 1.0;
            dt = dt * (M_PI/180.);
            dy = s_refDEC;
            dx = sin(dy+dt)/sqrt((1.0+cos(dy+dt))/2.0) -
                sin(dy)/sqrt((1.0+cos(dy))/2.0);
            if (dx==0.0) dx = 1.0;
            geo2 = dt / dx;
            dt = s_scaleX*cosr - s_scaleY* sinr;
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
            da = point_ra_m - s_refRA;
            if (fabs(point_dec)>twopi/4.0) throw("Angle too large for projection.");
            dd = 1.0 + sins * sin(s_refDEC) + coss * cos(s_refDEC) * cos(da);
            if (fabs(dd)<deps) throw("Angle too large for projection.");
            dd = 2.0 / dd;
            l = l * dd;
            m = dd * (sins * cos(s_refDEC) - coss * sin(s_refDEC) * cos(da));
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
    point_x = dx / s_scaleX + s_refX;
    point_y = dy / s_scaleY + s_refY;
    return std::make_pair<double,double>(point_x,point_y);
}  

double SkyDir::difference(const SkyDir& other)const
{
    // TODO: make this computationally efficient, avoid sqrt and asin at least for small angles
    return 2.*asin(0.5*(m_dir-other.dir()).mag());
}


//	point_x     coordinates of the projected point
//	point_y          in the projection coordinate system
//	point_ra    ra and dec of the point to 
//	point_dec        be projected        

int SkyDir::inverseProjection(  double point_x,   double point_y,
                       double *point_ra, double *point_dec) 

{
    // Based on ffwldp(), Copyright (C) 1994 Associated Universities, Inc. 
    // Washington DC, USA.
    // Since this is going to be quite heavily used, we don't wrap it
    // but copy the source code from wcsutil.c in the HEADAS software.
    // Modifications by DP to use input in Radian.
    // tested to give the same output as ffwldp, DP, 18 Oct 03

    float cosr, sinr, dx, dy, dz, temp, x, y, z;
    float sins, coss, dect, rat, dt, l, m, mg, da, dd, cos0, sin0;
    //float dec0, ra0, decout, raout;
    float decout, raout;
    float geo1, geo2, geo3;
    double twopi = 2.*M_PI, deps = 1.0e-5;
    //int   i;

    //   Offset from ref pixel 
    dx = (point_x-s_refX) * s_scaleX * DEGTORAD;
    dy = (point_y-s_refY) * s_scaleY * DEGTORAD;
    //   Take out rotation  
    cosr = cos(s_rot);
    sinr = sin(s_rot);
    if (s_rot!=0.0){
        temp = dx * cosr - dy * sinr;
        dy = dy * cosr + dx * sinr;
        dx = temp;
    }

    // default, linear result for error return   
    *point_ra = s_refRA + dx;
    *point_dec = s_refDEC + dy;

    // save computing time
    sins = dx*dx + dy*dy;
    cos0 = cos(s_refDEC);
    sin0 = sin(s_refDEC);

    // process by case 
    switch (s_projType) {
         case CAR:
             rat =  s_refRA + dx;
             dect = s_refDEC + dy;
             break;
         case SIN:
             if (sins>1.0) return(501);
             coss = sqrt (1.0 - sins);
             dt = sin0 * coss + cos0 * dy;
             if ((dt>1.0) || (dt<-1.0)) return(501);
             dect = asin (dt);
             rat = cos0 * coss - sin0 * dy;
             if ((rat==0.0) && (dx==0.0)) return(501);
             rat = atan2 (dx, rat) + s_refRA;
             break;
         case TAN:
             x = cos0*cos(s_refRA) - dx*sin(s_refRA) - dy*cos(s_refRA)*sin0;
             y = cos0*sin(s_refRA) + dx*cos(s_refRA) - dy*sin(s_refRA)*sin0;
             z = sin0                       + dy*         cos0;
             rat  = atan2( y, x );
             dect = atan ( z / sqrt(x*x+y*y) );
             break;
         case ARC:
             if (sins>=twopi*twopi/4.0) return(501);
             sins = sqrt(sins);
             coss = cos (sins);
             if (sins!=0.0) sins = sin (sins) / sins;
             else
                 sins = 1.0;
             dt = dy * cos0 * sins + sin0 * coss;
             if ((dt>1.0) || (dt<-1.0)) return(501);
             dect = asin (dt);
             da = coss - dt * sin0;
             dt = dx * sins * cos0;
             if ((da==0.0) && (dt==0.0)) return(501);
             rat = s_refRA + atan2 (dt, da);
             break;
         case NCP:
             dect = cos0 - dy * sin0;
             if (dect==0.0) return(501);
             rat = s_refRA + atan2 (dx, dect);
             dt = cos (rat-s_refRA);
             if (dt==0.0) return(501);
             dect = dect / dt;
             if ((dect>1.0) || (dect<-1.0)) return(501);
             dect = acos (dect);
             if (s_refDEC<0.0) dect = -dect;
             break;
         case GLS:
             dect = s_refDEC + dy;
             if (fabs(dect)>twopi/4.0) return(501);
             coss = cos (dect);
             if (fabs(dx)>twopi*coss/2.0) return(501);
             rat = s_refRA;
             if (coss>deps) rat = rat + dx / coss;
             break;
         case MER:
             dt = (s_scaleY * cosr + s_scaleX * sinr) * DEGTORAD;
             if (dt==0.0) dt = 1.0;
             l = dx; m = dy; // save original values
             dy = s_refDEC/2.0 + 45.0 * DEGTORAD;
             dx = dy + dt / 2.0;
             dy = log (tan (dy));
             dx = log (tan (dx));
             geo2 = dt / (dx - dy);
             geo3 = geo2 * dy;
             geo1 = cos (s_refDEC);
             if (geo1<=0.0) geo1 = 1.0;
             rat = l / geo1 + s_refRA;
             if (fabs(rat - s_refRA) > twopi) return(501);
             dt = 0.0;
             if (geo2!=0.0) dt = (m + geo3) / geo2;
             dt = exp (dt);
             dect = 2.0 * atan (dt) - twopi / 4.0;
             break;
         case AIT:
             dt = (s_scaleY*cosr + s_scaleX*sinr)*DEGTORAD;
             if (dt==0.0) dt = 1.0;
             l = dx; m = dy; // save original values
             dy = s_refDEC;
             dx = sin(dy+dt)/sqrt((1.0+cos(dy+dt))/2.0) -
                 sin(dy)/sqrt((1.0+cos(dy))/2.0);
             if (dx==0.0) dx = 1.0;
             geo2 = dt / dx;
             dt = (s_scaleX*cosr - s_scaleY* sinr)*DEGTORAD;
             if (dt==0.0) dt = 1.0;
             dx = 2.0 * cos(dy) * sin(dt/2.0);
             if (dx==0.0) dx = 1.0;
             geo1 = dt * sqrt((1.0+cos(dy)*cos(dt/2.0))/2.0) / dx;
             geo3 = geo2 * sin(dy) / sqrt((1.0+cos(dy))/2.0);
             rat = s_refRA;
             dect = s_refDEC;
             if ((l==0.0) && (m==0.0)) break;
             dz = 4.0 - l*l/(4.0*geo1*geo1) - ((m+geo3)/geo2)*((m+geo3)/geo2) ;
             if ((dz>4.0) || (dz<2.0)) return(501);;
             dz = 0.5 * sqrt (dz);
             dd = (m+geo3) * dz / geo2;
             if (fabs(dd)>1.0) return(501);;
             dd = asin (dd);
             if (fabs(cos(dd))<deps) return(501);;
             da = l * dz / (2.0 * geo1 * cos(dd));
             if (fabs(da)>1.0) return(501);;
             da = asin (da);
             rat = s_refRA + 2.0 * da;
             dect = dd;
             break;
         case STG:   // Stereographic
             dz = (4.0 - sins) / (4.0 + sins);
             if (fabs(dz)>1.0) return(501);
             dect = dz * sin0 + dy * cos0 * (1.0+dz) / 2.0;
             if (fabs(dect)>1.0) return(501);
             dect = asin (dect);
             rat = cos(dect);
             if (fabs(rat)<deps) return(501);
             rat = dx * (1.0+dz) / (2.0 * rat);
             if (fabs(rat)>1.0) return(501);
             rat = asin (rat);
             mg = 1.0 + sin(dect) * sin0 + cos(dect) * cos0 * cos(rat);
             if (fabs(mg)<deps) return(501);
             mg = 2.0 * (sin(dect) * cos0 - cos(dect) * sin0 * cos(rat)) / mg;
             if (fabs(mg-dy)>deps) rat = twopi/2.0 - rat;
             rat = s_refRA + rat;
             break;

         default:
             // fall through to here on error 
             return(504);
    }

    //  return ra in range
    raout = rat;
    decout = dect;
    if (raout-s_refRA>twopi/2.0) raout = raout - twopi;
    if (raout-s_refRA<-twopi/2.0) raout = raout + twopi;
    if (raout < 0.0) raout += twopi; 

    *point_ra  = raout;
    *point_dec  = decout;

    return(0);
}  



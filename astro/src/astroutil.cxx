#include <cmath>
#include "astroutil.h"


double GetJD(int An,int Me,int Gio,double utc)
{
	int A,B,D;
	double J_D;
	long int C;

	if (Me > 2);
	else {
		An = An - 1;
		Me = Me + 12;
		}
	A = (An / 100); B = 2 - A + (A / 4);
	C = (long int)(365.25 * An); if (An < 0) C = C - 1;
	D = (int)(30.6001 * (Me + 1));
	J_D = B + C + D + Gio + 1720994.5+ utc / 24.;
	return J_D;
}   /* Giorno_Giuliano */

double  	GetGMST(double J_D)//double Ora_Un_Dec)
{
	double M,T,T1,Tempo_Siderale_0,Tempo_Siderale_Ora,Tempo_Siderale_Loc;
        double Ora_Un_Dec=modf(J_D-0.5,&M)*24;  J_D-=Ora_Un_Dec/24; 
	T = ((J_D) - J2000) / 36525.;
	T1 = (24110.54841 + 8640184.812866 * T + 0.0093103 * T * T)/86400.0;
	Tempo_Siderale_0 = modf(T1,&M) * 24.;
	Tempo_Siderale_Ora = Tempo_Siderale_0 + Ora_Un_Dec * 1.00273790935;
	if (Tempo_Siderale_Ora < 0.) Tempo_Siderale_Ora = Tempo_Siderale_Ora + 24.;
	if (Tempo_Siderale_Ora >= 24.) Tempo_Siderale_Ora = Tempo_Siderale_Ora - 24.;
	Tempo_Siderale_Loc = Tempo_Siderale_Ora*15.;
	return Tempo_Siderale_Loc;
}   /* Calcolo_Tempo_Siderale */


double Kepler(double MeanAnomaly,double Eccentricity)
{
double E;              /* Eccentric Anomaly                    */
double Error;
double TrueAnomaly;
 
    E = MeanAnomaly;    /* Initial guess */
    do
        {
        Error = (E - Eccentricity*sin(E) - MeanAnomaly)
                / (1. - Eccentricity*cos(E));
        E -= Error;
        }
   while (fabs(Error) >= 0.000001);
 
    if (fabs(E-PI) < 0.000001)
        TrueAnomaly = PI;
      else
        TrueAnomaly = 2.*atan(sqrt((1.+Eccentricity)/(1.-Eccentricity))
                                *tan(E/2.));
    if (TrueAnomaly < 0)
        TrueAnomaly += PI2;
 
    return TrueAnomaly;
}





void matrix_transpose(double a[3][3], double b[3][3])
{
   int i, j;
   double temp[3][3];

   /* This loop transposes the matrix into a temporary matrix */
   for(i=0; i<3; i++) {
      for(j=0; j<3; j++) {
         temp[j][i] = a[i][j];
      }/* end for j */
   }/* end for i */

   /* And then we assign the temporary matrix to the destination matrix */
   /* This step is necessary in case the original and destination matrix
      are the same variable */
   for(i=0; i<3; i++) {
      for(j=0; j<3; j++) {
         b[j][i] = temp[j][i];
      }/* end for j */
   }/* end for i */

}/* end matrix_transpose() */

void matrix_multiply(double a[3][3], double b[3][3], double c[3][3])
{
   int i, j, k;
   double sum;

   for(i=0; i<3; i++) {
      for(k=0; k<3; k++) {
        sum = 0;
        for(j=0; j<3; j++) {
	   sum += a[i][j] * b[j][k];
        }/* end for j */
        c[i][k] = sum;
      }/* end for k */
   }/* end for i */
}/* end matrix_multiply() */

void calc_unit_vector(double a[3], double b[3])
{
   double sqr_root;
   sqr_root = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
   b[0] = a[0] / sqr_root;
   b[1] = a[1] / sqr_root;
   b[2] = a[2] / sqr_root;
}/* end calc_unit_vector() */

void vector_cross_product(double a[3], double b[3], double c[3])
{
   c[0] = a[1]*b[2] - a[2]*b[1];
   c[1] = a[2]*b[0] - a[0]*b[2];
   c[2] = a[0]*b[1] - a[1]*b[0];
}/* end vector_cross_product() */

void vector_matrix_multiply(const double a[3], double b[3][3], double c[3])
{
   int i;
   for (i=0;i<=2;i++) {
      c[i] = a[0]*b[0][i] + a[1]*b[1][i] + a[2]*b[2][i];
   }/* end for */
}/* end vector_matrix_multiply() */

double modulo(double a, double b)
{

   if ((a - floor(a/b)*b) >= 0.0) return(a - floor(a/b)*b);
   else return(a - floor(a/b)*b + b);
}/* fine modulo() */




void eqtogal(double ra, double dec,double *l,double *b)
{

	double Xe,Ye,Ze;
	
	double RotM[3][3] = {{-0.0548755604, 0.4941094279, -0.8676661490},{ -0.8734370902, -0.4448296300, -0.1980763734}, {-0.4838350155, 0.7469822445, 0.4559837762}};
	double Pos[3];
	double Gal[3];

	Pos[0]=Xe=cos(dec*D2R)*cos(ra*D2R);
	Pos[1]=Ye=cos(dec*D2R)*sin(ra*D2R);
	Pos[2]=Ze=sin(dec*D2R);
  
	vector_matrix_multiply(Pos, RotM, Gal);

	*l=atan2(Gal[1],Gal[0])*R2D;
	*b=atan(Gal[2]/sqrt(SQR(Gal[0])+SQR(Gal[1])))*R2D;

}

void galtoeq(double l,double b, double *ra, double *dec)
{

	double Xe,Ye,Ze;
	
	double RotM[3][3] = {{-0.0548755604, 0.4941094279, -0.8676661490},{ -0.8734370902, -0.4448296300, -0.1980763734}, {-0.4838350155, 0.7469822445, 0.4559837762}};
	double Pos[3];
	double Gal[3];
	double Tra[3][3];
	matrix_transpose(RotM,Tra);
	Pos[0]=Xe=cos(b*D2R)*cos(l*D2R);
	Pos[1]=Ye=cos(b*D2R)*sin(l*D2R);
	Pos[2]=Ze=sin(b*D2R);
  
	vector_matrix_multiply(Pos, Tra, Gal);

	*ra=atan2(Gal[1],Gal[0])*R2D;
	*dec=atan(Gal[2]/sqrt(SQR(Gal[0])+SQR(Gal[1])))*R2D;
	if(*ra<0.)*ra+=360;

}


void range (double *s, double m)
{
	while((*s)>=m) (*s)-=m;
	while((*s)<0) (*s)+=m;
}

/* transformation from spherical to cartesian coordinates */
void sphcart (SPHER s, CARTES *c)
{
	double rcb = s.r * cos(s.b);

	c->x = rcb * cos(s.l);
	c->y = rcb * sin(s.l);
	c->z = s.r * sin(s.b);
}

/* transformation from cartesian to spherical coordinates */
void car2sph (CARTES c,SPHER *s)	
{
	double rho = c.x*c.x + c.y*c.y;

	if (rho > 1e-35) {	/* standard case: off axis */
	    s->l = atan2(c.y, c.x);
	    range (&(s->l), 2*PI2);
	    s->b = atan2(c.z, sqrt(rho));
	    s->r = sqrt(rho + c.z*c.z);
	} else {		/* degenerate case; avoid math error */
	    s->l = 0.0;
	    if (c.z == 0.0)
			s->b = 0.0;
	    else
			s->b = (c.z > 0.0) ? PI2/2. : -PI2/2.;
	    s->r = fabs(c.z);
	}
}

double modulo_vet(double a[3])
{
	return sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
}

void vector_dot_product(double a[3],double b[3],double *mod,double *angle)
{
	*mod = a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
	*angle=R2D*acos(*mod/(modulo_vet(a)*modulo_vet(b)));

}

int vector2radec (double *pos, double *ra, double *dec)
{
   double xyproj;

   xyproj = sqrt (pow (pos[0], 2.0) + pow (pos[1], 2.0));
   if ((xyproj == 0.0) && (pos[2] == 0))
   {
      *ra = 0.0;
      *dec = 0.0;
      return 1;
   }
    else if (xyproj == 0.0)
   {
      *ra = 0.0;
      if (pos[2] < 0.0)
      *dec = -90.0;
       else
         *dec = 90.0;
      return 2;
   }
    else
   {
      *ra = atan2 (pos[1], pos[0]) * SECS2RADS / 54000.0;
      *dec = atan2 (pos[2], xyproj) * SECS2RADS / 3600.0;

      if (*ra < 0.0)
         *ra += 24.0;
   }
   return 0;
}


void Sfer2xyz (double ra, double dec,double *vector)
{

   vector[0] = cos (dec*D2R) * cos ( ra*D2R);
   vector[1] = cos (dec*D2R) * sin ( ra*D2R);
   vector[2] = sin (dec*D2R);

   return;
}

int InsideSAA(double lon, double lat)
{
	double perim[7][2] = {{-88.,-30.},{-88.,-12.},{-55.,-0.1},{-32.,-0.1},{-7,-12},
    {50.,-25},{50.,-30.}};
	double longmin=-88;
	double longmax=50.;
	double latmin=-30;
	double latmax=-0.1;
	double diffx[7],diffy[7],diffd[7];
	if((lon>=longmin)&&(lon<=longmax)&&(lat>=latmin)&&(lat<=latmax)){
	  int i;
	for (i=0;i<6;i++){
			diffx[i]=perim[i+1][0]-perim[i][0];
			diffy[i]=perim[i+1][1]-perim[i][1];
			//cout<<i<<"  "<<diffx[i]<<endl;
	}
	diffx[6]=perim[0][0]-perim[6][0];
	diffy[6]=perim[0][1]-perim[6][1];

	for (i=0;i<7;i++){
			diffd[i]=sqrt(SQR(diffx[i])+SQR(diffy[i]));
			//cout<<diffd[i]<<endl;
	}
	double xnew[7],ynew[7];
	int inside[7];
	int xtemp[7],txtemp=0,tins=0;
	for (i=0;i<7;i++){
			xnew[i]=((lon-perim[i][0])*diffx[i]+(lat-perim[i][1])*diffy[i])/diffd[i];
			ynew[i]=(lat-perim[i][1])*diffx[i]-(lon-perim[i][0])*diffy[i];
			xtemp[i]=((xnew[i] >=0.)&&(xnew[i]<=diffd[i]));
			inside[i]=(xtemp[i] == 1) && (ynew[i]<0.);
			txtemp+=xtemp[i];
			tins+=inside[i];
			//cout<<diffd[i]<<endl;
	}
	int insaa=(txtemp==tins)&&(txtemp >0);
	//cout<<insaa<<" "<<txtemp<<"  "<<tins<<endl;

	return insaa;
	}
	else
		return 0;
}


void GetRockMat(const double pp[3],const double rock,const int orb,double ppr[3])
{
	   //rock down for even orbits, up for odd orbits
    double rock_sign = -1. + 2.*(orb - 2*(orb/2));
 	double sinrock = sin(rock_sign*rock*D2R);
    double cosrock = cos(rock_sign*rock*D2R);
	double rockmat[3][3];
	double cost = pp[0]/sqrt(pp[0]*pp[0]+pp[1]*pp[1]);
    double sint = -pp[1]/sqrt(pp[0]*pp[0]+pp[1]*pp[1]);
    rockmat[0][0]=cosrock*cost*cost+sint*sint;
	rockmat[1][0]=(1.-cosrock)*cost*sint;
	rockmat[2][0]=-sinrock*cost;
    rockmat[0][1]=(1.-cosrock)*cost*sint;
	rockmat[1][1]=cosrock*sint*sint+cost*cost;
	rockmat[2][1]=sinrock*sint;
    rockmat[0][2]=sinrock*cost;
	rockmat[1][2]=-sinrock*sint;
	rockmat[2][2]=cosrock;
	

    ppr[0]=rockmat[0][0]*pp[0]+rockmat[0][1]*pp[1]+rockmat[0][2]*pp[2];
    ppr[1]=rockmat[1][0]*pp[0]+rockmat[1][1]*pp[1]+rockmat[1][2]*pp[2];
	ppr[2]=rockmat[2][0]*pp[0]+rockmat[2][1]*pp[1]+rockmat[2][2]*pp[2];
}

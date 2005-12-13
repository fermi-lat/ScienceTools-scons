/**
 * @file IRB_routines.cxx
 * @brief
 *
 * 3 last models implemented by Luis C. Reyes    lreyes@milkyway.gsfc.nasa.gov

 * Tau is calculated as a function of energy (GeV) and redshift for the following models
 * in the GLAST range:


 *  EBL model 0: Kneiske et al "Best Fit" (A&A 413, 807-815, 2004)
 *  EBL model 1: Primack, Bullock, Somerville (2005)  astro-ph 0502177
 *  EBL model 2: Kneiske et al "High-UV" (A&A 413, 807-815, 2004)
 *  EBL model 3: Stecker, Malkan, and Scully (astro-ph 0510449)

 // Old models
 *  EBL model 4: Salamon & Stecker (ApJ 1998, 493:547-554)
 *  EBL model 5: Primack & Bullock (1999) Valid for Energies < 500 GeV and Redshift < 5

 * For  models 4 and 5 the EBL attenuation turns on abruptly at 10 GeV. This is specially artificial for distant
 * blazars z > ~3. This can be improved by extrapolating the models to energies around 1-5 GeV

*************************************************************************************
First Models implemented by jchiang from Liz Hays' original code:

These models are valid in redshift and energy ranges not useful for GLAST:


	 * Date: Mon, 14 Feb 2005 17:39:32 -0500
	 * From: Julie McEnery <mcenery@milkyway.gsfc.nasa.gov>
	 *
	 * Hi Jim,
	 *
	 * Here is some code that I have been using to add cutoffs to spectra due
	 * to absorption on the EBL. The code was mostly written by Liz Hays, a
	 * grad student who used to work with me (now a postdoc at Chicago).
	 * There are a few more recent (and more believable) models that we
	 * should add, but this might be enought to get things started.
	 *
	 * Julie
	 *
	 *  calcIRB
	 *  11-06-02
	 *
	 *  Use an EBL model to calculate optical depth as a function of
	 *  energy for a given redshift.
	 *  Tau is calculated every .1 log10 TeV from min energy given
	 *  to max energy for some redshift.  Energies and redshift are
	 *  command line args.
	 *  Result output is a table of log10 Energy and Tau (optical depth)
	 *  in file tau_<modelname>_<redshift>.dat.
	 *
	 *  EBL model :  Stecker and de Jager (ApJ 2002)
	 *                "baseline"; valid for z<0.3
	 *  EBL model :  Stecker and de Jager (ApJ 2002)
	 *                "fast evolution"; valid for z<0.3
	 *  EBL model :  Primack's model (1999)
	 *  EBL model :  Primack's model (2004)
**************************************************************************************


//////////////////////////////////////////////////////////////////////////
 *
 * $Header$
 */




//-------------------------------------------------------------------

#include <fstream>
#include <cmath>
#include <iostream>
using namespace std;

namespace IRB {
void calcIRB(float redshift, int iModel, float minE, float maxE);

float calcKneiske(float energy, float redshift);
float calcPrimack05(float energy, float redshift);
float calcKneiske_HighUV(float energy, float redshift);
float calcStecker05(float energy, float redshift);

//old models
float calcSS(float energy, float redshift);
float calcPrimack99(float energy, float redshift);
float calcSJ1(float energy, float redshift);
float calcSJ2(float energy, float redshift);
float calcPrimack(float energy, float redshift);
float calcPrimack04(float energy, float redshift);


float calcKneiske(float energy, float redshift){
/************************************************************************
EBL model 0: Kneiske, Bretz, Mannheim, Hartmann (A&A 413, 807-815, 2004)
  Valid for redshift <= 5.0
  Here we have implemented the "best fit" model from their paper
************************************************************************/


int zindex=0, eindex=-1;

float zvalue[51];

for(int i=0; i<=50; i++) zvalue[i]= 0.1*i;

float evalue[11];

for(int i=0; i<=10; i++) evalue[i]= pow(10., 0.8+0.18*i);

//Number of energy entries in the opacity table
  int MAXEINDEX = 11;
//Number of redshift entries in the opacity table
  int MAXZINDEX = 51;

float tau1, tau2, tauvalue;

float tautable[11][51] = { {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.},
{0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.001,0.002,0.003,0.005,0.007,0.009,0.011,0.014,0.018,0.02,0.021,0.026,0.028,0.03,0.033,0.036,0.038,0.041,0.044,0.044},
{0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.001,0.003,0.007,0.012,0.018,0.026,0.031,0.041,0.047,0.056,0.064,0.075,0.082,0.093,0.101,0.11,0.122,0.131,0.137,0.142,0.156,0.161,0.17,0.176,0.182,0.188,0.197,0.198,0.2,0.209,0.217,0.221,0.219,0.224},
{0.,0.,0.,0.,0.,0.,0.,0.,0.002,0.007,0.016,0.031,0.047,0.063,0.082,0.105,0.122,0.148,0.165,0.195,0.215,0.234,0.263,0.28,0.301,0.326,0.346,0.362,0.381,0.401,0.423,0.437,0.446,0.468,0.489,0.508,0.515,0.53,0.543,0.557,0.573,0.58,0.599,0.609,0.613,0.616,0.619,0.64,0.653,0.66,0.662},
{0.,0.,0.,0.002,0.006,0.016,0.032,0.057,0.091,0.131,0.178,0.229,0.278,0.325,0.375,0.416,0.463,0.509,0.558,0.598,0.646,0.687,0.732,0.778,0.814,0.854,0.89,0.93,0.97,0.999,1.036,1.067,1.089,1.119,1.16,1.18,1.213,1.234,1.257,1.278,1.294,1.311,1.333,1.34,1.36,1.377,1.398,1.415,1.418,1.426,1.432},
{0.,0.004,0.014,0.032,0.063,0.104,0.158,0.222,0.297,0.382,0.475,0.572,0.67,0.763,0.854,0.946,1.036,1.125,1.212,1.296,1.377,1.46,1.54,1.611,1.687,1.755,1.828,1.89,1.958,2.021,2.069,2.126,2.178,2.231,2.286,2.325,2.363,2.4,2.436,2.475,2.503,2.539,2.569,2.587,2.602,2.618,2.658,2.676,2.698,2.71,2.725},
{0.,0.021,0.055,0.107,0.178,0.267,0.377,0.509,0.661,0.826,1.002,1.184,1.365,1.545,1.721,1.894,2.063,2.229,2.388,2.542,2.693,2.838,2.975,3.107,3.229,3.348,3.466,3.576,3.676,3.772,3.863,3.948,4.019,4.107,4.19,4.266,4.325,4.376,4.439,4.491,4.542,4.59,4.638,4.688,4.721,4.751,4.786,4.817,4.841,4.865,4.889},
{0.,0.053,0.132,0.244,0.394,0.584,0.815,1.086,1.388,1.711,2.048,2.39,2.729,3.053,3.371,3.666,3.958,4.242,4.513,4.775,5.02,5.26,5.492,5.709,5.916,6.113,6.308,6.483,6.652,6.811,6.969,7.119,7.25,7.379,7.505,7.618,7.72,7.822,7.921,8.01,8.098,8.179,8.251,8.307,8.363,8.414,8.468,8.517,8.564,8.599,8.624},
{0.,0.124,0.304,0.554,0.878,1.273,1.731,2.242,2.797,3.379,3.973,4.569,5.153,5.716,6.263,6.776,7.275,7.753,8.208,8.641,9.054,9.448,9.819,10.173,10.507,10.822,11.116,11.394,11.655,11.897,12.128,12.349,12.541,12.73,12.908,13.072,13.218,13.351,13.48,13.601,13.716,13.829,13.932,14.005,14.073,14.136,14.203,14.27,14.328,14.373,14.41},
{0.,0.287,0.686,1.207,1.855,2.616,3.469,4.399,5.38,6.387,7.397,8.391,9.346,10.253,11.114,11.91,12.671,13.384,14.054,14.686,15.273,15.828,16.346,16.829,17.289,17.71,18.106,18.472,18.817,19.142,19.445,19.732,19.979,20.22,20.459,20.676,20.873,21.055,21.238,21.407,21.565,21.726,21.876,21.993,22.095,22.191,22.298,22.41,22.508,22.59,22.657},
{0.,0.61,1.407,2.396,3.568,4.886,6.311,7.798,9.313,10.818,12.284,13.689,15.012,16.247,17.404,18.463,19.467,20.394,21.26,22.069,22.824,23.533,24.203,24.832,25.42,25.968,26.491,26.988,27.46,27.9,28.315,28.705,29.075,29.446,29.812,30.152,30.463,30.746,31.031,31.311,31.582,31.852,32.103,32.325,32.528,32.719,32.912,33.1,33.271,33.411,33.539}};

if(redshift < 0.){
   std::cerr<<"Invalid redshift (z < 0)..."<<std::endl;
   redshift = 0.;
   } else if (redshift > 5.){
#ifdef DEBUG
       std::cerr<<"This EBL model is valid only for z <= 5.0"<<std::endl;
#endif
       redshift=5.;
       }
if (energy >= 350.) {
   std::cerr<<"This EBL model is only valid for E < 350. GeV..."<<std::endl;
   energy = 350.;
   } else if (energy < evalue[0]) return 0.;

  //Determine redshift index...
  for(int i=0; i<MAXZINDEX-1; i++)
    if(redshift >= zvalue[i] && redshift < zvalue[i+1]) zindex = i;
  if(redshift >= zvalue[MAXZINDEX-1]) zindex = MAXZINDEX-1;

  // Determine energy index
  for(int i=0; i<MAXEINDEX-1; i++)
    if(energy >= evalue[i] && energy < evalue[i+1]) eindex = i;
  if(energy >= evalue[MAXEINDEX-1]) eindex = MAXEINDEX-1;


  if (zindex < MAXZINDEX-1){
  //Find tau for redshifts above and below source by extrapolating in energy
    if(eindex < MAXEINDEX-1){
    tau1 = tautable[eindex][zindex]+(tautable[eindex+1][zindex]-tautable[eindex][zindex])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);
    tau2 = tautable[eindex][zindex+1]+(tautable[eindex+1][zindex+1]-tautable[eindex][zindex+1])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);    }
     else{
       tau1=tautable[MAXEINDEX-1][zindex];
       tau2=tautable[MAXEINDEX-1][zindex+1];
       }
  //  extrapolate now in redshift
  tauvalue =tau1 + (tau2-tau1)*(redshift-zvalue[zindex])/(zvalue[zindex+1]-zvalue[zindex]);
  } else{
      if(eindex < MAXEINDEX-1)
        tauvalue = tautable[eindex][MAXZINDEX-1]+(tautable[eindex+1][MAXZINDEX-1]-tautable[eindex][MAXZINDEX-1])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);
       else tauvalue = tautable[MAXEINDEX-1][MAXZINDEX-1];
	}

return tauvalue;

}



float calcPrimack05(float energy, float redshift){
// EBL model 1: Primack & Bullock (2005) Valid for opacities < 15


  int zindex=0, eindex=-1;
  float tau1,tau2, **tautables, tauvalue;

  float zvalue[17] = {0., 0.1, 0.25, 0.5, 1., 1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 5.5, 6., 6.5, 7.};

  float evalue[40]={2., 2.488, 3.0950, 3.8510, 4.7910, 5.9600, 7.4150, 9.2250, 1.1480e+1, 1.4280e+1, 1.7760e+1, 2.2100e+1, 2.7490e+1, 3.4200e+1, 4.2550e+1, 5.2930e+1, 6.5850e+1, 8.1920e+1, 1.0190e+2, 1.2680e+2, 1.5770e+2, 1.9620e+2, 2.4410e+2, 3.0370e+2, 3.78e+2, 4.70e+2, 5.85e+2, 7.28e+2, 9.05e+2, 1.13e+3, 1.40e+3, 1.74e+3, 2.17e+3, 2.70e+3, 3.36e+3, 4.18e+3, 5.19e+3, 6.46e+3, 8.04e+3, 1.00e+4};


float tau0[1] ={0.};

float tau01[40] = {0., 0.0000001, 0.0000003, 0.0000008, 0.0000019, 0.0000042, 0.0000099, 0.0000238, 0.0000552, 0.0001199, 0.0002443, 0.0004892, 0.0010674, 0.0024132, 0.0050824, 0.0096015, 0.0163697, 0.0256939, 0.0380436, 0.0546355, 0.0777303, 0.1105819, 0.1570698, 0.2208418, 0.3037539, 0.4054053, 0.5221997, 0.6465960, 0.7691562, 0.8799724, 0.9709822, 1.0396483, 1.0923208, 1.1416358, 1.2064721, 1.3020910, 1.4366617, 1.6201703, 1.8610741, 2.1794928};

float tau025[40]={0.0000001, 0.0000005, 0.0000014, 0.0000035, 0.0000076, 0.0000171, 0.0000407, 0.0000968, 0.0002186, 0.0004617, 0.0009238, 0.0019085, 0.0042851, 0.0094652, 0.0190260, 0.0342809, 0.0560866, 0.0850821, 0.1230088, 0.1742253, 0.2459901, 0.3477387, 0.4895209, 0.6785036, 0.9165204, 1.1991670, 1.5127269, 1.8345708, 2.1389193, 2.4018006, 2.6075906, 2.7604884, 2.8892735, 3.0378014, 3.2507707, 3.5584134, 3.9840049, 4.5473202, 5.2793418, 6.2633874};

float tau05[40] = {0.0000010, 0.0000029, 0.0000067, 0.0000144, 0.0000312, 0.0000731, 0.0001749, 0.0004026, 0.0008668, 0.0017518, 0.0035597, 0.0078349, 0.0174737, 0.0360969, 0.0669131, 0.1117945, 0.1714340, 0.2479820, 0.3487295, 0.4873202, 0.6822443, 0.9530774, 1.3148751, 1.7737483, 2.3246879, 2.9468168, 3.6000372, 4.2326759, 4.7939800, 5.2462781, 5.5866135, 5.8646962, 6.1662335, 6.5838559, 7.1977531, 8.0583552, 9.1996407, 10.6771777, 12.6293777, 15.4169639};

float tau10[35] = {0.0000092, 0.0000193, 0.0000395, 0.0000864, 0.0002063, 0.0004962, 0.0011276, 0.0023725, 0.0047253, 0.0097697, 0.0218142, 0.0480538, 0.0964923, 0.1726325, 0.2780015, 0.4126927, 0.5828524, 0.8065691, 1.1128001, 1.5344083, 2.1009211, 2.8323235, 3.7352715, 4.7970329, 5.9754211, 7.1966803, 8.3668533, 9.3951736, 10.2286613, 10.8944157, 11.5071431, 12.2380499, 13.2666033, 14.7188514, 16.6776653};

float tau15[30] = {0.0000289, 0.0000579, 0.0001275, 0.0003103, 0.0007540, 0.0017071, 0.0035602, 0.0070077, 0.0144907, 0.0327386, 0.0725108, 0.1446813, 0.2558033, 0.4069964, 0.5984519, 0.8400723, 1.1570757, 1.5847601, 2.1601107, 2.9160698, 3.8756493, 5.0484552, 6.4230870, 7.9519874, 9.5477112, 11.0982863, 12.4918875, 13.6635642, 14.6535502, 15.6091534};

float tau20[27] = {0.0000625, 0.0001368, 0.0003343, 0.0008154, 0.0018518, 0.0038687, 0.0076352, 0.0157515, 0.0356483, 0.0792690, 0.1589536, 0.2827625, 0.4531616, 0.6713563, 0.9477445, 1.3074161, 1.7843432, 2.4149232, 3.2333568, 4.2665894,  5.5296426, 7.0184022, 8.6950112, 10.4807643, 12.2666076, 13.9356198, 15.4071088};

float tau25[26] = {0.0001093, 0.0002630, 0.0006461, 0.0014943, 0.0031830, 0.0063392, 0.0128773, 0.0287818, 0.0647205,
0.1332950, 0.2447116, 0.4043603, 0.6147429, 0.8837033, 1.2308171, 1.6848746, 2.2793325, 3.0487025, 4.0236373, 5.2262532, 6.6640363, 8.3175392, 10.1311691, 12.0153741, 13.8593746, 15.5607761};

float tau30[25] = {0.0002166, 0.0005354, 0.0012768, 0.0028085, 0.0057098, 0.0112874, 0.0242226, 0.0548200, 0.1172134, 0.2248061, 0.3857926, 0.6030031, 0.8803797, 1.2328916, 1.6865156, 2.2734022, 3.0271987, 3.9793934, 5.1548374, 6.5690724, 8.2179971, 10.0657856, 12.0419091, 14.0464631, 15.9680728};

float tau35[23] = {0.0005791, 0.0013993, 0.0031327, 0.0064560, 0.0127024, 0.0264943, 0.0592172, 0.1280992, 0.2507395, 0.4384755, 0.6944182, 1.0198036, 1.4263889, 1.9406097, 2.5965219, 3.4284109, 4.4663397, 5.7331447, 7.2439184, 8.9999333, 10.9746883, 13.1074494, 15.3066731};


float tau40[22] = {0.0015020, 0.0033793, 0.0070182, 0.0138902, 0.0289108, 0.0638794, 0.1374250, 0.2702333, 0.4775225, 0.7651981, 1.1348334, 1.5962845, 2.1734261, 2.9008888, 3.8146649, 4.9450719, 6.3128560, 7.9304825, 9.7980313, 11.8928097, 14.1623445, 16.5244052};

float tau45[20] = {0.0034667, 0.0072649, 0.0144116, 0.0297863, 0.0657044, 0.1418342, 0.2810762, 0.5021846, 0.8146987, 1.2220693, 1.7341971, 2.3738843, 3.1743106, 4.1729259, 5.4022331, 6.8829534, 8.6250875, 10.6260443, 12.8610481, 15.2794450};

float tau50[19] = {0.0071092, 0.0141624, 0.0288313, 0.0632518, 0.1382215, 0.2787677, 0.5066919, 0.8348414, 1.2687125, 1.8185965, 2.5079854, 3.3704304, 4.4431075, 5.7607600, 7.3463606, 9.2094083, 11.3455301, 13.7276341, 16.3016469};

float tau55[18] = {0.0132880, 0.0265899, 0.0574533, 0.1270139, 0.2628803, 0.4906496, 0.8268285, 1.2789404, 1.8567263, 2.5842328, 3.4973174, 4.6354043, 6.0346728, 7.7216602, 9.7080199, 11.9896983, 14.5392316, 17.2989585};

float tau60[16] = {0.0235960, 0.0496899, 0.1103218, 0.2348820, 0.4537640, 0.7883757, 1.2486188, 1.8424744, 2.5923866, 3.5366242, 4.7194613, 6.1811159, 7.9511663, 10.0444068, 12.4598110, 15.1731650};

float tau65[15] = {0.0414492, 0.0911960, 0.1991044, 0.3999018, 0.7218841, 1.1796636, 1.7796795, 2.5393540, 3.4965811, 4.7009047, 6.2001010, 8.0305576, 10.2107500, 12.7428649, 15.6087370};

float tau70[14] = {0.0724103, 0.1605235, 0.3354485, 0.6327967, 1.0749973, 1.6703149, 2.4294342, 3.3849692, 4.5888035,  6.0974870, 7.9579857, 10.1977161, 12.8222616, 15.8189200};


tautables = (float **)malloc(17*sizeof(float*));
tautables[0] = tau0;
tautables[1] = tau01;
tautables[2] = tau025;
tautables[3] = tau05;
tautables[4] = tau10;
tautables[5] = tau15;
tautables[6] = tau20;
tautables[7] = tau25;
tautables[8] = tau30;
tautables[9] = tau35;
tautables[10] = tau40;
tautables[11] = tau45;
tautables[12] = tau50;
tautables[13] = tau55;
tautables[14] = tau60;
tautables[15] = tau65;
tautables[16] = tau70;


int MaxEindex[17] = {0, 39, 39, 39, 34, 29, 26, 25, 24, 22, 21, 19, 18, 17, 15, 14, 13};

if(redshift < 0.){
     std::cerr<<"Invalid redshift (z < 0)..."<<std::endl;
     redshift = 0.;
     }else if (redshift > 7.) {
#ifdef DEBUG
           std::cerr<<"Maximum redshift for this model is z = 7.0... Calculating opacity for z = 7.0"<<std::endl;
#endif
	   redshift=7.;
	   }

if (energy >= 10000.) {
    std::cerr<<"This EBL model is only valid for E <= 10 TeV..."<<std::endl;
    energy = 1e4;
    } else if (energy < evalue[0]) return 0.;

  //Determine redshift index...
  for(int i=0; i<16; i++)
    if(redshift >= zvalue[i] && redshift < zvalue[i+1]) zindex = i;
  if(redshift >= zvalue[16]) zindex = 16;

  // Determine energy index
  for(int i=0; i<39; i++)
    if(energy >= evalue[i] && energy < evalue[i+1]) eindex = i;
  if(eindex < 0) return 0;



 if (zindex < 16){
  //Find tau for redshifts above and below source
  if(eindex < MaxEindex[zindex])
    tau1 = tautables[zindex][eindex]+(tautables[zindex][eindex+1]-tautables[zindex][eindex])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);
    else tau1 = tautables[zindex][MaxEindex[zindex]];
  if(eindex < MaxEindex[zindex+1])
  tau2 = tautables[zindex+1][eindex]+(tautables[zindex+1][eindex+1]-tautables[zindex+1][eindex])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);
   else tau2 = tautables[zindex+1][MaxEindex[zindex+1]];
  //interpolate in redshift
  tauvalue =tau1 + (tau2-tau1)*(redshift-zvalue[zindex])/(zvalue[zindex+1]-zvalue[zindex]);
  } else{
    //Use tau for source at z = 7
     if (eindex < MaxEindex[zindex]) tau1 = tautables[zindex][eindex]+(tautables[zindex][eindex+1]-tautables[zindex][eindex])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);
     else tau1 = tautables[zindex][MaxEindex[zindex]];
     tauvalue = tau1;
     }

   return tauvalue;


}




float calcKneiske_HighUV(float energy, float redshift){
/************************************************************************
EBL model 2: Kneiske, Bretz, Mannheim, Hartmann (A&A 413, 807-815, 2004)
  Valid for redshift <= 5.0
  Here we have implemented the "High UV" model from their paper
************************************************************************/



int zindex=0, eindex=-1;

float zvalue[51];

for(int i=0; i<=50; i++) zvalue[i]= 0.1*i;

float evalue[14];

for(int i=0; i<=13; i++) evalue[i]= pow(10., 0.26+0.18*i);

//Number of energy entries in the opacity table
  int MAXEINDEX = 14;
//Number of redshift entries in the opacity table
  int MAXZINDEX = 51;

float tau1, tau2, tauvalue;

float tautable[14][51] = {
{0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.001,0.001,0.001,0.001,0.001,0.001,0.001,0.001,0.001,0.001,0.001,0.002,0.002,0.002,0.002,0.002,0.002},
{0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.001,0.001,0.001,0.001,0.002,0.002,0.002,0.002,0.003,0.003,0.003,0.004,0.004,0.004,0.005,0.005,0.006,0.006,0.007,0.007,0.008,0.008,0.009,0.009,0.01,0.01,0.011,0.011,0.012,0.013,0.013,0.014},
{0.,0.,0.,0.,0.,0.,0.,0.,0.001,0.001,0.001,0.001,0.001,0.002,0.002,0.002,0.003,0.004,0.005,0.006,0.007,0.009,0.01,0.011,0.013,0.014,0.016,0.017,0.019,0.021,0.023,0.025,0.027,0.029,0.031,0.033,0.036,0.038,0.04,0.043,0.045,0.047,0.05,0.052,0.054,0.056,0.058,0.061,0.063,0.064,0.066},
{0.,0.,0.,0.,0.,0.001,0.002,0.003,0.004,0.006,0.008,0.009,0.011,0.014,0.017,0.021,0.025,0.03,0.034,0.039,0.045,0.051,0.056,0.063,0.069,0.076,0.083,0.089,0.097,0.103,0.11,0.117,0.125,0.132,0.138,0.145,0.152,0.158,0.165,0.171,0.177,0.182,0.187,0.192,0.197,0.201,0.206,0.21,0.214,0.216,0.219},
{0.,0.,0.,0.001,0.002,0.004,0.006,0.01,0.014,0.021,0.031,0.043,0.056,0.071,0.086,0.102,0.117,0.133,0.15,0.167,0.184,0.201,0.218,0.234,0.251,0.267,0.283,0.298,0.314,0.328,0.341,0.355,0.368,0.379,0.391,0.403,0.415,0.424,0.434,0.443,0.454,0.462,0.469,0.476,0.483,0.492,0.499,0.505,0.511,0.516,0.52},
{0.,0.001,0.002,0.004,0.007,0.013,0.022,0.037,0.057,0.085,0.119,0.157,0.196,0.234,0.27,0.305,0.339,0.371,0.403,0.434,0.464,0.495,0.523,0.552,0.581,0.607,0.633,0.659,0.686,0.707,0.729,0.753,0.774,0.793,0.814,0.836,0.854,0.868,0.882,0.898,0.917,0.931,0.944,0.953,0.962,0.973,0.984,0.993,1.001,1.009,1.017},
{0.,0.002,0.006,0.013,0.026,0.047,0.078,0.118,0.166,0.223,0.286,0.352,0.419,0.483,0.544,0.603,0.661,0.717,0.772,0.826,0.876,0.928,0.976,1.023,1.069,1.11,1.151,1.189,1.231,1.264,1.298,1.334,1.361,1.388,1.416,1.444,1.467,1.486,1.507,1.53,1.555,1.576,1.594,1.603,1.612,1.623,1.638,1.652,1.664,1.674,1.682},
{0.,0.008,0.023,0.049,0.086,0.138,0.2,0.274,0.362,0.462,0.571,0.682,0.793,0.897,0.996,1.088,1.178,1.262,1.344,1.422,1.495,1.568,1.637,1.702,1.765,1.823,1.878,1.931,1.987,2.032,2.079,2.126,2.164,2.201,2.24,2.279,2.311,2.339,2.367,2.397,2.429,2.455,2.478,2.493,2.507,2.523,2.541,2.558,2.573,2.587,2.597},
{0.,0.023,0.061,0.113,0.183,0.273,0.381,0.505,0.645,0.797,0.955,1.113,1.269,1.414,1.554,1.684,1.811,1.932,2.049,2.162,2.267,2.374,2.473,2.568,2.66,2.745,2.827,2.907,2.987,3.055,3.122,3.191,3.248,3.303,3.358,3.411,3.456,3.495,3.535,3.575,3.616,3.651,3.684,3.705,3.723,3.744,3.77,3.793,3.814,3.832,3.847},
{0.,0.048,0.117,0.211,0.331,0.476,0.643,0.831,1.039,1.263,1.496,1.731,1.964,2.187,2.402,2.605,2.806,2.997,3.181,3.357,3.522,3.685,3.837,3.982,4.12,4.247,4.368,4.483,4.597,4.698,4.797,4.891,4.971,5.051,5.132,5.208,5.272,5.328,5.386,5.444,5.503,5.558,5.61,5.641,5.667,5.694,5.731,5.766,5.796,5.823,5.845},
{0.,0.085,0.202,0.355,0.551,0.79,1.07,1.388,1.738,2.107,2.487,2.868,3.239,3.593,3.934,4.253,4.564,4.861,5.144,5.415,5.672,5.92,6.153,6.376,6.591,6.788,6.977,7.155,7.327,7.488,7.643,7.791,7.918,8.043,8.168,8.281,8.38,8.473,8.566,8.655,8.741,8.82,8.894,8.946,8.992,9.037,9.089,9.139,9.18,9.215,9.244},
{0.,0.156,0.371,0.659,1.021,1.455,1.949,2.494,3.078,3.687,4.304,4.917,5.516,6.087,6.639,7.156,7.657,8.132,8.585,9.014,9.418,9.807,10.175,10.517,10.843,11.147,11.433,11.7,11.953,12.191,12.415,12.627,12.81,12.987,13.158,13.314,13.455,13.583,13.708,13.826,13.938,14.043,14.139,14.21,14.27,14.326,14.393,14.46,14.518,14.565,14.602},
{0.,0.314,0.742,1.291,1.964,2.748,3.62,4.56,5.547,6.554,7.556,8.537,9.478,10.365,11.208,11.986,12.728,13.422,14.074,14.686,15.254,15.793,16.296,16.762,17.205,17.61,17.99,18.343,18.673,18.987,19.281,19.56,19.794,20.022,20.248,20.455,20.643,20.82,20.995,21.158,21.313,21.463,21.603,21.712,21.806,21.899,22.007,22.115,22.21,22.289,22.351},
{0.,0.629,1.444,2.446,3.625,4.943,6.358,7.827,9.318,10.794,12.225,13.594,14.882,16.082,17.205,18.232,19.204,20.1,20.938,21.721,22.446,23.131,23.779,24.384,24.948,25.475,25.977,26.454,26.907,27.332,27.733,28.111,28.466,28.82,29.167,29.492,29.793,30.072,30.348,30.617,30.879,31.139,31.381,31.594,31.789,31.974,32.164,32.349,32.518,32.656,32.777}};

if(redshift < 0.){
   std::cerr<<"Invalid redshift (z < 0)..."<<std::endl;
   redshift = 0.;
   } else if (redshift > 5.){
       std::cerr<<"This EBL model is valid only for z <= 5.0"<<std::endl;
       redshift=5.;
       }
if (energy >= 350.) {
   std::cerr<<"This EBL model is only valid for E < 350. GeV..."<<std::endl;
   energy = 350.;
   } else if (energy < evalue[0]) return 0.;

  //Determine redshift index...
  for(int i=0; i<MAXZINDEX-1; i++)
    if(redshift >= zvalue[i] && redshift < zvalue[i+1]) zindex = i;
  if(redshift >= zvalue[MAXZINDEX-1]) zindex = MAXZINDEX-1;

  // Determine energy index
  for(int i=0; i<MAXEINDEX-1; i++)
    if(energy >= evalue[i] && energy < evalue[i+1]) eindex = i;
  if(energy >= evalue[MAXEINDEX-1]) eindex = MAXEINDEX-1;


  if (zindex < MAXZINDEX-1){
  //Find tau for redshifts above and below source by extrapolating in energy
    if(eindex < MAXEINDEX-1){
    tau1 = tautable[eindex][zindex]+(tautable[eindex+1][zindex]-tautable[eindex][zindex])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);
    tau2 = tautable[eindex][zindex+1]+(tautable[eindex+1][zindex+1]-tautable[eindex][zindex+1])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);    }
     else{
       tau1=tautable[MAXEINDEX-1][zindex];
       tau2=tautable[MAXEINDEX-1][zindex+1];
       }
  //  extrapolate now in redshift
  tauvalue =tau1 + (tau2-tau1)*(redshift-zvalue[zindex])/(zvalue[zindex+1]-zvalue[zindex]);
  } else{
      if(eindex < MAXEINDEX-1)
        tauvalue = tautable[eindex][MAXZINDEX-1]+(tautable[eindex+1][MAXZINDEX-1]-tautable[eindex][MAXZINDEX-1])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);
       else tauvalue = tautable[MAXEINDEX-1][MAXZINDEX-1];
	}

return tauvalue;

}


float calcStecker05(float energy, float redshift){
//EBL model 3:  Stecker, Malkan, and Scully
//              Astro-ph 0510449
//		Valid for opacities  0.01 < tau < 100


double tau1, tau2, tauvalue;

int zindex, MAXZINDEX=9;

double zvalue[9] = {0., 0.03, 0.112, 0.2, 0.5, 1., 2., 3., 5.};

double EMIN [9] = {80., 60., 35., 25., 15., 10., 7., 5., 4.};

double coeff[9][5] = {{0., 0., 1., 1., 1.},
                     {229.021, -2886.11, -48.9451, 1215.529, -7222.76},
                     {821.897, -9679.05, -138.498, 3518.31, -21256.9},
		     {125926., -1.44e+06, -14469.8, 379740., -2.33e+06},
		     {531.759, -5820.68, -37.8721, 1064.03, -6749.95},
		     {320.272, -3324.34, -40.6947, 1067.11, -6561.12},
		     {297.859, -3003.72, -22.9643, 646.381, -4106.36},
		     {1077.53, -10739.3, -48.8784, 1557.5, -10410.4},
		     {5997.14, -59207.8, -79.8402, 4306.14, -33148.4}};


if (redshift < 0.){
   std::cerr<<"Invalid redshift (z<0) ..."<<std::endl;
   redshift=0.;
   } else if (redshift > 5.){
      std::cerr<<"This model is only valid for z <= 5."<<std::endl;
      redshift=5.;
      }

double x = log10(energy*1e+09);

//Find zindex
 for(int i=0; i<MAXZINDEX-1; i++)
    if(redshift >= zvalue[i] && redshift < zvalue[i+1]) zindex = i;
  if(redshift >= zvalue[MAXZINDEX-1]) zindex = MAXZINDEX-1;

if (energy <= EMIN[zindex])
   return 0.;

if (zindex == 0){
   tau1 = 0.;
   tau2 = pow(10., (coeff[1][0]*x+coeff[1][1])/(coeff[1][2]*x*x+coeff[1][3]*x+coeff[1][4]));
   tauvalue =tau2*redshift/zvalue[1];
   }
   else if (zindex < MAXZINDEX-1){
   tau1 = pow(10., (coeff[zindex][0]*x+coeff[zindex][1])/(coeff[zindex][2]*x*x+coeff[zindex][3]*x+coeff[zindex][4]));
   tau2 = pow(10., (coeff[zindex+1][0]*x+coeff[zindex+1][1])/(coeff[zindex+1][2]*x*x+coeff[zindex+1][3]*x+coeff[zindex+1][4]));
   tauvalue =tau1 + (tau2-tau1)*(redshift-zvalue[zindex])/(zvalue[zindex+1]-zvalue[zindex]);
   } else
      tauvalue = pow(10., (coeff[MAXZINDEX-1][0]*x+coeff[MAXZINDEX-1][1])/(coeff[MAXZINDEX-1][2]*x*x+coeff[MAXZINDEX-1][3]*x+coeff[MAXZINDEX-1][4]));

return tauvalue;

}


float calcSS(float energy, float redshift){
// EBL model 4: Salamon & Stecker (ApJ 1998, 493:547-554)
//We are using here the model with metallicity correction (see paper)
//The paper has opacities up to z=3, for z>3 opacity remains constant according to Stecker



  int zindex=0,eindex=0;
  int i;
  float tau1,tau2;

  float zvalue[6] = {0., 0.1, 0.5, 1., 2., 3.};

  //evalues in units of GeV
  float evalue[14] = {10., 15., 20., 30., 40., 50., 60., 80., 100., 150., 200., 300., 400., 500.};

  /*From Fig.6 of the paper mentioned above */
  float tau[14][6] = {{0., 0., 0., 0., 0.023, 0.18},
		      {0., 0., 0., 0., 0.23, 0.8},
		      {0., 0., 0., 0.055, 0.8, 1.9},
		      {0., 0., 0.03, 0.31, 2.05, 3.8},
		      {0., 0., 0.11, 0.8, 3.4, 5.9},
		      {0., 0.01, 0.23, 1.23, 4.4, 7.},
		      {0., 0.02, 0.4, 1.7, 5.8, 8.},
		      {0., 0.05, 0.72, 2.8, 7.8, 10.0},
		      {0., 0.09, 1.1, 3.4, 9., 12.},
		      {0., 0.2, 2., 5.4, 10.7, 15.},
		      {0., 0.3, 2.6, 7., 13., 16.},
		      {0., 0.5, 3.9, 9., 15., 18.},
		      {0., 0.7, 4.9, 10., 17., 19.5},
		      {0., 0.8, 5.1, 10.1, 18.0, 20.}};


  if(redshift < 0.){
     std::cerr<<"Invalid redshift (z < 0)..."<<std::endl;
     redshift = 0.;
     }

//According to the model by the authors (Stecker and Salomon) opacities for z>3 are
//the same as for z=3. This is related to star formation among other things
//This is a really sensitive topic among theory people...  beware :)
  if (redshift > 3.) redshift = 3.;


  //Determine redshift index...
  if(redshift>=3.) zindex=5;
    else
      for(i=0;i<5;i++){
         if(redshift>=zvalue[i] && redshift<zvalue[i+1]){
	   zindex=i;
         }
       }

//Ebl attenuation is negligible for photons with E < 10 GeV (0.01 TeV)
//If energy > 500 GeV assume attenuation equivalent to energy = 500 GeV (outside Glast Energy Range anyway...)
  if(energy <= 10.) return 0;
    else if (energy >= 500.) {
      eindex = 13;
      energy = 500.;
      }
     else
       for(i=0;i<13;i++){
        if(energy>=evalue[i] && energy<evalue[i+1]){
	  eindex=i;
         }
        }


// Extrapolate in Energy for redshifts above and below the source
tau1 = tau[eindex][zindex]+(tau[eindex+1][zindex]-tau[eindex][zindex])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);
tau2 = tau[eindex][zindex+1]+(tau[eindex+1][zindex+1]-tau[eindex][zindex+1])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);

//Extrapolate in redshift
return tau1 + (tau2-tau1)*(redshift-zvalue[zindex])/(zvalue[zindex+1]-zvalue[zindex]);

}


float calcPB99(float energy, float redshift){
// EBL model 5: Primack & Bullock (1999) Valid for Energies < 500 GeV
//and Redshift < 5 We are using here the LCDM model with Salpeter's
//stellar Initial Mass Function (IMF) The data provided by Bullock has
//opacities up to 10.

  int zindex=0,eindex=-1;
  float tau1,tau2, **tautables, tauvalue;

  float zvalue[9] = {0., 0.1, 0.5, 1., 1.5, 2., 3., 4., 5.};

  float evalue[86]={0.1000e+2, 0.1047e+2, 0.1097e+2, 0.1149e+2, 0.1203e+2, 0.1260e+2, 0.1320e+2, 0.1383e+2, 0.1448e+2, 0.1517e+2, 0.1589e+2, 0.1664e+2, 0.1743e+2, 0.1825e+2, 0.1912e+2, 0.2002e+2, 0.2097e+2, 0.2196e+2, 0.2300e+2, 0.2409e+2, 0.2524e+2, 0.2643e+2, 0.2768e+2, 0.2899e+2, 0.3037e+2, 0.3181e+2, 0.3331e+2, 0.3489e+2, 0.3654e+2, 0.3827e+2, 0.4009e+2, 0.4199e+2, 0.4398e+2, 0.4606e+2, 0.4824e+2, 0.5053e+2, 0.5292e+2, 0.5543e+2, 0.5805e+2, 0.6080e+2, 0.6368e+2, 0.6670e+2, 0.6986e+2, 0.7317e+2, 0.7663e+2, 0.8026e+2, 0.8407e+2, 0.8805e+2, 0.9222e+2, 0.9659e+2, 0.1012e+3, 0.1060e+3, 0.1110e+3, 0.1162e+3, 0.1217e+3, 0.1275e+3, 0.1335e+3, 0.1399e+3, 0.1465e+3, 0.1534e+3, 0.1607e+3, 0.1683e+3, 0.1763e+3, 0.1846e+3, 0.1934e+3, 0.2026e+3, 0.2121e+3, 0.2222e+3, 0.2327e+3, 0.2437e+3, 0.2553e+3, 0.2674e+3, 0.2801e+3, 0.2933e+3, 0.3072e+3, 0.3218e+3, 0.3370e+3, 0.3530e+3, 0.3697e+3, 0.3872e+3, 0.4055e+3, 0.4248e+3, 0.4449e+3, 0.4660e+3, 0.4880e+3, 0.5111e+3};


float tau0[1] ={0.};

float tau01[86] = {0.05044,0.04816,0.04598,0.04390,0.04191,0.04002,0.03821,0.03648,0.03483,0.03326,0.03175,0.03032,0.02894,0.02764,0.02639,0.02519,0.02407,
0.02300,0.02200,0.02106,0.02020,0.01941,0.01872,0.01810,0.01756,0.01719,0.01691,0.01668,0.01667,0.01672,0.01689,0.01722,0.01764,0.01825,0.01902,0.01984,
0.02084,0.02209,0.02339,0.02488,0.02659,0.02835,0.03037,0.03260,0.03492,0.03774,0.04057,0.04360,0.04713,0.05089,0.05465,0.05939,0.06423,0.06942,0.07540,
0.08144,0.08862,0.09661,0.10431,0.11347,0.12391,0.13421,0.14563,0.15887,0.17252,0.18757,0.20456,0.22131,0.24177,0.26293,0.28349,0.30777,0.33455,0.35996,
0.39091,0.42190,0.45415,0.48984,0.52425,0.56257,0.60443,0.64343,0.68610,0.73183,0.77681,0.82173};

float tau05[86]={0.05044,0.04816,0.04598,0.04390,0.04192,0.04002,0.03821,0.03650,0.03489,0.03335,0.03201,0.03078,0.02974,0.02891,0.02835,0.02805,0.02806,
0.02848,0.02955,0.03086,0.03288,0.03562,0.03916,0.04320,0.04734,0.05259,0.05949,0.06675,0.07506,0.08330,0.09376,0.10543,0.11700,0.13047,0.14464,0.16002,
0.17624,0.19429,0.21342,0.23369,0.25603,0.27839,0.30383,0.33098,0.35983,0.39217,0.42593,0.46072,0.49828,0.54202,0.58845,0.63685,0.68838,0.74716,0.81362,
0.87817,0.95018,1.03182,1.11620,1.20787,1.31027,1.42074,1.53603,1.66197,1.79468,1.93863,2.09184,2.25382,2.44020,2.62804,2.81407,3.01810,3.24102,3.46880,
3.70482,3.94419,4.21036,4.48947,4.74677,5.03711,5.32504,5.62250,5.92042,6.23185,6.54397,6.85156};

float tau10[71] = {0.05055,0.04838,0.04641,0.04466,0.04309,0.04165,0.04062,0.04058,0.04115,0.04222,0.04380,0.04713,0.05163,0.05689,0.06380,0.07313, 0.08424,0.09744,0.11312,0.13093,0.15043,0.17345,0.19764,0.22589,0.25653,0.28962,0.32740,0.36817,0.41008,0.45497,0.50754,0.56030,0.61270,0.67131,0.73566,
0.80161,0.87014,0.94555,1.02595,1.11570,1.20579,1.30465,1.41392,1.52256,1.64368,1.77331,1.91196,2.05822,2.21835,2.39601,2.58247,2.78855,2.99329,3.24001,
3.49657,3.75238,4.03416,4.34287,4.66736,5.00715,5.37851,5.77434,6.20143,6.64110,7.10022,7.60494,8.09390,8.62231,9.18755,9.77758,10.35777};

float tau15[55] = {0.04283,0.04471,0.04879,0.05416,0.05972,0.06915,0.08260,0.09855,0.11749,0.13897,0.16835,0.20046,0.23560,0.27589,0.32207,0.37472, 0.42664,0.49159,0.55870,0.63295,0.71158,0.79007,0.88211,0.98407,1.07970,1.19164,1.30595,1.43081,1.55807,1.69511,1.83965,1.99051,2.15071,2.31911,2.50800,
2.70071,2.90339,3.12064,3.35610,3.60785,3.86638,4.17478,4.47518,4.81546,5.14556,5.53774,5.95120,6.37901,6.83123,7.31972,7.83243,8.41666,8.99513,9.61588,
10.28970};

float tau20[47] = {0.02497,0.03678,0.05016,0.06877,0.09574,0.12887,0.15834,0.20098,0.25672,0.31359,0.37598,0.44497,0.53599,0.62514,0.71397,0.82767,
0.93907,1.06119,1.19013,1.33648,1.49061,1.64903,1.81300,1.99702,2.18411,2.37337,2.57794,2.80693,3.02583,3.26354,3.51345,3.80321,4.08439,4.39201,4.70966,
5.07996,5.42966,5.80825,6.23923,6.68865,7.15252,7.64205,8.16685,8.76766,9.35244,9.98335,10.71234};

float tau30[43] = {0.12760,0.15369,0.18239,0.21531,0.26280,0.30838,0.34810,0.40751,0.47517,0.53306,0.60393,0.67753,0.76306,0.84639,0.92888,1.03448,
1.15230,1.25744,1.36932,1.52043,1.66826,1.80743,1.97745,2.16587,2.36226,2.56421,2.78734,3.08298,3.36508,3.61257,3.96512,4.36544,4.72370,5.11846,5.59212,
6.09073,6.61199,7.09702,7.72980,8.43907,9.09844,9.78671,10.62800};

float tau40[31] = {0.63771,0.72971,0.82592,0.93823,1.05512,1.17521,1.30511,1.44529,1.59416,1.75078,1.92336,2.11099,2.30598,2.50734,2.73796,2.99315,
3.25266,3.52705,3.85299,4.19963,4.55735,4.93732,5.37807,5.85975,6.35328,6.86966,7.47273,8.12476,8.77295,9.49649,10.27070};

float tau50[3] = {8.45314,9.21388,10.01371};


tautables = (float **)malloc(9*sizeof(float*));
tautables[0] = tau0;
tautables[1] = tau01;
tautables[2] = tau05;
tautables[3] = tau10;
tautables[4] = tau15;
tautables[5] = tau20;
tautables[6] = tau30;
tautables[7] = tau40;
tautables[8] = tau50;


int MaxEindex[9] = {0, 85, 85, 70, 54, 46, 42, 30, 2};

if(redshift < 0.){
     std::cerr<<"Invalid redshift (z < 0)..."<<std::endl;
     redshift = 0.;
     } else if (energy > 500.) {
       std::cerr<<"This EBL model is only valid for E <= 500 GeV..."<<std::endl;
       return 10.;
       }


//For z > 5. the opacity remains constant (same value as at z = 5.)
if(redshift > 5.) redshift=5.0;

  //Determine redshift index...
  for(int i=0; i<8; i++)
    if(redshift >= zvalue[i] && redshift < zvalue[i+1]) zindex = i;
  if(redshift >= zvalue[8]) zindex = 8;

  // Determine energy index
  for(int i=0; i<85; i++)
    if(energy >= evalue[i] && energy < evalue[i+1]) eindex = i;
  if(eindex < 0) return 0;



 if (zindex < 8){
  //Find tau for redshifts above and below source
  if(eindex < MaxEindex[zindex])
    tau1 = tautables[zindex][eindex]+(tautables[zindex][eindex+1]-tautables[zindex][eindex])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);
    else tau1 = tautables[zindex][MaxEindex[zindex]];
  if(eindex < MaxEindex[zindex+1])
  tau2 = tautables[zindex+1][eindex]+(tautables[zindex+1][eindex+1]-tautables[zindex+1][eindex])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);
   else tau2 = tautables[zindex+1][MaxEindex[zindex+1]];
  //interpolate in redshift
  tauvalue =tau1 + (tau2-tau1)*(redshift-zvalue[zindex])/(zvalue[zindex+1]-zvalue[zindex]);
  } else{
    //Use tau for source at z = 5
     if (eindex < MaxEindex[zindex]) tau1 = tautables[zindex][eindex]+(tautables[zindex][eindex+1]-tautables[zindex][eindex])*(energy-evalue[eindex])/(evalue[eindex+1]-evalue[eindex]);
     else tau1 = tautables[zindex][MaxEindex[zindex]];
     tauvalue = tau1;
     }

   return tauvalue;


}


float calcSJ1(float energy, float redshift){
//  EBL model :  Stecker and de Jager (ApJ 2002) "baseline"; valid for z<0.3
//  Returns tau (optical depth)
//

//convert energy from GeV to MeV
  energy /= 1e3;

  static int firsttime = 1;//calculate z coefficients first time only
  int i,j;
  static double coeff[5];  //hold tau polynomial coefficients
  //array of coefficients for redshift dependent polynomials
  double redshiftCoeff[5][4] = {{-5.1512,   -4.4646,  -3.5143,  -0.80109},
			       {9.2964,    16.275,   10.7540,   2.51610},
			       {-4.8645,  -12.86,    -8.69982, -2.07510},
			       {1.1164,     3.9795,   2.747,    0.66596},
			       {-0.084039, -0.42147, -0.29644, -0.072889}};
  if(firsttime){
    double log10redshift = log10(redshift);  //polynomial is in log10 of redshift
    //calculate redshift dependent constants
    for(i=0;i<5;i++){
      //calculate polynomial coefficient
      coeff[i] = 0.;
      for(j=0;j<4;j++){
	//add redshift dependent polynomial terms to get optical density polynomial coeffiecient
	coeff[i] += redshiftCoeff[i][j]*pow(log10redshift,j);
      }
    }
    firsttime = 0;
  }

  float logtau = 0.;
  for(i=0;i<5;i++){
    //add polynomial terms to get optical density
    logtau += coeff[i]*pow((log10(energy)+2),i);
  }
  return pow(static_cast<float>(10.), static_cast<float>(logtau));  //return tau not log10(tau)
}





float calcSJ2(float energy, float redshift){
//  EBL model :  Stecker and de Jager (ApJ 2002) "fast evolution"; valid for z<0.3
//  Same as model 1 excpet for z polynomials' coefficients
//  Returns tau (optical depth)
//

  //convert energy from GeV to MeV
  energy /= 1e3;

  static int firsttime = 1;//calculate z coefficients first time only
  int i,j;
  static double coeff[5];  //hold tau polynomial coefficients
  //array of coefficients for redshift dependent polynomials
  double redshiftCoeff[5][4] = {{-5.4748,   -4.7036,  -3.5842,  -0.79882},
			       {10.444,     17.114,   11.173,    2.58430},
			       {-5.8013,   -13.733,   -9.2033,  -2.17670},
			       {1.4145,      4.3143,   2.9535,   0.71046},
			       {-0.11656,   -0.46363, -0.32339, -0.078903}};
  if(firsttime){
    double log10redshift = log10(redshift);  //for calculation use log10 of redshift
    //calculate redshift dependent constants
    for(i=0;i<5;i++){
      //calculate polynomial coefficient
      coeff[i] = 0.;
      for(j=0;j<4;j++){
	//add redshift dependent polynomial terms to get optical density polynomial coeffiecient
	coeff[i] += redshiftCoeff[i][j]*pow(log10redshift,j);
      }
    }
    firsttime = 0;
  }

  float logtau = 0.;
  for(i=0;i<5;i++){
    //add polynomial terms to get optical density
    logtau += coeff[i]*pow((log10(energy)+2),i);
  }
  return pow(static_cast<float>(10.),static_cast<float>(logtau));  //return tau not log10(tau)
}

float calcPrimack(float energy, float redshift){
//  EBL model for LCDM:  Bullock thesis (1999ish) based on model of
//  Somerville and Primack (1999).  Valid for 100 GeV to 100 TeV.
//  Input redshift and energy in TeV
//  Returns tau (optical depth)
//

//convert energy from GeV to MeV
  energy /= 1e3;

  static int firsttime = 1;//calculate z coefficients first time only
  static int zindex;
  int i;
  float c1,c2;
  float tau=0;

  //Define coefficients
  //Coeffs given for z=0.01,0.02,0.04... so defining bounds around these values
  float zboundslow[8] = {0.010, 0.025, 0.035, 0.07, 0.15, 0.25, 0.40, 0.75};
  float zboundsup[8]  = {0.025, 0.035, 0.070, 0.15, 0.25, 0.40, 0.75, 1.50};
  float tau1[8] = {0.49, 0.74, 0.99, 2.63, 5.59, 8.96, 0.96, 66.27};
  float tau2[8] = {12586.52, 19212.86, 20930.67, 226.28, 71.17, 507.96, 15.39, 0.0};
  float log10E1[8] = {12.59, 12.59, 12.58, 12.59, 12.57, 12.55, 11.47, 12.71};
  float log10E2[8] = {20.70, 20.64, 20.58, 16.36, 15.16, 15.35, 13.03, 1.00};
  float delta1[8] = {0.79, 0.79, 0.79, 0.80, 0.81, 0.82, 0.57, 0.95};
  float delta2[8] = {4.23, 4.19, 4.18, 2.22, 1.62, 1.64, 1.35, 1.00};

  //Determine closest redshift model...
  if(firsttime){
    for(i=0;i<8;i++){
      if(redshift>zboundslow[i] && redshift<=zboundsup[i]){
	zindex=i;
//		cout<<"index="<<zindex<<endl;
      }
    }
    firsttime = 0;
  }
  if(zindex<0 || zindex>7){
    return 0;
  }
  //Calc tau(E,z)
  c1 = (log10(energy) + 12 - log10E1[zindex])/delta1[zindex];
  c2 = (log10(energy) + 12 - log10E2[zindex])/delta2[zindex];
  tau = tau1[zindex]*exp(-pow(c1,2)) + tau2[zindex]*exp(-pow(c2,4));
  return tau;
}

float calcPrimack04(float energy, float redshift){
//  EBL model for LCDM: Data obtained from Bullock (3 Nov 04)
//  Valid for 100 GeV to 100 TeV.
//  Input redshift and energy in TeV
//  Returns tau (optical depth)
//

//convert energy from GeV to MeV
  energy /= 1e3;

  static int firsttime = 1;//calculate z coefficients first time only
  static int zindex,eindex;
  int i;
  float tau=0;
  float tau1,tau2;

  float zvalues[34] = {0.01,0.02,0.03,0.04,0.05,0.07,0.09,0.11,0.15, 0.20,0.25,0.30,
		       0.40,0.50,0.60,0.70,0.80,0.90,1.0, 1.2,1.4,1.6,1.8,2.0,2.2,
		       2.4,2.6,2.8,3.0,3.4,3.8,4.2,4.6,5.0};
  float evalues[50] = {.01,.01207,.01456,.01758,.02121,.02560,.03089,.03728,.04498,.05429,
		       .06551,.07906,.09541,.1151,.1389,.1677,.2024,.2442,.2947,.3556,
		       .4292,.5179,.6251,.7543,.9103,1.099,1.326,1.600,1.931,2.330,2.812,
		       3.393,4.095,4.942,5.964,7.197,8.685,10.48,12.65,15.26,18.42,22.23,
		       26.83,32.37,39.07,47.15,56.90,68.66,82.86,100.0};
  float tauvalues[34][50] = {
{.0000025,.0000051,.0000099,.0000184,.0000331,.0000620,.0001248,.0002517,.0004724,.0008179,.0013230,.0019910,.0028567,.0039637,.0054178,.0073652,.0100109,.0136187,.0183429,.0244250,.0318413,.0405631,.0502680,.0604771,.0706250,.0800332,.0881626,.0945478,.0994039,.1029253,.1058661,.1092853,.1144021,.1215271,.1315947,.1451345,.1632183,.1871207,.2201387,.2689654,.3453460,.4622684,.6383003,.8888257,1.2246720,1.6430671,2.1319566,2.6560831,3.1852815,3.6777244},

{.0000051,.0000104,.0000203,.0000379,.0000682,.0001281,.0002576,.0005170,.0009713,.0016811,.0027073,.0040709,.0058338,.0080892,.0110433,.0150088,.0203782,.0276905,.0373098,.0496500,.0646945,.0823352,.1019300,.1224901,.1428391,.1616564,.1778243,.1905457,.2000274,.2069731,.2128666,.2199009,.2306047,.2452971,.2656940,.2936820,.3303166,.3789798,.4460701,.5459609,.7013499,.9406487,1.3009542,1.8128153,2.4966052,3.3447964,4.3320565,5.3931379,6.4593477,7.4473987},

{.0000079,.0000161,.0000314,.0000584,.0001054,.0001981,.0003990,.0007986,.0014995,.0025916,.0041604,.0062499,.0089422,.0123867,.0169001,.0229483,.0311652,.0422947,.0569756,.0757271,.0986300,.1253926,.1550666,.1861096,.2167503,.2450015,.2691893,.2881122,.3021686,.3126131,.3213616,.3322020,.3489239,.3710717,.4026082,.4452866,.5008279,.5756099,.6785398,.8316200,1.0696142,1.4374917,1.9894153,2.7737923,3.8168740,5.1097941,6.6075234,8.2165508,9.8273935,11.3134775},

{.0000108,.0000222,.0000432,.0000803,.0001448,.0002727,.0005496,.0011005,.0020590,.0035525,.0056878,.0085290,.0121821,.0168598,.0229811,.0311907,.0423353,.0574203,.0773078,.1026780,.1336169,.1697016,.2095994,.2512590,.2922738,.3299488,.3621036,.3872130,.4057108,.4193666,.4313801,.4464655,.4696618,.4998108,.5418996,.5996429,.6758207,.7762992,.9173243,1.1256921,1.4490904,1.9511491,2.7038858,3.7726741,5.1855397,6.9397554,8.9660606,11.1282930,13.2846708,15.2748909},

{.0000140,.0000286,.0000557,.0001035,.0001865,.0003522,.0007108,.0014214,.0026592,.0045721,.0073024,.0109246,.0155696,.0215185,.0293086,.0397985,.0539512,.0731509,.0987308,.1310042,.1701071,.2157783,.2662040,.3186206,.3695692,.4169099,.4569401,.4880532,.5110250,.5281115,.5429060,.5617646,.5903828,.6303101,.6840608,.7573214,.8545945,.9830970,1.1621573,1.4287608,1.8446651,2.4902792,3.4563649,4.8069005,6.6200624,8.8394308,11.3997974,14.1407938,16.8601322,19.3377895},

{.0000210,.0000428,.0000831,.0001539,.0002778,.0005272,.0010649,.0021199,.0039431,.0067624,.0107363,.0160307,.0227798,.0313977,.0426951,.0579242,.0784776,.1062117,.1427601,.1890934,.2455718,.3105278,.3821059,.4563936,.5287157,.5948455,.6506978,.6936058,.7249439,.7486714,.7695312,.7980711,.8390698,.8974997,.9761038,1.0836535,1.2238106,1.4108548,1.6725055,2.0635664,2.6701307,3.6109538,5.0185409,6.9992666,9.6126604,12.8252535,16.4735489,20.3706379,24.2193584,27.7212086},

{.0000291,.0000591,.0001142,.0002107,.0003808,.0007273,.0014704,.0029159,.0054013,.0091882,.0145228,.0215702,.0305633,.0421177,.0571515,.0774068,.1048241,.1417826,.1903420,.2516289,.3256598,.4111212,.5046542,.6014795,.6950165,.7801722,.8511774,.9057529,.9455580,.9758574,1.0034366,1.0404924,1.0964937,1.1739789,1.2817096,1.4234067,1.6110183,1.8617895,2.2146547,2.7387462,3.5587485,4.8318152,6.7154007,9.3637686,12.8326702,17.0697899,21.8917274,26.9905262,32.0164680,36.5060349},

{.0000383,.0000774,.0001491,.0002745,.0004970,.0009544,.0019305,.0038120,.0070083,.0118731,.0186750,.0276240,.0390164,.0536472,.0727050,.0983920,.1331442,.1799164,.2411732,.3181762,.4108285,.5173688,.6334296,.7529955,.8679273,.9719477,1.0580500,1.1237996,1.1716386,1.2076843,1.2427741,1.2912430,1.3627270,1.4624109,1.5982695,1.7800047,2.0174866,2.3347573,2.7858834,3.4581571,4.5100722,6.1402316,8.5454102,11.9095364,16.2937565,21.6193390,27.6455250,33.9768829,40.1661911,45.6693916},

{.0000602,.0001211,.0002318,.0004246,.0007729,.0014988,.0030321,.0059340,.0107826,.0180653,.0281380,.0412801,.0579362,.0792299,.1072013,.1448285,.1957651,.2639978,.3527664,.4634907,.5957192,.7464498,.9092013,1.0748998,1.2331595,1.3740494,1.4897335,1.5767748,1.6395533,1.6885675,1.7400708,1.8124201,1.9198374,2.0672169,2.2692602,2.5352252,2.8841884,3.3534648,4.0199676,5.0305858,6.6075182,9.0396252,12.6186523,17.5638008,23.9419575,31.6063156,40.1734085,49.0632324,57.6005669,65.1148300},

{.0000953,.0001904,.0003618,.0006592,.0012077,.0023692,.0047893,.0092756,.0166732,.0274888,.0423897,.0614720,.0855678,.1164246,.1570039,.2116809,.2855697,.3845324,.5118698,.6683125,.8542290,1.0631298,1.2876836,1.5126367,1.7247800,1.9111396,2.0614269,2.1727231,2.2544188,2.3198068,2.3973231,2.5080111,2.6676173,2.8880241,3.1836786,3.5705090,4.0765519,4.7640624,5.7519512,7.2605124,9.6238260,13.2466593,18.5549965,25.7604027,34.9395027,45.8396339,57.8281136,70.0303574,81.5766678,91.5129089},

{.0001416,.0002805,.0005287,.0009587,.0017738,.0035172,.0071130,.0135567,.0240187,.0391276,.0594302,.0854213,.1180626,.1598843,.2150757,.2894487,.3897296,.5225406,.6922464,.9010277,1.1438762,1.4175025,1.7033803,1.9898465,2.2552600,2.4854150,2.6683033,2.8014338,2.8991172,2.9857092,3.0925429,3.2475908,3.4683819,3.7727315,4.1791549,4.7059512,5.3998842,6.3427930,7.7155728,9.8330183,13.1483698,18.1952419,25.4751911,35.2940598,47.6265182,62.0730629,77.6492615,93.3695450,107.9084015,120.2151566},

{.0002020,.0003976,.0007415,.0013430,.0025174,.0050272,.0100559,.0189681,.0331055,.0531917,.0799825,.1137779,.1560480,.2103604,.2823641,.3796158,.5107395,.6816656,.8996711,1.1649288,1.4692535,1.8066660,2.1601050,2.5061848,2.8253162,3.0960205,3.3081284,3.4614196,3.5781515,3.6871111,3.8288169,4.0379920,4.3290391,4.7323651,5.2614098,5.9577842,6.8602066,8.1143522,9.9499636,12.8083057,17.2647877,23.9926910,33.5960045,46.4144669,62.2446442,80.4185791,99.9398193,119.1089859,136.6255646,151.1040039},

{.0003750,.0007238,.0013325,.0024227,.0046534,.0093646,.0186154,.0337883,.0574213,.0895490,.1314635,.1837776,.2491638,.3343342,.4467319,.5982297,.8009317,1.0606687,1.3847685,1.7738607,2.2139947,2.6877604,3.1776619,3.6453803,4.0654249,4.4122744,4.6752863,4.8643250,5.0184240,5.1891217,5.4216566,5.7616463,6.2305574,6.8850799,7.7220941,8.7946672,10.2232542,12.2489491,15.2926912,20.0561447,27.4395027,38.3934174,53.6422081,73.4083328,97.2910385,123.8553543,151.3824005,177.6913605,200.8938446,219.1707153},

{.0006409,.0012157,.0022102,.0040718,.0079960,.0160723,.0309523,.0552435,.0908714,.1387568,.1992835,.2739742,.3680021,.4905005,.6542308,.8734453,1.1618029,1.5281328,1.9750736,2.4983022,3.0841620,3.7069919,4.3310242,4.9173489,5.4293365,5.8396335,6.1426473,6.3719277,6.5774426,6.8277073,7.1752243,7.6896620,8.4006758,9.3378458,10.5631580,12.1367531,14.2531080,17.3263321,22.0279636,29.4047165,40.7032089,57.1773987,79.4221878,107.6517029,140.5317993,176.1819153,211.9873047,245.2343292,273.4273071,294.5772095},

{.0010316,.0019236,.0034831,.0065359,.0130276,.0259649,.0488702,.0851557,.1361059,.2028487,.2852718,.3870355,.5156797,.6846939,.9108534,1.2136471,1.6049763,2.0909660,2.6710658,3.3460648,4.0848837,4.8586426,5.6151500,6.3080549,6.8997793,7.3613610,7.6994357,7.9688411,8.2454510,8.6007671,9.1135149,9.8680620,10.8669415,12.1737061,13.8477535,16.0400352,19.0449924,23.5058670,30.4428940,41.3266449,57.6000061,80.8480072,111.6191788,149.6880341,192.1118927,237.2953186,281.4008179,320.8649902,353.2617798,376.1623840},

{.0015822,.0029040,.0052666,.0101060,.0203127,.0399682,.0734324,.1242716,.1936172,.2825121,.3901303,.5232565,.6940599,.9202204,1.2208937,1.6196725,2.1216476,2.7437000,3.4728026,4.3005977,5.2069173,6.1212277,7.0029845,7.7998796,8.4543076,8.9549303,9.3275547,9.6463070,10.0157528,10.5205364,11.2506685,12.2757778,13.6231403,15.3746758,17.5997162,20.6042004,24.7196426,30.9868164,40.7933121,56.0494041,78.8002472,110.0164948,150.2688599,198.5087891,251.7331696,306.5486450,358.3540039,403.5839844,438.9971924,462.6222229},

{.0023323,.0042318,.0077586,.0151795,.0305428,.0590954,.1060632,.1741775,.2652210,.3780970,.5150199,.6856923,.9058837,1.2024446,1.5850730,2.0904922,2.7189827,3.4819548,4.3686051,5.3626976,6.4123969,7.4750772,8.4746923,9.3545408,10.0665617,10.6023760,11.0200396,11.4225674,11.9132576,12.6035891,13.5893307,14.9381409,16.6990814,18.9686794,21.8273335,25.6896229,31.2930832,39.9795074,53.3560867,74.0868759,104.0555573,145.2059631,195.7870789,255.1190186,319.1337280,383.0781860,442.1505737,492.0058289,529.4039917,552.4962769},

{.0033198,.0059968,.0111546,.0221605,.0448780,.0842574,.1471475,.2358327,.3504231,.4906310,.6612853,.8746309,1.1574939,1.5269624,2.0085938,2.6295226,3.3987002,4.3122849,5.3614655,6.5140977,7.7325115,8.9171391,10.0250196,10.9752769,11.7292585,12.3027420,12.7839422,13.2743921,13.9118614,14.8464317,16.1370621,17.8700314,20.0877514,22.9175549,26.5523319,31.5819740,39.0125198,50.4613037,68.5572968,95.8981781,134.3008423,185.3979492,247.9717712,318.9998474,394.0681152,466.3316650,531.7512817,585.1981201,623.6455688,644.9057007},

{.0045879,.0082667,.0156888,.0315303,.0629769,.1163273,.1984475,.3110440,.4513287,.6208281,.8283356,1.0936402,1.4395862,1.8950020,2.4908679,3.2347715,4.1450539,5.2187076,6.4402494,7.7601686,9.1091700,10.4246035,11.6249399,12.6330538,13.4198704,14.0368910,14.5574675,15.1680365,16.0321999,17.2207890,18.8465462,21.0206852,23.7426338,27.2259369,31.7689781,38.1232681,47.7311516,62.7484894,86.2885742,121.3426895,169.7968903,232.7742157,307.2283936,389.4161377,473.9151306,554.3731079,625.2631226,681.2216797,719.4566650,738.0036011},

{.0081561,.0149048,.0293595,.0599868,.1153082,.2040921,.3301500,.4935898,.6915356,.9308178,1.2274593,1.6142752,2.1168253,2.7732117,3.6073575,4.6285667,5.8429465,7.2386827,8.7808962,10.4018459,12.0263615,13.5543156,14.8929195,15.9775219,16.8166733,17.5086021,18.2600574,19.1714478,20.5460987,22.3740883,24.7857933,27.9291573,31.8217258,36.8591690,43.6847229,53.6025734,68.9864960,93.3830032,130.1435699,183.0844574,253.4440765,339.9285278,438.6616821,543.8255615,647.2558594,741.0090942,819.8956909,877.6684570,912.0178833,922.2794189},

{.0134350,.0254446,.0515775,.1042256,.1936908,.3271459,.5048601,.7251124,.9873999,1.3085383,1.7149667,2.2462935,2.9316137,3.8158379,4.9009924,6.2216892,7.7365499,9.4666367,11.3070288,13.2012749,15.0461454,16.7331295,18.1690998,19.2973595,20.2079315,21.0697365,22.1338768,23.5253754,25.4726048,28.0541744,31.4371738,35.6714783,40.9476929,47.8835106,57.5932045,72.2967682,95.4330444,132.2641144,185.4411621,259.1430969,353.4082336,465.8909607,587.7668457,713.0280151,830.6622314,933.7418213,1015.7018433,1071.6245117,1099.3741455,1098.0003662},

{.0209881,.0415147,.0852879,.1674996,.2981582,.4797239,.7129115,.9965588,1.3309464,1.7531850,2.2881141,2.9783375,3.8788505,4.9913101,6.3472095,7.9342823,9.7568560,11.7875633,13.9045362,16.0377769,18.0557957,19.8401279,21.3230133,22.5090427,23.5620117,24.6613865,26.1314583,28.0784225,30.7012062,34.0700493,38.3881721,43.6882515,50.5640106,59.7537155,73.1261368,94.0389175,126.7208481,176.5624084,248.3728027,345.1144714,462.2259521,598.8715820,741.4014893,883.4193115,1012.7566528,1121.2404785,1203.5305176,1254.4587402,1272.5877686,1257.9053955},

{.0328537,.0670181,.1365255,.2563175,.4296506,.6645458,.9547740,1.3025472,1.7238595,2.2656534,2.9368362,3.8261147,4.9228873,6.2861156,7.9019041,9.7753572,11.8932781,14.1675625,16.5253658,18.8522320,20.9827347,22.8421173,24.3551178,25.6280022,26.8400574,28.2784138,30.2275639,32.7558098,36.1238327,40.3141327,45.6603127,52.3978844,61.0204391,73.1281586,91.4313278,120.1551208,164.6086273,231.0925598,323.3544312,442.3539429,582.8101196,739.0636597,899.6646118,1054.4907227,1190.9136963,1302.3436279,1381.2889404,1424.8773193,1431.5122070,1401.5012207},

{.0489295,.1007808,.1980368,.3564041,.5799623,.8685060,1.2147985,1.6310076,2.1498880,2.7939165,3.6355002,4.6892362,6.0054197,7.5928497,9.4559860,11.5915146,13.9597769,16.4782314,19.0126896,21.4656162,23.6831417,25.5615692,27.1160793,28.4734020,29.9247570,31.6822319,34.0546646,37.2342567,41.2993317,46.3545227,52.7828102,60.9702187,71.6920471,87.2211380,111.0811462,148.5700684,206.5299377,290.0887756,401.8976746,541.7526855,704.8435669,877.3882446,1051.4663086,1214.7648926,1355.8801270,1466.1778564,1540.2949219,1574.7252197,1568.6307373,1524.1976318},

{.0698919,.1429723,.2714108,.4686201,.7350737,1.0701714,1.4726222,1.9592869,2.5643785,3.3419855,4.3157725,5.5457716,7.0602331,8.8671474,10.9624043,13.3216858,15.8949738,18.5923882,21.2573318,23.7909565,26.0490150,27.9372730,29.5310726,31.0138950,32.6838837,34.8355179,37.6682167,41.4425468,46.1818428,52.1067924,59.6213112,69.2319565,82.3233795,101.7770309,132.1956024,179.9557343,250.5629578,351.3544922,481.9575195,640.3628540,820.5341187,1009.1077271,1194.5085449,1363.1959229,1505.1507568,1611.2756348,1679.5175781,1703.7515869,1685.8015137,1626.9832764},

{.0953274,.1905495,.3512417,.5827505,.8920270,1.2658583,1.7191799,2.2724206,2.9677572,3.8595650,4.9682612,6.3543191,8.0261459,9.9998484,12.2745333,14.8211994,17.5571480,20.3657455,23.1409950,25.7095966,27.9750652,29.8844967,31.5293369,33.1871948,35.0998077,37.6363869,40.9651489,45.2213058,50.6476173,57.3686066,66.0349655,77.1351700,92.8405533,116.4428635,153.3794250,210.3892670,294.1528015,409.9071655,557.2755127,731.5310669,922.7761841,1120.8856201,1311.7990723,1483.8309326,1625.4498291,1729.2473145,1790.1838379,1805.8135986,1776.2541504,1704.8112793},

{.1205536,.2359138,.4187812,.6835776,1.0172951,1.4281335,1.9263721,2.5359547,3.3041141,4.2764778,5.5025992,7.0083780,8.8109808,10.9431658,13.3398905,16.0091572,18.8682041,21.7860947,24.5917339,27.1759090,29.4494591,31.3670444,33.1043053,34.9047585,37.0846291,39.9542694,43.6691017,48.3867531,54.3258438,61.7894402,71.3374481,84.0914307,102.2018127,129.7738037,173.2084198,239.8421783,334.7878723,462.8620911,620.3508301,807.4715576,1007.0064087,1212.5598145,1407.5791016,1579.9390869,1719.8162842,1819.5299072,1873.3068848,1880.5494385,1842.2270508,1760.9362793},

{.1672776,.3187798,.5504476,.8730475,1.2816159,1.7715181,2.3629091,3.0940311,4.0036311,5.1650438,6.5731192,8.3035326,10.3604298,12.7519054,15.4447231,18.3672485,21.4531727,24.5149975,27.4862595,30.1931114,32.5060387,34.4888840,36.3707809,38.4763107,41.1796646,44.7340584,49.4001617,55.2079353,62.6577072,71.9482269,83.9458694,100.2521133,124.0376968,161.0123291,218.6236267,302.7062073,422.5153809,580.2041016,771.9120483,989.8648682,1218.5278320,1448.8404541,1660.4600830,1843.6343994,1986.3106689,2081.1645508,2124.2534180,2114.8889160,2054.4631348,1948.0323486},

{.2458412,.4558475,.7734991,1.1906872,1.7041190,2.3213160,3.0590835,3.9636183,5.1079702,6.5000877,8.2144136,10.3057346,12.7453384,15.4831762,18.5324821,21.8426876,25.2145443,28.6142464,31.7737617,34.6264420,37.0064278,39.0858154,41.2940903,43.8760910,47.3643379,52.1987343,58.3204803,66.1178131,75.9551697,88.4007568,104.6160431,127.3863144,161.3346405,213.8285828,297.3041992,414.4442139,574.2076416,781.3302612,1023.3951416,1291.8970947,1568.7253418,1833.2229004,2069.8293457,2264.2526855,2407.2800293,2490.5993652,2512.1591797,2472.6672363,2375.4709473,2228.4392090},

{.5723186,.9905820,1.5660179,2.2958333,3.1454446,4.1525459,5.3656058,6.8297443,8.6376534,10.8605013,13.4860878,16.5489540,20.0235004,23.8251953,27.9106369,32.1903343,36.4244385,40.4479446,44.0329170,47.1248741,49.7670517,52.4467697,55.8297462,60.5604630,67.2168274,76.1172104,87.8678360,101.9897232,120.1142044,143.6823730,176.3945007,224.2591400,299.2895508,412.0993652,582.0324707,813.9988403,1109.4827881,1465.9428711,1853.9135742,2262.9362793,2649.1696777,2994.9675293,3279.1818848,3485.0361328,3604.8059082,3633.6601562,3576.9328613,3436.7375488,3224.6479492,2955.2036133},

{1.1985376,1.9373481,2.8779919,4.0088410,5.3195715,6.8717856,8.7500706,11.0257282,13.8257303,17.1008453,20.9132614,25.2568817,29.9504814,34.9658890,40.2010765,45.4776688,50.4299622,54.9631996,58.8433533,62.2191238,65.5732574,69.6626511,75.5796356,83.9234772,95.5301590,110.8342514,129.7628479,153.9268494,185.3259125,228.1193085,290.3393250,385.7432556,532.3311157,746.7047729,1051.6613770,1442.2770996,1914.6542969,2449.1293945,3009.2790527,3547.0776367,4033.3764648,4431.0502930,4726.4252930,4907.2836914,4960.9252930,4896.3745117,4719.0732422,4440.4169922,4084.0346680,3668.8935547},

{2.2847035,3.4448478,4.8615160,6.5435748,8.5317135,10.8831701,13.7439070,17.1454926,21.2445183,25.9573917,31.3355618,37.1615829,43.4020348,49.8443832,56.3541794,62.5913277,68.3098679,73.2726059,77.5370178,81.6690903,86.6372223,93.5547028,103.6007385,117.6985703,136.6007233,160.9809875,191.7304840,231.1605072,284.7713623,361.1321411,476.8981628,654.4828491,919.0540771,1292.9465332,1785.3502197,2394.1257324,3083.8627930,3820.9047852,4549.5512695,5221.6279297,5787.8774414,6215.2861328,6491.2324219,6601.6748047,6551.2958984,6347.7714844,6006.7783203,5553.4033203,5017.7158203,4432.6650391},

{3.9291742,5.6319675,7.6560192,10.0931473,12.9567118,16.3793488,20.4838638,25.3248081,31.0227718,37.4537735,44.5344276,52.1090469,59.9990540,67.9113998,75.5089264,82.5912552,88.7783203,94.1270828,99.1390991,104.9322205,112.7434769,124.2393494,140.5291595,162.9564667,191.7168274,229.0919495,276.4519348,339.4367981,429.5432129,563.0163574,765.8624268,1072.3195801,1509.5687256,2096.2722168,2831.5715332,3688.9140625,4615.3945312,5563.2475586,6450.4497070,7214.8144531,7816.1630859,8225.9042969,8430.7509766,8423.2832031,8215.2675781,7827.2006836,7285.8427734,6627.7895508,5893.7211914,5130.4179688},

{6.3109612,8.6872730,11.5248814,14.9055595,18.9481983,23.7646179,29.5348988,36.2363014,43.8928986,52.3467064,61.4437714,70.9443436,80.5852737,89.9298248,98.6303253,106.3606796,112.9892426,118.9916229,125.6174545,134.3950653,147.0416718,165.3233185,190.5916138,223.7687836,266.7203979,321.9298401,393.4595642,494.1884460,641.6058350,867.6368408,1208.8942871,1698.6368408,2371.8364258,3233.5891113,4258.0395508,5400.9121094,6579.8808594,7709.8393555,8711.2900391,9534.0996094,10000,10000,10000,10000,9959.3320312,9343.2236328,8566.5029297,7681.7456055,6740.4760742,5793.0253906}
};

  //Determine closest redshift model...
  if(firsttime){
    for(i=0;i<34;i++){
      if(redshift>=zvalues[i] && redshift<zvalues[i+1]){
	zindex=i;
      }
    }
    firsttime = 0;
  }
  if(zindex<0 || zindex>34){
    return 0;
  }
    for(i=0;i<50;i++){
      if(energy>=evalues[i] && energy<evalues[i+1]){
	eindex=i;
	//	cout<<"(zindex,eindex)=("<<zindex<<","<<eindex<<")"<<endl;
      }
    }

if (redshift==zvalues[zindex]){
  // Interpolating only in energy ...
tau = tauvalues[zindex][eindex]+((tauvalues[zindex][eindex+1]-tauvalues[zindex][eindex])*(energy-evalues[eindex])/(evalues[eindex+1]-evalues[eindex]));
}
if (redshift>zvalues[zindex]){
  // Interpolating first in energy and then in redshift ...
tau1 = tauvalues[zindex][eindex]+((tauvalues[zindex][eindex+1]-tauvalues[zindex][eindex])*(energy-evalues[eindex])/(evalues[eindex+1]-evalues[eindex]));
tau2 = tauvalues[zindex+1][eindex]+((tauvalues[zindex+1][eindex+1]-tauvalues[zindex+1][eindex])*(energy-evalues[eindex])/(evalues[eindex+1]-evalues[eindex]));
 tau = tau1 + (tau2-tau1)*(redshift-zvalues[zindex])/(zvalues[zindex+1]-zvalues[zindex]);
};
  return tau;
}


} // namespace IRB

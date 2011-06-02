#Author: Vlasios Vasileiou <vlasisva@gmail.com>
import sys,os,string,math,time
#from ROOT import gSystem,gROOT
#gSystem.Load('librootIrfLoader.so')
#gSystem.Load('libBackgroundEstimator.so')

#from ROOT import GANGSTER,TOOLS,BackgroundEstimator

def _CalculateBackground(ConfigFile, verbosity=1, first=[True]):

    TOOLS.LoadConfig(ConfigFile)
    RA = TOOLS.Get("GRB_RA")
    DEC = TOOLS.Get("GRB_DEC")
    FT2 = TOOLS.GetS("FT2_FILE")
    DATACLASS = TOOLS.GetS("DATA_CLASS")
    NewFT1ZenithTheta_Cut = TOOLS.Get("FT1ZENITH_THETA_CUT")

    CREATE_TIMEDATA_FILE = bool(TOOLS.Get("CREATE_TIMEDATA_FILE"))
    TIMEDATA_FILE = TOOLS.GetS("TIMEDATA_FILE")
    GRB_TRIGGER_TIME = TOOLS.Get("GRB_T0")
    TIMEDATA_NSTEPS_BEFORE = int(TOOLS.Get("TIMEDATA_NSTEPS_BEFORE"))
    TIMEDATA_NSTEPS_AFTER = int(TOOLS.Get("TIMEDATA_NSTEPS_AFTER"))
    GRB_NAME = TOOLS.GetS("GRB_NAME")	
    FT1_FILE=TOOLS.GetS("BASEDIR")+"/"+TOOLS.GetS("FT1_FILE")
    TIME_PER_STEP_AFTER = TOOLS.Get("TIME_PER_STEP_AFTER")
    TIME_PER_STEP_BEFORE= TOOLS.Get("TIME_PER_STEP_BEFORE")
    Iterations = TOOLS.Get("ITERATIONS")

    path = TOOLS.GetS("OUTPUT_DIR")+"/Bkg_Estimates/"
    if not os.path.exists(path): mkdir(path)
    Est=""; EstF=""; EstB=""
    Combine=False
    DataClassName = DATACLASS.partition("::")[0]
    if DATACLASS.find("::")==True:
	EstF = BackgroundEstimator(DataClassName+"::FRONT",True,first[0])
	EstB = BackgroundEstimator(DataClassName+"::BACK",True,False)
	Combine=True
	Est=EstF
    else :
	Est = BackgroundEstimator(DATACLASS,True,first[0])

    if (NewFT1ZenithTheta_Cut>0 and math.fabs(Est.FT1ZenithTheta_Cut-NewFT1ZenithTheta_Cut)>1) :
	if (first[0]): print "WARNING: Your are overriding the default FT1ZenithTheta_Cut (%.1fdeg). New value is %.0fdeg" %(Est.FT1ZenithTheta_Cut,NewFT1ZenithTheta_Cut)
	if (Combine==True):
	      EstB.FT1ZenithTheta_Cut=NewFT1ZenithTheta_Cut
	      EstF.FT1ZenithTheta_Cut=NewFT1ZenithTheta_Cut
        else : 
	      Est.FT1ZenithTheta_Cut=NewFT1ZenithTheta_Cut

    if (CREATE_TIMEDATA_FILE):
        fTimeData = file(TIMEDATA_FILE,"w")
        print "Creating time data file %s" % TIMEDATA_FILE
        aMET = GRB_TRIGGER_TIME-TIMEDATA_NSTEPS_BEFORE*TIME_PER_STEP_BEFORE
        for i in range (-TIMEDATA_NSTEPS_BEFORE,0):
    	    txt="%.2f_%.2f %lf 0 %f %s\n" %(TIME_PER_STEP_BEFORE*i,TIME_PER_STEP_BEFORE*(i+1),aMET,TIME_PER_STEP_BEFORE,FT1_FILE)
            fTimeData.write(txt)
            aMET+=TIME_PER_STEP_BEFORE
        for i in range (0,TIMEDATA_NSTEPS_AFTER) :
           txt="%.2f_%.2f %lf 0 %f %s\n" %(TIME_PER_STEP_AFTER*i,TIME_PER_STEP_AFTER*(i+1),aMET,TIME_PER_STEP_AFTER,FT1_FILE)
	   fTimeData.write(txt)
           aMET+=TIME_PER_STEP_AFTER
	fTimeData.close()

    try:
        tlist = file(TIMEDATA_FILE,"r")
    except:
        print "Can't open the timedata file %s" %TIMEDATA_FILE
        return -1
 
    nread=0
    if verbosity>1 : print "Processing %s (RA/DEC)=(%.3f/%.3f)tlist" %(GRB_NAME,RA,DEC)
    result = -1
    while (1) :
        astring = tlist.readline()
	if astring=="": break
        if astring[0]=='#': continue
	data = astring.split(" ")
	if (data.__len__()!=5): break
	JOB_SUFFIX = data[0]
	MET        = float(data[1])
	TIME_BEFORE = float(data[2])
	TIME_AFTER = float(data[3])
	FT1        = data[4]
        nread+=1
    
        GRB_DIR = TOOLS.GetS("OUTPUT_DIR")+"/Bkg_Estimates/"+JOB_SUFFIX+"/"
        if not os.path.exists(GRB_DIR): os.mkdir(GRB_DIR)

        DURATION=TIME_BEFORE+TIME_AFTER
        MET-=TIME_BEFORE
        if verbosity>1: print "%s (MET/DUR)=(%.1f %.1f) tlist" %(JOB_SUFFIX,MET,DURATION)
        if Combine==True: 
            result = EstF.Make_Background_Map(FT1, FT2, GRB_DIR, MET, DURATION, Iterations,verbosity)
            EstB.Make_Background_Map(FT1, FT2, GRB_DIR, MET, DURATION, Iterations,verbosity)
        else: result = Est.Make_Background_Map(FT1, FT2, GRB_DIR, MET, DURATION, Iterations,verbosity)

 
    if nread==0: print "The script couldn't read the contents of your TimeData file properly.Make sure that the format is correct"
    first[0]=False
    tlist.close()	

    GANGSTER.PlotBackground(1,verbosity)
    return result

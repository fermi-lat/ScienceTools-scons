import numpy,os,sys

def ProcessResults():
    from ROOT import *
    ROOTDIR = gROOT.GetDirectory("/")
    q_T05=[];q_T95=[];q_T90=[0,0,0];q_TRUE=0;
    inLimits=[0,0,0]
    nall=0
    gStyle.SetOptTitle(1)
    '''
    gT95_MED=TGraph();ROOTDIR.Add(gT95_MED)
    gT95_UL=TGraph();ROOTDIR.Add(gT95_UL)
    gT95_LL=TGraph();ROOTDIR.Add(gT95_LL)
    ig=0;
    '''
    hT95_med = 0;hT05_med=0; hT95_UL=0;hPlateau=0;hT95_LL=0
    first=True
    Plateau_TRUE=0
    for i in range(0,10000):
	aQ=numpy.zeros(3)
	filename="output/%d.root" %i
	if not os.path.exists(filename):continue
	dfile=TFile(filename,"read")
	if not dfile.cDurations.__nonzero__(): continue
        q_T95=dfile.t95.GetTitle().split("_")
        q_T05=dfile.t05.GetTitle().split("_")
        q_Plateau=dfile.plateau.GetTitle().split("_")
	for j in range(0,3): 
	    q_T95[j]=float(q_T95[j])
	    q_T05[j]=float(q_T05[j])
	    q_T90[j]=q_T95[j]-q_T05[j]
	    q_Plateau[j]=float(q_Plateau[j])

	if first:
	    q_TRUE=dfile.true.GetTitle().split("_")
	    for j in range(0,3): q_TRUE[j]=float(q_TRUE[j])
	    Plateau_TRUE=q_TRUE[2]
	    q_TRUE[2]=q_TRUE[1]-q_TRUE[0]
	    ROOTDIR.cd()
	    hT95_med = TH1F("hT95","Median T95s",50,q_TRUE[1]/8.,q_TRUE[1]*5.)
	    hT05_med = TH1F("hT05","Median T05s",50,q_TRUE[0]/8.,q_TRUE[0]*5.)
	    hT95_UL = TH1F("hT95_UL","Upper and Lower Limits of T95",50,q_TRUE[1]/8.,q_TRUE[1]*5.)
	    hT95_LL = TH1F("hT95_LL","Upper and Lower Limits of T95",50,q_TRUE[1]/8.,q_TRUE[1]*5.)
	    hPlateau = TH1F("hPlateau","Median Plateau levels",50,Plateau_TRUE/2.,Plateau_TRUE*2)
	    ROOTDIR.Add(hT95_med)
	    ROOTDIR.Add(hT05_med)
	    ROOTDIR.Add(hT95_UL)
	    ROOTDIR.Add(hT95_LL)
	    ROOTDIR.Add(hPlateau)
	    hT95_med.GetXaxis().SetTitle("Time (sec)");
	    hT05_med.GetXaxis().SetTitle("Time (sec)");
	    hT95_LL.GetXaxis().SetTitle("Time (sec)");
	    hT95_UL.GetXaxis().SetTitle("Time (sec)");
	    hPlateau.GetXaxis().SetTitle("Events)")
	    first=False
	    pass

	#if fabs(q_TRUE[1]/q_T90[1]-1)>0.05: continue
	hT95_med.Fill(q_T95[1])
	hT05_med.Fill(q_T05[1])
	hT95_LL.Fill(q_T95[0])
	hT95_UL.Fill(q_T95[2])
	hPlateau.Fill(q_Plateau[1])
	if q_TRUE[0]>q_T05[0] and q_TRUE[0]<q_T05[2]: inLimits[0]+=1
	if q_TRUE[1]>q_T95[0] and q_TRUE[1]<q_T95[2]: inLimits[1]+=1	
	if q_TRUE[2]>q_T90[0] and q_TRUE[2]<q_T90[2]: inLimits[2]+=1	
	nall+=1.
	pass
    
    print "%.2f+-%.2f %.2f+-%.2f  %.2f+-%.2f" %(inLimits[0]/nall,sqrt(inLimits[0])/nall,inLimits[1]/nall,sqrt(inLimits[1])/nall,inLimits[2]/nall,sqrt(inLimits[2])/nall);
    
    c = TCanvas("c","c"); ROOTDIR.Add(c)
    c.Divide(2,2); 
    c.cd(1); hT95_med.Draw();
    l=TLine(q_TRUE[1],0,q_TRUE[1],hT95_med.GetMaximum());    l.SetLineColor(3);    l.SetLineWidth(2); ROOTDIR.Add(l);
    hT95_med.Fit("landau")
    l.Draw("SAME")
    c.cd(2); hT05_med.Draw();
    l=TLine(q_TRUE[0],0,q_TRUE[0],hT05_med.GetMaximum());    l.SetLineColor(3);   l.SetLineWidth(2);  ROOTDIR.Add(l);
    l.Draw("SAME")
    c.cd(3); hT95_UL.SetMaximum(1.3*hT95_LL.GetMaximum());hT95_UL.Draw();hT95_LL.SetLineColor(2);hT95_LL.Draw("SAME");
    l=TLine(q_TRUE[1],0,q_TRUE[1],hT95_UL.GetMaximum());    l.SetLineColor(3);   l.SetLineWidth(2);  ROOTDIR.Add(l);
    l.Draw("SAME")
    c.cd(4); hPlateau.Draw();
    l=TLine(Plateau_TRUE,0,Plateau_TRUE,hPlateau.GetMaximum());    l.SetLineColor(3);  l.SetLineWidth(2);   ROOTDIR.Add(l);
    l.Draw("SAME")

    c.Update()
    
def TestEstimates():

    from ROOT import *

    ROOTDIR = gROOT.GetDirectory("/")
    cCoarse = TCanvas("cDuration_Coarse","cDuration_Coarse"); ROOTDIR.Add(cCoarse)
    cCoarse.Divide(1,3)
    gStyle.SetOptTitle(1)
    DT=1
    '''
    BKG_RATE=3
    GRB_SIG_EXTENDED_TSTART=100
    GRB_SIG_EXTENDED_PEAK_RATE=5
    GRB_SIG_EXTENDED_SLOPE=-1.5
    OBS_DURATION=2000
    '''
    #output_0
    '''
    BKG_RATE=0.1
    GRB_SIG_EXTENDED_TSTART=40
    GRB_SIG_EXTENDED_PEAK_RATE=0.2
    GRB_SIG_EXTENDED_SLOPE=-1.8
    OBS_DURATION=1000
    '''
    BKG_RATE=0.000005
    GRB_SIG_EXTENDED_TSTART=40
    GRB_SIG_EXTENDED_PEAK_RATE=0.0
    GRB_SIG_EXTENDED_SLOPE=-1.8
    OBS_DURATION=500
    
    time_bins = OBS_DURATION/DT

    #make times
    lTimes=[]
    for i in range(0,time_bins): lTimes.append((i+0.5)*DT)

    #make diff true BKG
    lbkg_diff=[]
    for i in range (0,time_bins):
	lbkg_diff.append(BKG_RATE*DT)
    
    #make diff true SIG
    lsig_diff=numpy.zeros(time_bins)

    #add extended emission
    F_0=log10(GRB_SIG_EXTENDED_PEAK_RATE)-GRB_SIG_EXTENDED_SLOPE*log10(GRB_SIG_EXTENDED_TSTART)
    for i in range (0,time_bins):
	time=lTimes[i]
	if time<GRB_SIG_EXTENDED_TSTART: continue
	lsig_diff[i]=pow(10,F_0+GRB_SIG_EXTENDED_SLOPE*log10(time))
	pass
    
    #add FREDs
    AddFRED(5,2,-2.0,lTimes,lsig_diff)
    AddFRED(8,2,-2.0,lTimes,lsig_diff)
    AddFRED(9,2,-2.0,lTimes,lsig_diff)
    AddFRED(10,0.5,-2.9,lTimes,lsig_diff)
    AddFRED(20,0.5,-4,lTimes,lsig_diff)
    AddFRED(40,0.2,-5,lTimes,lsig_diff)
    #AddFRED(80,1,-1.8,lTimes,lsig_diff)

    #######################################################################################
    gIntBkg_True = TGraphErrors();ROOTDIR.Add(gIntBkg_True);
    gIntBkg_True.SetTitle("True integrated background and signal");
    gIntBkg_True.GetXaxis().SetTitle("Time after trigger (sec)");
    gIntBkg_True.GetYaxis().SetTitle("Events");
    gIntSig_True = TGraph();ROOTDIR.Add(gIntSig_True)
    gDiffBkg_True = TGraph();ROOTDIR.Add(gDiffBkg_True)	
    gDiffSig_True = TGraph();ROOTDIR.Add(gDiffSig_True);gDiffSig_True.SetMarkerColor(2);
    gIntDet_True = TGraph();ROOTDIR.Add(gIntDet_True)

    #fill graphs
    intbkg=0;intsig=0;
    for i in range(0,time_bins):
	intbkg+=lbkg_diff[i]
	intsig+=lsig_diff[i]
	gIntBkg_True.SetPoint(i,lTimes[i],intbkg)
	gIntSig_True.SetPoint(i,lTimes[i],intsig)
	gIntDet_True.SetPoint(i,lTimes[i],intsig+intbkg)
	gDiffBkg_True.SetPoint(i,lTimes[i],lbkg_diff[i])
	gDiffSig_True.SetPoint(i,lTimes[i],lsig_diff[i])
	pass


    cSim = TCanvas("cSim","cSim");ROOTDIR.Add(cSim)
    cSim.Divide(2,2)
    
    cSim.GetPad(2).SetLogy()
    cSim.GetPad(2).SetLogx()
    cSim.GetPad(1).SetLogy()
    cSim.GetPad(1).SetLogx()
    
    cSim.cd(1); 
    gIntDet_True.SetTitle("True and simulated signal and bkg lightcurves")
    gIntDet_True.GetXaxis().SetTitle("Time after trigger (sec)")
    gIntDet_True.GetYaxis().SetTitle("Integrated number of events")
    gIntDet_True.Draw("APL"); 
    gIntBkg_True.Draw("PLSAME");
    cCoarse.cd(2); 
    gDiffSig_True.SetTitle("True GRB signal and background rates")
    gDiffSig_True.GetXaxis().SetTitle("Time after trigger (sec)");
    gDiffSig_True.GetYaxis().SetTitle("Events/bin");
    gDiffSig_True.Draw("ALP"); 
    gDiffBkg_True.Draw("LPSAME");

    cCoarse.cd(3);	
    gIntSig_True.SetTitle("Accumulated true signal");
    gIntSig_True.GetXaxis().SetTitle("Time after trigger (sec)");
    gIntSig_True.GetYaxis().SetTitle("Events");
    gIntSig_True.Draw("AC")

    q_T95=numpy.zeros(3)
    q_T05=numpy.zeros(3)
    q_T90=numpy.zeros(3)
    q_Plateau=numpy.zeros(3)
    q_DetTotal=numpy.zeros(3)
    T05=Double(0)
    T95=Double(0)
    Plateau_start=Double(0)
    Plateau_stop=Double(0)
    
    GANGSTER.FindPlateau(gDiffSig_True,gIntDet_True, gIntBkg_True, Plateau_start, Plateau_stop, 400);
    Plateau=gIntSig_True.Eval(Plateau_start);
    
    GANGSTER.FindT90(gIntSig_True,T05,T95,Plateau)
    T05_TRUE=1.*T05;T95_TRUE=1.*T95;PLATEAU_TRUE=1.*Plateau
    
    gIntBkg_Rand = TGraph();ROOTDIR.Add(gIntBkg_Rand)
    gIntDet_Rand = TGraph();ROOTDIR.Add(gIntDet_Rand);gIntDet_Rand.SetMarkerColor(2);
    gDiffBkg_Rand = TGraph();ROOTDIR.Add(gDiffBkg_Rand)
    gDiffSig_Rand = TGraph();ROOTDIR.Add(gDiffSig_Rand);gDiffSig_Rand.SetMarkerColor(2);
    gDiffDetMinusBkg_Rand = TGraph();ROOTDIR.Add(gDiffDetMinusBkg_Rand)
    gIntDet_Rand.SetTitle("Simulated signal and bkg lightcurves")
    gIntDet_Rand.GetXaxis().SetTitle("Time after trigger (sec)")
    gIntDet_Rand.GetYaxis().SetTitle("Integrated number of events")


    gIntBkg_True.SetName("gIntBkg_fine")
#    gIntBkg_Rand.SetName("gIntBkg_fine") #WRONG!
    gIntDet_Rand.SetName("gIntDet_fine")
    hT05 = TH1F("hT05","hT05",100,T05_TRUE-3*sqrt(T05_TRUE),T05_TRUE+3*sqrt(T05_TRUE));ROOTDIR.Add(hT05)
    hT95 = TH1F("hT95","hT95",100,T95_TRUE-10*sqrt(T95_TRUE),T95_TRUE+10*sqrt(T95_TRUE));ROOTDIR.Add(hT95)
    
    hT05_my = TH1F("hT05_my","hT05_my",100,T05_TRUE-3*sqrt(T05_TRUE),T05_TRUE+3*sqrt(T05_TRUE));ROOTDIR.Add(hT05_my)
    hT95_my = TH1F("hT95_my","hT95_my",100,T95_TRUE-10*sqrt(T95_TRUE),T95_TRUE+10*sqrt(T95_TRUE));ROOTDIR.Add(hT95_my)
    import os
    fPlateau = TF1("fPlateau","pol1");
    for j in range(0,50000):
	filename="output/%d.root" %j
	if os.path.exists(filename): continue
	dfile=TFile.Open(filename,"RECREATE")

	lbkg_diff_rand,lsig_diff_rand=MakeFluctuatedLC(lbkg_diff,lsig_diff)
	intbkg=0;intdet=0;intbkg_est=0;
        for i in range(0,time_bins):
	    adet=lsig_diff_rand[i]+lbkg_diff_rand[i]
	    abkg=lbkg_diff_rand[i]
	    intbkg+=abkg
	    intbkg_est+=BKG_RATE*DT
	    intdet+=adet
	    gIntBkg_Rand.SetPoint(i,lTimes[i],intbkg)
	    gIntDet_Rand.SetPoint(i,lTimes[i],intdet)
	    gDiffBkg_Rand.SetPoint(i,lTimes[i],abkg)
	    gDiffSig_Rand.SetPoint(i,lTimes[i],adet-abkg)
	    gDiffDetMinusBkg_Rand.SetPoint(i,lTimes[i],intdet-intbkg_est)
	    pass

	
        cCoarse.cd(1)
        gIntDet_Rand.Draw("AL")
	gIntBkg_True.Draw("LSAME")
#	gIntBkg_Rand.Draw("AL") #WRONG

	
	cSim.cd(3)
	gDiffSig_Rand.SetTitle("Simulated signal lightcurve");
	gDiffSig_Rand.GetXaxis().SetTitle("Time after trigger (sec)")
	gDiffSig_Rand.GetYaxis().SetTitle("Events/bin)")
	gDiffSig_Rand.Draw("AL")
	cSim.cd(4)
	gDiffBkg_Rand.SetTitle("Simulated bkg lightcurve");
	gDiffBkg_Rand.GetXaxis().SetTitle("Time after trigger (sec)")
	gDiffBkg_Rand.GetYaxis().SetTitle("Events/bin")
        gDiffBkg_Rand.Draw("AL")

	cSim.cd(2)
	gDiffDetMinusBkg_Rand.SetTitle("Simulated cumulative background-subtracted signal lightcurve")
	gDiffDetMinusBkg_Rand.GetXaxis().SetTitle("Time after trigger (sec)")
	gDiffDetMinusBkg_Rand.GetYaxis().SetTitle("Events")
	gDiffDetMinusBkg_Rand.Draw("AL")
	cSim.cd(1)
	gIntDet_Rand.SetLineColor(2)
        gIntBkg_Rand.SetLineColor(2)
	gIntDet_Rand.Draw("CSAME")
        gIntBkg_Rand.Draw("CSAME")
	#cSim.Update()
	
	cCoarse.Write()
	cSim.Write()
        FailedFraction=ROOT.Double(0)
        cDurations=GANGSTER.PerformPerturbedEstimation(q_T95,q_T05,q_T90,q_Plateau,q_DetTotal, 10000, 0,FailedFraction)
	if cDurations.__nonzero__()==0: 
	    os.unlink(filename)
	    j-=1
	    continue

	#GANGSTER.FindT95(gDiffDetMinusBkg_Rand,0,0,T05,T95,Plateau)
	dfile.cd()
	tn = TNamed("true","%f_%f_%f" %(T05_TRUE,T95_TRUE,PLATEAU_TRUE));tn.Write()
	tn = TNamed("t95","%f_%f_%f" %(q_T95[0],q_T95[1],q_T95[2]));tn.Write()
	tn = TNamed("t05","%f_%f_%f" %(q_T05[0],q_T05[1],q_T05[2]));tn.Write()
	tn = TNamed("plateau","%f_%f_%f" %(q_Plateau[0],q_Plateau[1],q_Plateau[2]));tn.Write()
	tn = TNamed("dettotal","%f_%f_%f" %(q_DetTotal[0],q_DetTotal[1],q_DetTotal[2]));tn.Write()
	cDurations.Write()
	dfile.Close()
	#sys.stdin.readline()
	pass
    #print "%f %f %f\n" %(T05,T95,Plateau)
    '''
    cResults = TCanvas("cResults","cResults");ROOTDIR.Add(cResults)
    cResults.Divide(2,2)
    cResults.cd(1); l = TLine(T05_TRUE,0,T05_TRUE,hT05.GetMaximum());l.SetLineColor(2);ROOTDIR.Add(l);hT05.Draw();l.Draw("SAME");
    cResults.cd(2); l = TLine(T95_TRUE,0,T95_TRUE,hT95.GetMaximum());l.SetLineColor(2);ROOTDIR.Add(l);hT95.Draw();l.Draw("SAME");
    cResults.Update()
    pass
    '''

def AddFRED(i0,peak,slope,times,rates):
    import math
    DT=times[1]-times[0]
    rates[i0]+=peak
    i=1
    t0=times[i0]
    F_0=log10(peak)-slope*log10(t0)
    while (1):
	if i0+i==len(times): break
	dtime=times[i0+i]-t0
	asig=pow(10,F_0+slope*log10(times[i0+i]))
	rates[i0+i]+=asig
	if asig<1e-3 : break
    	i+=1

#this gets an incoming flux and background diff curves and simulates a detection (i.e. produces a detected flux diff lcurve)
def MakeFluctuatedLC(lbkg_diff,lsig_diff):
    import random
    t=TRandom()
    t.SetSeed(random.randint(0,10000))
    lbkg_diff_rand=[]
    lsig_diff_rand=[]
    for i in range(0,lbkg_diff.__len__()):
	lsig_diff_rand.append(t.Poisson(lsig_diff[i]))
	#lbkg_diff_rand.append(t.Poisson(t.Gaus(lbkg_diff[i],0.15*lbkg_diff[i])))
	lbkg_diff_rand.append(t.Poisson(lbkg_diff[i]))
	pass
    return lbkg_diff_rand,lsig_diff_rand


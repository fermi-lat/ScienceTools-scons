def RecreateDataFiles(EMin,EMax,EBins,ZTheta_Cut,FT2_FILE,FT1_FILE_LIST,DataClass):
 
  from ROOT import GANGSTER,TOOLS,BackgroundEstimator

  Est = BackgroundEstimator(DataClass,EMin,EMax,EBins,ZTheta_Cut,False,True);
  Est.CreateDataFiles(FT1_FILE_LIST,FT2_FILE)

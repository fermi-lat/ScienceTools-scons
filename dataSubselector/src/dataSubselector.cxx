/**
 * @file dataSubselector.cxx
 * @brief  The main program body for the dataSubselection tool
 * 
 * @author Tom Stephens
 * @date Created:  17 Oct 2003
 * @date Last Modified:  $Date$
 * @version Revision:  $Revision$
 *
 * $Id$
 */



#include "../include/dataSubselector.h"

/**
 * @brief  Main program loop for the dataSubselection Tool
 * 
 * @author Tom Stephens
 * @date Created: 17 Oct 2003
 * @date Last modified 25 Nov 2003
 * 
 */
int main(int argc, char **argv)
{
  std::string *inFile,*outFile,*filter;
    try {
  
      // Here we create the Factories for IOService and Data for later use
      Goodi::DataIOServiceFactory iosvcCreator;
      Goodi::DataFactory dataCreator;
      
      // read in the parameters using hoops
      cutParameters cuts = cutParameters(argc,argv);
      
      // verify existence of input file
      Goodi::IDataIOService *ioService,*oService;  // create the ioservice
      Goodi::IEventData *idata,*odata;
      Goodi::DataType datatype = Goodi::Evt;  // Select the datatype you expect to find in the file
      inFile=cuts.getInputFilename();

      try{
        ioService = iosvcCreator.create(*inFile);  // associate it with the input file
        idata = dynamic_cast<Goodi::IEventData *>(dataCreator.create(datatype, ioService));
        std::cout << "nrows: " << idata->numEventRows() << std::endl;
//        delete data;
      }
      catch(...){
        std::cerr << "Error opening input file fv" << *inFile << "\n Exiting now.\n";
        return Goodi::Failure;
      }

      // build filtering string
      filter=cuts.getFilterExpression();
      std::cout << "filter expression is " << *filter << std::endl;
      if(DEBUG) std::cout << "Header string is " << *(cuts.getHeaderString()) << std::endl;      
      // open the input file with filter
      try{
        if(DEBUG) std::cout << "entring try block" << std::endl;
        odata = dynamic_cast<Goodi::IEventData *>(dataCreator.create(datatype, ioService));
        ioService->filter(/*idata->eventName()*/"EVENTS",*filter);
        printf("filtering complete\n");
        printf("data read\n");
        std::cout << "ioService->nrows: " << ioService->nrows("EVENTS") << std::endl;
      }
      catch(...){
        std::cerr << "Error filtering file " << inFile << "\n Exiting now.\n";
        return Goodi::Failure;
      }
      
      // write data to output file
      try {
        outFile=cuts.getOutputFilename();
        printf("Creating output service\n");
        oService = iosvcCreator.create(*outFile, Goodi::Fits, Goodi::Write);  // create the ioservice
        printf("Writing data\n");
        ioService->copyFile(oService);
        printf("Data written\n");
      }
      catch(...) {
        std::cerr << "Error writing output file " << outFile << "\n Exiting now.\n";
        return Goodi::Failure;
      }
      
      // update header keywords
      oService->makePrimaryCurrent();
      std::string keyword,svalue,comment;
      svalue="dataSubselection TOOL";
      oService->writeKey("CREATOR",svalue);
      svalue="Data was recut with the following parameters:\0";
      oService->writeHistory(svalue);
      oService->writeHistory(*(cuts.getHeaderString()));
      oService->makeCurrent("EVENTS");
      cuts.addDataSubspaceKeywords(oService);
      delete oService;
      delete ioService;
      
    }  // end main try block
    catch(...) {
      std::cerr << "dataSubselection program terminated...\n";
      return Goodi::Failure;
    }
    
//  }  //end else of if (argc<3)

  return Goodi::Success;
}

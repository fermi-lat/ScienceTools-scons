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
  char *inFile,*outFile,*filter;
/*  
  // check to be sure that we have enough information to run the command or print
  // the help information if requested.
  if (argc>1&&strcmp(argv[1],"-h")==0) {
    std::cout <<"\ndataSubselector -h  ==> Prints this message\n\n  otherwise \n\n";
    std::cout <<"dataSubselector <input file> <output file> [options]\n";
    std::cout <<"\n";
    std::cout <<"options:\n";
    std::cout <<"\n";
    std::cout <<"  -ra <value> -> RA for new search center\n";
    std::cout <<"  -dec <value> -> Dec for new search center\n";
    std::cout <<"  -rad <value> -> radius of new search region\n";
    std::cout <<"  -tmin <value> -> start time\n";
    std::cout <<"  -tmax <value> -> end time\n";
    std::cout <<"  -emin <value> -> lower energy limit\n";
    std::cout <<"  -emax <value> -> upper energy limit\n";
    std::cout <<"  -thetamin <value> -> minimum theta value\n";
    std::cout <<"  -thetamax <value> -> maximum theta value\n";
    std::cout <<"  -phimin <value> -> minimum phi value\n";
    std::cout <<"  -phimax <value> -> maximum phi value\n";
    std::cout <<"  -zmin <value> -> minimum zenith angle value\n";
    std::cout <<"  -zmax <value> -> maximum zenith angle value\n";
    std::cout <<"  -gammaProbMin <value> -> minimum quality value\n";
    std::cout <<"  -gammaProbMax <value> -> maximum quality value\n";
    std::cout <<"  -bgcut -> data passed background cut\n";
    std::cout <<"  -psfcut -> data passed PSF cut\n";
    std::cout <<"  -erescut -> data passed energy resolution cut\n";
    std::cout <<"\n";
//    std::cout <<"If the input and output file are the same the original file will be\n";
//    std::cout <<"overwritten.  ";
    std::cout <<"If no options are specified, the file will simply be\n";
    std::cout <<"copied.\n\n";
  } else if (argc<3) {
    std::cout << "\nYou need to specifiy at least an input and output file\n";
    std::cout << "usage:\n";
    std::cout << "   dataSubselector <input file> <output file> [options]\n";
    std::cout << "For a complete list of options use 'dataSubselector -h'\n\n";
  } else {  // we have a valid command so let's get started
*/    try {
  
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
        ioService = iosvcCreator.create(inFile);  // associate it with the input file
        idata = dynamic_cast<Goodi::IEventData *>(dataCreator.create(datatype, ioService));
        std::cout << "nrows: " << idata->numEventRows() << std::endl;
//        delete data;
      }
      catch(...){
        std::cerr << "Error opening input file fv" << inFile << "\n Exiting now.\n";
        return Goodi::Failure;
      }

      // build filtering string
      filter=cuts.getFilterExpression();
      printf("filter expression is %s\n",filter);
      printf("Header string is %s\n",cuts.getHeaderString());      
      // open the input file with filter
      try{
        printf("entring try block\n");
        odata = dynamic_cast<Goodi::IEventData *>(dataCreator.create(datatype, ioService));
        ioService->filter(/*idata->eventName()*/"EVENTS",filter);
        printf("filtering complete\n");
//        odata->read(ioService,1,ioService->nrows("Events"));
//        odata->read(ioService);
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
        oService = iosvcCreator.create(outFile, Goodi::Fits, Goodi::Write);  // create the ioservice
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
      oService->writeHistory(cuts.getHeaderString());
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

// File: BackgroundMixer.h
//
//
#ifndef BACKGROUND_MIXER_H
#define BACKGROUND_MIXER_H

#include <vector>
#include <string>
#include <fstream>


#include "PhotonInfo.h"


class BackgroundMixer
{
 public:
	 // Constructors
	 BackgroundMixer(const std::string &grbFile, const std::string &backgroundFile, const std::string &mixedDir, 
		 const double grbOffsetTime);
	 BackgroundMixer(const std::string &mixedFile);

	  
	 // No memory management function required
	 // So - need for destructor, copy constructor and assignment operator to be defined

	 std::string grbFile() const                        { return m_grbFile; }
	 std::string backgroundFile() const                 { return m_backgroundFile; }
	 std::string mixedDir() const                       { return m_mixedDir; }
	 std::string mixedFile() const                      { return m_mixedFile; }
	 double grbOffsetTime() const                       { return m_grbOffsetTime; }
	 const std::vector<PhotonInfo> &photonData() const  { return m_photonData; }
	 long ngrb() const                                  { return m_nGrb; }
	 long nbck() const                                  { return m_nBck; }


 private:

	 void mix();
	 void readBackground();
	 void readGRB();
	 void readMixedData();
	 void writeMixedData();


	 // data members
	 std::string              m_grbFile;
	 std::string              m_backgroundFile;
	 std::string              m_mixedDir;
	 std::string              m_mixedFile;
	 double					  m_grbOffsetTime;
	 std::vector<PhotonInfo>  m_photonData;
	 long                     m_nGrb;
	 long                     m_nBck;
};


class timeCmp
{
 public:
    bool operator()(PhotonInfo &data1, PhotonInfo &data2)
	{
	   return data1.time() < data2.time();    
	}
};



#endif // BACKGROUND_MIXER_H

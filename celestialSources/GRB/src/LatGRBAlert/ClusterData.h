// ClusterData.h
//
//
#ifndef CLUSTER_DATA_H
#define CLUSTER_DATA_H

#include <vector>
#include <functional>  // for unary_function

#include "PhotonInfo.h"
#include "ClusterInfo.h"




class ClusterData
{
 public:
	 ClusterData();

	 const double triggerLikelihood(const std::vector<PhotonInfo> &photonData);

 private:
	 void   calcAvgDistance();
	 void   calcUseDistance();
	 const double cosDistance(const long index1, const long index2) const;
	 void   fillUseDistance(const long index);
	 void   extractClusterData(const long chosenphot);
	 const double jointLikelihood() const;
	 const double likelihood(const double dist, const double dt, const double psf) const;
	 const double pdistValue(const double distance) const;
	 const double pdtValue(const double deltaTime) const;
	 const double psfValue(const double energy, const double denom) const;


	 // Data members
	 std::vector<PhotonInfo>           m_photonData;
	 std::vector<std::vector<double> > m_useDist;
	 std::vector<double>               m_avgDist;
	 std::vector<ClusterInfo>          m_extractedClusterData;
	 double                            m_jointLike;




	 struct greater_equal : public std::unary_function<ClusterInfo, ClusterInfo>
	 {
		 greater_equal(double value) : m_value(value) {}
		 const bool operator() (ClusterInfo &x) const;

		 double m_value;
	 };


	 struct less : public std::unary_function<ClusterInfo, ClusterInfo>
	 {
		 less(double value) : m_value(value) {}
		 const bool operator() (ClusterInfo &x) const;

		 double m_value;
	 };
};

class distCmp
{
 public:
    bool operator()(ClusterInfo &data1, ClusterInfo &data2)
	{
	   return data1.dist() < data2.dist();    
	}
};


#endif
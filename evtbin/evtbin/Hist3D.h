/** \file Hist3D.h
    \brief Three dimensional histogram.
*/
#ifndef evtbin_Hist3D_h
#define evtbin_Hist3D_h

#include <vector>

#include "evtbin/Hist.h"

namespace evtbin {

  class Binner;

  /** \class Hist3D
      \brief One dimensional histogram.
  */
  class Hist3D : public Hist {
    public:
      typedef std::vector<std::vector<std::vector<double> > > Cont_t;
      typedef Cont_t::const_iterator ConstIterator1;
      typedef std::vector<std::vector<double> >::const_iterator ConstIterator2;

      /** \brief Create a one dimensional histogram which uses the given binner objects
          to determine the indices.
          \param binner1 The binner object for the first dimension.
          \param binner2 The binner object for the second dimension.
          \param binner3 The binner object for the third dimension.
      */
      Hist3D(const Binner & binner1, const Binner & binner2, const Binner & binner3);

      virtual ~Hist3D() throw();

      /** \brief Increment the bin appropriate for the given value.
                 This is generic for N-dimensional histograms.
          \param value Vector giving the value being binned. The vector must have at least as
                 many values as the dimensionality of the histogram.
      */
      virtual void fillBin(const std::vector<double> & value, double weight = 1.);

      /** \brief Fill output vector with a 1-d representation of the histogram, suitable for storing as an image.
          \param image The output vector.
      */
      virtual void getImage(std::vector<float> & image) const;

      /** \brief Increment the bin appropriate for the given value.
          \param value1 The value being binned by the first binner.
          \param value2 The value being binned by the second binner.
          \param value3 The value being binned by the third binner.
      */
      void fillBin(double value1, double value2, double value3, double weight = 1.);

      const std::vector<std::vector<double> > & operator [](Cont_t::size_type index) const;

      ConstIterator1 begin() const;

      ConstIterator1 end() const;

    private:
      Cont_t m_data;
  };

  inline const std::vector<std::vector<double> > & Hist3D::operator [](Cont_t::size_type index) const { return m_data[index]; }

  inline Hist3D::ConstIterator1 Hist3D::begin() const { return m_data.begin(); }

  inline Hist3D::ConstIterator1 Hist3D::end() const { return m_data.end(); }

}

#endif

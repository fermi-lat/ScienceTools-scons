/** \file RootPlot.h
    \brief Interface of RootPlot class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootPlot_h
#define st_graph_RootPlot_h

#include <string>
#include <vector>

#include "st_graph/IPlot.h"
#include "st_graph/Sequence.h"

class TGraph;
class TH2D;
class TMultiGraph;

namespace st_graph {

  class IFrame;
  class ISequence;
  class RootPlotFrame;

  /** \class RootPlot
      \brief A Root implementation of the IFrame interface. This specific class displays a plot in its frame.
  */
  class RootPlot : public IPlot {
    public:
      /** \brief Construct a RootPlot object.
          \param parent The parent frame.
          \param style The style of the plot. Currently, either hist* or scat*, case insensitive, will produce
                 a histogram or scatter plot, respectively.
          \param x The first dimension.
          \param y The second dimension.
      */
      RootPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y);

      /** \brief Construct a RootPlot object.
          \param parent The parent frame.
          \param style The style of the plot.
          \param x The first dimension.
          \param y The second dimension.
          \param z The third dimension.
      */
      RootPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y,
        const std::vector<std::vector<double> > & z);

      virtual ~RootPlot();

      /// \brief Display this plot.
      virtual void display();

      /// \brief Hide this plot.
      virtual void unDisplay();

      /** \brief Create histogram plot as a Root object. Not part of API.
          \param x The first dimension.
          \param y The second dimension.
      */
      TGraph * createHistPlot(const ISequence & x, const ISequence & y);

      /** \brief Create scatter plot as a Root object. Not part of API.
          \param x The first dimension.
          \param y The second dimension.
      */
      TGraph * createScatterPlot(const ISequence & x, const ISequence & y);

      TH2D * createHistPlot2D(const ISequence & x, const ISequence & y, const std::vector<std::vector<double> > & z);

      /** \brief Return current Root graphical object. Not part of API.
      */
      TGraph * getTGraph();

    private:
      RootPlotFrame * m_parent;
      TMultiGraph * m_multi_graph;
      TGraph * m_graph;
      TH2D * m_th2d;
  };
}

#endif

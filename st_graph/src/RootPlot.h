/** \file RootPlot.h
    \brief Interface of RootPlot class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootPlot_h
#define st_graph_RootPlot_h

#include <string>

#include "st_graph/IFrame.h"
#include "st_graph/ValueSet.h"

class TGraph;

namespace st_graph {

  /** \class RootPlot
      \brief A Root implementation of the IFrame interface. This specific class displays a plot in its frame.
  */
  class RootPlot : public IFrame {
    public:
      /** \brief Construct a RootPlot object.
          \param parent The parent frame.
          \param style The style of the plot. Currently, either hist* or scat*, case insensitive, will produce
                 a histogram or scatter plot, respectively.
          \param x The first dimension.
          \param y The second dimension.
          \param z The third dimension.
      */
      RootPlot(IFrame * parent, const std::string & style, const std::string & title, const ValueSet & x, const ValueSet & y,
        const ValueSet & z = ValueSet());

      virtual ~RootPlot();

      /// \brief Display this plot.
      virtual void display();

      /// \brief Hide this plot.
      virtual void unDisplay();

      /** \brief Add the given (sub) frame to this container frame.
          \param frame The frame being added.
      */
      virtual void addFrame(IFrame * frame);

      /** \brief Remove the given (sub) frame to this container frame. If the frame is not currently in the container,
                 no harm done.
          \param frame The frame being removed.
      */
      virtual void removeFrame(IFrame * frame);

      /// \brief Get the X position of the left edge of the frame.
      virtual long getL() const;

      /** \brief Set the X position of the left edge of the frame.
          \param l The new position of the left edge.
      */
      virtual void setL(long l);

      /// \brief Get the X position of the right edge of the frame.
      virtual long getR() const;

      /** \brief Set the X position of the left edge of the frame.
          \param l The new position of the left edge.
      */
      virtual void setR(long r);

      /** \brief Create histogram plot as a Root object. Not part of API.
          \param title The plot title.
          \param x The first dimension.
          \param y The second dimension.
          \param z The third dimension.
      */
      TGraph * createHistPlot(const std::string & title, const ValueSet & x, const ValueSet & y, const ValueSet & z);

      /** \brief Create scatter plot as a Root object. Not part of API.
          \param title The plot title.
          \param x The first dimension.
          \param y The second dimension.
          \param z The third dimension.
      */
      TGraph * createScatterPlot(const std::string & title, const ValueSet & x, const ValueSet & y, const ValueSet & z);

      /** \brief Return current Root graphical object. Not part of API.
      */
      TGraph * getTGraph();

    private:
      IFrame * m_parent;
      TGraph * m_graph;
  };
}

#endif

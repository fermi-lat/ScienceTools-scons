/** \file RootEngine.h
    \brief Declaration for class which encapsulates the Root graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootEngine_h
#define st_graph_RootEngine_h

#include <list>
#include <string>
#include <vector>

#include "st_graph/Engine.h"

namespace st_graph {

  class IFrame;
  class ISequence;

  /** \class RootEngine
      \brief Declaration for class which encapsulates the Root graphics implementation.
  */
  class RootEngine : public Engine {
    public:
      /// \brief Create the Root graphics engine. Creates a Root TApplication object.
      RootEngine();

      /** \brief Run the graphics engine, displaying all graphical objects currently constructed.
          This runs the Root TApplication object to handle window event loop.
      */
      virtual void run();

      /// \brief Stop the graphics engine, undisplaying all graphical objects currently constructed.
      virtual void stop();

      /** \brief Create a self-contained two dimensional plot window.
          \param title The title of the plot.
          \param width The width of the plot frame.
          \param height The height of the plot frame.
          \param style The type of plot, e.g. hist, scat.
          \param x The first dimension being plotted, giving the bin definitions.
          \param y The second dimension being plotted, giving the bin values.
      */
      virtual IPlot * createPlot(const std::string & title, unsigned int width, unsigned int height, const std::string & style,
        const ISequence & x, const ISequence & y);

      /** \brief Create a self-contained three dimensional plot window.
          \param title The title of the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
          \param style The type of plot, e.g. hist, scat.
          \param x The first dimension being plotted, giving the x bin definitions.
          \param y The second dimension being plotted, giving the y bin definitions.
          \param z The third dimension being plotted.
      */
      virtual IPlot * createPlot(const std::string & title, unsigned int width, unsigned int height, const std::string & style,
        const ISequence & x, const ISequence & y, const std::vector<std::vector<double> > & z);

      /** \brief Create a top-level independent frame on the desktop. This frame's purpose is to hold other frames.
          \param receiver The receiver of GUI signals.
          \param width The width of the window.
          \param height The height of the window.
          \param title The title to display on the window.
      */
      virtual IFrame * createMainFrame(IEventReceiver * receiver, unsigned int width, unsigned int height,
        const std::string & title = "");

      /** \brief Create a plot which may be displayed in a plot frame.
          \param parent The parent frame in which the plot will be displayed. This must have been created by
                 createPlotFrame.
          \param style The plot style: currently hist* or scat* will be recognized, case insensitive, to mean
                 histogram or scatter plot, respectively.
          \param x The first dimension being plotted.
          \param y The first dimension being plotted.
      */
      virtual IPlot * createPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y);

      /** \brief Create a plot which may be displayed in a plot frame.
          \param parent The parent frame in which the plot will be displayed. This must have been created by
                 createPlotFrame.
          \param style The plot style:
          \param x The first dimension being plotted.
          \param y The second dimension being plotted.
          \param z The third dimension being plotted.
      */
      virtual IPlot * createPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y,
        const std::vector<std::vector<double> > & z);

      /** \brief Create a frame specifically devoted to holding plots.
          \param parent The frame in which to embed the plot frame.
          \param title The title of the plot.
          \param width The width of the frame in pixels.
          \param height The height of the frame in pixels.
      */
      virtual IFrame * createPlotFrame(IFrame * parent, const std::string & title, unsigned int width, unsigned int height);

      /** \brief Create a button whose events are bound to the given event receiver object.
          \param parent The frame in which to embed the button.
          \param receiver The event receiver which will process events from the button (clicks etc.)
          \param style The style of button, e.g. text, radio, etc.
          \param label The label appearing on the button.
      */
      virtual IFrame * createButton(IFrame * parent, IEventReceiver * receiver, const std::string & style,
        const std::string & label);

      virtual IFrame * createLabel(IFrame * parent, IEventReceiver * receiver, const std::string & label);

      virtual IFrame * createTextEntry(IFrame * parent, IEventReceiver * receiver, const std::string & content);

      virtual IFrame * createComposite(IFrame * parent, IEventReceiver * receiver);

      virtual IFrame * createGroupFrame(IFrame * parent, IEventReceiver * receiver, const std::string & label);

      virtual ITabFolder * createTabFolder(IFrame * parent, IEventReceiver * receiver);

      virtual std::string fileDialog(IFrame * parent, const std::string & initial_file_name, const std::string & style = "open");

      /// \brief Return whether graphics engine was successfully initialized.
      virtual bool initSucceeded() const { return m_init_succeeded; }

    private:
      void hideHidden(IFrame * frame);

      bool m_init_succeeded;
  };

}

#endif

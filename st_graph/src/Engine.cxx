/** \file Engine.cxx
    \brief Implementation of class which encapsulates a particular graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#include <stdexcept>

#ifndef BUILD_WITHOUT_ROOT
#include "RootEngine.h"
#endif

#include "st_graph/Engine.h"

namespace {
  using namespace st_graph;

#ifdef BUILD_WITHOUT_ROOT
  class NoOpEngine : public Engine {
    public:
      /// \brief Run the graphics engine, displaying all graphical objects currently constructed.
      virtual void run() {}

      /// \brief Stop the graphics engine, undisplaying all graphical objects currently constructed.
      virtual void stop() {}

      /** \brief Create a self-contained two dimensional plot window.
          \param title The title of the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
          \param style The type of plot, e.g. hist, scat.
          \param x The first dimension being plotted, giving the bin definitions.
          \param y The second dimension being plotted, giving the bin values.
      */
      virtual IPlot * createPlot(const std::string & title, unsigned int /* width */, unsigned int /* height */,
        const std::string & /* style */, const ISequence & /* x */, const ISequence & /* y */) {
        throw std::runtime_error("Cannot create plot " + title + "; graphical functions disabled.");
        return 0;
      }

      /** \brief Create a self-contained three dimensional plot window.
          \param title The title of the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
          \param style The type of plot, e.g. hist, scat.
          \param x The first dimension being plotted, giving the x bin definitions.
          \param y The second dimension being plotted, giving the y bin definitions.
          \param z The third dimension being plotted.
      */
      virtual IPlot * createPlot(const std::string & title, unsigned int /* width */, unsigned int /* height */,
        const std::string & /* style */, const ISequence & /* x */, const ISequence & /* y */,
        const std::vector<std::vector<double> > & /* z */) {
        throw std::runtime_error("Cannot create plot " + title + "; graphical functions disabled.");
        return 0;
      }

      /** \brief Create a top-level independent frame on the desktop. This frame's purpose is to hold other frames.
          \param receiver The receiver of GUI signals.
          \param width The width of the window.
          \param height The height of the window.
          \param title The title to display on the window.
      */
      virtual IFrame * createMainFrame(IEventReceiver * /* receiver */, unsigned int /* width */, unsigned int /* height */,
        const std::string & title = "") {
        throw std::runtime_error("Cannot create graphical main frame; graphical functions disabled.");
        return 0;
      }

      /** \brief Create a plot which may be displayed in a plot frame.
          \param parent The parent frame in which the plot will be displayed. This must have been created by
                 createPlotFrame.
          \param style The plot style: currently hist* or scat* will be recognized, case insensitive, to mean
                 histogram or scatter plot, respectively.
          \param x The first dimension being plotted.
          \param y The second dimension being plotted.
      */
      virtual IPlot * createPlot(IFrame * /* parent */, const std::string & style, const ISequence & /* x */,
        const ISequence & /* y */) {
        throw std::runtime_error("Cannot create " + style + " plot; graphical functions disabled.");
        return 0;
      }

      /** \brief Create a plot which may be displayed in a plot frame.
          \param parent The parent frame in which the plot will be displayed. This must have been created by
                 createPlotFrame.
          \param style The plot style:
          \param x The first dimension being plotted.
          \param y The second dimension being plotted.
          \param z The third dimension being plotted.
      */
      virtual IPlot * createPlot(IFrame * /* parent */, const std::string & style, const ISequence & /* x */,
        const ISequence & /* y */, const std::vector<std::vector<double> > & /* z */) {
        throw std::runtime_error("Cannot create " + style + " plot; graphical functions disabled.");
        return 0;
      }

      /** \brief Create a frame specifically devoted to holding plots.
          \param parent The frame in which to embed the plot frame.
          \param title The title of the plot.
          \param width The width of the frame in pixels.
          \param height The height of the frame in pixels.
      */
      virtual IFrame * createPlotFrame(IFrame * /* parent */, const std::string & title, unsigned int /* width */,
        unsigned int /* height */) {
        throw std::runtime_error("Cannot create plotting frame " + title + "; graphical functions disabled.");
        return 0;
      }

      /** \brief Create a button whose events are bound to the given event receiver object.
          \param parent The frame in which to embed the button.
          \param receiver The event receiver which will process events from the button (clicks etc.)
          \param style The style of button, e.g. text, radio, etc.
          \param label The label appearing on the button.
      */
      virtual IFrame * createButton(IFrame * /* parent */, IEventReceiver * /* receiver */, const std::string & style,
        const std::string & /* text */) {
        throw std::runtime_error("Cannot create " + style + " button; graphical functions disabled.");
        return 0;
      }

      virtual IFrame * createLabel(IFrame * /* parent */, IEventReceiver * /* receiver */, const std::string & label) {
        throw std::runtime_error("Cannot create label " + label + "; graphical functions disabled.");
        return 0;
      }

      virtual IFrame * createTextEntry(IFrame * /* parent */, IEventReceiver * /* receiver */, const std::string & content) {
        throw std::runtime_error("Cannot create text entry widget " + content + "; graphical functions disabled.");
        return 0;
      }

      virtual IFrame * createComposite(IFrame * /* parent */, IEventReceiver * /* receiver */) {
        throw std::runtime_error("Cannot create composite frame; graphical functions disabled.");
        return 0;
      }

      virtual IFrame * createGroupFrame(IFrame * /* parent */, IEventReceiver * /* receiver */, const std::string & label) {
        throw std::runtime_error("Cannot create group frame " + label + "; graphical functions disabled.");
        return 0;
      }

      virtual ITabFolder * createTabFolder(IFrame * /* parent */, IEventReceiver * /* receiver */) {
        throw std::runtime_error("Cannot create tab folder; graphical functions disabled.");
        return 0;
      }

      virtual std::string fileDialog(IFrame * /* parent */, const std::string & /* initial_file_name */,
        const std::string & style = "open") {
        throw std::runtime_error("Cannot create file dialog box; graphical functions disabled.");
        return 0;
      }

      /** \brief Determine whether windows cause the application to terminate by default when they are closed.
                 Windows with an explicit event receiver object are not affected by this.
          \param exit_on_close Flag indicating whether to exit or not.
      */
      virtual void setDefaultExitOnClose(bool) {}
  };
#endif

}

namespace st_graph {

  Engine::~Engine() {}

  Engine & Engine::instance() {
#ifndef BUILD_WITHOUT_ROOT
    static RootEngine s_engine;
#else
    static NoOpEngine s_engine;
#endif
    return s_engine;
  }

  Engine::Engine() {}

}

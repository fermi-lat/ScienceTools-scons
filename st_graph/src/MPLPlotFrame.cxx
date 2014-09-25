/** \file MPLPlotFrame.cxx
    \brief Implementation for MPLPlotFrame class.
    \author Tom Stephens, HEASARC/GSSC
*/

#include <EmbedPython.h>
#include <algorithm>
#include <cctype>
#include <list>
#include <map>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <iostream>

#include "MPLPlot.h"
#include "MPLPlotFrame.h"

#include "st_graph/IEventReceiver.h"

namespace st_graph {

  MPLPlotFrame::MPLPlotFrame(IFrame * parent, const std::string & title, unsigned int width, unsigned int height,
    bool delete_parent): MPLFrame(parent, 0, 0, delete_parent), m_axes(3), m_plots(), m_graphs(), m_title(title), m_canvas(0),
    m_multi_graph(0), m_th2d(Py_None), m_dimensionality(0) {
    
    // Send event messages back to parent.
    m_receiver = m_parent->getReceiver();

    // Hook together MPL primitives.
    PyObject * root_frame = m_parent->getPythonFrame();
    if (0 == root_frame)
      throw std::logic_error("MPLPlotFrame constructor was passed a parent frame which cannot contain other MPL frames");
    EP_CallMethod(root_frame,"title","(s)",title.c_str());

    // make a subframe to hold the plot and toolbar
    PyObject * subFrame = EP_CallMethod("Tkinter","Frame","(O)",root_frame);
    EP_CallMethod(subFrame,"pack","()");

    //convert width and height to inches at 100 dpi
    PyObject *h = PyFloat_FromDouble((double)height/100);
    PyObject *w = PyFloat_FromDouble((double)width/100);

    // define the sub-plot parameters
    PyObject *subPlotPars = EP_CreateObject("matplotlib.figure","SubplotParams","(ffff)",0.125,0.15,0.9,0.9);  // parameters are left,bottom,right,top

    // Create the matplotlib figure
  	PyObject *kwargs = PyDict_New();
  	PyDict_SetItemString(kwargs,"facecolor",PyString_FromString("w"));
  	PyDict_SetItemString(kwargs,"edgecolor",PyString_FromString("w"));
  	PyDict_SetItemString(kwargs,"subplotpars",subPlotPars);
    PyObject * fig = EP_CreateKWObject("matplotlib.figure","Figure",kwargs,"((OO)i)",w,h,100);
    Py_DECREF(kwargs);

//    PyObject * fig = EP_CreateObject("matplotlib.figure","Figure","((OO)i)",w,h,100);
    Py_DECREF(h);
    Py_DECREF(w);
    PyObject *canvas =  EP_CallMethod("matplotlib.backends.backend_tkagg","FigureCanvasTkAgg","(OO)",fig,subFrame);
    EP_CallMethod(canvas,"show","()");
    PyObject *pwidget = EP_CallMethod(canvas,"get_tk_widget","()");
    EP_CallMethod(pwidget,"pack","()");
    Py_DECREF(pwidget);

    // add the toolbar
    PyObject *toolbar = EP_CallMethod("matplotlib.backends.backend_tkagg", "NavigationToolbar2TkAgg","(OO)",canvas,subFrame);
    EP_CallMethod(toolbar,"update","()");
    pwidget = EP_GetMethod(canvas,"_tkcanvas");
    EP_CallMethod(pwidget,"pack","()");
    Py_DECREF(pwidget);
    Py_DECREF(toolbar);

    // creating the figure creates an axis that we need to turn off as we can't seem to access it properly later
	PyObject * axes = EP_CallMethod(fig,"gca","()");
	EP_CallMethod(axes,"set_visible","(O)",Py_False);
	Py_DECREF(axes);

    m_canvas = canvas;

    m_frame = fig;
  }

  MPLPlotFrame::~MPLPlotFrame() {

	reset();
    Py_DECREF(m_th2d);
	Py_DECREF(m_frame);
//    std::cout <<"Called MPLPlotFrame::~MPLPlotFrame()" << std::endl;

  }

  void MPLPlotFrame::display() {
//    MPLFrame::display();
//	  std::cout << "Displaying " << m_title << std::endl;

    try {
      // Display plot correctly for the current dimensionality. Get Root axes objects.
      if (m_dimensionality == 2) display2d();
      else if (m_dimensionality == 3) display3d();

      // Handle log/linear scaling.
      PyObject *axes = EP_CallMethod(m_frame,"gca","()");
      if (0 < m_dimensionality) EP_CallMethod(axes,"set_xscale","(s)",(Axis::eLog == m_axes[0].getScaleMode() ? "log" : "linear"));
      if (1 < m_dimensionality) EP_CallMethod(axes,"set_yscale","(s)",(Axis::eLog == m_axes[1].getScaleMode() ? "log" : "linear"));
      if (2 < m_dimensionality) EP_CallMethod(axes,"set_zscale","(s)",(Axis::eLog == m_axes[1].getScaleMode() ? "log" : "linear"));

//      EP_CallMethod(axes,"set_adjustable","(s)","datalim");
      EP_CallMethod(axes,"set_title","(s)",m_title.c_str());
      // Set axis labels
      if (m_dimensionality == 2){
    	  EP_CallMethod(axes,"set_xlabel","(sOO)",m_axes[0].getTitle().c_str(),Py_None,Py_None);
    	  EP_CallMethod(axes,"set_ylabel","(sOO)",m_axes[1].getTitle().c_str(),Py_None,Py_None);
      } else if (m_dimensionality == 3){
    	  axes = EP_CallMethod(m_frame,"gca","()");
    	  EP_CallMethod(axes,"set_xlabel","(sOO)",m_axes[0].getTitle().c_str(),Py_None,Py_None);
    	  EP_CallMethod(axes,"set_ylabel","(sOO)",m_axes[1].getTitle().c_str(),Py_None,Py_None);
    	  EP_CallMethod(axes,"set_zlabel","(sOO)",m_axes[2].getTitle().c_str(),Py_None,Py_None);
      }
      Py_DECREF(axes);

      // Get labels/markers from IPlots.
      for (std::list<MPLPlot *>::iterator itor = m_plots.begin(); itor != m_plots.end(); ++itor) {
        // Loop over plots, displaying each one's labels.
        std::vector<Marker> & marker((*itor)->getMarkers());
        for (std::vector<Marker>::iterator itor = marker.begin(); itor != marker.end(); ++itor) {
          addMarker(*itor);
        }
      }

    } catch (...) {
      throw;
    }

    // Force complete update of the display.
    EP_CallMethod(m_canvas,"show","()");
//    EP_CallMethod(m_canvas,"draw","()");
    // @todo call figure.canvas.draw() to redraw final plot?
  }

  void MPLPlotFrame::unDisplay() {
    // Delete all child graphs.
    for (std::list<PyObject *>::reverse_iterator itor = m_graphs.rbegin(); itor != m_graphs.rend(); ++itor) {
    	Py_DECREF(*itor);
    }
//    m_graphs.clear();
    MPLFrame::unDisplay();
  }

  void MPLPlotFrame::reset() {
    // Reset canvas.
	PyObject *pwidget = EP_CallMethod(m_canvas,"get_tk_widget","()");
	EP_CallMethod(pwidget,"destroy","()");
	Py_DECREF(pwidget);
    if (0 != m_canvas) {
    	Py_DECREF(m_canvas);
    	m_canvas = 0;
    }

    // Delete children.
    while (!m_plots.empty()) {
      // Find last child.
      std::list<MPLPlot *>::iterator itor = --m_plots.end();

      // Get pointer to the plot.
      MPLPlot * plot = *itor;

      // Break links between this and the child plot.
      removePlot(*itor);

      // Delete the child plot.
      delete plot;
    }

    unDisplay();
  }

  void MPLPlotFrame::addPlot(IPlot * plot) {
    MPLPlot * mpl_plot = dynamic_cast<MPLPlot *>(plot);
    if (0 == mpl_plot) throw std::logic_error("MPLPlotFrame::addPlot cannot add a non-MPL plot");

    if (m_plots.empty()) m_dimensionality = mpl_plot->getDimensionality();
    else if (m_dimensionality != mpl_plot->getDimensionality())
      throw std::logic_error("MPLPlotFrame::addPlot cannot overlay plots with different numbers of dimensions");
    else if (m_dimensionality > 2)
      throw std::logic_error("MPLPlotFrame::addPlot cannot overlay 3d plots");

    // Make certain plot is not added more than once.
    if (m_plots.end() == std::find(m_plots.begin(), m_plots.end(), mpl_plot)) {
      m_plots.push_back(mpl_plot);
      mpl_plot->setParent(this);
    }
  }

  void MPLPlotFrame::removePlot(IPlot * plot) {
    std::list<MPLPlot *>::iterator itor = std::find(m_plots.begin(), m_plots.end(), plot);
    if (m_plots.end() != itor) {
      MPLPlot * mpl_plot = dynamic_cast<MPLPlot *>(plot);
      if (0 != mpl_plot) mpl_plot->setParent(0);
      m_plots.erase(itor);
    }
  }

  void MPLPlotFrame::addMarker(Marker & marker) {
  	// Draw the point.  Python doesn't have a way to add just a point so you have to draw a scatter plot overlay
	PyObject * axes = EP_CallMethod(m_frame,"gca","()");
	//PyObject * axes = EP_CallMethod(m_frame,"add_subplot","(s)","111");  	// Draw the point.  Python doesn't have a way to add just a point so you have to draw a scatter plot overlay
  	EP_CallMethod(axes,"set_autoscale_on","(O)",Py_False); // turn off autoscaling so the plot doesn't change size
  	EP_CallMethod(axes,"scatter","([d][d]iss)",marker.m_x,marker.m_y,20,getColorString(marker.m_color).c_str(),"v");
  	PyObject *kwargs = PyDict_New();
  	PyDict_SetItemString(kwargs,"color",PyString_FromString(getColorString(marker.m_color).c_str()));
  	PyDict_SetItemString(kwargs,"rotation",PyFloat_FromDouble(45));
  	PyDict_SetItemString(kwargs,"verticalalignment",PyString_FromString("bottom"));
  	EP_CallKWMethod(axes,"annotate",kwargs,"(s(dd))",marker.m_text.c_str(),marker.m_x,marker.m_y);
  	Py_DECREF(kwargs);
  	Py_DECREF(axes);
  }

  const std::string & MPLPlotFrame::getTitle() const {
    return m_title;
  }

  std::vector<Axis> & MPLPlotFrame::getAxes() { return m_axes; }

  const std::vector<Axis> & MPLPlotFrame::getAxes() const { return m_axes; }

  void MPLPlotFrame::display2d() {
//	  std::cout << "display2D() for " << m_title << std::endl;

    for (std::list<MPLPlot *>::iterator itor = m_plots.begin(); itor != m_plots.end(); ++itor) {

      // Get numeric sequences from data.
      const std::vector<const ISequence *> sequences((*itor)->getSequences());

      // Unpack the sequences: first dimension is the x axis, second is the y.
      const ISequence * x = sequences.at(0);
      const ISequence * y = sequences.at(1);

      // Determine the style of the graph.
      std::string style = (*itor)->getStyle();

      // For matplotlib you want to send the formatting in with the object when it is created
      //  not change it after creation (it can be done, this is just much easier)
      std::string format = generateFormatString((*itor));

      // Depending on the style, create appropriate MPL plot object.
      PyObject * graph = 0;
      if (style == "hist")
        graph = createHistPlot(*x, *y, format);
      else
        graph = createScatterPlot(*x, *y, format);

      // Keep track of MPL object, so it can be deleted later.
      m_graphs.push_back(graph);

    }
  }

  void MPLPlotFrame::display3d() {
//	  std::cout << "display3D() for " << m_title << std::endl;

    if (m_plots.empty()) return;
    std::list<MPLPlot *>::iterator itor = m_plots.begin();

    // Get numeric sequences from data.
    const std::vector<const ISequence *> sequences((*itor)->getSequences());

    // Unpack the sequences: first dimension is the x axis, second is the y.
    const ISequence * x = sequences.at(0);
    const ISequence * y = sequences.at(1);

    // Get data being plotted.
    const std::vector<std::vector<double> > & z((*itor)->getZData());

    // Create MPL plotting object.
    m_th2d = createHistPlot2D(createRootName("H2D", *itor), *x, *y, z);

  }

  PyObject * MPLPlotFrame::createHistPlot(const ISequence & x, const ISequence & y,std::string format) {
    PyObject * retval = 0;
//	std::cout << "createHistPlot() for " << m_title << std::endl;

    // Get arrays of values.
    std::vector<double> x_low;
    std::vector<double> x_high;
    std::vector<double> y_value;

    // Interpret x as a set of intervals.
    x.getIntervals(x_low, x_high);

    // Interpret y as the value in each interval.
    y.getValues(y_value);

    // Combine ranges and values into one array for axis and one array for the data; needed for TGraph.
    std::vector<double> x_vals(x_low.size() * 4);
    std::vector<double> y_vals(x_low.size() * 4);

    // Use input arrays to create graphable data.
    unsigned long idx = 0;
    unsigned long ii = 0;

#if 0
    // First point plotted is at the base of the first bin.
    x_vals[idx] = x_low[ii];
    y_vals[idx] = 0.;

    ++idx;
#endif
    for (ii = 0; ii < x_low.size(); ++ii, ++idx) {
      // Plot the y value at the left edge.
      x_vals[idx] = x_low[ii];
      y_vals[idx] = y_value[ii];

      // Next plot the y value at the right edge.
      ++idx;
      x_vals[idx] = x_high[ii];
      y_vals[idx] = y_value[ii];

      // Exclude the last bin, which requires special handling.
      if (ii != x_low.size() - 1) {
        // See if the next bin's left edge is > than the right edge which was just plotted.
        double next = x_low[ii + 1];
        if (next > x_vals[idx]) {
          // There is a gap in the data, so plot a value of 0. in the gap.
          ++idx;
          x_vals[idx] = x_vals[idx - 1];
          y_vals[idx] = 0.;
          ++idx;
          x_vals[idx] = next;
          y_vals[idx] = 0.;
        }
#if 0
      } else {
        // Last input point is the last right edge, which should drop to 0.
        ++idx;
        x_vals[idx] = x_vals[idx - 1];
        y_vals[idx] = 0.;
#endif
      }
    }

    // Create the graph.
    // You can't pass vectors as arguments in a variable length argument list (get Illegal Instruction error)
    //    so lets make them into python lists
    PyObject *pyX = PyList_New(0);
    PyObject *pyY = PyList_New(0);
    for (unsigned int i = 0; i < idx; ++i){
    	PyList_Append(pyX,PyFloat_FromDouble(x_vals[i]));
    	PyList_Append(pyY,PyFloat_FromDouble(y_vals[i]));
    }
    // Set some formating keyword arguments
	PyObject *kwargs = PyDict_New();
	PyDict_SetItemString(kwargs,"linewidth",PyFloat_FromDouble(0.5));

    PyObject * axes = EP_CallMethod(m_frame,"add_subplot","(s)","111");
//    PyObject * axes = EP_CallMethod(m_frame,"gca","()");
    retval =  EP_CallKWMethod(axes,"plot",kwargs,"(OOs)",pyX,pyY,format.c_str());
    Py_DECREF(kwargs);
	Py_DECREF(pyX);
	Py_DECREF(pyY);
	Py_DECREF(axes);

    return retval;
  }

  PyObject * MPLPlotFrame::createScatterPlot(const ISequence & x, const ISequence & y,std::string format) {
    PyObject * retval = 0;
//	std::cout << "createScatterPlot() for " << m_title << std::endl;
    // Get arrays of values.
    std::vector<double> x_pts;
    std::vector<double> x_low_err;
    std::vector<double> x_high_err;
    std::vector<double> y_pts;
    std::vector<double> y_low_err;
    std::vector<double> y_high_err;

    x.getValues(x_pts);
    x.getSpreads(x_low_err, x_high_err);

    y.getValues(y_pts);
    y.getSpreads(y_low_err, y_high_err);

    // Create the graph.
    // You can't pass vectors as arguments in a variable length argument list (get Illegal Instruction error)
    //    so lets make them into python lists
    PyObject *pX = PyList_New(0);
    PyObject *pY = PyList_New(0);
    PyObject *pXlow = PyList_New(0);
    PyObject *pYlow = PyList_New(0);
    PyObject *pXhigh = PyList_New(0);
    PyObject *pYhigh = PyList_New(0);

    bool plotXErrors = false;
    bool plotYErrors = false;
    for (unsigned int i = 0; i< x_pts.size(); ++i){
    	if (0 != x_low_err[i] ) plotXErrors = true; // don't plot X error bars if the error values are all zero so we need to check
    	if (0 != y_low_err[i] ) plotYErrors = true; // don't plot Y error bars if the error values are all zero so we need to check
    	PyList_Append(pX,PyFloat_FromDouble(x_pts[i]));
    	PyList_Append(pY,PyFloat_FromDouble(y_pts[i]));
    	PyList_Append(pXlow,PyFloat_FromDouble(x_low_err[i]));
    	PyList_Append(pYlow,PyFloat_FromDouble(y_low_err[i]));
    	PyList_Append(pXhigh,PyFloat_FromDouble(x_high_err[i]));
    	PyList_Append(pYhigh,PyFloat_FromDouble(y_high_err[i]));
    }
    // Set some formating keyword arguments
	PyObject *kwargs = PyDict_New();
	PyDict_SetItemString(kwargs,"linewidth",PyFloat_FromDouble(0.5));

    PyObject * axes = EP_CallMethod(m_frame,"add_subplot","(s)","111");
//    PyObject * axes = EP_CallMethod(m_frame,"gca","()");
    if (plotXErrors && plotYErrors) {
    	retval = EP_CallKWMethod(axes,"errorbar",kwargs,"(OO[OO][OO]s)",pX,pY,pYlow,pYhigh,pXlow,pXhigh,format.c_str());
    } else if (plotXErrors && !plotYErrors){
    	retval = EP_CallKWMethod(axes,"errorbar",kwargs,"(OOO[OO]s)",pX,pY,Py_None,pXlow,pXhigh,format.c_str());
    } else if (plotYErrors && !plotXErrors){
    	retval = EP_CallKWMethod(axes,"errorbar",kwargs,"(OO[OO]Os)",pX,pY,pYlow,pYhigh,Py_None,format.c_str());
  	} else {
  		retval = EP_CallKWMethod(axes,"errorbar",kwargs,"(OOOOs)",pX,pY,Py_None,Py_None,format.c_str());
	}
    Py_DECREF(kwargs);
    Py_DECREF(pX);
    Py_DECREF(pY);
    Py_DECREF(pXlow);
    Py_DECREF(pYlow);
    Py_DECREF(pXhigh);
    Py_DECREF(pYhigh);
    Py_DECREF(axes);
//    retval = new TGraphAsymmErrors(x.size(), &x_pts[0], &y_pts[0], &x_low_err[0], &x_high_err[0], &y_low_err[0], &y_high_err[0]);
//    retval->SetEditable(kFALSE);

    return retval;
  }

  PyObject * MPLPlotFrame::createHistPlot2D(const std::string & root_name, const ISequence & x, const ISequence & y,
    const std::vector<std::vector<double> > & z) {

    typedef std::vector<double> Vec_t;
//	std::cout << "createHistPlot2D() for " << m_title << std::endl;

    // Set up x bins. There is one extra for Root's upper cutoff.
    Vec_t x_bins(x.size() + 1);

    // Set up intervals, big enough to hold either x or y bins.
    Vec_t lower(std::max(x.size(), y.size()));
    Vec_t upper(std::max(x.size(), y.size()));

    // Get intervals of x axis.
    x.getIntervals(lower, upper);

    PyObject *pX = PyList_New(0);
    // Use low edges of input sequence for all but the last bin.
    for (Vec_t::size_type ii = 0; ii < x.size(); ++ii) {
    	PyList_Append(pX,PyFloat_FromDouble(lower[ii]));
//    	x_bins[ii] = lower[ii];
    }

    // Last bin is taken from upper bound of last element in sequence.
    PyList_Append(pX,PyFloat_FromDouble(upper[x.size() - 1]));
//    x_bins[x.size()] = upper[x.size() - 1];

    // Set up y bins. There is one extra for Root's upper cutoff.
    Vec_t y_bins(y.size() + 1);

    // Get intervals of y axis.
    y.getIntervals(lower, upper);

    PyObject *pY = PyList_New(0);
    // Use low edges of all bins.
    for (Vec_t::size_type ii = 0; ii < y.size(); ++ii) {
        PyList_Append(pY,PyFloat_FromDouble(lower[ii]));
//    	y_bins[ii] = lower[ii];
    }

    // Last bin is overflow.
    PyList_Append(pY,PyFloat_FromDouble(upper[y.size() - 1]));
    y_bins[y.size()] = upper[y.size() - 1];

    PyObject *pZ = PyList_New(0);
    // Populate the histogram values
	for (unsigned int ii = 0; ii < x.size(); ++ii){
		PyObject *pYtemp = PyList_New(0);
		for (unsigned int jj = 0; jj < y.size(); ++jj){
			PyList_Append(pYtemp,PyFloat_FromDouble(z[ii][jj]));
			//hist->SetBinContent(ii + 1, jj + 1, z[ii][jj]);
		}
		PyList_Append(pZ,pYtemp);
	}

//    // Create the histogram used to draw the plot.
	PyObject *pres = EP_CallMethod("Lego","prepareLegoData","(OOO)",pX,pY,pZ); // this returns a tuple of the three lists needed for the surface plot
    Py_DECREF(pX);  // we're going to reuse these object
    Py_DECREF(pY);  //  to hold the prepared versions of
    Py_DECREF(pZ);  //  the X,Y,Z data for the final plot

    EP_LoadModule("mpl_toolkits.mplot3d.axes3d");
	PyObject *kwargs = PyDict_New();
	PyDict_SetItemString(kwargs,"projection",PyString_FromString("3d"));
	PyObject * axes = EP_CallKWMethod(m_frame,"gca",kwargs,"()");
    Py_DECREF(kwargs);
	EP_CallMethod(m_frame,"subplots_adjust","(ffff)",0.05,0.05,0.95,1.0);

    // Now lets set up some keyword arguments for the plot
	kwargs = PyDict_New();
	PyDict_SetItemString(kwargs,"rstride",PyInt_FromLong(2));
	PyDict_SetItemString(kwargs,"cstride",PyInt_FromLong(2));
	PyDict_SetItemString(kwargs,"color",PyString_FromString("w")); // white/gray histograms
	PyDict_SetItemString(kwargs,"edgecolors",PyString_FromString("k"));  // black edges
	PyDict_SetItemString(kwargs,"linewidths",PyFloat_FromDouble(0.5));
	// Now we actually make the plot
	PyObject *hist = EP_CallKWMethod(axes,"plot_surface",kwargs,"O",pres);
    Py_DECREF(kwargs);
    Py_DECREF(axes);

    return hist;
  }

  std::string MPLPlotFrame::createRootName(const std::string & prefix, void * ptr) const {
    // The root name of the object (by which it may be looked up) is its address, converted
    // to a string. This should prevent collisions.
    std::ostringstream os;
    os << prefix << " " << ptr;
    return os.str();
  }


  std::string MPLPlotFrame::generateFormatString(MPLPlot * plot) const {
    std::ostringstream os;

    os << getColorString(plot->getLineColor());

    // Handle line style: none, solid, dashed, dotted.
    std::string line_style = plot->getLineStyle();
//    int root_line_style = kSolid;
    if ("dashed" == line_style) {
		os << "--";
	} else if ("dotted" == line_style) {
		os << ":";
	} else if ("none" == line_style) {
		os << ","; // no lines. we use the ',' pixel marker because matplotlib doesn't have a "no lines" option.  This places a single pixel point at the location which is then hidden by the error bars.
	} else {
		os << "-";
	}

    // Note:  We don't deal with the curve type here because as far as I've been
    //        able to determine, all matplotlib plots are straight lines.  You don't
    //        have the option for a smooth curve.

    return os.str();
  }

  std::string MPLPlotFrame::getColorString(int color) const{
	  std::ostringstream os;
	  switch(color){
	  case Color::eWhite:
		  os << "w";
		  break;
	  case Color::eBlack:
		  os << "k";
		  break;
	  case Color::eRed:
		  os << "r";
		  break;
	  case Color::eGreen:
		  os << "g";
		  break;
	  case Color::eBlue:
		  os << "b";
		  break;
	  case Color::eYellow:
		  os << "y";
		  break;
	  case Color::eMagenta:
		  os << "m";
		  break;
	  case Color::eCyan:
		  os << "c";
		  break;
	  default:
		  os << "k";
		  break;
	  }
	 return os.str();
  }

}

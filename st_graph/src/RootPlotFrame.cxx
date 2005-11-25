/** \file RootPlotFrame.cxx
    \brief Implementation for RootPlotFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#include <algorithm>
#include <cctype>
#include <list>
#include <map>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "TAxis.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TMarker.h"
#include "TGFrame.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH2.h"
#include "TMultiGraph.h"
#include "TRootEmbeddedCanvas.h"

#include "RootPlot.h"
#include "RootPlotFrame.h"

#include "st_graph/IEventReceiver.h"

namespace st_graph {

  class StMarker : public TMarker {
    public:
      StMarker(Marker & marker);

      virtual ~StMarker();

      virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py);

      virtual void Draw(Option_t * option = "");

      virtual void SetColor(Color_t tcolor = 1);

      virtual void SetTextAngle(float_t angle);

    private:
      Marker * m_marker;
      TLatex * m_label;
  };

  class StLatex : public TLatex {
    public:
      StLatex(Marker & marker, StMarker & st_marker);

      virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py);

    private:
      Marker * m_marker;
      StMarker * m_st_marker;
  };

  class StEmbeddedCanvas : public TRootEmbeddedCanvas {
    public:
      typedef std::list<StMarker *> MarkerCont_t;

      StEmbeddedCanvas(RootPlotFrame * parent, const char * name = "0", const TGWindow * p = 0, UInt_t w = 10,
        UInt_t h = 10);

      virtual ~StEmbeddedCanvas();

      virtual Bool_t HandleContainerButton(Event_t * event);

      void addMarker(Marker & marker);

      void reset();

    private:
      MarkerCont_t m_marker_cont;
      RootPlotFrame * m_parent;
      double m_press_x;
      double m_press_y;
      UInt_t m_width;
      UInt_t m_height;
  };

  StMarker::StMarker(Marker & marker): TMarker(marker.m_x, marker.m_y, 23), m_marker(&marker), m_label(0) {
    if (!marker.m_text.empty()) m_label = new StLatex(marker, *this);
  }

  StMarker::~StMarker() { delete m_label; }

  void StMarker::ExecuteEvent(Int_t event, Int_t px, Int_t py) {
    double current_x = fX;
    double current_y = fY;
    TMarker::ExecuteEvent(event, px, py);
    if (fX != current_x) {
      if (0 != m_label) m_label->SetX(fX);
    }
    if (fY != current_y) {
      if (0 != m_label) m_label->SetY(fY);
    }
  }

  void StMarker::Draw(Option_t * option) {
    if (0 != m_label) m_label->Draw(option);
    TMarker::Draw(option);
  }

  void StMarker::SetColor(Color_t tcolor) {
    if (0 != m_label) m_label->SetTextColor(tcolor);
    TMarker::SetMarkerColor(tcolor);
  }

  void StMarker::SetTextAngle(float_t angle) {
    if (0 != m_label) m_label->SetTextAngle(angle);
  }

  StLatex::StLatex(Marker & marker, StMarker & st_marker): TLatex(marker.m_x, marker.m_y, (" " + marker.m_text).c_str()),
    m_marker(&marker), m_st_marker(&st_marker) {}

  void StLatex::ExecuteEvent(Int_t event, Int_t px, Int_t py) {
    double current_x = fX;
    double current_y = fY;
    TLatex::ExecuteEvent(event, px, py);
    if (fX != current_x) {
      m_st_marker->SetX(fX);
      m_marker->m_x = fX;
    }
    if (fY != current_y) {
      m_st_marker->SetY(fY);
      m_marker->m_y = fY;
    }
  }

  StEmbeddedCanvas::StEmbeddedCanvas(RootPlotFrame * parent, const char * name, const TGWindow * p, UInt_t w, UInt_t h):
    TRootEmbeddedCanvas(name, p, w, h), m_marker_cont(), m_parent(parent), m_press_x(0.), m_press_y(0.), m_width(w), m_height(h) {}

  StEmbeddedCanvas::~StEmbeddedCanvas() {
    for (MarkerCont_t::reverse_iterator itor = m_marker_cont.rbegin(); itor != m_marker_cont.rend(); ++itor) {
      delete *itor;
    }
  }

  Bool_t StEmbeddedCanvas::HandleContainerButton(Event_t * event) {
    Bool_t status = kTRUE;
    bool root_handles_event = true;
    if (0 != event) {
      // Determine if this event is inside the frame.
      double x_min, x_max, y_min, y_max;
      fCanvas->GetRangeAxis(x_min, y_min, x_max, y_max);
      double event_x, event_y;
      fCanvas->AbsPixeltoXY(event->fX, event->fY, event_x, event_y);
      bool in_frame = x_min <= event_x && event_x <= x_max && y_min <= event_y && event_y <= y_max;

      Int_t button = event->fCode;
      if (kButton3 == button) {
        // Button 3 events inside the plot window are handled by the event handler.
        if (in_frame) {
          root_handles_event = false;
          if (kButtonRelease == event->fType) {
            IEventReceiver * receiver = m_parent->getReceiver();
            double x;
            double y;
            fCanvas->AbsPixeltoXY(event->fX, event->fY, x, y);
            if (0 != receiver) receiver->rightClicked(m_parent, x, y);
          }
        }
      } else {
        // Find the object with which this event is associated.
        TObjLink * obj_link = 0;
        fCanvas->Pick(event->fX, event->fY, obj_link);

        // See if the associated object is a marker or label.
        TObject * obj = 0;
        if (0 != obj_link && 0 != (obj = obj_link->GetObject())) {
          std::string class_name = obj->ClassName();
          if (class_name != "TLatex" && class_name != "TMarker" && kButtonRelease != event->fType) {
            if (in_frame) {
              // These events are used to zoom: remap them to look like they come from the X axis.
              event->fY = fCanvas->YtoAbsPixel(y_min) + 1;
            }
          }
        }
      }
    }
    if (root_handles_event) {
      status = TRootEmbeddedCanvas::HandleContainerButton(event);
    }
    return status;
  }

  void StEmbeddedCanvas::addMarker(Marker & marker) {
    StMarker * st_marker = new StMarker(marker);
    st_marker->SetTextAngle(45);
    st_marker->SetColor(int(marker.m_color));
    st_marker->Draw();

    m_marker_cont.push_back(st_marker);
  }

  void StEmbeddedCanvas::reset() {
    for (MarkerCont_t::reverse_iterator itor = m_marker_cont.rbegin(); itor != m_marker_cont.rend(); ++itor) {
      delete *itor;
    }
    m_marker_cont.clear();
  }

  RootPlotFrame::RootPlotFrame(IFrame * parent, const std::string & title, unsigned int width, unsigned int height,
    bool delete_parent): RootFrame(parent, 0, 0, delete_parent), m_plots(), m_tgraphs(), m_title(title), m_canvas(0),
    m_multi_graph(0), m_th2d(0), m_dimensionality(0) {
    
    // Send event messages back to parent.
    m_receiver = m_parent->getReceiver();

    // Hook together Root primitives.
    TGCompositeFrame * root_frame = dynamic_cast<TGCompositeFrame *>(m_parent->getTGFrame());
    if (0 == root_frame)
      throw std::logic_error("RootPlotFrame constructor was passed a parent frame which cannot contain other Root frames");

    TGCanvas * top_canvas = new TGCanvas(root_frame, width, height);

    TGViewPort * vp = top_canvas->GetViewPort();

    root_frame->AddFrame(top_canvas);

    // Create a special Root TGCanvas which is suitable for embedding plots.
    //TRootEmbeddedCanvas * plot_canvas = new TRootEmbeddedCanvas(createRootName("TRootEmbeddedCanvas", this).c_str(), vp, vp->GetWidth(), vp->GetHeight(), kFixedSize);
    StEmbeddedCanvas * plot_canvas = new StEmbeddedCanvas(this, createRootName("StEmbeddedCanvas", this).c_str(), vp,
      vp->GetWidth(), vp->GetHeight());

    top_canvas->SetContainer(plot_canvas);

    plot_canvas->SetAutoFit(kFALSE);

    m_canvas = plot_canvas;

    m_frame = top_canvas;
  }

  // Note m_frame will be deleted in the base class destructor.
  RootPlotFrame::~RootPlotFrame() {
    // Note: This appears more complicated than necessary, but be careful changing it. Under some circumstances,
    // a RootPlot needs to delete its parent, but the parent will always attempt to delete the RootPlot in the
    // process. Thus it is important to ensure the child is detached at the right times to prevent deleting
    // the parent and/or the child twice.

    reset();

    // Delete Root widgets.
    delete m_multi_graph;
    delete m_th2d;
  }

  void RootPlotFrame::display() {
    RootFrame::display();

    // Save current pad.
    TVirtualPad * save_pad = gPad;

    // Select embedded canvas for drawing.
    gPad = m_canvas->GetCanvas();

    try {
      // Modifying axes must be done through Root TAxis objects.
      std::vector<TAxis *> root_axes(3);

      // Display plot correctly for the current dimensionality. Get Root axes objects.
      if (m_dimensionality == 2) display2d(root_axes);
      else if (m_dimensionality == 3) display3d(root_axes);

      // Flags indicating whether titles have been set.
      std::vector<bool> title_set(3, false);

      // Get titles from IPlots.
      for (std::list<RootPlot *>::iterator itor = m_plots.begin(); itor != m_plots.end(); ++itor) {
        // Extract IPlot-style axes.
        const std::vector<Axis> & axes = (*itor)->getAxes();

        // Loop over all dimensions, setting titles as needed.
        for (unsigned int index = 0; index != m_dimensionality; ++index) {
          // If title was not already set, set it.
          const std::string & title(axes[index].getTitle());
          if (!title_set[index] && !title.empty()) {
            root_axes[index]->SetTitle(title.c_str());
            root_axes[index]->CenterTitle(kTRUE);
            title_set[index] = true;
          }
        }

        // Loop over plots, displaying each one's labels.
        std::vector<Marker> & marker((*itor)->getMarkers());
        for (std::vector<Marker>::iterator itor = marker.begin(); itor != marker.end(); ++itor) {
          m_canvas->addMarker(*itor);
        }
      }

    } catch (...) {
      gPad = save_pad;
      throw;
    }

    // Force complete update of the display.
    gPad->Modified();
    gPad->Update();

    // Restore current pad.
    gPad = save_pad;
  }

  void RootPlotFrame::unDisplay() {
    // Delete all Root children. This must be done; otherwise there is some kind of seg fault because Root
    // still tries to redraw TGraphs which are associated with no display.
    for (std::list<TGraph *>::reverse_iterator itor = m_tgraphs.rbegin(); itor != m_tgraphs.rend(); ++itor) {
      if (0 != m_multi_graph) m_multi_graph->RecursiveRemove(*itor);
      delete *itor;
    }
    m_tgraphs.clear();
    RootFrame::unDisplay();
  }

  void RootPlotFrame::reset() {
    // Reset canvas.
    if (0 != m_canvas) m_canvas->reset();

    // Delete children.
    while (!m_plots.empty()) {
      // Find last child.
      std::list<RootPlot *>::iterator itor = --m_plots.end();

      // Get pointer to the plot.
      RootPlot * plot = *itor;

      // Break links between this and the child plot.
      removePlot(*itor);

      // Delete the child plot.
      delete plot;
    }

    unDisplay();
  }

  void RootPlotFrame::addPlot(IPlot * plot) {
    RootPlot * root_plot = dynamic_cast<RootPlot *>(plot);
    if (0 == root_plot) throw std::logic_error("RootPlotFrame::addPlot cannot add a non-Root plot");

    if (m_plots.empty()) m_dimensionality = root_plot->getDimensionality();
    else if (m_dimensionality != root_plot->getDimensionality())
      throw std::logic_error("RootPlotFrame::addPlot cannot overlay plots with different numbers of dimensions");
    else if (m_dimensionality > 2)
      throw std::logic_error("RootPlotFrame::addPlot cannot overlay 3d plots");

    // Make certain plot is not added more than once.
    if (m_plots.end() == std::find(m_plots.begin(), m_plots.end(), root_plot)) {
      m_plots.push_back(root_plot);
      root_plot->setParent(this);
    }
  }

  void RootPlotFrame::removePlot(IPlot * plot) {
    std::list<RootPlot *>::iterator itor = std::find(m_plots.begin(), m_plots.end(), plot);
    if (m_plots.end() != itor) {
      RootPlot * root_plot = dynamic_cast<RootPlot *>(plot);
      if (0 != root_plot) root_plot->setParent(0);
      m_plots.erase(itor);
    }
  }

  void RootPlotFrame::addMarker(Marker & marker) {
    // Save current pad.
    TVirtualPad * save_pad = gPad;

    // Select embedded canvas for drawing.
    gPad = m_canvas->GetCanvas();
    m_canvas->addMarker(marker);

    // Force complete update of the display.
    gPad->Modified();
    gPad->Update();

    // Restore current pad.
    gPad = save_pad;
  }

  const std::string & RootPlotFrame::getTitle() const {
    return m_title;
  }

  void RootPlotFrame::display2d(std::vector<TAxis *> & axes) {
    axes.assign(3, 0);

    // Create or get parent multi-graph.
    getMultiGraph();

    int color = 1;
    for (std::list<RootPlot *>::iterator itor = m_plots.begin(); itor != m_plots.end(); ++itor, ++color) {
      // Get numeric sequences from data.
      const std::vector<const ISequence *> sequences((*itor)->getSequences());

      // Unpack the sequences: first dimension is the x axis, second is the y.
      const ISequence * x = sequences.at(0);
      const ISequence * y = sequences.at(1);

      // Determine the style of the graph.
      std::string style = (*itor)->getStyle();

      // Depending on the style, create appropriate Root plot object.
      TGraph * tgraph = 0;
      if (style == "hist")
        tgraph = createHistPlot(*x, *y);
      else
        tgraph = createScatterPlot(*x, *y);

      // TODO: Warning: after 7 colors, the colors start being things like "white" so some hacking needed for
      // 8 or more layered plots.
      tgraph->SetLineColor(color);

      // Keep track of Root object, so it can be deleted later.
      m_tgraphs.push_back(tgraph);

      // Connect Root objects.
      m_multi_graph->Add(tgraph, "L");
    }

    // Draw parent TMultiGraph object.
    m_multi_graph->Draw("A");

    // Get axes. Set all three dimensions even though this is 2D.
    axes.resize(3);
    axes[0] = m_multi_graph->GetXaxis();
    axes[1] = m_multi_graph->GetYaxis();
    axes[2] = 0;
  }

  void RootPlotFrame::display3d(std::vector<TAxis *> & axes) {
    axes.assign(3, 0);

    if (m_plots.empty()) return;
    std::list<RootPlot *>::iterator itor = m_plots.begin();

    // Get numeric sequences from data.
    const std::vector<const ISequence *> sequences((*itor)->getSequences());

    // Unpack the sequences: first dimension is the x axis, second is the y.
    const ISequence * x = sequences.at(0);
    const ISequence * y = sequences.at(1);

    // Get data being plotted.
    const std::vector<std::vector<double> > & z((*itor)->getZData());

    // Create Root plotting object.
    m_th2d = createHistPlot2D(createRootName("TH2D", *itor), *x, *y, z);

    m_th2d->Draw("lego");

    // Get axes.
    axes[0] = m_th2d->GetXaxis();
    axes[1] = m_th2d->GetYaxis();
    axes[2] = m_th2d->GetZaxis();
  }

  TGraph * RootPlotFrame::createHistPlot(const ISequence & x, const ISequence & y) {
    TGraph * retval = 0;

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
    retval = new TGraph(idx, &*x_vals.begin(), &*y_vals.begin());

    retval->SetEditable(kFALSE);

    return retval;
  }

  TGraph * RootPlotFrame::createScatterPlot(const ISequence & x, const ISequence & y) {
    TGraph * retval = 0;
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
    retval = new TGraphAsymmErrors(x.size(), &x_pts[0], &y_pts[0], &x_low_err[0], &x_high_err[0], &y_low_err[0], &y_high_err[0]);
    retval->SetEditable(kFALSE);

    return retval;
  }

  TH2D * RootPlotFrame::createHistPlot2D(const std::string & root_name, const ISequence & x, const ISequence & y,
    const std::vector<std::vector<double> > & z) {

    TH2D * hist = 0;

    typedef std::vector<double> Vec_t;

    // Set up x bins. There is one extra for Root's upper cutoff.
    Vec_t x_bins(x.size() + 1);

    // Set up intervals, big enough to hold either x or y bins.
    Vec_t lower(std::max(x.size(), y.size()));
    Vec_t upper(std::max(x.size(), y.size()));
    
    // Get intervals of x axis.
    x.getIntervals(lower, upper);

    // Use low edges of input sequence for all but the last bin.
    for (Vec_t::size_type ii = 0; ii < x.size(); ++ii) x_bins[ii] = lower[ii];

    // Last bin is taken from upper bound of last element in sequence.
    x_bins[x.size()] = upper[x.size() - 1];

    // Set up y bins. There is one extra for Root's upper cutoff.
    Vec_t y_bins(y.size() + 1);

    // Get intervals of y axis.
    y.getIntervals(lower, upper);

    // Use low edges of all bins.
    for (Vec_t::size_type ii = 0; ii < y.size(); ++ii) y_bins[ii] = lower[ii];

    // Last bin is overflow.
    y_bins[y.size()] = upper[y.size() - 1];

    // Create the histogram used to draw the plot.
    hist = new TH2D(root_name.c_str(), getTitle().c_str(), x_bins.size() - 1, &x_bins[0], y_bins.size() - 1, &y_bins[0]);

    // Populate the histogram.
    for (unsigned int ii = 0; ii < x.size(); ++ii)
      for (unsigned int jj = 0; jj < y.size(); ++jj)
        hist->SetBinContent(ii + 1, jj + 1, z[ii][jj]);

    return hist;
  }

  std::string RootPlotFrame::createRootName(const std::string & prefix, void * ptr) const {
    // The Root name of the object (by which it may be looked up) is its address, converted 
    // to a string. This should prevent collisions.
    std::ostringstream os;
    os << prefix << " " << ptr;
    return os.str();
  }

  TMultiGraph * RootPlotFrame::getMultiGraph() {
    if (0 == m_multi_graph) {
      class StMultiGraph : public TMultiGraph {
        public:
          StMultiGraph(const char * name, const char * title): TMultiGraph(name, title) {}
          virtual Int_t DistancetoPrimitive(Int_t px, Int_t py) {
            // The following code was copied from TMultiGraph in Root 4.02.00 and modified.
            // This is undesirable, but currently the only solution which works. Without this
            // override, the line:
            //      if (dist < kMaxDiff) {gPad->SetSelected(g); return dist;}
            // causes the click event to be associated with one of the TGraphs. This is a
            // problem if this event is a "button release" when dragging and dropping another
            // object. Because the event gets tied to the TGraph, the button release is not
            // handled as a "drop", so the object being dragged springs back to its original
            // position.
            //
            // At first a simpler solution was tried in the overridden method, in which the maximum
            // distance was always returned. However, this broke mouse clicks which were not
            // associated with the TMultiGraph, because the line:
            // distance = fHistogram->DistancetoPrimitive(px,py);
            // was not getting executed. When this method is called, the correct histogram
            // axis gets selected and that axis can then respond to events, otherwise
            // no object responds to it. In other words, Root depends on the correct
            // object being selected as a side-effect of computing the distance to the
            // object!
            
            //*-*- Are we on the axis?
               const Int_t kMaxDiff = 10;
               Int_t distance = 9999;
               // Changed following line, using 0 != to silence warning on Windows.
               // if (fHistogram) {
               if (0 != fHistogram) {
                  distance = fHistogram->DistancetoPrimitive(px,py);
                  if (distance <= 0) return distance;
               }
            
            //*-*- Loop on the list of graphs
               // Changed following line, using 0 == to silence warning on Windows.
               // if (!fGraphs) return distance;
               if (0 == fGraphs) return distance;
               TGraph *g;
               TIter   next(fGraphs);
               // Added 0 != to silence warning on Windows.
               // Changed following line, using 0 != to silence warning on Windows.
               // while ((g = (TGraph*) next())) {
               while (0 != (g = (TGraph*) next())) {
                  Int_t dist = g->DistancetoPrimitive(px,py);
                  if (dist <= 0) return 0;
                  // Commenting out gPad->SetSelected(g) is the only modification to base class method.
                  if (dist < kMaxDiff) {/* gPad->SetSelected(g); */ return dist;}
               }
               return distance;
            }
      };

      m_multi_graph = new StMultiGraph(createRootName("TMultiGraph", this).c_str(), m_title.c_str());
    }
    return m_multi_graph;
  }

}

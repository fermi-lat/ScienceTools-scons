/** \file st_graph.cxx
    \brief Simple plotting application.
    \author James Peachey, HEASARC/GSSC
*/
#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_graph/Engine.h"
#include "st_graph/PlotHist.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

class StGraphApp : public st_app::StApp {
  public:
    StGraphApp();
    virtual ~StGraphApp() throw();
    virtual void run();
    virtual void prompt();
    virtual void plot();
    virtual void readBins(const std::string & file, const std::string & table, const std::string & low_bin,
      const std::string & high_bin, st_graph::PlotHist::IntervalCont_t & intervals) const;

    virtual void readData(const std::string & file, const std::string & table, const std::string & data_field,
      st_graph::PlotHist * plot) const;

  private:
    st_graph::PlotHist * m_hist_plot;
};

StGraphApp::StGraphApp(): m_hist_plot(0) {}

StGraphApp::~StGraphApp() throw() { delete m_hist_plot; }

void StGraphApp::run() {
  prompt();
  plot();
}

void StGraphApp::prompt() {
  st_app::AppParGroup & pars = getParGroup("st_graph");
  pars.Prompt();
  pars.Save();
}

void StGraphApp::plot() {
  using namespace st_graph;

  st_app::AppParGroup & pars = getParGroup("st_graph");

  // Create intervals for histogram plot, first dimension.
  PlotHist::IntervalCont_t intervals1;

  // Read intervals for first dimension from bin definition file.
  readBins(pars["binfile1"], pars["bintable1"], pars["lowbin1"], pars["highbin1"], intervals1);

  PlotHist * plot = 0;
  if (bool(pars["vector1"])) {
    // Create intervals for histogram plot, second dimension.
    PlotHist::IntervalCont_t intervals2;

    // Read intervals for second dimension from bin definition file.
    readBins(pars["binfile2"], pars["bintable2"], pars["lowbin2"], pars["highbin2"], intervals2);

    // Create 2D plot.
    plot = Engine::instance().createPlotHist2D(pars["data1"], 800, 600, intervals1, intervals2);
  } else {
    // Create 1D plot.
    plot = Engine::instance().createPlotHist1D(pars["data1"], 800, 600, intervals1);
  }

  // Read data from data file.
  readData(pars["datafile1"], pars["datatable1"], pars["data1"], plot);

  // Display the plot.
  Engine::instance().run();
}

void StGraphApp::readBins(const std::string & file, const std::string & table, const std::string & low_bin,
  const std::string & high_bin, st_graph::PlotHist::IntervalCont_t & intervals) const {
  using namespace st_graph;

  // Open bin definition file.
  std::auto_ptr<const tip::Table> table_ptr(tip::IFileSvc::instance().readTable(file, table));

  // Change size of output intervals to match file's size.
  intervals.resize(table_ptr->getNumRecords());

  PlotHist::IntervalCont_t::iterator intv_itor = intervals.begin();

  // Fill intervals using given fields for low/high.
  for (tip::Table::ConstIterator itor = table_ptr->begin(); itor != table_ptr->end(); ++itor, ++intv_itor)
    *intv_itor = PlotHist::Interval_t((*itor)[low_bin].get(), (*itor)[high_bin].get());
}

void StGraphApp::readData(const std::string & file, const std::string & table, const std::string & data_field,
  st_graph::PlotHist * plot) const {
  using namespace st_graph;

  // Open data file.
  std::auto_ptr<const tip::Table> table_ptr(tip::IFileSvc::instance().readTable(file, table));

  int index = 0;

  // Fill plot using given field.
  for (tip::Table::ConstIterator itor = table_ptr->begin(); itor != table_ptr->end(); ++itor, ++index) {
    if (1 == plot->dimensionality()) {
      // Scalar field.
      double value;
      (*itor)[data_field].get(value);
      plot->set(index, value);
    } else {
      // Vector field.
      std::vector<double> value;
      (*itor)[data_field].get(value);
      for (unsigned int ii = 0; ii < value.size(); ++ii)
        plot->set(index, ii, value[ii]);
    }
  }
}

st_app::StAppFactory<StGraphApp> g_factory;

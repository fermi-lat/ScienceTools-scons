/** \file st_graph.cxx
    \brief Simple plotting application.
    \author James Peachey, HEASARC/GSSC
*/
#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_graph/Axis.h"
#include "st_graph/Engine.h"
#include "st_graph/IPlot.h"
#include "st_graph/Sequence.h"

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
      const std::string & high_bin, std::vector<double> & low, std::vector<double> & high) const;

    virtual void readData(const std::string & file, const std::string & table, const std::string & data_field,
      bool is_vector, std::vector<std::vector<double> > & data) const;

  private:
    st_graph::IPlot * m_hist_plot;
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

  typedef std::vector<double> Vec_t;
  typedef IntervalSequence<Vec_t::iterator> IntervalSeq_t;
  typedef PointSequence<Vec_t::iterator> PointSeq_t;

  // Vectors to hold bins.
  Vec_t low1;
  Vec_t high1;

  // Extract some parameters which will be used several times.
  std::string lowbin1 = pars["lowbin1"];
  std::string highbin1 = pars["highbin1"];
  std::string lowbin2 = pars["lowbin2"];
  std::string highbin2 = pars["highbin2"];

  // Read intervals for first dimension from bin definition file.
  readBins(pars["binfile1"], pars["bintable1"], lowbin1, highbin1, low1, high1);

  // Array to hold the data being plotted.
  std::vector<Vec_t> data;

  // Read the data into the array.
  readData(pars["datafile1"], pars["datatable1"], pars["data1"], pars["vector1"], data);

  Vec_t low2;
  Vec_t high2;
  if (bool(pars["vector1"])) {

    // Read intervals for second dimension from bin definition file.
    readBins(pars["binfile2"], pars["bintable2"], lowbin2, highbin2, low2, high2);

    // Create 3D plot.
    m_hist_plot = Engine::instance().createPlot(pars["data1"], 800, 600, "scat",
      IntervalSeq_t(low1.begin(), low1.end(), high1.begin()), IntervalSeq_t(low2.begin(), low2.end(), high2.begin()), data);

    // Set axes titles.
    std::vector<Axis> & axes(m_hist_plot->getAxes());
    axes[0].setTitle(lowbin1 + "/" + highbin1);
    axes[1].setTitle(lowbin2 + "/" + highbin2);
    axes[2].setTitle(pars["data1"]);
  } else {
    // Create 2D plot.
    m_hist_plot = Engine::instance().createPlot(pars["data1"], 800, 600, "scat",
      IntervalSeq_t(low1.begin(), low1.end(), high1.begin()), PointSeq_t(data[0].begin(), data[0].end()));

    // Set axes titles.
    std::vector<Axis> & axes(m_hist_plot->getAxes());
    axes[0].setTitle(lowbin1 + "/" + highbin1);
    axes[1].setTitle(pars["data1"]);
  }

  // Display the plot.
  Engine::instance().run();
}

void StGraphApp::readBins(const std::string & file, const std::string & table, const std::string & low_bin,
  const std::string & high_bin, std::vector<double> & low, std::vector<double> & high) const {
  using namespace st_graph;

  // Open bin definition file.
  std::auto_ptr<const tip::Table> table_ptr(tip::IFileSvc::instance().readTable(file, table));

  // Change size of output intervals to match file's size.
  low.resize(table_ptr->getNumRecords());
  high.resize(table_ptr->getNumRecords());

  // Output iterators.
  std::vector<double>::iterator low_itor = low.begin();
  std::vector<double>::iterator high_itor = high.begin();

  // Fill intervals using given fields for low/high.
  for (tip::Table::ConstIterator itor = table_ptr->begin(); itor != table_ptr->end(); ++itor, ++low_itor, ++high_itor) {
    *low_itor = (*itor)[low_bin].get();
    *high_itor = (*itor)[high_bin].get();
  }
}

void StGraphApp::readData(const std::string & file, const std::string & table, const std::string & data_field,
  bool is_vector, std::vector<std::vector<double> > & data) const {
  using namespace st_graph;

  // Open data file.
  std::auto_ptr<const tip::Table> table_ptr(tip::IFileSvc::instance().readTable(file, table));

  // Resize output vector.
  std::vector<double>::size_type num_rec = table_ptr->getNumRecords();

  // Vector columns will utilize the full data vector of vectors.
  if (is_vector) data.resize(num_rec);
  else {
    // Scalar columns use just the first vector within the data vector of vectors.
    data.resize(1);
    data[0].resize(num_rec);
  }

  // Use index to fill vector of vectors.
  std::vector<double>::size_type index = 0;

  // Fill plot using given field.
  for (tip::Table::ConstIterator itor = table_ptr->begin(); itor != table_ptr->end(); ++itor, ++index) {
    if (!is_vector) {
      // Scalar field.
      data[0][index] = (*itor)[data_field].get();
    } else {
      // Vector field.
      (*itor)[data_field].get(data[index]);
    }
  }
}

st_app::StAppFactory<StGraphApp> g_factory;

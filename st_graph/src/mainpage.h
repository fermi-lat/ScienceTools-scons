/**
    \mainpage st_graph package

    \author  James Peachey peachey@lheamail.gsfc.nasa.gov

    \section intro Introduction
    The st_graph package provides rudimentary plotting capabilities
    using Root for its underlying graphics engine. The first version
    of st_graph is deliberately underpowered and oversimplified. The
    hope is to get developers started with plotting so that actual
    use cases can drive further development.

    \section info Information For Clients
    Basically two steps are needed to produce a plot using st_graph.
    First, a graphics engine which encapsulates Root's main application
    event loop is created. This engine also functions as an abstract
    factory for the creation of plots.

    \subsection sample Sample Code
\verbatim

  // Create local reference to engine singleton. This engine is an abstract factory for graphical objects.
  Engine & engine(Engine::instance());

  // Create a histogram plot, size 900 x 600, with the given bin definitions.
  // intervals is a container of start/stop values for the histogram.
  PlotHist1D * plot_hist_1 = engine.createPlotHist1D("Plot 1", 900, 600, intervals);

  // Fill plot with sinusoids.
  for (int ii = 0; ii < num_intervals; ++ii) {
    plot_hist_1->set(ii, sine_wave[ii]);
  }

  // Display all graphical objects.
  engine.run();

\endverbatim

    The class Engine is the abstract factory for plots. It is used
    to create a 1-d histogram plot, an object of a class named
    PlotHist1D. This plot is then populated with
    data. The display is created when Engine::run() is called.
    The above example is excerpted from src/test/test_st_graph.cxx.
    When the test code is run, it displays several plots of
    sinusoidal curves.

*/

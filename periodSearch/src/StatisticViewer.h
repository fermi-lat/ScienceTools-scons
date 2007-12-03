/** \file StatisticViewer.h
    \brief Declaration of StatisticViewer class.
    \authors Masaharu Hirayama, GSSC
             James Peachey, HEASARC/GSSC
*/
#ifndef periodSearch_StatisticViewer_h
#define periodSearch_StatisticViewer_h

#include <string>
#include <vector>

namespace st_stream {
  class StreamFormatter;
}

namespace tip {
  class Table;
}

/** \class StatisticViewer
    \brief Handler of a graphical plot, a text output, and a FITS file output of numeric data, designed to display
           statistical data obtained from pulsation searches and periodicity tests.
*/
class StatisticViewer {
  public:
    typedef unsigned long index_type;
    typedef std::vector<double> data_type;

    /** \brief Create a viewer object for plotting and writing designated data for human viewing.
        \param num_axis The number of axes of data to be viewed. For one-dimensional histogram,
               for example, give 2 for num_axis (1 for X-axis and 1 for Y-axis).
        \param num_element The number of data elements to be viewed (common to all axes).
    */
    StatisticViewer(index_type num_axis, data_type::size_type num_element);

    /** \brief Set a pair of iterators of data array to be viewed.
        \param axis_index The index of axis, for which a set of iterators is to be set.
        \param begin The iterator of data array, which points to the first element to be viewed.
        \param copy_data If true, the given data will be copied into an internal data storage of this object.
                         If not, the data will not be copied and the original data will be read in
                         every time this object needs them (in plot method, etc.).
    */
    void setData(index_type axis_index, const data_type::const_iterator & begin, bool copy_data);

    /** \brief Set an axis label.
        \param axis_index The index of axis, for which an axis label is to be set.
        \param label The axis label to set.
    */
    void setLabel(index_type axis_index, const std::string & label);

    /** \brief Set a unit of an axis.
        \param axis_index The index of axis, for which a unit is to be set.
        \param label The unit for the axis to set.
    */
    void setUnit(index_type axis_index, const std::string & unit);

    /** \brief Set a title of a plot, a text output, and a FITS output.
        \param title The title to set.
    */
    void setTitle(const std::string & title);

    /** \brief Set a caption of a plot, a text output, and a FITS output.
        \param caption The caption to set.
    */
    void setCaption(const std::string & caption);

    /** \brief Display a graphical plot of designated data.
        \param x_index The axis index of the data to be used for X-axis of the plot.
        \param y_index The axis index of the data to be used for Y-axis of the plot.
    */
    void plot(index_type x_axis_index = 0, index_type y_axis_index = 1) const;

    /** \brief Write a text output to an output stream. The output will be controlled by chatness level automatically.
        \param os StreamFormatter object, to which the text output is to be forwarded.
    */
    st_stream::StreamFormatter & write(st_stream::StreamFormatter & os) const;

    /** \brief Write a text data (title, caption, etc.) into a FITS header and numerical data into a FITS table.
        \param table FITS table, which the data are wirtten into.
    */
    tip::Table & write(tip::Table & table) const;

  private:
    std::vector<data_type> m_data_cont;
    std::vector<data_type::const_iterator> m_begin_cont;
    data_type::size_type m_num_element;
    std::vector<std::string> m_label_cont;
    std::vector<std::string> m_unit_cont;
    std::string m_title;
    std::string m_caption;

    enum ChatLevel { eIncludeCaption = 2, eIncludeData = 3 };
};

#endif

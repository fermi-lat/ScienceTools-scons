// -*- mode: c++ -*-
%module observationSim
%{
#include "observationSim/Spacecraft.h"
#include "observationSim/../src/LatSc.h"
#include "observationSim/../src/EgretSc.h"
#include "observationSim/Event.h"
#include "observationSim/EventContainer.h"
#include "observationSim/FitsTable.h"
#include "observationSim/ScData.h"
#include "observationSim/ScDataContainer.h"
#include "observationSim/Simulator.h"
#include "observationSim/Roi.h"
#include <vector>
#include <string>
%}
%include stl.i
%include ../observationSim/Spacecraft.h
%include ../src/LatSc.h
%include ../src/EgretSc.h
%include ../observationSim/Event.h
%include ../observationSim/EventContainer.h
%include ../observationSim/FitsTable.h
%include ../observationSim/ScData.h
%include ../observationSim/ScDataContainer.h
%include ../observationSim/Simulator.h
%include ../observationSim/Roi.h
%template(DoubleVector) std::vector<double>;
%template(StringVector) std::vector<std::string>;
%extend observationSim::Simulator {
   void generate_events(double simulationTime, EventContainer &events,
                        ScDataContainer &scData, latResponse::Irfs &response,
                        Spacecraft *spacecraft, 
                        Roi *roi=0) {
      self->generateEvents(simulationTime, events, scData, response,
                           spacecraft, 0, roi);
   }
   void genNumEvents(long numEvents, EventContainer &events,
                     ScDataContainer &scData, latResponse::Irfs &response,
                     Spacecraft *spacecraft, 
                     Roi *roi=0) {
      self->generateEvents(numEvents, events, scData, response, 
                           spacecraft, 0, roi);
   }
}
%extend observationSim::EventContainer {
   void fetchEventAttributes(int begin, int end, char *attribute,
                             std::vector<double> &data) {
      
      data.clear();
      data.reserve(end-begin);

      std::vector<observationSim::Event> my_events = self->getEvents();
      
// apparent direction coordinates
      if (std::string(attribute) == "ra") {
         for (int i = begin; i < end; i++)
            data.push_back(my_events[i].appDir().ra());
      } else if (std::string(attribute) == "dec") {
         for (int i = begin; i < end; i++)
            data.push_back(my_events[i].appDir().dec());
      } else if (std::string(attribute) == "l") {
         for (int i = begin; i < end; i++)
            data.push_back(my_events[i].appDir().l());
      } else if (std::string(attribute) == "b") {
         for (int i = begin; i < end; i++)
            data.push_back(my_events[i].appDir().b());
         
// energy and arrival time
      } else if (std::string(attribute) == "energy") {
         for (int i = begin; i < end; i++)
            data.push_back(my_events[i].energy());
      } else if (std::string(attribute) == "time") {
         for (int i = begin; i < end; i++)
            data.push_back(my_events[i].time());
      }
      return;
   }
}

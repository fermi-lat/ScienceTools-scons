/** \file Placer.h
    \brief Interface for Placer class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_Placer_h
#define st_graph_Placer_h

namespace st_graph {

  class IFrame;

  /** \class Placer
  */
  class Placer {
    public:
      Placer(IFrame * frame): m_frame(frame) {}

      virtual ~Placer() {}

      virtual long get() const = 0;

    protected:
      IFrame * m_frame;
  };

#if 0
  class Center : public Placer {
    public:
      Center(IFrame * frame): Placer(frame) {}

      IFrame * leftOf(Placer & neighbor, long offset = 0) { m_frame->setHCenter(neighbor.get() - offset); return m_frame; }

      IFrame * rightOf(Placer & neighbor, long offset = 0) { m_frame->setHCenter(neighbor.get() + offset); return m_frame; }

      virtual long get() const { return m_frame->getHCenter(); }
  };
#endif

  class LeftEdge : public Placer {
    public:
      LeftEdge(IFrame * frame): Placer(frame) {}

      IFrame * leftOf(Placer & neighbor, long offset = 0) { m_frame->setL(neighbor.get() - offset); return m_frame; }

      IFrame * rightOf(Placer & neighbor, long offset = 0) { m_frame->setL(neighbor.get() + offset); return m_frame; }

      virtual long get() const { return m_frame->getL(); }
  };

  class RightEdge : public Placer {
    public:
      RightEdge(IFrame * frame): Placer(frame) {}

      IFrame * leftOf(Placer & neighbor, long offset = 0) { m_frame->setR(neighbor.get() - offset); return m_frame; }

      IFrame * rightOf(Placer & neighbor, long offset = 0) { m_frame->setR(neighbor.get() + offset); return m_frame; }

      virtual long get() const { return m_frame->getR(); }
  };

}

#endif

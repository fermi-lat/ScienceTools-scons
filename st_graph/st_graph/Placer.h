/** \file Placer.h
    \brief Interface for HPlacer and VPlacer classes.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_Placer_h
#define st_graph_Placer_h

namespace st_graph {

  class IFrame;

  /** \class HPlacer
  */
  class HPlacer {
    public:
      HPlacer(IFrame * frame): m_frame(frame) {}

      virtual ~HPlacer() {}

      virtual long getH() const = 0;

      virtual void adopt(IFrame * frame) { m_frame = frame; }

    protected:
      IFrame * m_frame;
  };

  /** \class VPlacer
  */
  class VPlacer {
    public:
      VPlacer(IFrame * frame): m_frame(frame) {}

      virtual ~VPlacer() {}

      virtual long getV() const = 0;

      virtual void adopt(IFrame * frame) { m_frame = frame; }

    protected:
      IFrame * m_frame;
  };

  class Center : public HPlacer, public VPlacer {
    public:
      Center(IFrame * frame): HPlacer(frame), VPlacer(frame) {}

      IFrame * above(const VPlacer & neighbor, long offset = 0) {
        VPlacer::m_frame->setVCenter(neighbor.getV() - offset);
        return VPlacer::m_frame;
      }

      IFrame * below(const VPlacer & neighbor, long offset = 0) {
        VPlacer::m_frame->setVCenter(neighbor.getV() + offset);
        return VPlacer::m_frame;
      }

      IFrame * leftOf(const HPlacer & neighbor, long offset = 0) {
        HPlacer::m_frame->setHCenter(neighbor.getH() - offset);
        return HPlacer::m_frame;
      }

      IFrame * rightOf(const HPlacer & neighbor, long offset = 0) {
        HPlacer::m_frame->setHCenter(neighbor.getH() + offset);
        return HPlacer::m_frame;
      }

      virtual long getH() const { return HPlacer::m_frame->getHCenter(); }

      virtual long getV() const { return VPlacer::m_frame->getVCenter(); }

      virtual void adopt(IFrame * frame) { HPlacer::adopt(frame); VPlacer::adopt(frame); }

  };

  class LeftEdge : public HPlacer {
    public:
      LeftEdge(IFrame * frame): HPlacer(frame) {}

      IFrame * leftOf(const HPlacer & neighbor, long offset = 0) { m_frame->setL(neighbor.getH() - offset); return m_frame; }

      IFrame * rightOf(const HPlacer & neighbor, long offset = 0) { m_frame->setL(neighbor.getH() + offset); return m_frame; }

      IFrame * stretchTo(const HPlacer & neighbor, long offset = 0) {
        long edge = neighbor.getH() + offset;
        edge = edge > 0 ? edge : 0;
        long size = m_frame->getR() - edge;
        if (0 <= size) {
          m_frame->setL(edge);
          m_frame->setWidth(size);
        }
        return m_frame;
      }

      virtual long getH() const { return m_frame->getL(); }
  };

  class RightEdge : public HPlacer {
    public:
      RightEdge(IFrame * frame): HPlacer(frame) {}

      IFrame * leftOf(const HPlacer & neighbor, long offset = 0) { m_frame->setR(neighbor.getH() - offset); return m_frame; }

      IFrame * rightOf(const HPlacer & neighbor, long offset = 0) { m_frame->setR(neighbor.getH() + offset); return m_frame; }

      IFrame * stretchTo(const HPlacer & neighbor, long offset = 0) {
        long size = neighbor.getH() + offset - m_frame->getL();
        if (0 <= size) m_frame->setWidth(size);
        return m_frame;
      }

      virtual long getH() const { return m_frame->getR(); }
  };

  class TopEdge : public VPlacer {
    public:
      TopEdge(IFrame * frame): VPlacer(frame) {}

      IFrame * above(const VPlacer & neighbor, long offset = 0) { m_frame->setT(neighbor.getV() - offset); return m_frame; }

      IFrame * below(const VPlacer & neighbor, long offset = 0) { m_frame->setT(neighbor.getV() + offset); return m_frame; }

      IFrame * stretchTo(const VPlacer & neighbor, long offset = 0) {
        long edge = neighbor.getV() + offset;
        edge = edge > 0 ? edge : 0;
        long size = m_frame->getB() - edge;
        if (0 <= size) {
          m_frame->setT(edge);
          m_frame->setHeight(size);
        }
        return m_frame;
      }

      virtual long getV() const { return m_frame->getT(); }
  };

  class BottomEdge : public VPlacer {
    public:
      BottomEdge(IFrame * frame): VPlacer(frame) {}

      IFrame * above(const VPlacer & neighbor, long offset = 0) { m_frame->setB(neighbor.getV() - offset); return m_frame; }

      IFrame * below(const VPlacer & neighbor, long offset = 0) { m_frame->setB(neighbor.getV() + offset); return m_frame; }

      IFrame * stretchTo(const VPlacer & neighbor, long offset = 0) {
        long size = neighbor.getV() + offset - m_frame->getT();
        if (0 <= size) m_frame->setHeight(size);
        return m_frame;
      }

      virtual long getV() const { return m_frame->getB(); }
  };

}

#endif

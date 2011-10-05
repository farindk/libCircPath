
#ifndef LOGGING_HH
#define LOGGING_HH

#include "path.hh"
#include "visualizer.hh"


class AlgorithmLogging
{
public:
  AlgorithmLogging() : m_visualizer(NULL), m_enableLogging(false) { }
  virtual ~AlgorithmLogging() { }

  virtual void setVisualizer(Visualizer* visual) { m_visualizer=visual; }
  virtual void enableLogging(bool enable_logging=true) { m_enableLogging=enable_logging; }

  bool isLoggingEnabled() const { return m_enableLogging; }
  Visualizer* getVisualizer() { return m_visualizer; }

private:
  // for visualization

  Visualizer* m_visualizer;
  bool m_enableLogging;
};


#endif

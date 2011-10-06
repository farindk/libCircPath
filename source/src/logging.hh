
#ifndef LOGGING_HH
#define LOGGING_HH

#include <iostream>
using namespace std;

#include "path.hh"
#include "visualizer.hh"


#define DBG 0


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



class Log
{
public:
  Log() : d_rec(0) { }

#if DBG
  void enter(const char* funcname)
  {
    b(); cout << "+------------- " << funcname << " -----------\n";
    d_rec++;
  }

  void leave()
  {
    d_rec--;
    b(); cout << "+--------------------------------\n";
    b(); cout << endl;
  }

  void b() const
  {
    for (int i=0;i<d_rec;i++) cout << "| ";
  }

  const char* lhs()
  {
    char* p = buf;
    for (int i=0;i<d_rec;i++) { *p++ = '|'; *p++ = ' '; }
    *p++ = 0;
    return buf;
  }
#else
  void enter(const char* funcname) { }
  void leave() { }
  void b() const { }
  const char* lhs() { return ""; }
#endif

private:
  int d_rec;
  char buf[1000];
};


#endif

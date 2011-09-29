
#ifndef TYPE_EDGES_HH
#define TYPE_EDGES_HH

#include "type_matrix.hh"


enum Topology { Cylinder, Torus };


/* The EdgeCosts class manages the edge costs of the graph.
   Two types of graph are supported:
   - Torus graphs (with edges in the directions S,SE,E), and
   - Cylindrical graphcs (with edges towards NE,E,SE).

   The NodeCosts class is a simplified EdgeCosts class for
   which all outgoing edges have the same value.
 */
template <class Cost, Topology topology=Torus> class EdgeCosts;
template <class Cost, Topology topology=Torus> class NodeCosts;


template <class T> struct Triple { T a,b,c; };
template <class T> struct Quad { T a,b,c,d; };



template <class Cost>
class EdgeCosts_General : public Matrix< Quad<Cost> >
{
public:
  typedef Cost CostType;

  static const bool variableEdges = false;

  bool hasNE() const { return true; }
  bool hasE()  const { return true; }
  bool hasSE() const { return true; }
  bool hasS()  const { return true; }

  void setCostNE(int x,int y, Cost c) { (*this)(x,y).a=c; }
  void setCostE (int x,int y, Cost c) { (*this)(x,y).b=c; }
  void setCostSE(int x,int y, Cost c) { (*this)(x,y).c=c; }
  void setCostS (int x,int y, Cost c) { (*this)(x,y).d=c; }

  /* Cost of _outgoing_ edges.
     vertically, the modulo-operation is handled in this class,
     horizontally, it must be taken care of in the main program
   */
  Cost costNE(int x,int y) const { return (*this)(x,y%h()).a; }
  Cost costE (int x,int y) const { return (*this)(x,y%h()).b; }
  Cost costSE(int x,int y) const { return (*this)(x,y%h()).c; }
  Cost costS (int x,int y) const { return (*this)(x,y%h()).d; }

private:
  inline int h() const { return Matrix<Triple<Cost> >::getHeight(); }  // only to enhance code readability
};




/* Torus
 */
template <class Cost>
class EdgeCosts<Cost,Torus> : public Matrix< Triple<Cost> >
{
public:
  typedef Cost CostType;

  static const bool variableEdges = false;

  bool hasNE() const { return false; }
  bool hasE()  const { return true; }
  bool hasSE() const { return true; }
  bool hasS()  const { return true; }

  void setCostE (int x,int y, Cost c) { (*this)(x,y).a=c; }
  void setCostSE(int x,int y, Cost c) { (*this)(x,y).b=c; }
  void setCostS (int x,int y, Cost c) { (*this)(x,y).c=c; }

  /* Cost of _outgoing_ edges.
     vertically (y),   the coordinate can be arbitrary (but positive)
     horizontally (x), the coordinate must be in the range [0;w[
   */
  Cost costE (int x,int y) const { return (*this)(x,y% Matrix<Triple<Cost> >::getHeight()).a; }
  Cost costSE(int x,int y) const { return (*this)(x,y% Matrix<Triple<Cost> >::getHeight()).b; }
  Cost costS (int x,int y) const { return (*this)(x,y% Matrix<Triple<Cost> >::getHeight()).c; }
};


template <class Cost>
class NodeCosts<Cost,Torus> : public Matrix<Cost>
{
public:
  typedef Cost CostType;

  static const bool variableEdges = false;

  bool hasNE() const { return false; }
  bool hasE()  const { return true; }
  bool hasSE() const { return true; }
  bool hasS()  const { return true; }

  void setCost(int x,int y, Cost c) { (*this)(x,y)=c; }

  /* Cost of _outgoing_ edges.
     vertically (y),   the coordinate can be arbitrary (but positive)
     horizontally (x), the coordinate must be in the range [0;w[
   */
  Cost costE (int x,int y) const { return (*this)(x,y% Matrix<Triple<Cost> >::getHeight()); }
  Cost costSE(int x,int y) const { return (*this)(x,y% Matrix<Triple<Cost> >::getHeight()); }
  Cost costS (int x,int y) const { return (*this)(x,y% Matrix<Triple<Cost> >::getHeight()); }
};



/* Cylinder
 */
template <class Cost>
class EdgeCosts<Cost,Cylinder> : public Matrix< Triple<Cost> >
{
public:
  typedef Cost CostType;

  static const bool variableEdges = false;

  bool hasNE() const { return true; }
  bool hasE()  const { return true; }
  bool hasSE() const { return true; }
  bool hasS()  const { return false; }

  void setCostNE(int x,int y, Cost c) { (*this)(x,y).a=c; }
  void setCostE (int x,int y, Cost c) { (*this)(x,y).b=c; }
  void setCostSE(int x,int y, Cost c) { (*this)(x,y).c=c; }

  /* Cost of _outgoing_ edges.
     vertically, the modulo-operation is handled in this class,
     horizontally, it must be taken care of in the main program
   */
  Cost costNE(int x,int y) const { return (*this)(x,y%h()).a; }
  Cost costE (int x,int y) const { return (*this)(x,y%h()).b; }
  Cost costSE(int x,int y) const { return (*this)(x,y%h()).c; }

private:
  inline int h() const { return Matrix<Triple<Cost> >::getHeight(); }  // only to enhance code readability
};




template <class Cost>
class NodeCosts<Cost,Cylinder> : public Matrix<Cost>
{
public:
  typedef Cost CostType;

  static const bool variableEdges = false;

  bool hasNE() const { return true; }
  bool hasE()  const { return true; }
  bool hasSE() const { return true; }
  bool hasS()  const { return false; }

  void setCost(int x,int y, Cost c) { (*this)(x,y)=c; }

  /* Cost of _outgoing_ edges.
     vertically, the modulo-operation is handled in this class,
     horizontally, it must be taken care of in the main program
   */
  Cost costNE(int x,int y) const { return (*this)(x,y%h()); }
  Cost costE (int x,int y) const { return (*this)(x,y%h()); }
  Cost costSE(int x,int y) const { return (*this)(x,y%h()); }

private:
  inline int h() const { return Matrix<Triple<Cost> >::getHeight(); }  // only to enhance code readability
};


#endif

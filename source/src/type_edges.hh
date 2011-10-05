
#ifndef TYPE_EDGES_HH
#define TYPE_EDGES_HH

#include "type_matrix.hh"
#include "type_costtraits.hh"


enum Topology { Cylinder, Torus };


/* The EdgeCosts class manages the edge costs of the graph.
   Two types of graph are supported:
   - Torus graphs (with edges in the directions S,SE,E), and
   - Cylindrical graphcs (with edges towards NE,E,SE).

   The NodeCosts class is a simplified EdgeCosts class for
   which all incoming edges have the same value.

   TODO: do we still need the topology parameter?
 */
template <class Cost, Topology topology=Torus> class EdgeCosts;
template <class Cost, Topology topology=Torus> class NodeCosts;


template <class T> struct Triple { T a,b,c; };
template <class T> struct Quad { T a,b,c,d; };



template <class Cost>
class EdgeCosts_General : public Matrix< Quad<Cost> >
{
public:
  EdgeCosts_General(bool planar) { mPlanar=planar; }

  typedef Cost CostType;

  bool isPlanar() const { return mPlanar; }
  static const bool hasNE = true;
  static const bool hasE  = true;
  static const bool hasSE = true;
  static const bool hasS  = true;

  void setCostNE(int x,int y, Cost c) { (*this)(x,y).a=c; }
  void setCostE (int x,int y, Cost c) { (*this)(x,y).b=c; }
  void setCostSE(int x,int y, Cost c) { (*this)(x,y).c=c; }
  void setCostS (int x,int y, Cost c) { (*this)(x,y).d=c; }

  /* Cost of _incoming_ edges.
     vertically, the modulo-operation is handled in this class,
     horizontally, it must be taken care of in the main program
   */
  Cost costNE(int x,int y) const { return (*this)(x,y).a; }
  Cost costE (int x,int y) const { return (*this)(x,y).b; }
  Cost costSE(int x,int y) const { return (*this)(x,y).c; }
  Cost costS (int x,int y) const { return (*this)(x,y).d; }

private:
  //inline int h() const { return Matrix<Triple<Cost> >::getHeight(); }  // only to enhance code readability

  bool mPlanar;
};




/* Torus
 */
template <class Cost>
class EdgeCosts<Cost,Torus> : public Matrix< Triple<Cost> >
{
public:
  typedef Cost CostType;

  bool isPlanar() const { return true; }
  static const bool hasNE = false;
  static const bool hasE  = true;
  static const bool hasSE = true;
  static const bool hasS  = true;

  void setCostE (int x,int y, Cost c) { (*this)(x,y).a=c; }
  void setCostSE(int x,int y, Cost c) { (*this)(x,y).b=c; }
  void setCostS (int x,int y, Cost c) { (*this)(x,y).c=c; }

  /* Cost of _incoming_ edges.
     vertically (y),   the coordinate can be arbitrary (but positive)
     horizontally (x), the coordinate must be in the range [0;w[
   */
  Cost costNE(int x,int y) const { return CostTraits<Cost>::LargeVal(); }
  Cost costE (int x,int y) const { return (*this)(x,y).a; }
  Cost costSE(int x,int y) const { return (*this)(x,y).b; }
  Cost costS (int x,int y) const { return (*this)(x,y).c; }
};


template <class Cost>
class NodeCosts<Cost,Torus> : public Matrix<Cost>
{
public:
  typedef Cost CostType;

  bool isPlanar() const { return true; }
  static const bool hasNE = false;
  static const bool hasE  = true;
  static const bool hasSE = true;
  static const bool hasS  = true;

  void setCost(int x,int y, Cost c) { (*this)(x,y)=c; }

  /* Cost of _incoming_ edges.
     vertically (y),   the coordinate can be arbitrary (but positive)
     horizontally (x), the coordinate must be in the range [0;w[
   */
  Cost costE (int x,int y) const { return (*this)(x,y); }
  Cost costSE(int x,int y) const { return (*this)(x,y); }
  Cost costS (int x,int y) const { return (*this)(x,y); }
};



/* Cylinder
 */
template <class Cost>
class EdgeCosts<Cost,Cylinder> : public Matrix< Triple<Cost> >
{
public:
  typedef Cost CostType;

  bool isPlanar() const { return false; }
  static const bool hasNE = true;
  static const bool hasE  = true;
  static const bool hasSE = true;
  static const bool hasS  = false;

  void setCostNE(int x,int y, Cost c) { (*this)(x,y).a=c; }
  void setCostE (int x,int y, Cost c) { (*this)(x,y).b=c; }
  void setCostSE(int x,int y, Cost c) { (*this)(x,y).c=c; }

  /* Cost of _incoming_ edges.
     vertically, the modulo-operation is handled in this class,
     horizontally, it must be taken care of in the main program
   */
  Cost costNE(int x,int y) const { return (*this)(x,y).a; }
  Cost costE (int x,int y) const { return (*this)(x,y).b; }
  Cost costSE(int x,int y) const { return (*this)(x,y).c; }

private:
  //inline int h() const { return Matrix<Triple<Cost> >::getHeight(); }  // only to enhance code readability
};




template <class Cost>
class NodeCosts<Cost,Cylinder> : public Matrix<Cost>
{
public:
  typedef Cost CostType;

  bool isPlanar() const { return false; }
  static const bool hasNE = true;
  static const bool hasE  = true;
  static const bool hasSE = true;
  static const bool hasS  = false;

  void setCost(int x,int y, Cost c) { (*this)(x,y)=c; }

  /* Cost of _incoming_ edges.
     vertically, the modulo-operation is handled in this class,
     horizontally, it must be taken care of in the main program
   */
  Cost costNE(int x,int y) const { return (*this)(x,y); }
  Cost costE (int x,int y) const { return (*this)(x,y); }
  Cost costSE(int x,int y) const { return (*this)(x,y); }

private:
  //inline int h() const { return Matrix<Triple<Cost> >::getHeight(); }  // only to enhance code readability
};


#endif

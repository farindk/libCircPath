
#ifndef TYPE_CELL_HH
#define TYPE_CELL_HH

// #include "util_path.hh"



template <class Cost, class Index=short>
class Cell_Cost
{
public:
  typedef Cost CostSumType;

  void clear() { costSum=Cost(0); }

  void propagateFrom(const Cell_Cost& cell, const Cost& cost, Index prevX, Index prevY)
  {
    costSum = cost;
  }

  Cost  costSum;
};


template <class Cost, class Index=short>
class Cell_BackPtr
{
public:
  typedef Index IndexType;

  void clear() { setNoPredecessor(); }

  void propagateFrom(const Cell_BackPtr& cell, const Cost& cost, Index f_prevX, Index f_prevY)
  {
    prevX = f_prevX;
    prevY = f_prevY;
  }

  void setNoPredecessor() { prevY=prevX=-1; }
  void setPredecessor(Index& x, Index& y) const { y=prevY; x=prevX; }
  bool hasPredecessor() const { return prevX>=0; }

  Index prevX;
  Index prevY;
};


template <class Cost, class Index=short>
class Cell_Std
  : public Cell_Cost<Cost,Index>,
    public Cell_BackPtr<Cost,Index>
{
public:
  void clear()
  {
    Cell_Cost<Cost,Index>::clear();
    Cell_BackPtr<Cost,Index>::clear();
  }

  void propagateFrom(const Cell_Std& cell, const Cost& cost, Index f_prevX, Index f_prevY)
  {
    Cell_Cost<Cost,Index>::propagateFrom(cell,cost,f_prevX,f_prevY);
    Cell_BackPtr<Cost,Index>::propagateFrom(cell,cost,f_prevX,f_prevY);
  }

  void processBounds(Index row, Index lasttop, Index firstbottom) { }
};


template <class Cost, class Index=short>
class Cell_Touched
  : public Cell_Std<Cost,Index>
{
public:
  void clear()
  {
    Cell_Std<Cost,Index>::clear();

    touchedAbove=false;
    touchedBelow=false;
  }

  void propagateFrom(const Cell_Touched& cell, const Cost& cost, Index prevX, Index prevY)
  {
    Cell_Std<Cost,Index>::propagateFrom(cell,cost,prevX,prevY);

    touchedAbove = cell.touchedAbove;
    touchedBelow = cell.touchedBelow;
  }

  void processBounds(Index row, Index pathAbove_lasttop, Index pathBelow_firstbottom)
  {
    touchedAbove |= (row <= pathAbove_lasttop);
    touchedBelow |= (row >= pathBelow_firstbottom);
  }

  bool  touchedAbove;
  bool  touchedBelow;
};

#endif


#ifndef TYPE_COSTTRAITS_HH
#define TYPE_COSTTRAITS_HH

#include <climits>
#include <cfloat>


/*
  For each type that can serve as cost, we need to know
  - what type to use when summing costs together (SumType),
  - a large value that effectively serves as infinity (LargeValue()).
 */
template <class T> class CostTraits;

template <> class CostTraits<unsigned char>
{
public:
  typedef int SumType;
  typedef unsigned char CostType;

  static inline CostType LargeVal() { return 255; }
  static inline SumType MaxSum() { return INT_MAX; }
  static inline SumType LargeSum() { return MaxSum()/2; }
};

template <> class CostTraits<short>
{
public:
  typedef int SumType;
  typedef short CostType;

  static inline CostType LargeVal() { return SHRT_MAX; }
  static inline SumType MaxSum() { return INT_MAX; }
  static inline SumType LargeSum() { return MaxSum()/2; }
};

template <> class CostTraits<int>
{
public:
  typedef int SumType;
  typedef int CostType;

  static inline CostType LargeVal() { return INT_MAX/4; }
  static inline SumType MaxSum() { return INT_MAX; }
  static inline SumType LargeSum() { return MaxSum()/2; }
};

template <> class CostTraits<double>
{
public:
  typedef double SumType;
  typedef double CostType;

  static inline CostType LargeVal() { return DBL_MAX/4; }
  static inline SumType MaxSum() { return DBL_MAX; }
  static inline SumType LargeSum() { return MaxSum()/2; }
};

template <> class CostTraits<float>
{
public:
  typedef float SumType;
  typedef float CostType;

  static inline CostType LargeVal() { return FLT_MAX/4; }
  static inline SumType MaxSum() { return FLT_MAX; }
  static inline SumType LargeSum() { return MaxSum()/2; }
};


#endif

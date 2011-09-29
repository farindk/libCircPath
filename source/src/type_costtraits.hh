
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

  static inline SumType MaxSum() { return INT_MAX; }
  static inline SumType LargeSum() { return MaxSum()/2; }
};

template <> class CostTraits<short>
{
public:
  typedef int SumType;

  static inline SumType MaxSum() { return SHRT_MAX; }
  static inline SumType LargeSum() { return MaxSum()/2; }
};

template <> class CostTraits<int>
{
public:
  typedef int SumType;
  static inline SumType MaxSum() { return INT_MAX; }
  static inline SumType LargeSum() { return MaxSum()/2; }
};

template <> class CostTraits<double>
{
public:
  typedef double SumType;
  static inline SumType MaxSum() { return DBL_MAX; }
  static inline SumType LargeSum() { return MaxSum()/2; }
};

template <> class CostTraits<float>
{
public:
  typedef float SumType;
  static inline SumType MaxSum() { return FLT_MAX; }
  static inline SumType LargeSum() { return MaxSum()/2; }
};


#endif

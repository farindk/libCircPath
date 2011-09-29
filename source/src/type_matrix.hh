
#ifndef TYPE_MATRIX_HH
#define TYPE_MATRIX_HH


template <class Cell>
class Matrix
{
public:
  Matrix() : mAllocData(0), mDataPtr(0) { mWidth=mHeight=mStride=0; }
  ~Matrix() { if (mAllocData) delete[] mAllocData; }

  typedef Cell CellType;


  void create(int w,int h, int border=0)
  {
    mStride = h+2*border;
    mWidth  = w;
    mHeight = h;

    mAllocData= new Cell[w*mStride];
    mDataPtr = mAllocData+border;
  }

  int getWidth()  const { return mWidth; }
  int getHeight() const { return mHeight; }

  /* */ Cell& operator()(int x,int y)
  {
    return mDataPtr[x*mStride+y];
  }
  const Cell& operator()(int x,int y) const
  {
    //assert(x>=0 && x<mWidth);
    //assert(y>=-1 && y<=mHeight);

    return mDataPtr[x*mStride+y];
  }

private:
  Cell* mAllocData;
  Cell* mDataPtr;
  int mStride;
  int mWidth,mHeight;
};



template <class Cell>
class CellMatrix : public Matrix<Cell>
{
public:
  //typedef          Cell              CellType;
  typedef typename Cell::IndexType   IndexType;
  typedef typename Cell::CostSumType CostSumType;
};

#endif

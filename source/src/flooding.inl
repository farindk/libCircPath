
#include <assert.h>

template <class Cell, class EdgeCostsT>
inline void fillNode(CellMatrix<Cell>& nodes, const EdgeCostsT& cost, int r, int c, int rModH, int cModW)
{
  typedef typename CostTraits<typename EdgeCostsT::CostType>::SumType CostSum;

  CostSum cE;  if (cost.hasE)  cE  = nodes(c-1,r  ).costSum + cost.costE (cModW, rModH);
  CostSum cSE; if (cost.hasSE) cSE = nodes(c-1,r-1).costSum + cost.costSE(cModW, rModH);
  CostSum cS;  if (cost.hasS)  cS  = nodes(c  ,r-1).costSum + cost.costS (cModW, rModH);
  CostSum cNE; if (cost.hasNE) cNE = nodes(c-1,r+1).costSum + cost.costNE(cModW, rModH);

  //std::cout << r << ";" << c << " - " << cE << " " << cSE << " " << cS << " " << cNE << "\n";

  if (cost.hasE && (!cost.hasSE || cE <= cSE))
    {
      // E && !SE
      // E && SE && E<=SE
      // SE cannot be minimum (does not exist, or larger than E)
      // -> E exists, SE does not matter
      // *E S NE

      if (cost.hasNE && (!cost.hasS || cNE <= cS))
	{
	  // -> NE exists, S does not matter
	  // *E *NE

	  if   (cE <= cNE) goto toE;
	  else             goto toNE;
	}
      else
	{
	  // -> NE does not matter
	  // *E S

	  if (!cost.hasS || cE <= cS) goto toE;
	  else                        goto toS;
	}
    }
  else
    {
      // !E
      // E && SE && E>SE
      // E cannot be minimum (does not exist, or larger than SE)
      // -> E does not matter
      // SE S NE

      if (cost.hasNE && (!cost.hasS || cNE <= cS))
	{
	  // -> NE exists, S does not matter
	  // SE *NE

	  if (!cost.hasSE || cNE <= cSE) goto toNE;
	  else                           goto toSE;
	}
      else
	{
	  // -> NE does not matter
	  // SE S

	  if (cost.hasSE && (!cost.hasS || cSE <= cS))
	    {
	      // -> SE exists, S does not matter
	      // *SE

	      goto toSE;
	    }
	  else
	    {
	      // -> SE does not matter
	      // S

	      if (cost.hasS) goto toS;
	      else { assert(false); /* do edge defined! */ }
	    }
	}
    }

 toE:  nodes(c,r).propagateFrom( nodes(c-1,r  ), cE,  c-1,r   ); return;
 toNE: nodes(c,r).propagateFrom( nodes(c-1,r+1), cNE, c-1,r+1 ); return;
 toS:  nodes(c,r).propagateFrom( nodes(c  ,r-1), cS,  c  ,r-1 ); return;
 toSE: nodes(c,r).propagateFrom( nodes(c-1,r-1), cSE, c-1,r-1 ); return;
}


template <class EdgeCostsT, class CellMatrixT>
void Flooding<EdgeCostsT,CellMatrixT>::FloodColumn(const EdgeCostsT& cost, CellMatrixT& nodes,
						   int c, int cModW, int firstFloodRow, int lastFloodRow,
						   int abovePath, int belowPath)
{
  int h = cost.getHeight();

  //typedef typename EdgeCostsT::CostType Cost;
  //typedef typename CostTraits<typename EdgeCostsT::CostType>::SumType CostSum;

#if 0
  // this is the readable version, but which requires the modulo operation

  for (int r=firstFloodRow;r<=lastFloodRow;r++)
    {
      //std::cout << r << " " << c << " " << r%h << " " << cModW << "\n";
      fillNode(nodes, cost, r,c, r %h,cModW);
      nodes(c,r).processBounds(r, abovePath, belowPath);

      /*
	std::cout << c << ";" << r << " -> "
	<< nodes(c,r).costSum << " "
	<< nodes(c,r).prevX << " "
	<< nodes(c,r).prevY << "\n";
      */
    }

  return;
#endif

  int rs    =firstFloodRow;
  int rsMod =rs%h;
  int step  =rs/h;
  int re    =std::min(lastFloodRow,(step+1)*h-1);
  int offset=step*h;

  for (;;)
    {
      for (int r=rs;r<=re;r++)
	{
	  //std::cout << r << " " << c << " " << r-offset << " " << cModW << "\n";
	  fillNode(nodes, cost, r,c, r-offset /*%h*/,cModW);
	  nodes(c,r).processBounds(r, abovePath, belowPath);
	}

      step++;
      rs = step*h;
      if (rs>lastFloodRow) break;

      rsMod=0;
      re = std::min(lastFloodRow,(step+1)*h-1);
      offset += h;
    }
}


/* This function will overwrite the whole first column and
   the whole area between (including) the rows 'startrow' and 'endrow+h'
*/
template <class EdgeCostsT, class CellMatrixT>
void Flooding<EdgeCostsT,CellMatrixT>::Flood_Unrestricted(const EdgeCostsT& cost, CellMatrixT& nodes,
							  int firstStartRow, int lastStartRow,
							  int firstFloodRow, int lastFloodRow)
{
  int w = cost.getWidth();
  int h = cost.getHeight();

  typedef typename EdgeCostsT::CostType Cost;
  typedef typename CostTraits<typename EdgeCostsT::CostType>::SumType CostSum;

  // --- first column ---

  // fill first column with high cost and no predecessor

  for (int r=firstFloodRow; r<=lastFloodRow; r++)
    {
      nodes(0,r).costSum = CostTraits<Cost>::LargeSum();
      nodes(0,r).setNoPredecessor(); // (TODO CHECK: not really needed)
    }


  // Set cost for start-nodes to zero.

  for (int r=firstStartRow; r<=lastStartRow; r++)
    { nodes(0,r).clear(); }

  nodes(0,firstFloodRow-1).costSum = CostTraits<Cost>::LargeSum();
  nodes(0,lastFloodRow +1).costSum = CostTraits<Cost>::LargeSum();


  if (cost.hasS) {
    for (int r=lastStartRow+1;r<=lastFloodRow;r++)
      {
	CostSum cS = nodes(0,r-1).costSum + cost.costS(0,r%h);
        nodes(0,r).propagateFrom( nodes(0,r-1), cS,  0,r-1 );
      }
  }

  // --- process remaining columns (except last) ---

  for (int c=1;c<w;c++)
    {
      // put blocking markers
      if (cost.hasS || cost.hasSE) nodes(c,firstFloodRow-1).costSum = CostTraits<Cost>::LargeSum();
      if (cost.hasNE)              nodes(c,lastFloodRow +1).costSum = CostTraits<Cost>::LargeSum();


      // process flooding area

      FloodColumn(cost, nodes, c,c, firstFloodRow,lastFloodRow);
    }

  // --- process last column ---

  nodes(w,firstFloodRow-1).costSum = CostTraits<Cost>::LargeSum();

  FloodColumn(cost, nodes, w,0, firstFloodRow,lastFloodRow);
}


template <class EdgeCostsT, class CellMatrixT>
void Flooding<EdgeCostsT,CellMatrixT>::Flood_Bounded(const EdgeCostsT& cost, CellMatrixT& matrix, int startrow,
						     const Path& abovepath,
						     const Path& belowpath)
{
  typedef typename EdgeCostsT::CostType Cost;
  typedef typename CostTraits<typename EdgeCostsT::CostType>::SumType CostSum;

  const int w = cost.getWidth();
  const int h = cost.getHeight();

  /* We need a border around the node-matrix because we want to set blocking values
     for a simplified handling of the bounding paths.
  */
  assert(startrow>=abovepath.topRow(0));
  assert(startrow<=belowpath.bottomRow(0));


  // --- first column ---

  // fill first column with high cost and no predecessor

  const CostSum blockCost = CostTraits<Cost>::LargeSum();

  int start = abovepath.topRow(0);
  int end   = belowpath.bottomRow(0);
  for (int r=start; r<=end; r++)
    {
      matrix(0,r).costSum = blockCost;
      matrix(0,r).setNoPredecessor(); // (TODO CHECK: not really needed)
    }


  /* Initialize first node
  */
  matrix(0,startrow).clear();
  matrix(0,startrow).processBounds(startrow, abovepath.bottomRow(0), belowpath.topRow(0));

  // propagate in first column
  if (cost.hasS) {
    for (int r=startrow+1;r<=end;r++)
      {
	CostSum cS = matrix(0,r-1).costSum + cost.costS(0,r%h);
        matrix(0,r).propagateFrom( matrix(0,r-1), cS,  0,r-1 );
      }
  }


  // === fill graph column by column ===

  int first_row = start;
  int last_row  = end;

  for (int c=1;c<=w;c++)
    {
      // valid range in previous column

      const int prev_first_row = first_row;
      const int prev_last_row  = last_row;

      first_row = abovepath.topRow(c);
      last_row  = belowpath.bottomRow(c);


      // set guarding values

      matrix(c,first_row-1).costSum = blockCost;
      for (int r = prev_first_row-1; r>= first_row-1; r--)
	matrix(c-1,r).costSum = blockCost;

      for (int r=prev_last_row+1; r<=last_row+1; r++)
	matrix(c-1,r).costSum = blockCost;

      matrix(c,first_row-1).costSum = blockCost;


      // process column

      FloodColumn(cost, matrix, c,c%w, first_row, last_row, abovepath.bottomRow(c), belowpath.topRow(c));
    }
}


#if 0
  /*
    Flood the graph between (including) "abovepath" and "belowpath".
  */
  static void Flood_Bounded(const CostMatrix& cost, NodeMatrix& nodes, int startrow,
			    const ColumnPath& abovepath,
			    const ColumnPath& belowpath)
  {
    typedef typename CostMatrix::CostType Cost;

    const typename NodeMatrix::IndexType w = cost.AskWidth();
    const typename NodeMatrix::IndexType h = cost.AskHeight();

    /* We need a border around the node-matrix because we want to set blocking values
       for a simplified handling of the bounding paths.
    */
    assert(nodes.AskBorder()>=1);

    assert(startrow>=abovepath.toprow[0]);
    assert(startrow<=belowpath.bottomrow[0]);


    /* In each column, we will search the rows "first_row" until (including) "last_row".
     * Note that first_row and last_row can be mapped to the same node.
     */

    // initialize for first column
    int first_row = startrow;
    int last_row  = belowpath.bottomrow[0];

    /* Block unused elements in this column with guarding values.
       We always have to block:
       - the node just above first_row (for the top-most SE edge in the next column)
       - all nodes below last_row until last_row[c+1]
     */

    const Cost blockCost = CostTraits<Cost>::LargeSum();

    /* blocks for the next column.
       first_row-1 blocks for the next columns and also for the top-most element of the current row
    */
    for (int r=first_row-1; r<=belowpath.bottomrow[1]; r++)
      nodes(0,r).costSum = blockCost;

    /* Initialize first node
       Note that only the node 'startrow' will be initialized to zero cost, even though last_row
       may be larger. This is because the full range will be checked in the last column anyway.
    */
    nodes(0,startrow).clear();
    nodes(0,startrow).processBounds(startrow, abovepath.bottomrow[0], belowpath.toprow[0]);


    // === fill graph column by column ===

    for (int c=1;c<w+1;c++)
      {
	// valid range in previous column

	const int prev_first_row = first_row;
	const int prev_last_row  = last_row;

	first_row = max(first_row, abovepath.toprow[c]);
	last_row  = belowpath.bottomrow[c];


	// set guarding values

	nodes(c,first_row-1).costSum = blockCost;
	if (c<w)
	  for (int r=last_row+1; r<=belowpath.bottomrow[c+1]; r++)
	    nodes(c,r).costSum = blockCost;


	// process column

	for (int r=first_row; r<=last_row ;r++)
	  {
	    fillNode_Torus(nodes, cost, r,c, c%w, c-1);
	    nodes(c,r).processBounds(r, abovepath.bottomrow[c], belowpath.toprow[c]);
	  }
      }
  }


  static void Flood_RightToLeft_Bounded(const CostMatrix& cost, NodeMatrix& nodes,
					const ColumnPath& abovepath,
					const ColumnPath& belowpath,
					const Point2D<int>& touchingpoint)
  {
    const int w = cost.AskWidth();
    const int h = cost.AskHeight();

    typedef typename CostMatrix::CostType Cost;
    const Cost blockCost = CostTraits<Cost>::LargeSum();


    const int touching_column = touchingpoint.x;
    const int touching_row    = touchingpoint.y;

    assert(touching_row <= abovepath.bottomrow[touching_column]);
    assert(touching_row >= belowpath.toprow   [touching_column]);


    /* In each column, we are searching the rows "first_row" until (including) "last_row".
     * Note that first_row and last_row can be mapped to the same node.
     */

    // fill in touching column

    nodes(touching_column,touching_row).clear();

    /* fill remainder of the column (take only 'S' edges) */

    for (int r=touching_row-1; r>= abovepath.toprow[touching_column]; r--)
      {
	nodes(touching_column,r).propagateFrom(nodes(touching_column,r+1),
					       nodes(touching_column,r+1).costSum + cost.CostS(touching_column,r),
					       touching_column, r+1);
      }

    /* Blocking: in each processed column, we place the blocking values to the right of this column
       and the value just below the current active column.
     */
    nodes(touching_column, touching_row+1).costSum = blockCost;


    // fill remaining graph column by column (to the left side)

    for (int c=touching_column-1;c>=0;c--)
      {
	// valid range in previous column

	int first_row = abovepath.toprow[c];
	int last_row  = belowpath.bottomrow[c];

	int prev_first_row = abovepath.toprow   [c+1];
	int prev_last_row  = belowpath.bottomrow[c+1];

	if (c==touching_column-1)
	  prev_last_row = touching_row;

	// first, fill in guard values for this column

	for (int r=prev_first_row-1;r>=first_row;r--)
	  nodes(c+1,r).costSum = blockCost;

	nodes(c,last_row+1).costSum = blockCost;

	// process column (bottom to top)

	/* TODO note: after a subdivision, the flooding for the lower region can
	   be optimized, since only those nodes on the left side that have correspondences
	   on the right side, have to be computed. Hence, computation can stop at a maximum
	   top row. The same optimization is not possible for the upper region, because
	   the region to be computed is on top of the range, not on bottom, but the computation
	   direction is from the bottom up.
	*/

	for (int r=last_row; r>=first_row; r--)
	  {
	    Cost cE  = nodes(c+1,r  ).costSum + cost.CostE (c,r);
	    Cost cSE = nodes(c+1,r+1).costSum + cost.CostSE(c,r);
	    Cost cS  = nodes(c  ,r+1).costSum + cost.CostS (c,r);

	    if (c==0) cS = blockCost;

	    if (cE <= cSE && cE <= cS)
	      {
		nodes(c,r).propagateFrom( nodes(c+1,r), cE, c+1,r);
	      }
	    else if (cSE <= cE && cSE <= cS)
	      {
		nodes(c,r).propagateFrom( nodes(c+1,r+1), cSE, c+1,r+1);
	      }
	    else
	      {
		nodes(c,r).propagateFrom( nodes(c,r+1), cS, c,r+1);
	      }
	  }
      }
  }
#endif


#if 0
template <class CostMatrix, class NodeMatrix>
class Flooding<Cylinder,CostMatrix,NodeMatrix>
{
public:
  static void Flood_Unrestricted(const CostMatrix& cost, NodeMatrix& nodes, int startrow, int endrow=-1)
  {
    typedef typename CostMatrix::CostType Cost;

    typename NodeMatrix::IndexType w = cost.AskWidth();
    typename NodeMatrix::IndexType h = cost.AskHeight();

    //typename NodeMatrix::CellType*const* nodep = nodes.AskFrame();

    if (endrow<0) endrow=startrow;


    // --- first column ---

    // fill first column with high cost and no predecessor

    for (int r=0; r<h; r++)
      {
	nodes(0,r).costSum = CostTraits<Cost>::LargeSum();
	nodes(0,r).setNoPredecessor(); // CHECK: not needed ?
      }


    // Set cost for start-node or for all nodes in [0;h-1] to zero.

    for (int r=startrow; r<=endrow; r++)
      { nodes(0,r).clear(); }


    // --- process remaining columns ---

    const Cost blockerCost = CostTraits<Cost>::LargeSum();

    for (int c=1;c<=w;c++)
      {
	int firstFill = startrow-c;
	int lastFill  = endrow+c;

	// If the computation "delta" exceeds the valid range, reduce it.
	// Otherwise (delta is smaller than valid range), put some blocker-costs to prevent reading uncomputed nodes.
	if (firstFill<0) { firstFill=0;   } else { nodes(c-1,firstFill).costSum = blockerCost; }
	if (lastFill>=h) { lastFill =h-1; } else { nodes(c-1,lastFill ).costSum = blockerCost; }

	// Put blocker-costs to prevent using invalid nodes.
	nodes(c-1,firstFill-1).costSum = blockerCost;
	nodes(c-1,lastFill +1).costSum = blockerCost;

	// process main area (NE, E, and SE)

	for (int r=firstFill;r<=lastFill;r++)
	  {
	    fillNode_Cylinder(nodes, cost, r,c, c-1);
	  }
      }
  }


  static void Flood_Bounded(const CostMatrix& cost, NodeMatrix& nodes, int startrow,
			    const ColumnPath& abovepath,
			    const ColumnPath& belowpath)
  {
    typedef typename CostMatrix::CostType Cost;

    typename NodeMatrix::IndexType w = cost.AskWidth();
    typename NodeMatrix::IndexType h = cost.AskHeight();

    //typename NodeMatrix::CellType*const* nodep = nodes.AskFrame();

    const Cost blockCost = CostTraits<Cost>::LargeSum();

    assert(nodes.AskBorder()>=1);


    /* In each column, we are searching the rows "first_row" until (including) "last_row".
     * Note that first_row and last_row can be mapped to the same node.
     */

    int first_row = startrow;
    int last_row  = startrow;

    // initialize first column

    nodes(0,startrow).clear();
    nodes(0,startrow).processBounds(startrow, abovepath.bottomrow[0], belowpath.toprow[0]);

    // fill graph column by column

    for (int c=1;c<w+1;c++)
      {
	// set guarding values

	if (first_row>0) { nodes(c-1,first_row-2).costSum = blockCost; }
	/**/               nodes(c-1,first_row-1).costSum = blockCost;

	/**/                nodes(c-1,last_row +1).costSum = blockCost;
	if (last_row<h-1) { nodes(c-1,last_row +2).costSum = blockCost; }

	first_row = max(first_row-1, abovepath.toprow   [c]);
	last_row  = min(last_row +1, belowpath.bottomrow[c]);


	// process column

	for (int r=first_row; r<=last_row ;r++)
	  {
	    fillNode_Cylinder(nodes, cost, r,c, c-1);
	    nodes(c,r).processBounds(r, abovepath.bottomrow[c], belowpath.toprow[c]);
	  }
      }
  }





  static void Flood_RightToLeft_Bounded(const CostMatrix& cost, NodeMatrix& nodes,
					const ColumnPath& abovepath,
					const ColumnPath& belowpath,
					const Point2D<int>& touchingpoint)
  {
    const int w = cost.AskWidth();
    const int h = cost.AskHeight();

    typedef typename CostMatrix::CostType Cost;
    const Cost blockCost = CostTraits<Cost>::LargeSum();


    const int touching_column = touchingpoint.x;
    const int touching_row    = touchingpoint.y;

    assert(touching_row <= abovepath.bottomrow[touching_column]);
    assert(touching_row >= belowpath.toprow   [touching_column]);


    /* In each column, we are searching the rows "first_row" until (including) "last_row".
     * Note that first_row and last_row can be mapped to the same node.
     */

    // fill in touching column

    nodes(touching_column,touching_row).clear();

    // fill remaining graph column by column (to the left side)

    for (int c=touching_column-1;c>=0;c--)
      {
	// valid range in previous column

	int first_row = abovepath.toprow[c];
	int last_row  = belowpath.bottomrow[c];

	int prev_first_row = abovepath.toprow   [c+1];
	int prev_last_row  = belowpath.bottomrow[c+1];


	// first, fill in guard values for this column (into column to the right)

	for (int r=prev_first_row-1;r>=first_row-1;r--)
	  nodes(c+1,r).costSum = blockCost;

	for (int r=prev_last_row+1;r<=last_row+1;r++)
	  nodes(c+1,r).costSum = blockCost;

	// process column

	for (int r=first_row; r<=last_row; r++)
	  {
	    Cost cNE = nodes(c+1,r-1).costSum + cost.CostNE(c,r);
	    Cost cE  = nodes(c+1,r  ).costSum + cost.CostE (c,r);
	    Cost cSE = nodes(c+1,r+1).costSum + cost.CostSE(c,r);

	    if (cE <= cSE && cE <= cNE)
	      {
		nodes(c,r).propagateFrom( nodes(c+1,r), cE, c+1,r);
	      }
	    else if (cSE <= cE && cSE <= cNE)
	      {
		nodes(c,r).propagateFrom( nodes(c+1,r+1), cSE, c+1,r+1);
	      }
	    else
	      {
		nodes(c,r).propagateFrom( nodes(c+1,r-1), cNE, c+1,r-1);
	      }
	  }
      }
  }
};
#endif


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


/* TODO: this function could be optimized a bit further, removing the modulo operation as far as possible.
 */
template <class Cell, class EdgeCostsT>
inline void fillNode_reverse(CellMatrix<Cell>& nodes, const EdgeCostsT& cost, int r, int c)
{
  typedef typename CostTraits<typename EdgeCostsT::CostType>::SumType CostSum;

  const int w=cost.getWidth();
  const int h=cost.getHeight();

  CostSum cE;  if (cost.hasE)  cE  = nodes(c+1,r  ).costSum + cost.costE ((c+1)%w,(r  )%h);
  CostSum cSE; if (cost.hasSE) cSE = nodes(c+1,r+1).costSum + cost.costSE((c+1)%w,(r+1)%h);
  CostSum cS;  if (cost.hasS)  cS  = nodes(c  ,r+1).costSum + cost.costS ((c  )%w,(r+1)%h);
  CostSum cNE; if (cost.hasNE) cNE = nodes(c+1,r-1).costSum + cost.costNE((c+1)%w,(r-1)%h);

  //std::cout << c << ";" << r << " - " << cE << " " << cSE << " " << cS << " " << cNE << "\n";

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

 toE:  nodes(c,r).propagateFrom( nodes(c+1,r  ), cE,  c+1,r   ); return;
 toNE: nodes(c,r).propagateFrom( nodes(c+1,r-1), cNE, c+1,r-1 ); return;
 toS:  nodes(c,r).propagateFrom( nodes(c  ,r+1), cS,  c  ,r+1 ); return;
 toSE: nodes(c,r).propagateFrom( nodes(c+1,r+1), cSE, c+1,r+1 ); return;
}


template <class EdgeCostsT, class CellMatrixT>
void Flooding<EdgeCostsT,CellMatrixT>::FloodColumn(const EdgeCostsT& cost, CellMatrixT& nodes,
						   int c, int cModW, int firstFloodRow, int lastFloodRow,
						   int abovePath, int belowPath)
{
  int h = cost.getHeight();

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


template <class EdgeCostsT, class CellMatrixT>
void Flooding<EdgeCostsT,CellMatrixT>::Flood_RightToLeft_Bounded(const EdgeCostsT& cost,
								 CellMatrixT& nodes,
								 const Path& abovepath,
								 const Path& belowpath,
								 const Position& touchingpoint)
{
  typedef typename EdgeCostsT::CostType Cost;
  typedef typename CostTraits<typename EdgeCostsT::CostType>::SumType CostSum;

  const int w = cost.getWidth();
  const int h = cost.getHeight();

  const int touching_column = touchingpoint.x;
  const int touching_row    = touchingpoint.y;

  //std::cout << "touching point: " << touching_column << ";" << touching_row << "\n";

  assert(touching_row <= abovepath.bottomRow(touching_column));
  assert(touching_row >= belowpath.topRow   (touching_column));


  /* In each column, we are searching the rows "first_row" until (including) "last_row".
   * Note that first_row and last_row can be mapped to the same node.
   */

  int first_row = abovepath.topRow(touching_column);
  int last_row  = touching_row;

  // fill in touching column

  const CostSum blockCost = CostTraits<Cost>::LargeSum();

  nodes(touching_column,touching_row).clear();

  /* fill remainder of the column (take only 'S' edges) */

  if (cost.hasS) {
    for (int r=last_row-1; r>= first_row; r--)
      {
	//std::cout << "process " << touching_column << "; " << r << "\n";

	CostSum c = nodes(touching_column,r+1).costSum + cost.costS(touching_column%w,(r+1)%h);
	//std::cout << nodes(touching_column,r+1).costSum << " + " <<  cost.costS(touching_column,(r+1)%h) << " = " << c << "\n";
	nodes(touching_column,r).propagateFrom(nodes(touching_column,r+1), c, touching_column, r+1);
      }
  }


  /* Blocking: in each processed column, we place the blocking values to the right of this column
     and the value just below the current active column.
  */
  //nodes(touching_column, touching_row+1).costSum = blockCost;


  // fill remaining graph column by column (to the left side)

  for (int c=touching_column-1;c>=0;c--)
    {
      // valid range in previous column

      int prev_first_row = first_row;
      int prev_last_row  = last_row;

      first_row = abovepath.topRow(c);
      last_row  = belowpath.bottomRow(c);

      if (c==touching_column-1) prev_last_row = touching_row;


      // first, fill in guard values for this column

      for (int r=prev_first_row-1;r>=first_row-1;r--)
	{
	  nodes(c+1,r).costSum = blockCost;
	  //std::cout << "block top " << c+1 << ";" << r << " = " << nodes(c+1,r).costSum << "\n";
	}

      for (int r=prev_last_row+1; r<=last_row+1;r++)
	{
	  nodes(c+1,r).costSum = blockCost;
	  //std::cout << "block bottom " << c+1 << ";" << r << "\n";
	}

      nodes(c,last_row+1).costSum = blockCost;
      //std::cout << "block S " << c << ";" << last_row+1 << "\n";

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
	  fillNode_reverse(nodes, cost, r,c);
	}
    }
}


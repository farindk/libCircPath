#include "shapematching.hh"
#include "type_edges.hh"
#include "scp_farin.hh"

#define SQRT05 0.7071067811865475244008443621


/***  VERTEX_BASED  *********************************************************/
template <class CostType>
void ShapeMatching<CostType, VERTEX_BASED>::setDissimilarityMatrix(
        int n, CostType *dissimilarity) {
  NodeCosts_Torus<double> cost;
  cost.create(n,n);
  int nodeI = 0;
  for (int r=0; r<n; r++) {
    for (int c=0; c<n; c++,nodeI++) {
      cost.setCost(c,r, dissimilarity[nodeI]);
    }
  }
  
  SCP_Farin<Torus, NodeCosts_Torus<CostType> > iccv07;
  iccv07.setEdgeCosts(cost);
  matching = iccv07.computeMinCostCircularPath();
}



/***  EDGE_BASED  ***********************************************************/
template <class CostType>
void ShapeMatching<CostType, EDGE_BASED>::setDissimilarityMatrix(
        int n, CostType *dissimilarity) {
  EdgeCosts_Torus<double> cost;
  cost.create(n,n);
  int num   = n*n;
  int nodeI = 0;
  CostType H, D, V;
  for (int r=0; r<n-1; r++) {
    for (int c=0; c<n-1; c++,nodeI++) {
      H = 0.5*   (dissimilarity[nodeI]+dissimilarity[(c+1)*n+ r  ]);
      D = SQRT05*(dissimilarity[nodeI]+dissimilarity[(c+1)*n+ r+1]);
      V = 0.5*   (dissimilarity[nodeI]+dissimilarity[(c  )*n+ r+1]);
      cost.setCostE (c,r, H);
      cost.setCostSE(c,r, D);
      cost.setCostS (c,r, V);
    }
    H = 0.5*   (dissimilarity[nodeI]+dissimilarity[        r  ]);
    D = SQRT05*(dissimilarity[nodeI]+dissimilarity[        r+1]);
    V = 0.5*   (dissimilarity[nodeI]+dissimilarity[(n-1)*n+r+1]);
    cost.setCostE (n-1,r, H);
    cost.setCostSE(n-1,r, D);
    cost.setCostS (n-1,r, V);
    nodeI++;
  }
  for (int c=0; c<n-1; c++,nodeI++) {
    H = 0.5*   (dissimilarity[nodeI]+dissimilarity[(c+1)*n+ n-1]);
    D = SQRT05*(dissimilarity[nodeI]+dissimilarity[(c+1)*n     ]);
    V = 0.5*   (dissimilarity[nodeI]+dissimilarity[(c  )*n     ]);
    cost.setCostE (c,n-1, H);
    cost.setCostSE(c,n-1, D);
    cost.setCostS (c,n-1, V);
  }
  H = 0.5*   (dissimilarity[nodeI]+dissimilarity[        n-1]);
  D = SQRT05*(dissimilarity[nodeI]+dissimilarity[          0]);
  V = 0.5*   (dissimilarity[nodeI]+dissimilarity[(n-1)*n    ]);
  cost.setCostE (n-1,n-1, H);
  cost.setCostSE(n-1,n-1, D);
  cost.setCostS (n-1,n-1, V);
  
  SCP_Farin<Torus, EdgeCosts_Torus<CostType> > iccv07;
  iccv07.setEdgeCosts(cost);
  matching = iccv07.computeMinCostCircularPath();
}


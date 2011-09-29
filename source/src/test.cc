
#include <iostream>

using namespace std;


//#include "scp_dynprog.hh"

#include "type_matrix.hh"
#include "type_cell.hh"
#include "type_edges.hh"


void test()
{
  Cell_Std<float> cell;


  Matrix< Cell_Std<float> > matrix;
  matrix.create(10,10);

  for (int x=0;x<matrix.getWidth();x++)
    for (int y=-1;y<matrix.getHeight()+1;y++)
      matrix(x,y).clear();

  EdgeCosts<float,Torus> bla;


  //SCP_DynProg_Torus< EdgeCosts<float,Torus>, Cell_Std<float> > algo;
}

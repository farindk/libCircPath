
#include <iostream>

using namespace std;


//#include "scp_dynprog.hh"

#include "type_matrix.hh"
#include "type_cell.hh"
#include "type_edges.hh"
#include "path.hh"
#include "flooding.hh"


void test_tinyTorus()
{
  static const float edges[2][2][3] = {
    { { 8,12,3 }, { 7,4,5  } },
    { { 2,9,10 }, { 11,1,6 } }
  };

  Cell_Std<float> cell;

  int w = 2;
  int h = 2;

  CellMatrix< Cell_Std<float> > matrix;
  matrix.create(w+1,h*2,1);

  EdgeCosts<float,Torus> bla;
  bla.create(w,h);
  for (int x=0;x<bla.getWidth();x++)
    for (int y=0;y<bla.getHeight();y++)
      {
	std::cout << "x=" << x << " y=" << y << " -> "
		  << edges[y][x][0] << ";"
		  << edges[y][x][1] << ";"
		  << edges[y][x][2] << "\n";

	bla.setCostE (x,y, edges[y][x][0]);
	bla.setCostSE(x,y, edges[y][x][1]);
	bla.setCostS (x,y, edges[y][x][2]);
      }

  typedef Flooding< EdgeCosts<float,Torus>, CellMatrix< Cell_Std<float> > > MyFlooding;

  MyFlooding::Flood_Unrestricted(bla,matrix, 0,h/2, 0,2*h-1);

  std::cout << "B.R.corner cost= " << matrix(w,h).costSum << "\n";
}


void test_largeTorus()
{
  Cell_Std<float> cell;

  int w = 100;
  int h = 100;

  CellMatrix< Cell_Std<float> > matrix;
  matrix.create(w+1,h*2,1);

  for (int x=0;x<matrix.getWidth();x++)
    for (int y=-1;y<matrix.getHeight()+1;y++)
      matrix(x,y).clear();

  EdgeCosts<float,Torus> bla;
  bla.create(w,h);
  for (int x=0;x<bla.getWidth();x++)
    for (int y=0;y<bla.getHeight();y++)
      {
	bla.setCostE (x,y, 1);
	bla.setCostSE(x,y, 3);
	bla.setCostS (x,y, 1);
      }

  typedef Flooding< EdgeCosts<float,Torus>, CellMatrix< Cell_Std<float> > > MyFlooding;

  MyFlooding::Flood_Unrestricted(bla,matrix, 0,0, 0,2*h-1);

  std::cout << "B.R.corner cost= " << matrix(w,h).costSum << "\n";
}

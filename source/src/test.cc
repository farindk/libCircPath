
#include <iostream>

using namespace std;


//#include "scp_dynprog.hh"

#include "type_matrix.hh"
#include "type_cell.hh"
#include "type_edges.hh"
#include "path.hh"
#include "flooding.hh"
#include "scp_dynprog.hh"


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

  EdgeCosts_Torus<float> bla;
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

  typedef Flooding< EdgeCosts_Torus<float>, CellMatrix< Cell_Std<float> > > MyFlooding;

  MyFlooding::Flood_Unrestricted(bla,matrix, 0,h/2, 0,2*h-1);

  std::cout << "B.R.corner cost= " << matrix(w,h).costSum << "\n";
}

#include <stdlib.h>
void test_largeTorus()
{
  Cell_Std<float> cell;

  int w = 20000;
  int h = 20000;

  CellMatrix< Cell_Std<float> > matrix;
  matrix.create(w+1,h*2,1);

  NodeCosts_Torus<unsigned char> bla;
  bla.create(w,h);
  for (int x=0;x<bla.getWidth();x++)
    for (int y=0;y<bla.getHeight();y++)
      {
	int r = rand()%256;
	bla.setCost (x,y, r);
	//bla.setCostSE(x,y, r);
	//bla.setCostS (x,y, r);
      }

  std::cout << "flood...\n";

  typedef Flooding< NodeCosts_Torus<unsigned char>, CellMatrix< Cell_Std<float> > > MyFlooding;

  MyFlooding::Flood_Unrestricted(bla,matrix, 0,0, 0,2*h-1);

  std::cout << "B.R.corner cost= " << matrix(w,h).costSum << "\n";
}

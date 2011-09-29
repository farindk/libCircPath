
#ifndef TYPE_BASIC_HH
#define TYPE_BASIC_HH

struct Position
{
  Position() { }
  Position(int _x, int _y) : x(_x), y(_y) { }

  int x,y;
};

#endif

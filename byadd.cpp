#include <stdio.h>

struct Data {
    int x, y;
    int (*board)[2];
};

void f(struct Data &r) {
  printf("This here is the value %i %i\n", r.x, r.y);
  printf("This is the value in struct %i\n", r.board[1][1]);
  r.x = 3;
}
int main() {
  int board[][2] = {{1,2}, {3,4}};
  int (*p)[2] = board;
  struct Data b[1];
  b[0].x = 1, b[0].y = 2; b[0].board = p;
  f(b[0]);
  printf("This the board.x: %i\n", b[0].x);
  return 0;
}

#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <algorithm>
using namespace std;
const int N = 4;
const bool OK = 1;
const int dx[] = {-1,0,1,0};
const int dy[] = {0,-1,0,1};

int cell[N][N];
int out[N][N];

void init_rand() { srand(time(NULL)); }
int getrand() { return rand(); }

// http://ja.wikipedia.org/wiki/Xorshift
unsigned xor128(void) { 
  static unsigned x = 123456789;
  static unsigned y = 362436069;
  static unsigned z = 521288629;
  static unsigned w = 88675123; 
  unsigned t;
 
  t = x ^ (x << 11);
  x = y; y = z; z = w;
  return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)); 
}

void getrxy(int &x,int &y) {
  int RN = getrand() % (N*N);
  x = RN/N; y = RN%N;
}

bool good(int x,int y) {
  if(x > 0 && y < N-1 && cell[x-1][y] && cell[x-1][y+1] && cell[x][y+1])
    return 0;
  if(x > 0 && y > 0 && cell[x-1][y] && cell[x-1][y-1] && cell[x][y-1])
    return 0;
  if(x < N-1 && y < N-1 && cell[x+1][y] && cell[x][y+1] && cell[x+1][y+1])
    return 0;
  if(x < N-1 && y > 0 && cell[x+1][y] && cell[x][y-1] && cell[x+1][y-1])
    return 0;
  return 1;
}

void visit1(int x,int y) {
  cell[x][y] = -1;
  int RN = getrand() % (1<<4);
  int nx, ny;
  for(int i = 0;i < 4;i++) {
    nx = x+dx[i]; ny = y+dy[i];
    if(0 > nx || 0 > ny || nx >= N || ny >= N) continue;
    if(cell[nx][ny]) continue;
    if(((RN>>i)&1) && good(nx,ny)) {
      visit1(nx,ny);
    }
  }
}

int visit2(int x,int y) {
  if(cell[x][y]) return 0;
  cell[x][y] = 1;
  int ret = 1;
  for(int d = 0;d < 4;d++) {
    int nx = x+dx[d], ny = y+dy[d];
    if(0 > nx || 0 > ny || nx >= N || ny >= N) continue;
    ret += visit2(nx,ny);
  }
  return ret;
}

int main() {
  memset(cell,0,sizeof cell);
  int sx, sy; getrxy(sx,sy); visit1(sx,sy); // mm
  
  for(int i = 0;i < N;i++) {
    for(int j = 0;j < N;j++) {
      if(cell[i][j] == 0) {
	out[i][j] = visit2(i,j);
      }
      if(cell[i][j] == -1) {
	out[i][j] = -1;
      }
    }
  }
  
  printf("PROBLEM\n");
  for(int i = 0;i < N;i++) {
    for(int j = 0;j < N;j++) {
      if(out[i][j] > 0) printf("%3d",out[i][j]);
      else printf("%3c",'o');
    }
    printf("\n");
  }
  printf("SOLUTION\n");
  for(int i = 0;i < N;i++) {
    for(int j = 0;j < N;j++) {
      if(out[i][j] > 0) printf("%3d",out[i][j]);
      else if(out[i][j] == 0) printf("%3c",'W');
      else printf("%3c",'B');
    }
    printf("\n");
  }
  return 0;
}

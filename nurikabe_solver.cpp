#include <cstdio>
#include <vector>
#include <stack>
#include <iostream>
#include <cstring>
using namespace std;

/*
  実行時間計測用
*/
#include <sys/time.h>
#include <unistd.h>

struct __bench__ {
  double start;
  __bench__() {
    start = sec();
  }
  ~__bench__() {
    fprintf(stdout, "%.6f sec\n", sec() - start);
  }
  double sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
  }
  operator bool() { return false; }
};

#define benchmark if ( __bench__ __b__ = __bench__()); else


#define UNSPCF 0
#define WHITE -1
#define BLACK -2
#define DUM -3

#define debug(x) printf(x)


const int dx[] = {-1,0,1,0};
const int dy[] = {0,-1,0,1};

int N;

struct puzzle{
/*
  パズルのボードを表すクラス
*/
  vector< vector<int> > id;
  vector< vector<int> > cell;
  /*
    cell[i][j]はマス(i,j)の状態を表す.
      -cell[i][j] > 0 なら白マス
      -cell[i][j] == 0(UNSPCF) なら未定
      -cell[i][j] == -1(WHITE) なら白マス確定
      -cell[i][j] == -2(BLACK) なら黒
      -cell[i][j] == -3(DUM) なら番兵
  */

  puzzle(){};
  puzzle(int n) {
    cell = vector< vector<int> >(N+2, vector<int>(N+2,UNSPCF));
    id = vector< vector<int> >(N+2, vector<int>(N+2,0));
    /* 番兵をしく */
    for(int i = 0;i <= N+1;i++) {
      cell[0][i] = DUM;
      cell[i][0] = DUM;
      cell[N+1][i] = DUM;
      cell[i][N+1] = DUM;
    }
  }

  void print() {
    for(int i = 1;i <= N;i++) {
      for(int j = 1;j <= N;j++) {
        if(cell[i][j] > 0) printf("%3d",cell[i][j]);
        else if(cell[i][j] == UNSPCF)  printf("%3c",'N');
        else if(cell[i][j] == WHITE) printf("%3c",'W');
        else if(cell[i][j] == BLACK) printf("%3c",'B');
      }
      printf("\n");
    }
  }
} ques, answer;

vector< vector<bool> > used, used2;

/*
連結していて数字がp.cell[[i][j]であるマスを数える
対象 : 黒, 白
*/
int count(puzzle &p, int x, int y) {
  if(used[y][x]) return 0;
  used[y][x] = true;

  int res = 1;

  for(int r = 0;r < 4;r++) {
    int rx = x + dx[r], ry = y + dy[r];

    if(p.cell[ry][rx] == DUM) continue;

    if(p.cell[ry][rx] == p.cell[y][x]) {
      res += count(p, rx, ry);
    }
  }
  return res;
}

/*
  確定しているマス群の周りを黒マスで埋める
  対象 : 白
*/
bool fill_ar(puzzle &p, int x, int y) {
  if(used2[y][x]) return false;
  used2[y][x] = true;

  bool ret = false;
  for(int r = 0;r < 4;r++) {
    int rx = x + dx[r], ry = y + dy[r];

    if(p.cell[ry][rx] == DUM) continue;

    if(p.cell[ry][rx] == UNSPCF) {
      p.cell[ry][rx] = BLACK;
      ret = true;
    }

    if(p.id[ry][rx] == p.id[y][x]) {
      if(fill_ar(p, rx, ry)) {
        ret = true;
      }
    }
  }

  return ret;
}

/*
  マス群に隣接する未確定のマスの数(vcnt)を数える
  ついでに同色のマスの数(cnt)も数える
  対象 : 黒,　白
*/
void count_UNSPCF(puzzle &p,int x,int y,int &cnt,int &vcnt) {
  if(used[y][x]) return ;
  used[y][x] = true;

  cnt++;

  for(int r = 0;r < 4;r++) {
    int rx = x + dx[r], ry = y + dy[r];

    if(p.cell[ry][rx] == DUM) continue;

    if(p.cell[ry][rx] == UNSPCF || (p.cell[y][x] > 0 && p.cell[ry][rx] == WHITE)) {
        vcnt++;
    }

    if(p.cell[ry][rx] == p.cell[y][x]) {
      count_UNSPCF(p, rx, ry, cnt, vcnt);
    }
  }

  return ;
}

/*
  マス群に隣接する未確定のマス１つ（仮定）を埋める。
  対象 : 黒, 白
*/
void fill_theUNSPCF(puzzle &p,int x,int y) {
  if(used2[y][x]) return ;
  used2[y][x] = true;

  int res = 0;
  for(int r = 0;r < 4;r++) {
    int rx = x + dx[r], ry = y + dy[r];

    if(p.cell[ry][rx] == DUM) continue;

    if(p.cell[ry][rx] == UNSPCF) {
        p.id[ry][rx] = p.id[y][x];
        p.cell[ry][rx] = p.cell[y][x];
        used2[ry][rx] = 1;
        return ;
    }

    if(p.cell[ry][rx] == p.cell[y][x]) {
      fill_theUNSPCF(p, rx, ry);
    }
  }
}

/*
  未確定のマス群が"数の無い島"になってないかチェックする
  対象 : 未確定
*/
bool check_UNSPCFs(puzzle &p,int x,int y) {
  if(used[y][x]) return false;
  used[y][x] = true;

  bool ret = false;

  for(int r = 0;r < 4;r++) {
    int rx = x + dx[r], ry = y + dy[r];

    if(p.cell[ry][rx] == DUM) continue;

    if(p.cell[ry][rx] != BLACK && p.cell[ry][rx] != UNSPCF) {
      ret = true; // 数 or N or Wを見つけた
    }

    if(p.cell[ry][rx] == UNSPCF) {
      if(check_UNSPCFs(p, rx, ry))
        ret = true;
    }
  }

  return ret;
}

/*
  "数の無い島"になっている未確定のマス群を黒で塗りつぶす
*/
void fill_theUNSPCFs(puzzle &p,int x,int y) {
  if(used2[y][x]) return ;
  used2[y][x] = true;
  p.cell[y][x] = BLACK;

  for(int r = 0;r < 4;r++) {
    int rx = x + dx[r], ry = y + dy[r];

    if(p.cell[ry][rx] == UNSPCF) {
        fill_theUNSPCFs(p, rx, ry);
    }
  }
}

bool setbok(puzzle &p,int x,int y) {
  return (p.cell[y-1][x]==BLACK && p.cell[y-1][x-1]==BLACK && p.cell[y][x-1]==BLACK) ||
         (p.cell[y+1][x]==BLACK && p.cell[y+1][x-1]==BLACK && p.cell[y][x-1]==BLACK) ||
         (p.cell[y+1][x]==BLACK && p.cell[y+1][x+1]==BLACK && p.cell[y][x+1]==BLACK) ||
         (p.cell[y-1][x]==BLACK && p.cell[y-1][x+1]==BLACK && p.cell[y][x+1]==BLACK);
}

/*
  定石手法によりパズルを埋める
*/

//#define DEBUG1
//#define DEBUG2

bool prep(puzzle &p) {
  /*
    colored : 進捗があったかどうか、すなわちある未確定のマスを白または黒にしたか
  */
  bool colored = false;

#ifdef DEBUG2
  printf("%d\n",__LINE__);
  p.print();// getchar();
#endif // DEBUG2

  /*
   隣接ブロック法 :
   異なる島の白マスが1つの未定のマスを挟んで隣り合っているとき、その未定のマスは必ず黒マス
  */
  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++) {
      if(p.cell[i][j] > 0) {

        if(i+2 <= N && p.id[i+2][j] > 0 && p.id[i+2][j] != p.id[i][j]) {
          colored |= (p.cell[i+1][j] == UNSPCF);
          p.cell[i+1][j] = BLACK;
        }
        if(i > 2 &&p.cell[i-2][j] > 0 && p.id[i-2][j] != p.id[i][j]) {
          colored |= (p.cell[i-1][j] == UNSPCF);
          p.cell[i-1][j] = BLACK;
        }
        if(j+2 <= N && p.cell[i][j+2] > 0 && p.id[i][j+2] != p.id[i][j]) {
          colored |= (p.cell[i][j+1] == UNSPCF);
          p.cell[i][j+1] = BLACK;
        }
        if(j > 2 && p.cell[i][j-2] > 0 && p.id[i][j-2] != p.id[i][j]) {
          colored |= (p.cell[i][j-1] == UNSPCF);
          p.cell[i][j-1] = BLACK;
        }
        if(p.cell[i+1][j+1] > 0 && p.id[i+1][j+1] != p.id[i][j]) {
          colored |= (p.cell[i+1][j] == UNSPCF || p.cell[i][j+1] == UNSPCF);
          p.cell[i+1][j] = p.cell[i][j+1] = BLACK;
        }
        if(p.cell[i+1][j-1] > 0 && p.id[i+1][j-1] != p.id[i][j]) {
          colored |= (p.cell[i+1][j] == UNSPCF || p.cell[i][j-1] == UNSPCF);
          p.cell[i+1][j] = p.cell[i][j-1] = BLACK;
        }
        if(p.cell[i-1][j+1] > 0 && p.id[i-1][j+1] != p.id[i][j]) {
          colored |= (p.cell[i-1][j] == UNSPCF || p.cell[i][j+1] == UNSPCF);
          p.cell[i-1][j] = p.cell[i][j+1] = BLACK;
        }
        if(p.cell[i-1][j-1] > 0 && p.id[i-1][j-1] != p.id[i][j]) {
          colored |= (p.cell[i-1][j] == UNSPCF || p.cell[i][j-1] == UNSPCF);
          p.cell[i-1][j] = p.cell[i][j-1] = BLACK;
        }
      }
    }
  }

#ifdef DEBUG2
  printf("%d\n",__LINE__);
  p.print();// getchar();
#endif // DEBUG2

  /*
    確定白マス隣接法:
    ある島に確定した島がある場合
  */
  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++) {
      if(p.cell[i][j] == WHITE) {
        if(p.cell[i+1][j] > 0) { p.cell[i][j] = p.cell[i+1][j]; p.id[i][j] = p.id[i+1][j]; }
        else if(p.cell[i-1][j] > 0) { p.cell[i][j] = p.cell[i-1][j]; p.id[i][j] = p.id[i-1][j]; }
        else if(p.cell[i][j+1] > 0) { p.cell[i][j] = p.cell[i][j+1]; p.id[i][j] = p.id[i][j+1]; }
        else if(p.cell[i][j-1] > 0) { p.cell[i][j] = p.cell[i][j-1]; p.id[i][j] = p.id[i][j-1]; }
      }
    }
  }

#ifdef DEBUG2
  printf("%d\n",__LINE__);
  p.print();// getchar();
#endif // DEBUG2

  /*
    確定法:
    確定している島の周りは必ず黒マス
  */

  // 初期化
  for(int i = 1;i <= N;i++){
    for(int j = 1;j <= N;j++){
      used[i][j] = used2[i][j] = false;
    }
  }

  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++) {
      if(p.cell[i][j] > 0 && !used[i][j]) {
        if(count(p, j, i) == p.cell[i][j]) {
          if(fill_ar(p, j, i))
            colored = true;
        }
      }
    }
  }

#ifdef DEBUG2
  printf("%d\n",__LINE__);
  p.print();// getchar();
#endif // DEBUG2
  /*
    延ばし法:
    ある黒のブロックが他のブロックと繋がっていなく、その黒のマス群を他の黒のマス群とつなぐ方法が１つしかない場合、その1つの方法をとる.
    これは白すなわち島でも同じことが言える.
    黒はすでに全域木になっている場合の処理が必要
  */

  int num_black = 0;
  int bfld = 0, wfld = 0;
  // 初期化
  for(int i = 1;i <= N;i++){
    for(int j = 1;j <= N;j++){
      used[i][j] = used2[i][j] = false;

      if(p.cell[i][j] == BLACK) {
        num_black++;
      }
    }
  }

  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++) {
      if(!used[i][j]) {
          /*
            cnt : 同じ色のマスの数
            vcnt : 同色ブロック群に隣接する未確定のマスの数
          */
        int cnt = 0, vcnt = 0;
        if(!wfld && p.cell[i][j] > 0) {
          count_UNSPCF(p, j, i, cnt, vcnt);
          if(vcnt == 1 && cnt < p.cell[i][j]) {
        //    printf("NOBASITERUYO %d %d W\n",i,j);
            fill_theUNSPCF(p, j, i);
            colored = true;
            wfld = 1;
          }
        }
        if(!bfld && p.cell[i][j] == BLACK) {
          count_UNSPCF(p, j, i, cnt, vcnt);
          if(vcnt == 1 && cnt < num_black) {
         //   printf("NOBASITERUYO %d %d B\n",i,j);
            fill_theUNSPCF(p, j, i);
            colored = true;
            bfld = 1;
          }
        }
      }
    }
  }

#ifdef DEBUG2
  printf("%d\n",__LINE__);
  p.print();// getchar();
#endif // DEBUG2
  /*
    空の島法 :
    黒マスで囲まれた島で、島に数字がないときは島のマスはすべて黒
  */
  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++){
      used[i][j] = used2[i][j] = false;
    }
  }

  for(int i = 1;i <= N;i++){
    for(int j = 1;j <= N;j++){
      if(p.cell[i][j] == UNSPCF && !used[i][j]) {
        if(!check_UNSPCFs(p, j, i)) {
          fill_theUNSPCFs(p, j, i);
          colored = true;
        }
      }
    }
  }
#ifdef DEBUG2
  printf("%d\n",__LINE__);
  p.print();// getchar();
#endif // DEBUG2

  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++) {
      // 2x2ブロックになるので黒マス置けない
      if(p.cell[i][j] == UNSPCF && setbok(p,j,i)) {
        if(p.cell[i+1][j] > 0) { p.cell[i][j] = p.cell[i+1][j]; p.id[i][j] = p.id[i+1][j]; }
        else if(p.cell[i-1][j] > 0) { p.cell[i][j] = p.cell[i-1][j]; p.id[i][j] = p.id[i-1][j]; }
        else if(p.cell[i][j+1] > 0) { p.cell[i][j] = p.cell[i][j+1]; p.id[i][j] = p.id[i][j+1]; }
        else if(p.cell[i][j-1] > 0) { p.cell[i][j] = p.cell[i][j-1]; p.id[i][j] = p.id[i][j-1]; }
        else p.cell[i][j] = WHITE;
        colored = true;
      }
    }
  }

  return colored;
}

// マスが塗りつぶされたマス群の端かどうか
bool onedge(puzzle &p,int x,int y) {
  for(int r = 0;r < 4;r++) {
    int rx = x+dx[r], ry = y+dy[r];
    if(p.cell[ry][rx] != UNSPCF && p.cell[ry][rx] != DUM)
      return true;
  }
  return false;
}

bool solved;
int nofedge;
int check1(puzzle &p,int x,int y) {
  if(used[y][x]) return 0;
  used[y][x] = 1;

  int ret = 1;

  for(int r = 0;r < 4;r++) {
    int rx = x + dx[r], ry = y + dy[r];

    if(p.cell[ry][rx] == DUM) continue;

    if(p.cell[ry][rx] == BLACK) {
      nofedge++;
      ret += check1(p,rx,ry);
    }
  }

  return ret;
}

int check2(puzzle &p,int x,int y) {
  if(used[y][x]) return 0;
  used[y][x] = 1;

  int ret = 1;
  for(int r = 0;r < 4;r++) {
    int rx = x + dx[r], ry = y + dy[r];

    if(p.cell[ry][rx] == DUM) continue;

    if(p.cell[ry][rx] > 0 && p.cell[ry][rx] != p.cell[y][x]) solved = false;

    if(p.cell[ry][rx] > 0) {
      ret += check2(p,rx,ry);
    }
  }

  return ret;
}
/*
  パズルの正当性判定には検証すればよい
   1. 黒マスが全域木になっている事
   2. 島には１つの数字しかない
   3. 島の白マスの個数がその島の数と一致する
   4. 2x2の黒マスブロックが無い
*/

bool valid1(puzzle &p) {
  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++) {
       if(p.cell[i-1][j]==BLACK && p.cell[i-1][j-1]==BLACK &&
            p.cell[i][j-1]==BLACK && p.cell[i][j]==BLACK ) {
//            printf("FAIL! %d %d\n",i,j);
            return false;
       }
       if(p.cell[i][j] > 0) {
          if(p.cell[i-1][j] > 0 && p.id[i-1][j] != p.id[i][j]) return false;
          if(p.cell[i+1][j] > 0 && p.id[i+1][j] != p.id[i][j]) return false;
          if(p.cell[i][j-1] > 0 && p.id[i][j-1] != p.id[i][j]) return false;
          if(p.cell[i][j+1] > 0 && p.id[i][j+1] != p.id[i][j]) return false;
       }
    }
  }
  return true;
}

bool valid2(puzzle &p) {
  solved = true;

  int num_black = 0;
  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++) {
      if(p.cell[i][j] == BLACK)
        num_black++;
    }
  }

  /* 1のチェック */
  for(int i = 1;i <= N;i++)
    for(int j = 1;j <= N;j++)
      used[i][j] = false;

  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++) {
      if(!used[i][j] && p.cell[i][j] == BLACK) {
        int cnt = 0, vcnt = 0;
        count_UNSPCF(p, j, i, cnt, vcnt);
        if(vcnt == 0 && cnt != num_black) {
          solved = false;
   //       printf("X %d\n",__LINE__);
        }
      }
    }
  }

  return solved;
}

bool valid3(puzzle &p) {
  solved = true;

  /* 2,3のチェック */
  for(int i = 1;i <= N;i++)
    for(int j = 1;j <= N;j++)
      used[i][j] = false;

  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++) {
      if(!used[i][j] && p.cell[i][j] > 0) {
        int cnt = 0, vcnt = 0;
        count_UNSPCF(p, j, i, cnt, vcnt);
        if(vcnt == 0 && p.cell[i][j] != cnt) {
          solved = false;
          //printf("X %d\n",__LINE__);
        }
      }
    }
  }

  return solved;
}

bool valid4(puzzle &p) {
  solved = true;

  int num_black = 0;
  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++) {
      if(p.cell[i][j] == UNSPCF || p.cell[i][j] == WHITE){
        solved = false;
  //      printf("X %d\n",__LINE__);
      }
    }
  }

  int nofedge = 0;
  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++) {
      if(p.cell[i][j] == BLACK) {
        num_black++;
        if(p.cell[i-1][j] == BLACK) nofedge++;
        if(p.cell[i][j-1] == BLACK) nofedge++;
      }
    }
  }

  // 木であるか判定
  //if(num_black > 0 && nofedge != num_black-1) solved = false;

  return solved;
}

bool found;
/*
枝狩り案 :
まわりに未確定マスが無い黒マス群について :
他の黒マス群がある場合、ルールに矛盾
*/

void solve(puzzle &p) {
  if(found) return ;

#ifdef DEBUG1
puts("prep s");
#endif // DEBUG1

 while(prep(p)) ;

    if(!valid1(p)) {
#ifdef DEBUG1
    printf("%d\n",__LINE__);
#endif // DEBUG1
      return ;
    }

    if(!valid2(p)) {
#ifdef DEBUG1
    printf("%d\n",__LINE__);
#endif // DEBUG1
      return ;
    }

    if(!valid3(p)) {
#ifdef DEBUG1
    printf("%d\n",__LINE__);
#endif // DEBUG1
      return ;
    }


#ifdef DEBUG1
p.print();
puts("prep f"); //getchar();
#endif // DEBUG1

 if(valid4(p)) {

#ifdef DEBUG1
    printf("\nFOUND!\n");
#endif // DEBUG1

    answer = p;
    found = 1;
    return ;
  }

  bool ret = false;
  for(int i = 1;i <= N;i++) {
    for(int j = 1;j <= N;j++) {
      int &cur = p.cell[i][j];

      // 塗りつぶされたマス群に隣接したマスでない場合飛ばす
      if(!onedge(p, j, i)) continue;

      if(cur != UNSPCF) continue;

 #ifdef DEBUG1
 printf("kuromasu okuyo %d %d\n",i,j);
 #endif // DEBUG1

      // 黒マスを置いてみる
      puzzle NewPuzzle = p;
      NewPuzzle.cell[i][j] = BLACK;

      solve(NewPuzzle);
      if(!found) {
        /* 白マス確定となる. 隣接する島があればその島のマスとなる. */
        if(p.cell[i+1][j] > 0) { cur = p.cell[i+1][j]; p.id[i][j] = p.id[i+1][j]; }
        else if(p.cell[i-1][j] > 0) { cur = p.cell[i-1][j]; p.id[i][j] = p.id[i-1][j]; }
        else if(p.cell[i][j+1] > 0) { cur = p.cell[i][j+1]; p.id[i][j] = p.id[i][j+1]; }
        else if(p.cell[i][j-1] > 0) { cur = p.cell[i][j-1]; p.id[i][j] = p.id[i][j-1]; }
        else cur = WHITE;

        solve(p);
      }
    }
  }
  return ;
}

int main(int argc, char *fp[]) {
  if(argc < 2) {
    printf("ERROR\n");
    return 1;
  }
  freopen(fp[1],"r",stdin);
  //  freopen("s.txt","w",stdout);

  /*
    found : 真 iff 解が見つかった
  */
  found = false;

  /*
    パズルの読み込み
  */
  scanf("%d",&N);

  ques = puzzle(N);
  answer = puzzle(N);

  int id_att = 1;
  for(int i = 1;i <= N;i++) {
    for(int j =1;j <= N;j++) {
      scanf("%d",&ques.cell[i][j]);
      if(ques.cell[i][j] > 0) {
        ques.id[i][j] = id_att++;
      }
    }
  }
#ifdef DEBUG1
puts("Input Completed.\n");
#endif // DEBUG1


  benchmark {

  /*
    深さ優先探索によりパズルの解を探す
  */
  used = vector< vector<bool> >(N+2, vector<bool>(N+2));
  used2 = vector< vector<bool> >(N+2, vector<bool>(N+2));

  solve(ques);

  if(found) {
    answer.print();
  }else {
    puts("There is no solution!");
  }

  }
  return 0;
}

#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

int player, nextplayer;
const int SIZE = 8;
array<std::array<int, SIZE>, SIZE> board;
array<array<int, SIZE>, SIZE> ideal{
  20, 0, 10, 8, 8, 10, 0, 20,
   0, 0,  8, 2, 2,  8, 0,  0,
  10, 8,  0, 4, 4,  0, 8, 10,
   8, 2,  4, 1, 1,  4, 2,  8,
   8, 2,  4, 1, 1,  4, 2,  8,
  10, 8,  0, 4, 4,  0, 8, 10,
   0, 0,  8, 2, 2,  8, 0,  0,
  20, 0, 10, 8, 8, 10, 0, 20
};
struct Point{
  int x, y, nice, deep, max, min;
  Point() : Point(0, 0, 0, 0, 0, 0) {}
  Point(int x, int y) :
  x(x), y(y),nice(0),deep(0),max(0),min(0) {}
  Point(int x, int y, int nice, int deep) :
  x(x), y(y),nice(nice),deep(deep),max(-100),min(100) {}
	Point(int x, int y, int nice, int deep,int max, int min) :
  x(x), y(y),nice(nice),deep(deep),max(max),min(min) {}
  Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
};
std::vector<Point> next_valid_spots;

void read_board(std::ifstream& fin) {
    fin >> player;
    nextplayer = 3 - player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}
void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y, 0, 0});
    }
}

const array<Point, SIZE> directions{
        Point(-1, -1),
        Point(-1, 0),
        Point(-1, 1),
        Point(0, -1),
        Point(0, 1),
        Point(1, -1),
        Point(1, 0),
        Point(1, 1)
    };
bool terminal(Point& a){
return false;
}
bool is_spot_on_board(Point p) {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
bool is_disc_at(Point p, int disc) {
        if (!is_spot_on_board(p))
            return false;
        if (board[p.x][p.y] != disc)
            return false;
        return true;
    }
bool is_spot_valid(Point center, int disc){
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, 3 - disc))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && board[p.x][p.y] != 0) {
                if (is_disc_at(p, disc))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
vector<Point> get_valid_spots(Point a, int disc){
        std::vector<Point> valid_spots;
        int deep = a.deep + 1;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j, 0, deep, a.max, a.min);
                if (board[i][j] != 0)
                    continue;
                if (is_spot_valid(p, disc))
                    valid_spots.push_back(p);

            }
        }
        return valid_spots;
    }
vector<Point> flip_discs(Point center, int disc){
        std::vector<Point> flip;

         for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, 3 - disc))
                continue;
          std::vector<Point> mayflip({p});
          p = p + dir;
            while (is_spot_on_board(p) && board[p.x][p.y] != 0) {
                if (is_disc_at(p, disc)) {
                    for (Point s: mayflip) {
                      flip.push_back(s);
                    }
                    break;
                }
                mayflip.push_back(p);
                p = p + dir;
            }
        }
        return flip;
    }
void calculate(array<array<int, SIZE>, SIZE>, Point &b){
  int count = 0;
   for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
            if(board[i][j] == 1) {
              count += ideal[i][j];
            }else if(board[i][j] == 2)
              count -= ideal[i][j];
            }
   }
   b.nice = count;
}

void Min(Point&);
void Max(Point& b){

    if(terminal(b) || b.deep > 3) {
        calculate(board, b);
        return;
    }

    vector<Point> valid;
    valid = get_valid_spots(b, player);

    int max = 0;
    for(Point a : valid){
      vector<Point> flip;
      board[a.x][a.y] = player;
      flip = flip_discs(a, player);
      for(Point p : flip) board[p.x][p.y] = player;
      Min(a);

      if(a.nice > max){
          b.nice = a.nice;
          max = a.nice;
        }
      board[a.x][a.y] = 0;
      for(Point p : flip) board[p.x][p.y] = nextplayer;
      if(max >= b.min) break;
      if(max > b.max) b.max = max;
  }
  return;
}
void Min(Point& a){
    if(terminal(a) || a.deep > 3) {
        calculate(board, a);
        return;
    }

    vector<Point> valid;
    valid = get_valid_spots(a, nextplayer);

    int min = 100;
    for(Point b: valid){
        vector<Point> flip;
        board[b.x][b.y] = nextplayer;
        flip = flip_discs(b, nextplayer);
        for(Point p : flip) board[p.x][p.y] = nextplayer;

        Max(b);
        if(b.nice < min){
          a.nice = b.nice;
          min = b.nice;
        }
        board[b.x][b.y] = 0;
        for(Point p : flip) board[p.x][p.y] = player;
        if(min <= a.max) break;
        if(min < a.min) a.min = min;
    }
  return;
}
void write_valid_spot(std::ofstream& fout) {

    int best = 0;
    for(Point a : next_valid_spots){
        vector<Point> flip;
        board[a.x][a.y] = player;
        flip = flip_discs(a, player);
        for(Point p : flip) board[p.x][p.y] = player;

        if(best > a.max) a.max = best;
        Min(a);

        if(a.nice > best){
            cout << a.x << a.y << endl;
            best = a.nice;
            fout.flush();
        }

        board[a.x][a.y] = 0;
        for(Point p : flip) board[p.x][p.y] = nextplayer;
  }
    return;
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}

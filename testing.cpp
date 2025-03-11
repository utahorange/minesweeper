#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;

std::string padWithZeroes(int n, int len) {
    stringstream oss;
    oss << n;
    string s = oss.str();
    while (s.size() != len) {
        s = "0" + s;
    }
    return s;
}
int main() {
    struct Coord{
        int x;
        int y;
        Coord(int a,int b) : x(a), y(b) {}
        bool operator==(const Coord& a) {
            return (a.x == x && a.y == y);
        }
    };
    
    Coord a = Coord(1,2);
    Coord b = Coord(1,3);
    if (a==b) {
        cout << "cooked" << endl;
    } else {
        cout << "extremely cooked" << endl;
    }
    string s = "\033[1;32m" + std::to_string(1) + "\033[0m\n";
    cout << s << endl;
    cout << "\033[1;32m#\033[0m\n";

    std::vector<Coord> coords;
    // coords.insert(b);
    // for (coord i : coords) {
    //     cout << "haha" << endl;
    // }
}


int main() {
    int score = 35;
    cout << padWithZeroes(score,7) << endl;
}
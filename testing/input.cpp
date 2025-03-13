// #include <iostream>
// using namespace std;
// int main() {
//     // cout << "\035[1;32mO\035[0m\n" << endl;
//     // cout << "\033[1;31mF\033[0m\n" << endl;
//     // cout << "\033[1;32m#\033[0m\n" << endl;

// }

// #include <stdio.h>

// int main ()
// {
//   int c;
//   puts ("Enter text. Include a dot ('.') in a sentence to exit:");
//   do {
//     c=getchar();
//     putchar (c);
//   } while (c != '.');
//   return 0;
// }

#include <iostream>
using namespace std;

bool isdigit(string s) {
  for (char& c : s) {
    if (!isdigit(c)) return false;
  }
  return true;
}


int getMoveCoords(int& r, int&c) {
    string s;
    std::cout << "Input x: ";
    getline(cin,s);
    if (isdigit(s)) {
        c = stoi(s); 
    } else {
        return -1;
    }
    std::cout << "Input y: ";
    getline(cin,s); // take in y
    if (isdigit(s)) {
        r = stoi(s);
    } else {
        return -1;
    }
    return 0;
}
int main() {
  // int c;
  // do {
  //   c=getchar();
  //   putchar (c);
  // } while (c != '.');
  // return 0;
  // int r;
  // int c;
  // int i = getMoveCoords(r,c);
  // cout << i << endl;
  // cout << r << endl;
  // cout << c << endl;
  // cout << isdigit("") << endl;
  string s;
  do {
    getline(cin,s);
    cout << s;
  } while( 1==1);

  // int r = 0;
  // int c = 0;
  // std::cout << "Input x: ";
  // r = getchar();
  // if(!isdigit(r)) return 0;

  // cout << r - '0' << endl;

  // std::cin >> c; // take in x

  // std::cout << "Input y: ";
  // std::cin >> r; // take in y
  int r = 2;
  int c = 1;
  char *gameBoard = new char[r][c];

}
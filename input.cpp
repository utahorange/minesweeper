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

int main() {
  // int c;
  // puts ("Enter text. Include a dot ('.') in a sentence to exit:");
  // do {
  //   c=getchar();
  //   putchar (c);
  // } while (c != '.');
  // return 0;
  string s = "\033[1;31mF\033[0m";
  cout << s.size() << endl;
  s = "\033[1;34m" + s + "\033[0m"; 
  cout << s << endl;
  s = s.substr(7);
  cout << s.size() << endl;
  cout << s << endl;
}
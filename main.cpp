#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::cin;

vector<char *> split(string);

int main() {
  int child;
  string cmd;

  while (true) {
    cout << "> ";
    getline(cin, cmd);

    if (cmd == "exit") {
      break;
    }

    child = fork();

    if (child == 0) {
      vector<char *> vcmds = split(cmd);
      char **cmds = &vcmds[0];
      execvp(cmds[0], cmds);
      exit(0);
    } else {
      wait(&child);
    }
  }

  return 0;
}

vector<char *> split(string cmd) {
  // dividir string en por cada espacio
  vector<char *> args;
  size_t desde = 0;
  size_t hasta = cmd.find(" ");

  while (hasta != string::npos) {
    char *palabra = new char(hasta - desde);
    strcpy(palabra, cmd.substr(desde, hasta - desde).c_str());
    args.push_back(palabra);

    desde = hasta + 1;
    hasta = cmd.find(" ", desde);
  }

  // guardar la ultima palabra
  char *palabra = new char(hasta - desde);
  strcpy(palabra, cmd.substr(desde, hasta - desde).c_str());
  args.push_back(palabra);
  args.push_back(NULL);

  return args;
}
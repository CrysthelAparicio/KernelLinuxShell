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
void simplepipe(char **);

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

      if (vcmds.size() == 4 && strcmp(cmds[1], "|") == 0) {
        simplepipe(cmds);
      } else {
        execvp(cmds[0], cmds);
      }
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

void simplepipe(char **commands) {
  // si contiene el caracter '|' y solo hay 4 argumentos es un pipe simple:
  // pipe[1] => data => pipe[0]
  int pipes[2];
  pipe(pipes);

  int primer_comando = fork();
  if (primer_comando == 0) {
    // cerramos stdout y lo cambiamos por nuestro pipe
    close(STDOUT_FILENO);
    dup(pipes[1]);
    close(pipes[0]);
    close(pipes[1]);
    execlp(commands[0], commands[0], NULL);
  }

  int segundo_comando = fork();
  // cerramos stdin y lo cambiamos por nuestro pipe
  if (segundo_comando == 0) {
    close(STDIN_FILENO);
    dup(pipes[0]);
    close(pipes[1]);
    close(pipes[0]);
    execlp(commands[2], commands[2], NULL);
  }

  // cerrar los pipes y esperar la ejecucion de los procesos hijos
  close(pipes[0]);
  close(pipes[1]);
  wait(&primer_comando);
  wait(&segundo_comando);
}

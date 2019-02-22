#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using std::ofstream; //para los archivos
using std::ifstream;
using std::string;
using std::vector;
using std::cout;
using std::cin;

int shell_cat(char**);
vector<char *> split(string);
void simplepipe(char **);
void multiplepipe(vector<char *>);
void exec_command(char **, int);
void pipe_command(char **, int, int *);
int ismultiplepipe(vector<char *>);

int main() {
  int child;
  string cmd;

  while (true) {
    cout << "> ";
    getline(cin, cmd);
    //obtiene la entrada y el comando

    if (cmd == "exit") {
      break;
    }

    child = fork();

    if (child == 0) {
      vector<char *> vcmds = split(cmd);
      char **cmds = &vcmds[0];

      if (vcmds.size() == 4 && strcmp(cmds[1], "|") == 0) {
        simplepipe(cmds);
      } else if (ismultiplepipe(vcmds) != -1) {
        multiplepipe(vcmds);
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
  // dividir string por cada espacio
  vector<char *> args;
  size_t desde = 0;
  size_t hasta = cmd.find(" ");

  while (hasta != string::npos) {
    char *palabra = new char();
    strcpy(palabra, cmd.substr(desde, hasta - desde).c_str());
    args.push_back(palabra); //la cantidad de argumentos y empuja elementos a la palabra
  //c_str extrae del string la ruta
    desde = hasta + 1;
    hasta = cmd.find(" ", desde);
  }

  // guardar la ultima palabra
  char *palabra = new char();
  strcpy(palabra, cmd.substr(desde, hasta - desde).c_str());
  args.push_back(palabra);
  args.push_back(NULL);

  return args;
}

void simplepipe(char **commands) {
  // si contiene el caracter '|' y solo hay 4 argumentos es un pipe simple:
  // write => pipe[1] => data => pipe[0] <= read
  int pipes[2];
  pipe(pipes);

  int primer_comando = fork();
  if (primer_comando == 0) {
    // cerramos stdout y lo cambiamos por nuestro pipe
    dup2(pipes[1], STDOUT_FILENO);
    close(pipes[0]);
    close(pipes[1]);
    execlp(commands[0], commands[0], NULL);
  }

  int segundo_comando = fork();
  if (segundo_comando == 0) {
    // cerramos stdin y lo cambiamos por nuestro pipe
    dup2(pipes[0], STDIN_FILENO);
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

void multiplepipe(vector<char *> commands) {
  int size = (int) commands.size() / 2;
  char *cmds[size];

  // crear arreglo con los comandos (sin caracteres de pipe)
  int count = 0;
  for (int i = 0; i < commands.size() - 1; i += 1) {
    if (strcmp(commands[i], "|") != 0) {
      cmds[count] = commands[i];
      count += 1;
    }
  }

  

  // iniciar el piping the procesos
  int child = fork();
  if (child == 0) {
    exec_command(cmds, size);
  }
  wait(&child);
}

void exec_command(char **cmds, int current_cmd) {
  // si hay mas de un proceso habra que conectar sus pipes
  if (current_cmd > 1) {
    int pipe_in[2];
    pipe(pipe_in);

    int child = fork();
    if (child == 0) {
      pipe_command(cmds, current_cmd - 1, pipe_in);
    }

    // cambiamos stdin por nuestro pipe
    dup2(pipe_in[0], STDIN_FILENO);
    close(pipe_in[0]);
    close(pipe_in[1]);
    wait(&child);
  }

  // ejecutar el comando actual
  execlp(cmds[current_cmd - 1], cmds[current_cmd - 1], NULL);
}

void pipe_command(char **cmds, int current_cmd, int *pipe_out) {
  // cambiar stdout por nuestro pipe
  dup2(pipe_out[1], STDOUT_FILENO);
  close(pipe_out[0]);
  close(pipe_out[1]);

  // continuar con el siguiente comando
  exec_command(cmds, current_cmd);
}

int ismultiplepipe(vector<char *> commands) {
  // dados los commands revisar si es un pipe multiple
  // comando 1 | comando 2 | comando 3 | ... | comando n
  bool ispipe = false;
  bool haspipe = false;
  int pipe_count = 0;

  for (int i = 0; i < commands.size() - 1; i += 1, ispipe = !ispipe) {
    if (strcmp(commands[i], "|") == 0) {
      haspipe = true;
    }
    if (strcmp(commands[i], "|") == 0 && i == 0) {
      return -1;
    } else if (strcmp(commands[i], "|") == 0 && i == commands.size() - 2) {
      return -1;
    } else if (ispipe && strcmp(commands[i], "|") != 0) {
      return -1;
    } else if (!ispipe && strcmp(commands[i], "|") == 0) {
      return -1;
    } else if (ispipe && strcmp(commands[i], "|") == 0) {
      pipe_count += 1;
    }
  }

  if (!haspipe) {
    return -1;
  }
  return pipe_count;
}

int shell_cat(char** tokens) {
    if (tokens[1] == NULL) {
        perror("\"cat\" missing argument.\n");

    } else {
        if (strcmp(tokens[1], ">") == 0) {
            if (tokens[2]== NULL) {
                perror("\"cat\" missing argument.\n");
            } else {
                char *line;
                size_t buffer_size;
                line = NULL;
                buffer_size = 0;
                string str;
                
                ofstream file(tokens[2]);
                do {
                    getline(cin, str);
                    file  << str << "\n";
                }while(!cin.eof()); //end of file
               
               /*
                Para abrir un fichero para lectura, debe crear un objeto ifstream que se usará 
                como cin . Para crear un fichero de escritura, se crea un objeto ofstream 
                que se comporta como cout .
               */
                

            }
        } else {
            int controlador = 0;
            int tokenCount = 1;
            int i = 0;

            do {
                if (controlador == 0 || tokens[tokenCount][i] == 0) {
                    FILE *file;
                    char line[100];
                    file = fopen(tokens[tokenCount], "r");
                    while (fscanf(file, "%[^\n]\n", line) != EOF) {
                    // lee el file linea por linea entrando a la linea de bufer
                    // Lee hasta que encuentra un ENTER
                        printf("%s\n", line);
                    }
                    fclose(file);
                    tokenCount++;
                    i = 0;
                }

                i++;
                controlador++;
            } while (tokens[tokenCount] != NULL);
        }

    }
    return 1;
}

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <stdlib.h>
#include <cassert>

using namespace std;

int main(int argc, char** argv, char** envp)
{
  int pidn = atoi(argv[1]);

  assert(false);
  perror("Error");
  abort();
}

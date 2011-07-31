#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

const char* foo() { return "foo"; }

void breakpoint() {
  kill(getpid(), SIGTRAP);
}

int main()
{
  for(int i=0; i<10; ++i) {
    if(i == 5) breakpoint();
  }
}

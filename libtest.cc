#include <iostream>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

using namespace std;

struct ConstructorRunner
{
  template<typename T_FN>
  ConstructorRunner(T_FN fn) {
    fn();
  }    
};

extern "C" {
  void h(int);
    
  void abort(void) {
    std::cout << "\n\nabort()\n\n\n";
    h(1);
  }

  void __assert_fail(const char* assertion, const char *, unsigned int line, const char* function)
  {
    abort();
  }
    
  void h(int)
  {
    int f = fork();

    if(f == 0) {
      char exename[100] = {0};
      char pid[10] = {0};
      int pidn= getppid();

      char* debugger = getenv("JITDEBUG_EXE");
      if(!debugger) {
        std::cout << "No debugger specified\n";
        exit(0);
      }

      snprintf(exename, 100, "/proc/%d/exe", pidn);
      snprintf(pid, 10, "%d", pidn);

      char * const args[4] = {
        debugger,
        exename,
        pid,
        0,
      };

      execlp(debugger, debugger, exename, pid, 0);

      perror("Error");
      exit(1);
    }
    else if(f > 0) {
      volatile bool cont = false;
      while(!cont);
      return;
    }
    else {
      std::cerr << "There was an error calling fork\n";
      exit(1);
    }

    std::cout << "Version with f=" << f << " reached the end...\n";
    exit(1);
  }

  void install()
  {
    sigset_t blocked;
    sigemptyset(&blocked);
    sigaddset(&blocked, SIGSTKFLT);
    sigaddset(&blocked, SIGSEGV);
    sigaddset(&blocked, SIGTRAP);
    sigaddset(&blocked, SIGILL);
    sigaddset(&blocked, SIGBUS);
    sigaddset(&blocked, SIGFPE);

    struct sigaction sa;
    sa.sa_handler = h;
    sa.sa_mask = blocked;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGSTKFLT, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGTRAP, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);

    //sa.sa_handler = child_exitend;
    //sa.sa_mask = SA_RESTART | SA_NOCLDSTOP;
    // sigaction(SIGCHLD, &sa, NULL);
  }
}

ConstructorRunner r(install);

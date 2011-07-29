#include <iostream>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <sys/wait.h>

// TODOs: - move the exename buffer to static area
//        - figure out how big the exename buffer should be
//        - make constants for both buffer lengths
//        - break out 'start_debugger()' function
//        - figure out how to call real abort() and replace exit(1)s
//          with that
//        - make functions static

using namespace std;

/// You can make an instance of this class and pass a nullary function
/// (or function-like object) as a constructor parameter and it will
/// call that function for you. Intended to be used for global/static
/// variables, so that those functions are called at static
/// initialization time.
///
/// This is because I didn't know about GCC's
/// "__attribute__((constructor))", but I'll leave it so that the code
/// is more portable.
struct ConstructorRunner
{
  template<typename T_FN>
  ConstructorRunner(T_FN fn) {
    fn();
  }    
};


extern "C" {
  /// This function gets called when the library determines that it
  /// should spawn GDB.
  static void gdbNaoPlz(int);
    
  /// We override libc's "abort()" so that we can trigger the JIT
  /// debugger then.
  void abort(void) {
    std::cout << "\n\nabort()\n\n\n";
    gdbNaoPlz(1);
    exit(1);
  }

  /// This gets called behind the scenes by glic's 'assert()' macro
  /// when it fails. We override it so we can trigger the debugger.
  void __assert_fail(const char* assertion,
		     const char * file,
		     unsigned int line,
		     const char* function)
  {
    (void) assertion;
    (void) file;
    (void) line;
    (void) function;
    abort();
  }
    

  static void gdbNaoPlz(int p)
  {
    // When starting the debugger, we:
    //   1. fork()
    //   2. In the parent, enter an infinite loop. (The debugger can
    //      break us out of the loop.)
    //   3. In the child, spawn a debugger and attach it to the
    //      parent.
    (void) p;
    
    int f = fork();

    if(f == 0) {
      // Child. Spawn the debugger.
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

      execlp(debugger, debugger, exename, pid, (char*)NULL);

      perror("Error");
      exit(1);
    }
    else if(f > 0) {
      // Enter an infinite loop. 'cont' is volatile so that the
      // debugger can change it and the loop will break.
      volatile bool cont = false;
      while(!cont)
        ;
      return;
    }
    else {
      std::cerr << "There was an error calling fork\n";
      exit(1);
    }

    std::cout << "Version with f=" << f << " reached the end...\n";
    exit(1);
  }


  /// Install all the signal handlers we need.
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
    sa.sa_handler = gdbNaoPlz;
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


/// Call install() to set up all signal handlers.
ConstructorRunner r(install);

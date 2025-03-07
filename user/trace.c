#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];

  // mask command 
  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  // Negative value -> system call failed
  if (trace(atoi(argv[1])) < 0) {
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }
  
  // Sao chép vào nargv nếu có nhiều command
  for(i = 2; i < argc && i < MAXARG; i++){
    nargv[i-2] = argv[i]; 
    // argv[0] là trace, [1] là mask
  }
  nargv[argc-2] = 0; // Required by exec()
  exec(nargv[0], nargv); // Chạy lệnh trong nargv
  printf("trace: exec failed\n");
  exit(0);
}

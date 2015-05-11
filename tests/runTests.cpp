#include <bandit/bandit.h>

int main(int argc, char* argv[]) {
  printf("----------------------------------------------------------------\n");
  int result = bandit::run(argc, argv);
  printf("----------------------------------------------------------------\n");
  return result;
}


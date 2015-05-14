#include <bandit/bandit.h>

#include <stdint.h>

int main(int argc, char* argv[]) {
  printf("----------------------------------------------------------------\n");
  fprintf(stdout, "    void* = %lu bytes (%lu bits)\n", sizeof(void*),     sizeof(void*)*8);
  fprintf(stdout, "     char = %lu bytes (%lu bits)\n", sizeof(char),      sizeof(char)*8);
  fprintf(stdout, "short int = %lu bytes (%lu bits)\n", sizeof(short int), sizeof(short int)*8);
  fprintf(stdout, "      int = %lu bytes (%lu bits)\n", sizeof(int),       sizeof(int)*8);
  fprintf(stdout, " long int = %lu bytes (%lu bits)\n", sizeof(long int),  sizeof(long int)*8);
  fprintf(stdout, "  int64_t = %lu bytes (%lu bits)\n", sizeof(int64_t),   sizeof(int64_t)*8);
  fprintf(stdout, " uint64_t = %lu bytes (%lu bits)\n", sizeof(uint64_t),  sizeof(uint64_t)*8);
  fprintf(stdout, "   size_t = %lu bytes (%lu bits)\n", sizeof(size_t),    sizeof(size_t)*8);
  printf("----------------------------------------------------------------\n");
  int result = bandit::run(argc, argv);
  printf("----------------------------------------------------------------\n");
  return result;
}


#include <cUtils/specs/verboseRunner.h>

#include <stdint.h>

int main(int argc, char* argv[]) {
  printf("----------------------------------------------------------------\n");
  fprintf(stdout, "    void* = %zu bytes (%zu bits)\n", sizeof(void*),     sizeof(void*)*8);
  fprintf(stdout, "     char = %zu bytes (%zu bits)\n", sizeof(char),      sizeof(char)*8);
  fprintf(stdout, "short int = %zu bytes (%zu bits)\n", sizeof(short int), sizeof(short int)*8);
  fprintf(stdout, "      int = %zu bytes (%zu bits)\n", sizeof(int),       sizeof(int)*8);
  fprintf(stdout, " long int = %zu bytes (%zu bits)\n", sizeof(long int),  sizeof(long int)*8);
  fprintf(stdout, "  int64_t = %zu bytes (%zu bits)\n", sizeof(int64_t),   sizeof(int64_t)*8);
  fprintf(stdout, " uint64_t = %zu bytes (%zu bits)\n", sizeof(uint64_t),  sizeof(uint64_t)*8);
  fprintf(stdout, "   size_t = %zu bytes (%zu bits)\n", sizeof(size_t),    sizeof(size_t)*8);
  printf("----------------------------------------------------------------\n");
  int result = SpecRunner::runAllUsing(new VerboseRunner());
  printf("----------------------------------------------------------------\n");
  return result;
}


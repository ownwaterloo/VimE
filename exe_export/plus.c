#include <stdio.h>

#ifdef LIB
__declspec(dllexport) int plus(int x, int y) { return x+y; }
#else
__declspec(dllimport) int plus(int x, int y);
#endif

int main(int argc, char* argv[])
{
      if ( argc<3 ) {
            printf("usage plus x y\n");
            return -1;
      } else {
            int x=0, y=0;
            sscanf(argv[1], "%d", &x);
            sscanf(argv[2], "%d", &y);
            printf("%d+%d=%d\n", x, y, plus(x, y) );
      }
      return 0;
}

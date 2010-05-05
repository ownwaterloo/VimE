#include <stdio.h>

#include <windows.h>

#include "stack.h"

void* f1(void* p)
{
      return printf("f1(%s);\n", p);
}

void* f2(void* p)
{
      printf("f2(%d);\n", (int)p );
      return 0;
}

LONG __stdcall Ignore(PEXCEPTION_POINTERS p) {
      printf("%p\n", p->ExceptionRecord->ExceptionAddress);
      return EXCEPTION_CONTINUE_SEARCH;
}

int main(void)
{
      stack_t s;
      void* p;

      stack_ctor(&s);
      //SetUnhandledExceptionFilter(Ignore);

      p = stack_call(&s, f1, "dyncall" );
      printf("%p\n\n", p);

      p = stack_call(&s, f2, (void*)1212 );
      printf("%p\n\n", p);

      stack_dtor(&s);
      return 0;
}

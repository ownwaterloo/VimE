#ifndef RK_SNIPPET_H_20100503_
#define RK_SNIPPET_H_20100503_

#include <setjmp.h>

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

typedef void* (*routine)(void* arg);

typedef struct {
      jmp_buf     caller_;
      jmp_buf     callee_;
      void*       thread_;
      unsigned    tid_;
      void*       event_;

      routine     f_;
      void*       arg_;
} stack_t;


void
stack_ctor(stack_t* self);

void
stack_dtor(stack_t* self);


void*
stack_call(stack_t* self,routine callee, void* arg);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif      /* include guard */

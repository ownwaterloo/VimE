#include <stdio.h>

#include <windows.h>
#include <process.h>

#include "stack.h"


static LONG Ignore(EXCEPTION_RECORD ctx)
{
      printf("exception : %x\n", ctx.ExceptionCode);
      return EXCEPTION_CONTINUE_SEARCH;
}


static unsigned __stdcall
protected_thread(void* arg)
{
      //__try {

            stack_t* ctx = (stack_t*)arg;
            if ( 0 ==setjmp(ctx->callee_) ) {
                  printf("protected thread %u created\n", ctx->tid_);
                  SetEvent(ctx->event_);
                  SuspendThread(ctx->thread_);
                  /** never return until resume it */
            }
            else  {
                  void* p;
                  printf("enter protected stack frame %p\n", (void*)arg);
                  printf("enter protected call\n");
                  p = ctx->f_(ctx->arg_);
                  printf("leave protected call\n");
                  printf("leave protected stack frame %p\n", (void*)arg);
                  longjmp(ctx->caller_, (int)p );
                  /* the generalization of setjmp/longjmp should return void* !!! */
            }
            
      //} __except( Ignore(*(GetExceptionInformation())->ExceptionRecord) ) {

      //}
      return 0;
}


#ifdef __cplusplus
extern "C"
#endif
void
stack_ctor(stack_t* self)
{
      self->thread_ = (void*)_beginthreadex
            (NULL, 0
            ,protected_thread, self
            ,CREATE_SUSPENDED, &self->tid_);
      self->event_ = CreateEvent
            (NULL       /* ignore */
            ,FALSE      /* auto reset */
            ,FALSE      /* nonsignaled */
            ,NULL       /* anonymous */);
      ResumeThread(self->thread_);
      WaitForSingleObject(self->event_, INFINITE);
      CloseHandle(self->event_);
}


#ifdef __cplusplus
extern "C"
#endif
void*
stack_call(stack_t* self,routine callee, void* arg)
{
      int r;
      self->f_   = callee;
      self->arg_ =  arg;
      if ( r=setjmp(self->caller_), r==0 ) {
            printf("enter caller stack frame %p\n", (void*)&self);
            //__try {
                  longjmp(self->callee_, 1);
            //}
            //__except( Ignore( *(GetExceptionInformation())->ExceptionRecord ) ) {

            //}

      } else {
            printf("leave caller stack frame %p\n", (void*)&self);
      }
      return (void*)r;
}


#ifdef __cplusplus
extern "C"
#endif
void
stack_dtor(stack_t* self)
{
      ResumeThread(self->thread_);
      WaitForSingleObject(self->thread_, INFINITE);
      CloseHandle(self->thread_);
}

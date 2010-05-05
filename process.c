#include <assert.h>
#include <errno.h>
#include <windows.h>

#include "ipc.h"
#include "id.h"

typedef struct {
      HANDLE      handle_;
      struct {
            HANDLE      read;
            HANDLE      write;
      } stdin_, stdout_, stderr_;
} process_t;

static void
process_close_
(
      process_t*  self
)
{
      CloseHandle(self->stdin_.read);
      CloseHandle(self->stdin_.write);
      CloseHandle(self->stdout_.read);
      CloseHandle(self->stdout_.write);
      CloseHandle(self->stderr_.read);
      CloseHandle(self->stderr_.write);
      CloseHandle(self->handle_);
}

int
process_read
(
      process_t*  process,
      void*       buf,
      size_t      bytes,
      int         timeout
)
{
      DWORD read, avail;
      if ( timeout < 0 ) {
            if ( !ReadFile(process->stdout_.read, buf, bytes, &read, 0) ) {
                  errno = GetLastError();
                  return -1;
            }
            return read;
      }

      if ( timeout >0 )
            Sleep(timeout);
      if ( !PeekNamedPipe(process->stdout_.read, buf, 1, &read, &avail, NULL) )
      {
            errno = GetLastError();
            return -1;
      }
      if ( !ReadFile(process->stdout_.read, buf
            , bytes<avail? bytes: avail, &read, NULL) )
      {
            errno = GetLastError();
            return -1;
      }
      return read;
}

int
process_write
(
      process_t*  process,
      void const* buf,
      size_t      bytes
)
{
      DWORD write;
      if (!WriteFile(process->stdin_.write, buf, bytes, &write, 0) ) {
            errno = GetLastError();
            return -1;
      }
      return write;
}

int 
process_start
(
      process_t*  self,
      char*       cmdline
)
{

      STARTUPINFOA            si;
      PROCESS_INFORMATION     pi;
      SECURITY_ATTRIBUTES     sa;
      int r = -1;

      self->handle_ = self->stdin_.read = self->stdin_.write
                    = self->stdout_.read = self->stdout_.write
                    = self->stderr_.read = self->stdout_.write = 0;

      sa.nLength = sizeof sa;
      sa.bInheritHandle = 1;
      sa.lpSecurityDescriptor = 0;

      do {

            if ( !CreatePipe(&self->stdin_.read, &self->stdin_.write, &sa, 0)) {
                  errno = GetLastError();
                  break;
            }
            if (!SetHandleInformation(self->stdin_.write,HANDLE_FLAG_INHERIT,0))
            {
                  errno = GetLastError();
                  break;
            }

            if (!CreatePipe(&self->stdout_.read, &self->stdout_.write, &sa, 0)){
                  errno = GetLastError();
                  break;
            }
            if (!SetHandleInformation(self->stdout_.read,HANDLE_FLAG_INHERIT,0))
            {
                  errno = GetLastError();
                  break;
            }

            if (!CreatePipe(&self->stderr_.read, &self->stdout_.write, &sa, 0)){
                  errno = GetLastError();
                  break;
            }
            if (!SetHandleInformation(self->stderr_.read,HANDLE_FLAG_INHERIT,0))
            {
                  errno = GetLastError();
                  break;
            }

            si.cb = sizeof si;
            GetStartupInfoA(&si);
            si.dwFlags |= STARTF_USESTDHANDLES;
            si.hStdInput = self->stdin_.read;
            si.hStdOutput = self->stdout_.write;
            si.hStdError = self->stderr_.write;

            if (!CreateProcessA(0, cmdline, 0, 0, TRUE, 0, 0, 0, &si, &pi) ) {
                  errno = GetLastError();
                  break;
            }
            r = 0;
            CloseHandle(pi.hThread);
            self->handle_ = pi.hProcess;
      } while (0);

      if ( r==-1 )
            process_close_(self);

      return r;
}


int
process_wait
(
      process_t*  self,
      int         timeout
)
{
      switch ( WaitForSingleObject
                  (self->handle_,timeout<0? INFINITE: timeout) )
      {
      case WAIT_OBJECT_0:
            return 0;
      case WAIT_TIMEOUT:
            return 1;
      case WAIT_FAILED:
            errno = GetLastError();
            return -1;
      }
      return -1;
}

int
process_kill
(
      process_t*  self,
      int         timeout
)
{
      int r = process_wait(self, timeout);
      do {
            if ( r<0 ) break;
            if ( r==0 ) {
                  DWORD c;
                  GetExitCodeProcess(self->handle_, &c);
                  r = c;
                  break;
            }
            r = -2;
            TerminateProcess(self->handle_, r);
      } while (0);

      process_close_(self);
      return r;
}



static id_t i2p_ = ID();

int
start_process(char* cmdline, char* mode)
{
      int id;
      int r = -1;
      process_t* p = 0;

      do {
            if ( id = id_acquire(&i2p_), id==id_invalid )
                  break;
            if ( p = malloc(sizeof *p), !p)
                  break;
            id_set(&i2p_, id, p);
            r = process_start(p, cmdline);
      } while (0);

      if ( r!=-1 ) {
            return id;
      }

      free(p);
      if (id != id_invalid )
            id_release(&i2p_, id);
      return r;
}

int
kill_process(int id, int timeout)
{
      int r;
      process_t* p;

      assert( id_get_pre(&i2p_, id) );

      p = id_get(&i2p_, id);
      r = process_kill(p, timeout);
      free(p);
      id_release(&i2p_, id);

      return r;
}

int
wait_process(int id, int timeout)
{
      assert( id_get_pre(&i2p_, id) );
      return process_wait(id_get(&i2p_, id), timeout);
}

int
read_from_process(int id, void* buf, size_t bytes, int timeout)
{
      assert( id_get_pre(&i2p_, id) );
      return process_read(id_get(&i2p_, id), buf, bytes, timeout);
}

int
write_to_process(int id, void const* buf, size_t bytes)
{
      assert( id_get_pre(&i2p_, id) );
      return process_write(id_get(&i2p_, id), buf, bytes);
}

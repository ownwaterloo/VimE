#ifndef IPC_H_
#define IPC_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

int
start_process(char* cmdline, char* mode);

int
kill_process(int id, int timeout);

int
wait_process(int id, int timeout);

int
read_from_process(int id, void* buf, size_t bytes, int timeout);

int
write_to_process(int id, void const* buf, size_t bytes);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif      /* include guard */

#ifndef RK_SNIPPET_ID_H_20100505_
#define RK_SNIPPET_ID_H_20100505_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

typedef struct {
      size_t            avail_;
} id_test_;

struct id_elem_;

typedef struct {
      int               head_;
      struct id_elem_*  vector_;
      size_t            count_;

#if defined(ID_TEST)
      id_test_          test_;
#endif

} id_t;

enum {
      id_invalid = -12,
};

#define ID() {id_invalid}

static __inline id_t
      id         (void) { id_t self = ID(); return self; }
void  id_dtor    (id_t*       self);
int   id_acquire (id_t*       self);
void  id_release (id_t*       self, int id);
int   id_set_pre (id_t const* self, int id);
void  id_set     (id_t*       self, int id,  void const* data);
int   id_get_pre (id_t const* self, int id);
void* id_get     (id_t const* self, int id);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif      /* include guard */

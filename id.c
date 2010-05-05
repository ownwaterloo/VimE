#include <assert.h>
#include <stdlib.h>
#include <limits.h>

#include "id.h"

typedef struct id_elem_ {
      int   next_;
      void* data_;
} id_elem_t;


#if defined(ID_TEST)
static void id_test_increase_(id_t* self) { ++self->test_.avail_; }
static void id_test_decrease_(id_t* self) {
      assert(self->test_.avail_>0); --self->test_.avail_; }
static void id_test_expand_(id_t* self, size_t more) {
      assert(self->test_.avail_==0); self->test_.avail_ = more; }

#else
static void id_test_increase_(id_t* self) { }
static void id_test_decrease_(id_t* self) { }
static void id_test_expand_(id_t* self, size_t more) {}

#endif


void
id_dtor(id_t* self) {
      free(self->vector_);
}

static size_t id_grow_(size_t s) {
      enum { bits = CHAR_BIT * sizeof s };
      s |= s>>1;
      s |= s>>2;
      s |= s>>4;
      if ( bits > 8 )  s |= s>>8;
      if ( bits > 16)  s |= s>>16;
      if ( bits > 32 ) s |= s>>32;
      return s+1;
}

static int
id_acquire_(id_t* self)
{
      int id;
      assert( self->head_ != id_invalid );
      id = self->head_;
      self->head_ = self->vector_[id].next_;
      id_test_decrease_(self);
      return id;
}

int
id_acquire(id_t* self)
{
      if ( self->head_ != id_invalid) {
            return id_acquire_(self);
      } else {
            size_t s = id_grow_(self->count_);
            void*  p = realloc(self->vector_, s*sizeof self->vector_[0]);
            if (p) {
                  size_t i;
                  self->vector_ = (id_elem_t*)p;

                  assert( s > self->count_ );
                  id_test_expand_(self, s - self->count_ );

                  self->head_ = self->count_;
                  for (i=self->count_; i<s-1; ++i)
                        self->vector_[i].next_ = i+1;
                  self->vector_[i].next_ = id_invalid;
                  self->count_ = s;
                  return id_acquire_(self);
            }
            return id_invalid;
      }
}

void
id_release(id_t* self, int id)
{
      assert( 0<= id && (size_t)id < self->count_ );
      assert( self->count_ > 0 );
      self->vector_[id].next_ = self->head_;
      self->head_ = id;
      id_test_increase_(self);
}

static int id_is_invalid(id_t const* self, int id) {
      return 0<=id && (size_t)id < self->count_;
}

int
id_set_pre(id_t const* self, int id) { return id_is_invalid(self, id); }
int
id_get_pre(id_t const* self, int id) { return id_is_invalid(self, id); }

void
id_set(id_t* self, int id, void const* data )
{
      assert( id_set_pre(self, id) );
      self->vector_[id].data_ = (void*)data;
}

void*
id_get(id_t const* self, int id)
{
      assert( id_get_pre(self, id) );
      return self->vector_[id].data_;
}

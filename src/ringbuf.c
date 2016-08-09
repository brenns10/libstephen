#include "libstephen/rb.h"

#include <stdlib.h>
#include <string.h>

void rb_init(smb_rb *rb, int dsize, int init)
{
  rb->dsize = dsize;
  rb->nalloc = init;
  rb->start = 0;
  rb->count = 0;
  rb->data = calloc(dsize, init);
}

void rb_destroy(smb_rb *rb)
{
  free(rb->data);
}

void rb_grow(smb_rb *rb)
{
  int oldalloc = rb->nalloc;
  rb->nalloc *= 2;
  rb->data = realloc(rb->data, rb->nalloc * rb->dsize);

  for (int i = 0; i < rb->count; i++) {
    int oldindex = (rb->start + i) % oldalloc;
    int newindex = (rb->start + i) % rb->nalloc;
    if (oldindex != newindex) {
      memcpy(rb->data + newindex * rb->dsize,
             rb->data + oldindex * rb->dsize, rb->nalloc);
    }
  }
}

void rb_push_front(smb_rb *rb, void *src)
{
  if (rb->count >= rb->nalloc) {
    rb_grow(rb);
  }

  // ensure the new start index is still positive
  int newstart = (rb->start + rb->nalloc - 1) % rb->nalloc;
  rb->start = newstart;
  memcpy(rb->data + rb->start * rb->dsize, src, rb->dsize);
  rb->count++;
}

void rb_pop_front(smb_rb *rb, void *dst)
{
  int newstart = (rb->start + 1) % rb->nalloc;
  memcpy(dst, rb->data + rb->start * rb->dsize, rb->dsize);
  rb->start = newstart;
  rb->count--;
}

void rb_push_back(smb_rb *rb, void *src)
{
  if (rb->count >= rb->nalloc) {
    rb_grow(rb);
  }

  int index = (rb->start + rb->count) % rb->nalloc;
  memcpy(rb->data + index * rb->dsize, src, rb->dsize);
  rb->count++;
}

void rb_pop_back(smb_rb *rb, void *dst)
{
  int index = (rb->start + rb->count - 1) % rb->nalloc;
  memcpy(dst, rb->data + index * rb->dsize, rb->dsize);
  rb->count--;
}

/***************************************************************************//**

  @file         ringbuftest.c

  @author       Stephen Brennan

  @date         Created Saturday, 23 May 2015

  @brief        Tests for the ring buffer.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the
                Revised BSD License.  See LICENSE.txt for details.

*******************************************************************************/
#include <stdio.h>

#include "libstephen/ut.h"
#include "libstephen/rb.h"

void print_ringbuf(smb_rb *rb)
{
  printf("dsize:%d, nalloc:%d, start:%d, count:%d\n",
         rb->dsize, rb->nalloc, rb->start, rb->count);
  for (int i = 0; i < rb->count; i++) {
    int index, v;
    index = (rb->start + i) % rb->nalloc;
    memcpy(&v, rb->data + index * rb->dsize, rb->dsize);
    printf("%d: %c\n", index, v);
  }
}

int test_push_front(void)
{
  smb_rb rb;
  rb_init(&rb, sizeof(int), 4);
  rb.start = 1;

  int v = 'a';
  rb_push_front(&rb, &v);
  TA_INT_EQ(((int*)rb.data)[0], 'a');
  TA_INT_EQ(rb.count, 1);
  TA_INT_EQ(rb.start, 0);

  v = 'b';
  rb_push_front(&rb, &v);
  TA_INT_EQ(((int*)rb.data)[3], 'b');
  TA_INT_EQ(((int*)rb.data)[0], 'a');
  TA_INT_EQ(rb.count, 2);
  TA_INT_EQ(rb.start, 3);

  v = 'c';
  rb_push_front(&rb, &v);
  TA_INT_EQ(((int*)rb.data)[2], 'c');
  TA_INT_EQ(((int*)rb.data)[3], 'b');
  TA_INT_EQ(((int*)rb.data)[0], 'a');
  TA_INT_EQ(rb.count, 3);
  TA_INT_EQ(rb.start, 2);

  v = 'd';
  rb_push_front(&rb, &v);
  TA_INT_EQ(((int*)rb.data)[1], 'd');
  TA_INT_EQ(((int*)rb.data)[2], 'c');
  TA_INT_EQ(((int*)rb.data)[3], 'b');
  TA_INT_EQ(((int*)rb.data)[0], 'a');
  TA_INT_EQ(rb.count, 4);
  TA_INT_EQ(rb.start, 1);

  v = 'e';
  rb_push_front(&rb, &v);
  TA_INT_EQ(((int*)rb.data)[0], 'e');
  TA_INT_EQ(((int*)rb.data)[1], 'd');
  TA_INT_EQ(((int*)rb.data)[2], 'c');
  TA_INT_EQ(((int*)rb.data)[3], 'b');
  TA_INT_EQ(((int*)rb.data)[4], 'a');
  TA_INT_EQ(rb.count, 5);
  TA_INT_EQ(rb.start, 0);

  rb_destroy(&rb);
  return 0;
}

int test_pop_front(void)
{
  smb_rb rb;
  rb_init(&rb, sizeof(int), 4);
  rb.start = 1;

  int v = 'a';
  rb_push_front(&rb, &v); // a
  v = 'b';
  rb_push_back(&rb, &v); // ab
  v = 'c';
  rb_push_front(&rb, &v); // cab
  v = 'd';
  rb_push_back(&rb, &v); // cabd
  v = 'e';
  rb_push_front(&rb, &v); // ecabd

  TA_INT_EQ(rb.count, 5);
  rb_pop_front(&rb, &v);
  TA_INT_EQ(v, 'e');
  rb_pop_front(&rb, &v);
  TA_INT_EQ(v, 'c');
  rb_pop_front(&rb, &v);
  TA_INT_EQ(v, 'a');
  rb_pop_front(&rb, &v);
  TA_INT_EQ(v, 'b');
  rb_pop_front(&rb, &v);
  TA_INT_EQ(v, 'd');
  rb_destroy(&rb);
  return 0;
}

int test_push_back(void)
{
  smb_rb rb;
  rb_init(&rb, sizeof(int), 4);

  int v = 'a';
  rb_push_back(&rb, &v);
  TA_INT_EQ(((int*)rb.data)[0], 'a');
  TA_INT_EQ(rb.count, 1);

  v = 'b';
  rb_push_back(&rb, &v);
  TA_INT_EQ(((int*)rb.data)[0], 'a');
  TA_INT_EQ(((int*)rb.data)[1], 'b');
  TA_INT_EQ(rb.count, 2);

  v = 'c';
  rb_push_back(&rb, &v);
  TA_INT_EQ(((int*)rb.data)[0], 'a');
  TA_INT_EQ(((int*)rb.data)[1], 'b');
  TA_INT_EQ(((int*)rb.data)[2], 'c');
  TA_INT_EQ(rb.count, 3);

  v = 'd';
  rb_push_back(&rb, &v);
  TA_INT_EQ(((int*)rb.data)[0], 'a');
  TA_INT_EQ(((int*)rb.data)[1], 'b');
  TA_INT_EQ(((int*)rb.data)[2], 'c');
  TA_INT_EQ(((int*)rb.data)[3], 'd');
  TA_INT_EQ(rb.count, 4);

  v = 'e';
  rb_push_back(&rb, &v);
  TA_INT_EQ(((int*)rb.data)[0], 'a');
  TA_INT_EQ(((int*)rb.data)[1], 'b');
  TA_INT_EQ(((int*)rb.data)[2], 'c');
  TA_INT_EQ(((int*)rb.data)[3], 'd');
  TA_INT_EQ(((int*)rb.data)[4], 'e');
  TA_INT_EQ(rb.count, 5);
  TA_INT_EQ(rb.nalloc, 8);

  rb_destroy(&rb);
  return 0;
}

int test_pop_back(void)
{
  smb_rb rb;
  rb_init(&rb, sizeof(int), 4);
  rb.start = 1;

  int v = 'a';
  rb_push_front(&rb, &v); // a
  v = 'b';
  rb_push_back(&rb, &v); // ab
  v = 'c';
  rb_push_front(&rb, &v); // cab
  v = 'd';
  rb_push_back(&rb, &v); // cabd
  v = 'e';
  rb_push_front(&rb, &v); // ecabd
  //print_ringbuf(&rb);

  TA_INT_EQ(rb.count, 5);
  rb_pop_back(&rb, &v);
  TA_INT_EQ(v, 'd');
  rb_pop_back(&rb, &v);
  TA_INT_EQ(v, 'b');
  rb_pop_back(&rb, &v);
  TA_INT_EQ(v, 'a');
  rb_pop_back(&rb, &v);
  TA_INT_EQ(v, 'c');
  rb_pop_back(&rb, &v);
  TA_INT_EQ(v, 'e');
  rb_destroy(&rb);
  return 0;
}

void ringbuf_test(void)
{
  smb_ut_group *group = su_create_test_group("test/ringbuftest.c");

  smb_ut_test *push_front = su_create_test("push_front", test_push_front);
  su_add_test(group, push_front);

  smb_ut_test *pop_front = su_create_test("pop_front", test_pop_front);
  su_add_test(group, pop_front);

  smb_ut_test *push_back = su_create_test("push_back", test_push_back);
  su_add_test(group, push_back);

  smb_ut_test *pop_back = su_create_test("pop_back", test_pop_back);
  su_add_test(group, pop_back);

  su_run_group(group);
  su_delete_group(group);
}

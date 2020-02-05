project(
  'libstephen', 'c',
  version : '0.1.0',
)

sources = [
  'src/args.c',
  'src/arraylist.c',
  'src/bitfield.c',
  'src/charbuf.c',
  'src/hashtable.c',
  'src/hta.c',
  'src/iter.c',
  'src/linkedlist.c',
  'src/log.c',
  'src/ringbuf.c',
  'src/smbunit.c',
  'src/string.c',
  'src/util.c',
  'src/lisp/gc.c',
  'src/lisp/lex.c',
  'src/lisp/types.c',
  'src/lisp/util.c',
  'src/regex/codegen.c',
  'src/regex/instr.c',
  'src/regex/lex.c',
  'src/regex/parse.c',
  'src/regex/pike.c',
  'src/regex/util.c',
]

inc = include_directories('inc')

libstephen = library(
  'stephen', sources, include_directories : inc, install: true
)
libstephen_dep = declare_dependency(
  include_directories : inc,
  link_with : libstephen
)

libedit = dependency('libedit')

regex = executable('regex', 'util/regex.c', dependencies : libstephen_dep)
lisp = executable(
  'lisp', 'util/lisp.c',
  dependencies : [libstephen_dep, libedit]
)

test_sources = [
  'test/argstest.c',
  'test/arraylisttest.c',
  'test/bitfieldtest.c',
  'test/charbuftest.c',
  'test/hashtabletest.c',
  'test/hta.c',
  'test/itertest.c',
  'test/linkedlisttest.c',
  'test/listtest.c',
  'test/logtest.c',
  'test/main.c',
  'test/re_codegen.c',
  'test/re_lex.c',
  'test/re_parse.c',
  'test/re_pike.c',
  'test/ringbuftest.c',
  'test/stringtest.c',
]
testexe = executable('testexe', test_sources, dependencies: libstephen_dep)
test('unit test', testexe)

pkg = import('pkgconfig')
pkg.generate(libstephen)

install_headers(
  'inc/libstephen/ad.h',
  'inc/libstephen/al.h',
  'inc/libstephen/base.h',
  'inc/libstephen/bf.h',
  'inc/libstephen/cb.h',
  'inc/libstephen/hta.h',
  'inc/libstephen/ht.h',
  'inc/libstephen/lisp.h',
  'inc/libstephen/list.h',
  'inc/libstephen/ll.h',
  'inc/libstephen/log.h',
  'inc/libstephen/rb.h',
  'inc/libstephen/re.h',
  'inc/libstephen/re_internals.h',
  'inc/libstephen/str.h',
  'inc/libstephen/ut.h',
  'inc/libstephen/util.h',
  'inc/libstephen/base.h',
  subdir : 'libstephen',
)

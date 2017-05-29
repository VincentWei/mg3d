INCLUDES=-I../include

noinst_PROGRAMS=stars exscn3d test zbuffer mg3d scene_test testzbuf

stars_SOURCES=stars.c
exscn3d_SOURCES=exscn3d.c
test_SOURCES=test.c
zbuffer_SOURCES=zbuffer.c
mg3d_SOURCES=mg3d.c
scene_test_SOURCES=scene_test.c
testzbuf_SOURCES=testzbuf.c

stars_LDADD=../src/libmg3d.la @APP_LIBS@
exscn3d_LDADD=../src/libmg3d.la @APP_LIBS@
test_LDADD=../src/libmg3d.la @APP_LIBS@
zbuffer_LDADD=../src/libmg3d.la @APP_LIBS@
mg3d_LDADD=../src/libmg3d.la @APP_LIBS@
scene_test_LDADD=../src/libmg3d.la -lpthread @APP_LIBS@
testzbuf_LDADD=../src/libmg3d.la @APP_LIBS@


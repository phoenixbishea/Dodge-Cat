ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS= GameManager.h

bin_PROGRAMS= PussyWetter
PussyWetter_CPPFLAGS= -I$(top_srcdir) -std=c++11
PussyWetter_SOURCES= GameManager.cpp
PussyWetter_CXXFLAGS= $(OGRE_CFLAGS) $(OIS_CFLAGS)
PussyWetter_LDADD= $(OGRE_LIBS) $(OIS_LIBS)
PussyWetter_LDFLAGS= -I/usr/include/SDL -lOgreOverlay -lboost_system -lSDL -lSDL_mixer

EXTRA_DIST= buildit makeit
AUTOMAKE_OPTIONS= foreign

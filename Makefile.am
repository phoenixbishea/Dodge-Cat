ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS= GameManager.h

bin_PROGRAMS= Game
Game_CPPFLAGS= -I$(top_srcdir) -std=c++11
Game_SOURCES= GameManager.cpp
Game_CXXFLAGS= $(OGRE_CFLAGS) $(OIS_CFLAGS)  
Game_LDADD= $(OGRE_LIBS) $(OIS_LIBS)
Game_LDFLAGS= -I/usr/include/SDL -I/lusr/opt/cegui-0.8.4/include/CEGUI -lOgreOverlay -lboost_system -lSDL -lSDL_mixer  -lCEGUIBase 

EXTRA_DIST= buildit makeit
AUTOMAKE_OPTIONS= foreign

ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS= GameManager.h BulletPhysics.hpp

bin_PROGRAMS= PussyWetter
PussyWetter_CPPFLAGS= -I$(top_srcdir) -std=c++11
PussyWetter_SOURCES= GameManager.cpp BulletPhysics.cpp
PussyWetter_CXXFLAGS= $(OGRE_CFLAGS) $(OIS_CFLAGS) -I/usr/include/bullet -I/usr/include/SDL -I/lusr/opt/cegui-0.8.4/include/cegui-0
PussyWetter_LDADD= $(OGRE_LIBS) $(OIS_LIBS)
PussyWetter_LDFLAGS= -lOgreOverlay -lboost_system -lSDL -lSDL_mixer -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -lCEGUIBase -lCEGUIOgreRenderer

EXTRA_DIST= buildit makeit
AUTOMAKE_OPTIONS= foreign
ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS= GameManager.hpp BulletPhysics.hpp Player.hpp ExtendedCamera.hpp

bin_PROGRAMS= PussyWetter
PussyWetter_CPPFLAGS= -I$(top_srcdir) -std=c++11
PussyWetter_SOURCES= GameManager.cpp BulletPhysics.cpp Player.cpp ExtendedCamera.cpp
PussyWetter_CXXFLAGS= $(OGRE_CFLAGS) $(OIS_CFLAGS) -I/usr/include/bullet -I/usr/include/SDL
PussyWetter_LDADD= $(OGRE_LIBS) $(OIS_LIBS)
PussyWetter_LDFLAGS= -lOgreOverlay -lboost_system -lSDL -lSDL_mixer -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath

EXTRA_DIST= buildit makeit
AUTOMAKE_OPTIONS= foreign

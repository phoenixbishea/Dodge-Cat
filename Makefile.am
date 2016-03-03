ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS= NewGameManager.hpp BulletPhysics.hpp ExtendedCamera.hpp Listener.hpp

bin_PROGRAMS= DodgeCat
DodgeCat_CPPFLAGS= -I$(top_srcdir) -std=c++11
DodgeCat_SOURCES= NewGameManager.cpp BulletPhysics.cpp ExtendedCamera.cpp Listener.cpp
DodgeCat_CXXFLAGS= $(OGRE_CFLAGS) $(OIS_CFLAGS) -I/usr/include/bullet -I/usr/include/SDL
DodgeCat_LDADD= $(OGRE_LIBS) $(OIS_LIBS)
DodgeCat_LDFLAGS= -lOgreOverlay -lboost_system -lSDL -lSDL_mixer -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath

EXTRA_DIST= buildit makeit
AUTOMAKE_OPTIONS= foreign


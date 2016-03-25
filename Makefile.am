ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS= GameManager.hpp BulletPhysics.hpp ExtendedCamera.hpp Player.hpp Sound.hpp Wall.hpp Cat.hpp

bin_PROGRAMS= DodgeCat
DodgeCat_CPPFLAGS= -I$(top_srcdir) -std=c++11
DodgeCat_SOURCES= GameManager.cpp BulletPhysics.cpp ExtendedCamera.cpp Player.cpp Sound.cpp Vector.cpp Quaternion.cpp
DodgeCat_CXXFLAGS= $(OGRE_CFLAGS) $(OIS_CFLAGS) -I/usr/include/bullet -I/usr/include/SDL -I/usr/local/include/cegui-0
DodgeCat_LDADD= $(OGRE_LIBS) $(OIS_LIBS)
DodgeCat_LDFLAGS= -lOgreOverlay -lboost_system -lSDL -lSDL_mixer -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -lCEGUIBase-0 -lCEGUIOgreRenderer-0

EXTRA_DIST= buildit makeit
AUTOMAKE_OPTIONS= foreign


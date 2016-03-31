ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS= GameManager.hpp BulletPhysics.hpp Player.hpp Sound.hpp Wall.hpp Cat.hpp PlayerInputComponent.hpp PlayerPhysicsComponent.hpp PlayerGraphicsComponent.hpp PlayerCameraComponent.hpp Vector.hpp Quaternion.hpp PlayerData.hpp

bin_PROGRAMS= DodgeCat
DodgeCat_CPPFLAGS= -I$(top_srcdir) -std=c++11
DodgeCat_SOURCES= GameManager.cpp BulletPhysics.cpp Sound.cpp Vector.cpp Quaternion.cpp NetManager.cpp
DodgeCat_CXXFLAGS= $(OGRE_CFLAGS) $(OIS_CFLAGS) -I/usr/include/bullet -I/usr/include/SDL -I/usr/local/include/cegui-0
DodgeCat_LDADD= $(OGRE_LIBS) $(OIS_LIBS)
DodgeCat_LDFLAGS= -lOgreOverlay -lboost_system -lSDL -lSDL_mixer -lSDL_net -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -lCEGUIBase-0 -lCEGUIOgreRenderer-0 -lSDL_net -R/lusr/lib/cegui-0.8

EXTRA_DIST= buildit makeit
AUTOMAKE_OPTIONS= foreign


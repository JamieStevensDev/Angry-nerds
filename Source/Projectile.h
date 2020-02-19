//
// Created by j36-stevens on 10/04/2019.
//

#ifndef ANGRYNERDS_PROJECTILE_H
#define ANGRYNERDS_PROJECTILE_H

//#include "../Libs/ASGE/include/Engine/Game.h"
#include "Components/GameObject.h"
#include <Engine/OGLGame.h>

class Projectile : public GameObject
{
 public:
  Projectile();
  ~Projectile();

  void setVector(bool click, float x, float y);
  void resetVector();
  void calculateTrejectory();
  void shoot(const ASGE::GameTime& game_time);

 private:
  vector2 click_start = vector2(0, 0);
  vector2 click_end = vector2(0, 0);
  vector2 calculate = vector2(0, 0);

  bool launch = false;
};

#endif // ANGRYNERDS_PROJECTILE_H

//
// Created by j36-stevens on 10/04/2019.
//

#include "Projectile.h"

Projectile::Projectile() {}

Projectile::~Projectile() {}

void Projectile::setVector(bool click, float x, float y)
{
  if (!launch)
  {
    if (click)
    {
      click_start.x = x;
      click_start.y = y;
    }
    else
    {
      click_end.x = x;
      click_end.y = y;

      calculateTrejectory();
    }
  }
}

void Projectile::resetVector()
{
  calculate.x = 0;
  calculate.y = 0;

  launch = false;
}

void Projectile::calculateTrejectory()
{
  calculate.x = click_start.x - click_end.x;
  calculate.y = click_start.y - click_end.y;
  launch = true;
}

void Projectile::shoot(const ASGE::GameTime& game_time)
{
  float x_pos = spriteComponent()->getSprite()->xPos();
  float y_pos = spriteComponent()->getSprite()->yPos();

  x_pos += float(calculate.x * 3 * game_time.delta.count() / 1000.f);
  y_pos += float(calculate.y * 5 * game_time.delta.count() / 1000.f);

  if (launch)
  {
    calculate.y += 250.f * static_cast<float>(game_time.delta.count() / 1000.f);
  }

  spriteComponent()->getSprite()->xPos(x_pos);
  spriteComponent()->getSprite()->yPos(y_pos);
}

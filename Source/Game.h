#pragma once
#include <Engine/OGLGame.h>
#include <string>

#include "Components/GameObject.h"
#include "Projectile.h"
#include "Utility/Rect.h"

/**
 *  An OpenGL Game based on ASGE.
 */
class Angry : public ASGE::OGLGame
{
 public:
  Angry();
  ~Angry();
  virtual bool init() override;

 private:
  void keyHandler(const ASGE::SharedEventData data);
  void clickHandler(const ASGE::SharedEventData data);
  void setupResolution();
  bool loadBackgrounds();
  void ProjectileInit();
  void TargetInit();
  void BlockInit();
  void ProjectileColission();
  void launchSprite(const ASGE::SharedEventData data);

  virtual void update(const ASGE::GameTime&) override;
  virtual void render(const ASGE::GameTime&) override;

  int key_callback_id = -1;   /**< Key Input Callback ID. */
  int mouse_callback_id = -1; /**< Mouse Input Callback ID. */

  // Add your GameObjects
  GameObject background_layer;
  GameObject menu_layer;
  GameObject Cow[3];
  GameObject IceBlock[3];
  GameObject WoodBlock[3];

  Projectile SmugAlien;

  bool in_menu = true;
  bool win_level = false;
  bool lose_level = false;
  bool reset = false;
  int menu = 0;
  int targets_remaining = 3;
  int aliens_remaining = 3;
  int score = 0;
};
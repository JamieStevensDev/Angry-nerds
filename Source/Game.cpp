#include <string>

#include <Engine/DebugPrinter.h>
#include <Engine/Input.h>
#include <Engine/InputEvents.h>
#include <Engine/Keys.h>
#include <Engine/Sprite.h>

#include "Game.h"

/**
*   @brief   Default Constructor.
*   @details Consider setting the game's width and height
             and even seeding the random number generator.
*/
Angry::Angry()
{
  std::srand(static_cast<unsigned int>(time(nullptr)));
}

/**
 *   @brief   Destructor.
 *   @details Remove any non-managed memory and callbacks.
 */
Angry::~Angry()
{
  this->inputs->unregisterCallback(static_cast<unsigned int>(key_callback_id));
  this->inputs->unregisterCallback(
    static_cast<unsigned int>(mouse_callback_id));
}

/**
*   @brief   Initialises the game.
*   @details The game window is created and all assets required to
                         run the game are loaded. The keyHandler and
clickHandler callback should also be set in the initialise function.
*   @return  True if the game initialised correctly.
*/
bool Angry::init()
{
  setupResolution();
  if (!initAPI())
  {
    return false;
  }

  toggleFPS();
  renderer->setWindowTitle("Angry Birds!");
  renderer->setWindowedMode(ASGE::Renderer::WindowMode::WINDOWED);
  renderer->setClearColour(ASGE::COLOURS::BLACK);

  // input handling functions
  inputs->use_threads = false;

  key_callback_id =
    inputs->addCallbackFnc(ASGE::E_KEY, &Angry::keyHandler, this);

  mouse_callback_id =
    inputs->addCallbackFnc(ASGE::E_MOUSE_CLICK, &Angry::clickHandler, this);

  ProjectileInit();
  TargetInit();
  BlockInit();

  if (!loadBackgrounds())
  {
    return false;
  }

  if (!menu_layer.addSpriteComponent(renderer.get(),
                                     "data/Textures/Backdrops/menu.jpg"))
  {
    return false;
  }

  return true;
}

void Angry::ProjectileInit()
{
  if (SmugAlien.addSpriteComponent(renderer.get(),
                                   "data/Textures/Projectile/"
                                   "alienYellow_square.png"))
  {
    ASGE::Sprite* projectileSprite = SmugAlien.spriteComponent()->getSprite();
    projectileSprite->xPos((640 / 2.f) - (projectileSprite->width() / 2.f));
    projectileSprite->yPos(700);
  }
}

void Angry::TargetInit()
{
  float last_position = 0;
  float gap = 300;
  for (int i = 0; i < 3; ++i)
  {
    if (Cow[i].addSpriteComponent(renderer.get(),
                                  "data/Textures/Target/cow.png"))
    {
      ASGE::Sprite* TargetSprite = Cow[i].spriteComponent()->getSprite();
      TargetSprite->xPos(last_position + static_cast<float>(game_width / 3) +
                         Cow[i].spriteComponent()->getSprite()->width());
      TargetSprite->yPos(750);
      last_position = last_position + gap;
      Cow[i].visibility = true;
    }
  }
}

void Angry::BlockInit()
{
  for (int i = 0; i < 3; ++i)
  {
    if (WoodBlock[i].addSpriteComponent(renderer.get(),
                                        "data/Textures/Blocks/"
                                        "elementWood011.png"))
    {
      ASGE::Sprite* WoodSprite = WoodBlock[i].spriteComponent()->getSprite();
      WoodSprite->xPos();
      WoodSprite->yPos();
      WoodBlock[i].visibility = true;
    }

    if (IceBlock[i].addSpriteComponent(renderer.get(),
                                       "data/Textures/Blocks/"
                                       "elementGlass013.png"))
    {
      ASGE::Sprite* IceSprite = IceBlock[i].spriteComponent()->getSprite();
      IceSprite->xPos();
      IceSprite->yPos();
      IceBlock[i].visibility = true;
    }
  }
}

bool Angry::loadBackgrounds()
{
  std::string filename = "data/Textures/Backdrops/lvl";
  filename += std::to_string(std::rand() % 3 + 1);
  filename += ".png";

  if (!background_layer.addSpriteComponent(renderer.get(), filename))
  {
    return false;
  }

  return true;
}

/**
*   @brief   Sets the game window resolution
*   @details This function is designed to create the window size, any
             aspect ratio scaling factors and safe zones to ensure the
                         game frames when resolutions are changed in size.
*   @return  void
*/
void Angry::setupResolution()
{
  // how will you calculate the game's resolution?
  // will it scale correctly in full screen? what AR will you use?
  // how will the game be framed in native 16:9 resolutions?
  // here are some abritrary values for you to adjust as you see fit
  // https://tinyurl.com/y6sqbr78
  game_width = 1920;
  game_height = 1080;
}

/**
*   @brief   Processes any key inputs
*   @details This function is added as a callback to handle the game's
                         keyboard input. For this game, calls to this function
                         are thread safe, so you may alter the game's state as
you see fit.
*   @param   data The event data relating to key input.
*   @see     KeyEvent
*   @return  void
*/
void Angry::keyHandler(const ASGE::SharedEventData data)
{
  auto key = static_cast<const ASGE::KeyEvent*>(data.get());

  if (key->key == ASGE::KEYS::KEY_ESCAPE)
  {
    signalExit();
  }

  if (key->key == ASGE::KEYS::KEY_SPACE &&
      key->action == ASGE::KEYS::KEY_PRESSED)
  {
    if (menu == 0)
    {
      in_menu = !in_menu;
      menu = 1;
    }
    else if (menu == 1)
    {
      menu = 2;
    }
    else if (win_level || lose_level)
    {
      menu = 1;
      reset = true;
      win_level = false;
      lose_level = false;
    }
  }
}

/**
*   @brief   Processes any click inputs
*   @details This function is added as a callback to handle the game's
                     mouse button input. For this game, calls to this function
             are thread safe, so you may alter the game's state as you
             see fit.
*   @param   data The event data relating to key input.
*   @see     ClickEvent
*   @return  void
*/
void Angry::clickHandler(const ASGE::SharedEventData data)
{
  auto click = static_cast<const ASGE::ClickEvent*>(data.get());
  double x_pos = click->xpos;
  double y_pos = click->ypos;

  ASGE::DebugPrinter() << x_pos << "," << y_pos << std::endl;

  launchSprite(data);
}

/**
*   @brief   Updates the scene
*   @details Prepares the renderer subsystem before drawing the
                     current frame. Once the current frame is has finished
                     the buffers are swapped accordingly and the image shown.
*   @return  void
*/
void Angry::update(const ASGE::GameTime& game_time)
{
  if (!win_level && !lose_level)
  {
    if (menu == 2)
    {
      SmugAlien.shoot(game_time);

      ProjectileColission();

      if (targets_remaining == 0)
      {
        win_level = true;
      }
      if (aliens_remaining == 0)
      {
        lose_level = true;
      }
    }
    if (reset)
    {
      targets_remaining = 3;
      aliens_remaining = 3;
      score = 0;

      for (int i = 0; i < 3; ++i)
      {
        Cow[i].visibility = true;
      }
      SmugAlien.resetVector();
      reset = false;
      SmugAlien.spriteComponent()->getSprite()->xPos(
        (640 / 2.f) -
        (SmugAlien.spriteComponent()->getSprite()->width() / 2.f));
      SmugAlien.spriteComponent()->getSprite()->yPos(700);
    }
  }
}

/**
*   @brief   Renders the scene
*   @details Renders all the game objects to the current frame.
                 Once the current frame is has finished the buffers are
                         swapped accordingly and the image shown.
*   @return  void
*/
void Angry::render(const ASGE::GameTime& game_time)
{
  renderer->setFont(0);

  if (in_menu)
  {
    renderer->renderSprite(*menu_layer.spriteComponent()->getSprite());
    std::string next_str = "press space to go to the instructions";
    renderer->renderText(next_str, 300, 850, 2.0, ASGE::COLOURS::BLACK);
  }

  else if (menu == 1)
  {
    renderer->renderSprite(*menu_layer.spriteComponent()->getSprite());
    std::string instruction_str = "Click and drag from the alien and release "
                                  "to send the alien towards the cow.";
    renderer->renderText(instruction_str, 300, 850, 1.0, ASGE::COLOURS::BLACK);
    std::string instruction1_str = "Capture all the cows in order to win, "
                                   "before you run out of aliens!";
    renderer->renderText(instruction1_str, 300, 875, 1.0, ASGE::COLOURS::BLACK);
    std::string play_str = "press space when ready to play!";
    renderer->renderText(play_str, 300, 900, 1.0, ASGE::COLOURS::BLACK);
  }
  else if (menu == 2)
  {
    renderer->renderSprite(*background_layer.spriteComponent()->getSprite());

    std::string score_str = "Score:" + std::to_string(score);
    renderer->renderText(score_str, 1300, 75, 1.0, ASGE::COLOURS::BLACK);
    std::string cows_str = "Cows Left:" + std::to_string(targets_remaining);
    renderer->renderText(cows_str, 900, 75, 1.0, ASGE::COLOURS::BLACK);
    std::string aliens_str =
      "Aliens Remaining:" + std::to_string(aliens_remaining);
    renderer->renderText(aliens_str, 500, 75, 1.0, ASGE::COLOURS::BLACK);
    renderer->renderSprite(*SmugAlien.spriteComponent()->getSprite());

    for (int i = 0; i < 3; ++i)
    {
      if (Cow[i].visibility)
      {
        renderer->renderSprite(*Cow[i].spriteComponent()->getSprite());
      }
    }
  }
  if (lose_level)
  {
    renderer->renderText("You Lose", 700, 500, 2.0, ASGE::COLOURS::BLACK);
  }
  else if (win_level)
  {
    renderer->renderText(
      "Congratulations", 700, 500, 2.0, ASGE::COLOURS::BLACK);
  }
}

void Angry::ProjectileColission()
{
  for (int i = 0; i < 3; ++i)
  {
    if (SmugAlien.spriteComponent()->getBoundingBox().isInside(
          Cow[i].spriteComponent()->getBoundingBox()) &&
        Cow[i].visibility)
    {
      Cow[i].visibility = false;
      targets_remaining--;
      score += 5;
    }

    if (SmugAlien.spriteComponent()->getSprite()->yPos() >= float(game_height))
    {
      SmugAlien.spriteComponent()->getSprite()->xPos(
        (640 / 2.f) -
        (SmugAlien.spriteComponent()->getSprite()->width() / 2.f));
      SmugAlien.spriteComponent()->getSprite()->yPos(700);
      SmugAlien.resetVector();
      aliens_remaining--;
    }

    if (SmugAlien.spriteComponent()->getSprite()->xPos() >= float(game_width) ||
        SmugAlien.spriteComponent()->getSprite()->xPos() <= 0)
    {
      SmugAlien.spriteComponent()->getSprite()->xPos(
        (640 / 2.f) -
        (SmugAlien.spriteComponent()->getSprite()->width() / 2.f));
      SmugAlien.spriteComponent()->getSprite()->yPos(700);
      SmugAlien.resetVector();
      aliens_remaining--;
    }
  }
}

void Angry::launchSprite(const ASGE::SharedEventData data)
{
  auto click = static_cast<const ASGE::ClickEvent*>(data.get());

  double x_pos = click->xpos;
  double y_pos = click->ypos;

  if (menu == 2)
  {
    SmugAlien.setVector(
      click->action == ASGE::MOUSE::BUTTON_PRESSED, float(x_pos), float(y_pos));
  }
}

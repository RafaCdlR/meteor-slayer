
#include "raylib.h"

#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ASTEROID_TEXTURE_SCALE 50

typedef struct {
  Vector2 pos;
  Texture2D texture;
  Rectangle hitbox;
} Asteroid;

typedef enum { Start, Running, Stop, GameOver } GameState;

typedef struct {
  Vector2 pos;
  Color color;
} Star;

int main() {
  const int screenWidth = 1280;
  const int screenHeight = 800;

  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
  InitWindow(screenWidth, screenHeight, "Asteroid Game");

  // Textures
  SearchAndSetResourceDir("resources");
  Texture2D asteroid_texture = LoadTexture("asteroid.png");

  if (asteroid_texture.id == 0) {
    CloseWindow();
    fprintf(stderr, "No cargó la textura\n");
    exit(1);
  }

  srand(time(NULL));

  SetTargetFPS(60);

  // Control variables
  GameState state = Start;
  double timer = 0.0;
  double interval = 1.0;
  double speed = 50.0;

  // Game variables
  unsigned max_asteroids = 10;
  unsigned asteroid_count = 0;
  Asteroid asteroids[max_asteroids];
  bool is_asteroid_active[max_asteroids];
  unsigned score = 0;

  memset(is_asteroid_active, 0, sizeof(is_asteroid_active));

  unsigned max_stars = 200;
  Star stars[max_stars];
  for (unsigned i = 0; i < max_stars; ++i) {
    stars[i].pos = (Vector2){(float)(rand() % screenWidth),
                             (float)(rand() % screenHeight)};
    stars[i].color = (rand() % 2 == 0) ? DARKGRAY : LIGHTGRAY;
  }

  while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or
                               // presses the Close button on the window
  {

    if ((IsKeyPressed(KEY_ENTER) && state == Start) ||
        (IsKeyPressed(KEY_R) && state == GameOver)) {
      state = Running;
      score = 0;
      for (int i = 0; i < max_asteroids; ++i)
        is_asteroid_active[i] = false;
      timer = 0.0;
      interval = 1.0;
      speed = 50.0;
    }

    if (IsKeyPressed(KEY_Q) && state == Running) {
      state = GameOver;
    }

    if (state == Running) {
      timer += GetFrameTime();

      // Counts number of active asteroids
      asteroid_count = 0;
      for (int i = 0; i < max_asteroids; ++i)
        asteroid_count += is_asteroid_active[i];

      // Adds a new asteroid
      if (timer >= interval && asteroid_count < max_asteroids) {
        // Find first free position
        unsigned free_pos = 0;
        while (is_asteroid_active[free_pos]) {
          free_pos++;
        }
        Asteroid aster = (Asteroid){
            (Vector2){(float)(rand() % (screenWidth - ASTEROID_TEXTURE_SCALE)),
                      0},
            asteroid_texture,
            (Rectangle){aster.pos.x, aster.pos.y, ASTEROID_TEXTURE_SCALE,
                        ASTEROID_TEXTURE_SCALE}};
        asteroids[free_pos] = aster;
        is_asteroid_active[free_pos] = true;

        // Dificultad incremental ;)
        speed *= 1.03;
        interval *= 0.99;

        timer = 0.0;
      }

      // Moves asteroids downwards
      for (int i = 0; i < max_asteroids; ++i) {
        if (is_asteroid_active[i]) {
          asteroids[i].pos.y += speed * GetFrameTime();
          asteroids[i].hitbox.y = asteroids[i].pos.y;
        }

        // And checks if it's game over
        if (is_asteroid_active[i] && asteroids[i].pos.y > screenHeight) {
          state = GameOver;
          break;
        }
      }

      // Checks user clicks
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 click = GetMousePosition();

        for (int i = 0; i < max_asteroids; ++i) {
          if (is_asteroid_active[i] &&
              CheckCollisionPointRec(click, asteroids[i].hitbox)) {
            is_asteroid_active[i] = false;
            score += 1;
            break;
          }
        }
      }
    }

    BeginDrawing();

    ClearBackground(BLACK);

    // Draw stars
    for (int i = 0; i < max_stars; i++)
      DrawPixel((int)stars[i].pos.x, (int)stars[i].pos.y, stars[i].color);

    if (state == Running) {
      for (int i = 0; i < max_asteroids; ++i) {
        if (is_asteroid_active[i]) {

          DrawRectangle(asteroids[i].hitbox.x, asteroids[i].hitbox.y,
                        asteroids[i].hitbox.width, asteroids[i].hitbox.height,
                        RED);

          DrawTextureEx(asteroids[i].texture, asteroids[i].pos, 0.0,
                        ASTEROID_TEXTURE_SCALE, WHITE);
        }
      }
      DrawText(TextFormat("Score: %d", score), screenWidth - 150, 10, 20,
               RAYWHITE);
      DrawText(TextFormat("FPS: %d", GetFPS()), 10, screenHeight - 30, 20,
               RAYWHITE);
    }

    if (state == Start) {
      DrawText("Meteor slayer",
               screenWidth / 2 - MeasureText("Meteor slayer", 40) / 2,
               screenHeight / 2 - 100, 40, RAYWHITE);
      DrawText("Presiona ENTER",
               screenWidth / 2 - MeasureText("Presiona ENTER", 20) / 2,
               screenHeight / 2 - 60, 20, RAYWHITE);
    }

    if (state == GameOver) {
      DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 40) / 2,
               screenHeight / 2 - 100, 40, RAYWHITE);
      DrawText(TextFormat("Puntuación final: %d", score),
               screenWidth / 2 -
                   MeasureText(TextFormat("Puntuación final: %d", score), 20) /
                       2,
               screenHeight / 2 - 60, 20, RAYWHITE);
    }

    EndDrawing();
  }

  UnloadTexture(asteroid_texture);
  CloseWindow();
  return 0;
}

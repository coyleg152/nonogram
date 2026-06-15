// File: Nonogram.c
#include "raylib.h"
#include <time.h>

#define FONT_FILE "ModeNumbers24px.png"

const int T_FPS = 30;
const int W_WIDTH = 800;
const int W_HEIGHT = 600;
const int FONT_SIZE = 24;
const int GRID_SIZE = 300;
const int TILES_PER_ROW = 10;
const int TILE_SHADOW_PX = 3;
const int TILE_GEN_PERCENT = 40;

const int GRID_X = (W_WIDTH - GRID_SIZE) / 2;
const int GRID_Y = (W_HEIGHT - GRID_SIZE) / 2;
const int TILE_SIZE = GRID_SIZE / TILES_PER_ROW;
const int TEXT_OFFSET = (TILE_SIZE - FONT_SIZE) / 2;
const int VEC_SIZE = (TILES_PER_ROW + 1) / 2;

enum
{
  TILE_EMPTY = 0,
  TILE_FULL = 1,
  TILE_X = 2
};

typedef struct
{
  int x;
  int y;
}
int_pair;


int_pair get_mouse_tile()
{
  // If the mouse is hovering over a grid tile, return its indices
  int_pair result = (int_pair) {-1, -1};
  Vector2 mouse_pos = GetMousePosition();
  if (mouse_pos.x >= GRID_X && mouse_pos.x < GRID_X + GRID_SIZE
    && mouse_pos.y >= GRID_Y && mouse_pos.y < GRID_Y + GRID_SIZE)
  {
    result.x = ((int) mouse_pos.x - GRID_X) / TILE_SIZE;
    result.y = ((int) mouse_pos.y - GRID_Y) / TILE_SIZE;
  }
  return result;
}


void build_solution(char solution_board[TILES_PER_ROW][TILES_PER_ROW])
{
  // Randomly generate the solution board
  SetRandomSeed(time(NULL));
  for (int i = 0; i < TILES_PER_ROW; i++)
  {
    for (int j = 0; j < TILES_PER_ROW; j++)
    {
      if (GetRandomValue(0, 99) < TILE_GEN_PERCENT)
        solution_board[i][j] = TILE_FULL;
      else solution_board[i][j] = TILE_EMPTY;
    }
  }
}


int check_solution(char input_board[TILES_PER_ROW][TILES_PER_ROW],
  char solution_board[TILES_PER_ROW][TILES_PER_ROW])
{
  // Return 1 if all of the full tiles match, return 0 otherwise
  for (int i = 0; i < TILES_PER_ROW; i++)
  {
    for (int j = 0; j < TILES_PER_ROW; j++)
    {
      char a = input_board[i][j];
      char b = solution_board[i][j];
      if ((a == TILE_FULL && b != TILE_FULL)
        || (a != TILE_FULL && b == TILE_FULL)) return 0;
    }
  }
  return 1;
}


void draw_grid(int_pair mouse_tile,
  char input_board[TILES_PER_ROW][TILES_PER_ROW])
{
  // Draw basic tiles
  DrawRectangle(GRID_X, GRID_Y, GRID_SIZE, GRID_SIZE, GRAY);
  Rectangle tile_rect = (Rectangle)
    {GRID_X, GRID_Y, TILE_SIZE - TILE_SHADOW_PX, TILE_SIZE - TILE_SHADOW_PX};
  for (int i = 0; i < TILES_PER_ROW; i++)
  {
    for (int j = 0; j < TILES_PER_ROW; j++)
    {
      DrawRectangleRec(tile_rect, LIGHTGRAY);
      tile_rect.x += TILE_SIZE;
    }
    tile_rect.x = GRID_X;
    tile_rect.y += TILE_SIZE;
  }

  // Draw selected tile
  if (mouse_tile.x != -1 && mouse_tile.y != -1)
  {
    tile_rect.x = GRID_X + mouse_tile.x * TILE_SIZE;
    tile_rect.y = GRID_Y + mouse_tile.y * TILE_SIZE;
    DrawRectangle(tile_rect.x, tile_rect.y, TILE_SIZE, TILE_SIZE, DARKGRAY);
    DrawRectangleRec(tile_rect, GRAY);
  }

  // Draw marked tiles
  tile_rect = (Rectangle) {GRID_X, GRID_Y, TILE_SIZE, TILE_SIZE};
  for (int i = 0; i < TILES_PER_ROW; i++)
  {
    for (int j = 0; j < TILES_PER_ROW; j++)
    {
      if (input_board[j][i] == TILE_FULL) DrawRectangleRec(tile_rect, BLACK);
      else if (input_board[j][i] == TILE_X)
      {
        DrawLine(tile_rect.x, tile_rect.y,
          tile_rect.x + TILE_SIZE, tile_rect.y + TILE_SIZE, BLACK);
        DrawLine(tile_rect.x, tile_rect.y + TILE_SIZE,
          tile_rect.x + TILE_SIZE, tile_rect.y, BLACK);
      }
      tile_rect.x += TILE_SIZE;
    }
    tile_rect.x = GRID_X;
    tile_rect.y += TILE_SIZE;
  }
}


int main()
{
  typedef struct
  {
    int arr[VEC_SIZE];
    int len;
  }
  vec;

  char input_board[TILES_PER_ROW][TILES_PER_ROW];
  for (int i = 0; i < TILES_PER_ROW; i++)
    for (int j = 0; j < TILES_PER_ROW; j++)
      input_board[i][j] = TILE_EMPTY;

  char solution_board[TILES_PER_ROW][TILES_PER_ROW];
  build_solution(solution_board);

  // Build hints for columns
  vec hint_cols[TILES_PER_ROW];
  for (int i = 0; i < TILES_PER_ROW; i++)
  {
    vec * v = hint_cols + i;
    v->len = 0;
    int chain = 0;
    for (int j = 0; j < TILES_PER_ROW; j++)
    {
      if (solution_board[i][j] == TILE_FULL) chain++;
      else if (chain > 0)
      {
        v->arr[v->len] = chain;
        v->len += 1;
        chain = 0;
      }
    }
    if (chain > 0)
    {
      v->arr[v->len] = chain;
      v->len += 1;
    }
  }

  // Build hints for rows
  vec hint_rows[TILES_PER_ROW];
  for (int i = 0; i < TILES_PER_ROW; i++)
  {
    vec * v = hint_rows + i;
    v->len = 0;
    int chain = 0;
    for (int j = 0; j < TILES_PER_ROW; j++)
    {
      if (solution_board[j][i] == TILE_FULL) chain++;
      else if (chain > 0)
      {
        v->arr[v->len] = chain;
        v->len += 1;
        chain = 0;
      }
    }
    if (chain > 0)
    {
      v->arr[v->len] = chain;
      v->len += 1;
    }
  }

  InitWindow(W_WIDTH, W_HEIGHT, "Nonogram");
  SetTargetFPS(T_FPS);

  Texture2D font = LoadTexture(FONT_FILE);
  int is_solved = 0;

  while (!WindowShouldClose() && !is_solved)
  {
    // Handle mouse input
    int_pair mouse_tile = get_mouse_tile();
    if (mouse_tile.x != -1 && mouse_tile.y != -1)
    {
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      {
        if (input_board[mouse_tile.x][mouse_tile.y] == TILE_FULL)
          input_board[mouse_tile.x][mouse_tile.y] = TILE_EMPTY;
        else input_board[mouse_tile.x][mouse_tile.y] = TILE_FULL;
        is_solved = check_solution(input_board, solution_board);
      }
      else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
      {
        if (input_board[mouse_tile.x][mouse_tile.y] == TILE_X)
          input_board[mouse_tile.x][mouse_tile.y] = TILE_EMPTY;
        else input_board[mouse_tile.x][mouse_tile.y] = TILE_X;
        is_solved = check_solution(input_board, solution_board);
      }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);
    draw_grid(mouse_tile, input_board);

    // Draw hints for columns
    Rectangle source = (Rectangle) {0, 0, FONT_SIZE, FONT_SIZE};
    Vector2 draw_spawn = (Vector2)
      {GRID_X + TEXT_OFFSET, GRID_Y + TEXT_OFFSET - TILE_SIZE};
    for (int i = 0; i < TILES_PER_ROW; i++)
    {
      vec * v = hint_cols + i;
      Vector2 draw_pos = draw_spawn;
      for (int j = v->len - 1; j >= 0; j--)
      {
        source.x = v->arr[j] * FONT_SIZE;
        DrawTextureRec(font, source, draw_pos, WHITE);
        draw_pos.y -= TILE_SIZE;
      }
      draw_spawn.x += TILE_SIZE;
    }

    // Draw hints for rows
    draw_spawn.x = GRID_X + TEXT_OFFSET - TILE_SIZE;
    draw_spawn.y = GRID_Y + TEXT_OFFSET;
    for (int i = 0; i < TILES_PER_ROW; i++)
    {
      vec * v = hint_rows + i;
      Vector2 draw_pos = draw_spawn;
      for (int j = v->len - 1; j >= 0; j--)
      {
        source.x = v->arr[j] * FONT_SIZE;
        DrawTextureRec(font, source, draw_pos, WHITE);
        draw_pos.x -= TILE_SIZE;
      }
      draw_spawn.y += TILE_SIZE;
    }

    EndDrawing();
  }

  UnloadTexture(font);
  CloseWindow();
  return 0;
}

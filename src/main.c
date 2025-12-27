#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "openRTS.h"

struct s_sprites	sprites;

void	draw_units(t_unit **units, int count)
{
	for (int i = 0; i < count; i++)
	{
		switch (units[i]->type)
		{
		case UNIT_SOLDIER:
			DrawTexture(sprites.sprite_soldier, (int)units[i]->position.x, (int)units[i]->position.y, WHITE);
			break;
		default:
			break;
		}
	}
}

void	load_sprites(void)
{
	sprites.sprite_soldier = LoadTexture("sprites/soldier_front.png");
}

void update_units(t_unit **units, int count, float dt)
{
	for (int i = 0; i < count; i++)
	{
		float dx = units[i]->destination.x - units[i]->position.x;
		float dy = units[i]->destination.y - units[i]->position.y;

		float dist = sqrtf(dx * dx + dy * dy);

		if (dist < 0.001f)
		{
			units[i]->position = units[i]->destination;
			continue;
		}

		float max_step = units[i]->speed * dt;

		if (dist <= max_step)
		{
			units[i]->position = units[i]->destination;
		}
		else
		{
			float nx = dx / dist;
			float ny = dy / dist;

			units[i]->position.x += nx * max_step;
			units[i]->position.y += ny * max_step;
		}
	}
}

void send_unit_to(t_unit *unit, float x, float y)
{
	// Adjust for unit size to center the destination
	x = (x - unit->px_size.x / 2.0f);
	y = (y - unit->px_size.y / 2.0f);

	// Snap to grid
	x = roundf(x / 64.0f) * 64.0f;
	y = roundf(y / 64.0f) * 64.0f;

	unit->destination.x = x;
	unit->destination.y = y;
}

t_unit	*make_soldier(float x, float y)
{
	t_unit	*unit = calloc(1, sizeof(t_unit));

	assert(unit != NULL);

	unit->health =			MAX_HEALTH_SOLDIER;
	unit->position.x =		x;
	unit->position.y =		y;
	unit->destination.x =	x;
	unit->destination.y =	y;
	unit->type =			UNIT_SOLDIER;
	unit->speed =			SPEED_SOLDIER;
	unit->selected =		false;
	unit->tile_size.x =		TILE_SIZE_X_SOLDIER;
	unit->tile_size.y =		TILE_SIZE_Y_SOLDIER;
	unit->px_size.x =		PX_SIZE_X_SOLDIER;
	unit->px_size.y =		PX_SIZE_Y_SOLDIER;
	return (unit);
}

void	draw_tile_grid(Camera2D camera, t_map map)
{
	const float tile_size_px_x = 64.0f;
	const float tile_size_px_y = 64.0f;

	Vector2 top_left = GetScreenToWorld2D((Vector2){0, 0}, camera);
	Vector2 bottom_right = GetScreenToWorld2D((Vector2){(float)GetScreenWidth(), (float)GetScreenHeight()}, camera);

	int start_x = (int)(top_left.x / tile_size_px_x) - 1;
	int start_y = (int)(top_left.y / tile_size_px_y) - 1;
	int end_x = (int)(bottom_right.x / tile_size_px_x) + 1;
	int end_y = (int)(bottom_right.y / tile_size_px_y) + 1;

	if (start_x < 0) start_x = 0;
	if (start_y < 0) start_y = 0;
	if (end_x > map.width) end_x = map.width;
	if (end_y > map.height) end_y = map.height;

	DrawRectangle(start_x * tile_size_px_x, start_y * tile_size_px_y,
				  (end_x - start_x) * tile_size_px_x, (end_y - start_y) * tile_size_px_y, WHITE);

	for (int x = start_x; x <= end_x; x++)
	{
		DrawLineV((Vector2){x * tile_size_px_x, start_y * tile_size_px_y},
				  (Vector2){x * tile_size_px_x, end_y * tile_size_px_y}, LIGHTGRAY);
	}
	for (int y = start_y; y <= end_y; y++)
	{
		DrawLineV((Vector2){start_x * tile_size_px_x, y * tile_size_px_y},
				  (Vector2){end_x * tile_size_px_x, y * tile_size_px_y}, LIGHTGRAY);
	}
}

void handle_controls(Camera2D *camera, float dt)
{
	const float camera_speed = 500.0f;

	if (IsKeyDown(KEY_W))
		camera->target.y -= camera_speed * dt;
	if (IsKeyDown(KEY_S))
		camera->target.y += camera_speed * dt;
	if (IsKeyDown(KEY_A))
		camera->target.x -= camera_speed * dt;
	if (IsKeyDown(KEY_D))
		camera->target.x += camera_speed * dt;
}

int	main(void)
{
	const int	screenWidth = GetScreenWidth();
	const int	screenHeight = GetScreenHeight();

	Camera2D camera = { 0 };
	camera.target = (Vector2){ 0.0f, 0.0f };
	camera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f};
;
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	t_map map = { 10, 10 };

	float		dt;

	t_unit		*units[MAX_UNITS];

	units[0] = make_soldier(100.0f, 100.0f);
	units[0]->destination.x = 400.0f;
	units[0]->destination.y = 400.0f;

	InitWindow(screenWidth, screenHeight, "openRTS");
	SetTargetFPS(60);
	load_sprites();

	while (!WindowShouldClose())
	{
		dt = GetFrameTime();

		BeginDrawing();
		ClearBackground(BLACK);
		BeginMode2D(camera);
		draw_tile_grid(camera, map);
		draw_units(units, 1);
		handle_controls(&camera, dt);
		EndMode2D();
		if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
		{
			Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);
			send_unit_to(units[0], mouse_pos.x, mouse_pos.y);
		}
		update_units(units, 1, dt);
		EndDrawing();
	}
	CloseWindow();
	return (0);
}
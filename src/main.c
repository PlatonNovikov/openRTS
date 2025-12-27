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
	unit->destination.x = x;
	unit->destination.y = y;
}

t_unit	*make_soldier(float x, float y)
{
	t_unit	*unit = calloc(1, sizeof(t_unit));

	assert(unit != NULL);

	unit->health = MAX_HEALTH_SOLDIER;
	unit->position.x = x;
	unit->position.y = y;
	unit->destination.x = x;
	unit->destination.y = y;
	unit->type = UNIT_SOLDIER;
	unit->speed = SPEED_SOLDIER;
	return (unit);
}

int	main(void)
{
	const int	screenWidth = 1920;
	const int	screenHeight = 1080;

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
		ClearBackground(WHITE);
		draw_units(units, 1);
		EndDrawing();
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			Vector2 mouse_pos = GetMousePosition();
			send_unit_to(units[0], mouse_pos.x, mouse_pos.y);
		}
		update_units(units, 1, dt);
	}
	CloseWindow();
	return (0);
}
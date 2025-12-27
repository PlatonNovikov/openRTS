#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "../include/openRTS.h"
#include "../include/a_star.h"

struct s_sprites	sprites;

Vector2 map_to_screen(Vector2i tile_pos)
{
	Vector2	screen_pos;

	screen_pos.x = tile_pos.x * TILE_SIZE_PX;
	screen_pos.y = tile_pos.y * TILE_SIZE_PX;
	return (screen_pos);
}

Vector2i screen_to_map(Vector2i screen_pos)
{
	Vector2i	tile_pos;

	tile_pos.x = screen_pos.x / TILE_SIZE_PX;
	tile_pos.y = screen_pos.y / TILE_SIZE_PX;
	return (tile_pos);
}

void	draw_units(t_unit **units, int count)
{
	for (int i = 0; i < count; i++)
	{
		switch (units[i]->type)
		{
		case UNIT_SOLDIER:
			DrawTexture(sprites.sprite_soldier,
						map_to_screen(units[i]->position).x,
						map_to_screen(units[i]->position).y,
						WHITE);
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

void	send_unit_to(t_unit *unit, t_map map, Vector2i tile_dest)
{
	unit->destination = tile_dest;
	unit->path = pathfinding(map, unit->position, unit->destination);
}

void update_units(t_unit **units, int count, float dt)
{
	static float accumulated_time = 0.0f;
	accumulated_time += dt;

	// Двигаем юнитов с фиксированным интервалом
	float move_interval = 0.1f; // время между шагами в секундах

	if (accumulated_time >= move_interval)
	{
		accumulated_time -= move_interval;

		for (int i = 0; i < count; i++)
		{
			t_unit *unit = units[i];

			if (unit->path.size == 0)
				continue;

			// Берем первый узел пути (текущая цель)
			// Путь уже развернут в правильном порядке
			t_node *next_node = unit->path.nodes[0];
			Vector2i next_pos = {next_node->x, next_node->y};

			// Текущая позиция в grid-координатах
			Vector2i current_pos = {
				(int)unit->position.x,
				(int)unit->position.y
			};

			// Если уже на целевом узле
			if (current_pos.x == next_pos.x && current_pos.y == next_pos.y)
			{
				// Удаляем достигнутый узел из пути
				for (int j = 0; j < unit->path.size - 1; j++)
				{
					unit->path.nodes[j] = unit->path.nodes[j + 1];
				}
				unit->path.size--;

				// Если путь не пуст, двигаемся к следующему узлу
				if (unit->path.size > 0)
				{
					next_node = unit->path.nodes[0];
					next_pos = (Vector2i){next_node->x, next_node->y};
				}
				else
				{
					continue; // Путь завершен
				}
			}

			// Движение к следующему узлу
			if (current_pos.x < next_pos.x)
				unit->position.x += 1;
			else if (current_pos.x > next_pos.x)
				unit->position.x -= 1;

			if (current_pos.y < next_pos.y)
				unit->position.y += 1;
			else if (current_pos.y > next_pos.y)
				unit->position.y -= 1;
		}
	}
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
	unit->tpf =				TPF_SOLDIER;
	unit->selected =		false;
	unit->tile_size.x =		TILE_SIZE_X_SOLDIER;
	unit->tile_size.y =		TILE_SIZE_Y_SOLDIER;
	unit->px_size.x =		PX_SIZE_X_SOLDIER;
	unit->px_size.y =		PX_SIZE_Y_SOLDIER;
	return (unit);
}

void	draw_tile_grid(Camera2D camera, t_map map)
{
	int start_x = (int)(camera.target.x - camera.offset.x / camera.zoom) / TILE_SIZE_PX;
	int start_y = (int)(camera.target.y - camera.offset.y / camera.zoom) / TILE_SIZE_PX;
	int end_x = (int)(camera.target.x + (GetScreenWidth() - camera.offset.x) / camera.zoom) / TILE_SIZE_PX + 1;
	int end_y = (int)(camera.target.y + (GetScreenHeight() - camera.offset.y) / camera.zoom) / TILE_SIZE_PX + 1;

	if (start_x < 0) start_x = 0;
	if (start_y < 0) start_y = 0;
	if (end_x > map.width) end_x = map.width;
	if (end_y > map.height) end_y = map.height;

	for (int y = start_y; y < end_y; y++)
	{
		for (int x = start_x; x < end_x; x++)
		{
			Color tile_color = map.tiles[y][x].is_blocked ? RED : LIGHTGRAY;
			DrawRectangle(x * TILE_SIZE_PX, y * TILE_SIZE_PX, TILE_SIZE_PX, TILE_SIZE_PX, tile_color);
			DrawRectangleLines(x * TILE_SIZE_PX, y * TILE_SIZE_PX, TILE_SIZE_PX, TILE_SIZE_PX, DARKGRAY);
		}
	}
}

void	handle_controls(Camera2D *camera, t_map map, float dt)
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

	//DEBUG

	//make tile unpassable
	if (IsKeyDown(KEY_U))
	{
		Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), *camera);
		int tile_x = (int)(mouse_pos.x / TILE_SIZE_PX);
		int tile_y = (int)(mouse_pos.y / TILE_SIZE_PX);
		if (tile_x >= 0 && tile_x < map.width && tile_y >= 0 && tile_y < map.height)
			map.tiles[tile_y][tile_x].is_blocked = true;
	}
}

void	handle_mouse(t_unit **units_selected, size_t selected_count, t_map map, Camera2D *camera)
{
	float wheel;

	//right click to move selected units
	if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
		{
			Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), *camera);
			if (screen_to_map((Vector2i){(int)mouse_pos.x, (int)mouse_pos.y}).x < 0 ||
				screen_to_map((Vector2i){(int)mouse_pos.x, (int)mouse_pos.y}).x >= map.width ||
				screen_to_map((Vector2i){(int)mouse_pos.x, (int)mouse_pos.y}).y < 0 ||
				screen_to_map((Vector2i){(int)mouse_pos.x, (int)mouse_pos.y}).y >= map.height)
				return ;
			for (size_t i = 0; i < selected_count; i++)
			{
				send_unit_to(units_selected[i], map, screen_to_map((Vector2i){(int)mouse_pos.x, (int)mouse_pos.y}));
			}
		}

	//wheel zoom
	wheel = GetMouseWheelMove();
	if (wheel != 0)
	{
		camera->zoom += wheel * 0.1f;
		if (camera->zoom < 0.1f)
			camera->zoom = 0.1f;
	}
}

t_map	create_map(int width, int height)
{
	t_map	map;
	int		x;

	map.width = width;
	map.height = height;

	map.tiles = calloc(height, sizeof(t_tile *));
	for (x = 0; x < height; x++)
	{
		map.tiles[x] = calloc(width, sizeof(t_tile));
	}
	return (map);
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

	t_map map = create_map(100, 100);
	map.nodes = create_node_grid(map);

	float		dt;

	t_unit		*units[MAX_UNITS];
	size_t		unit_count = 0;
	t_unit		*units_selected[MAX_UNITS];
	size_t		selected_count = 0;

	units[0] = make_soldier(2, 2);
	units[0]->destination.x = 4;
	units[0]->destination.y = 4;

	units_selected[0] = units[0];
	unit_count++;
	selected_count++;

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
		handle_controls(&camera, map, dt);
		EndMode2D();
		handle_mouse(units_selected, selected_count, map, &camera);
		update_units(units, 1, dt);
		EndDrawing();
	}
	CloseWindow();
	return (0);
}
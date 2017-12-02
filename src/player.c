#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "./player.h"
#include "./platforms.h"

#define PLAYER_WIDTH 50.0f
#define PLAYER_HEIGHT 50.0f
#define PLAYER_SPEED 500.0f
#define PLAYER_GRAVITY 1500.0f

struct player_t {
    float x, y;
    float dx, dy;
};

struct player_t *create_player(float x, float y)
{
    struct player_t *player = malloc(sizeof(struct player_t));

    if (player == NULL) {
        return NULL;
    }

    player->x = x;
    player->y = y;
    player->dx = 0.0f;
    player->dy = 0.0f;

    return player;
}

void destroy_player(struct player_t * player)
{
    free(player);
}

int render_player(const struct player_t * player,
                  SDL_Renderer *renderer)
{
    if (SDL_SetRenderDrawColor(renderer, 96, 255, 96, 255) < 0) {
        return -1;
    }

    SDL_Rect rect;
    rect.x = (int)roundf(player->x);
    rect.y = (int)roundf(player->y);
    rect.w = (int)roundf(PLAYER_WIDTH);
    rect.h = (int)roundf(PLAYER_HEIGHT);

    if (SDL_RenderFillRect(renderer, &rect) < 0) {
        return -1;
    }

    return 0;
}

void update_player(struct player_t * player,
                   const struct platforms_t *platforms,
                   Uint32 delta_time)
{
    float d = (float) delta_time / 1000.0f;

    float dx = player->dx;
    float dy = player->dy + PLAYER_GRAVITY * d;

    float x = player->x + dx * d;
    float y = fmodf(player->y + dy * d, 600.0f);

    struct rect_t player_object = {
        .x = x,
        .y = y,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };

    /* TODO(#6): Implement collision for the left/right sides */
    if (platforms_rect_object_collide(platforms, &player_object)) {
        dy = -player->dy * 0.75f;
        x = player->x + dx * d;
        y = fmodf(player->y + dy * d, 600.0f);
    }

    player->dx = dx;
    player->dy = dy;
    player->x = x;
    player->y = y;
}

void player_move_left(struct player_t *player)
{
    player->dx = -PLAYER_SPEED;
}

void player_move_right(struct player_t *player)
{
    player->dx = PLAYER_SPEED;
}

void player_stop(struct player_t *player)
{
    player->dx = 0.0f;
}

void player_jump(struct player_t *player)
{
    player->dy = -500.0f;
}
#include <assert.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "color.h"
#include "game/sprite_font.h"
#include "log.h"
#include "math/point.h"
#include "str.h"
#include "system/error.h"
#include "system/lt.h"

struct Log
{
    Lt *lt;

    const Sprite_font *font;
    Vec font_size;

    Color *colors;
    char **buffer;
    size_t cursor;
    size_t capacity;
};

Log *create_log(const Sprite_font *font,
                Vec font_size,
                size_t capacity)
{
    Lt *lt = create_lt();
    if (lt == NULL) {
        return NULL;
    }

    Log *log = PUSH_LT(lt, malloc(sizeof(Log)), free);
    if (log == NULL) {
        throw_error(ERROR_TYPE_LIBC);
        RETURN_LT(lt, NULL);
    }
    log->lt = lt;
    log->font = font;
    log->font_size = font_size;
    log->capacity = capacity;

    log->buffer = PUSH_LT(lt, calloc(capacity, sizeof(char*)), free);
    if (log->buffer == NULL) {
        throw_error(ERROR_TYPE_LIBC);
        RETURN_LT(lt, NULL);
    }

    log->colors = PUSH_LT(lt, calloc(capacity, sizeof(Color)), free);
    if (log->colors == NULL) {
        throw_error(ERROR_TYPE_LIBC);
        RETURN_LT(lt, NULL);
    }

    log->cursor = 0;

    return log;
}

void destroy_log(Log *log)
{
    assert(log);
    for (size_t i = 0; i < log->capacity; ++i) {
        if (log->buffer[i]) {
            free(log->buffer[i]);
        }
    }
    RETURN_LT0(log->lt);
}

int log_render(const Log *log,
               SDL_Renderer *renderer,
               Point position)
{
    assert(log);
    assert(renderer);
    (void) position;

    for (size_t i = 0; i < log->capacity; ++i) {
        const size_t j = (i + log->cursor) % log->capacity;
        if (log->buffer[j]) {
            if (sprite_font_render_text(log->font,
                                        renderer,
                                        vec_sum(position,
                                                vec(0.0f, FONT_CHAR_HEIGHT * log->font_size.y * (float) i)),
                                        log->font_size,
                                        log->colors[j],
                                        log->buffer[j]) < 0) {
                return -1;
            }
        }
    }

    return 0;
}

int log_push_line(Log *log, const char *line, Color color)
{
    assert(log);
    assert(line);

    const size_t next_cursor = (log->cursor + 1) % log->capacity;

    if (log->buffer[log->cursor] != NULL) {
        free(log->buffer[log->cursor]);
    }

    log->buffer[log->cursor] = string_duplicate(line, NULL);
    log->colors[log->cursor] = color;

    if (log->buffer[log->cursor] == NULL) {
        return -1;
    }

    log->cursor = next_cursor;

    return 0;
}

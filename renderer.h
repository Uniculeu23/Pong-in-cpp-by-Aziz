#ifndef RENDERER_H
#define RENDERER_H

#include "utils.h"

static void render_background() {
    u32* pixel = (u32*)render_state.memory;
    for(int y = 0; y < render_state.height; y++) {
        for(int x = 0; x < render_state.width; x++) {
            *pixel = x * 0xff00ff + y * 0x00ff00;
            pixel++;
        }
    }
}

static void clear_screen(u32 color) {
    u32* pixel = (u32*)render_state.memory;
    for(int y = 0; y < render_state.height; y++) {
        for(int x = 0; x < render_state.width; x++) {
            *pixel = color;
            pixel++;
        }
    }
}

static void draw_rect_in_pixels(
    int x0, int y0,
    int x1, int y1,
    u32 color
) {

    x0 = clamp(0, x0, render_state.width);
    x1 = clamp(0, x1, render_state.width);
    y0 = clamp(0, y0, render_state.height);
    y1 = clamp(0, y1, render_state.height);

    for(int y = y0; y < y1; y++) {
        u32* pixel = (u32*)render_state.memory + x0 + y * render_state.width;
        for(int x = x0; x < x1; x++) {
            *pixel = color;
            pixel++;
        }
    }
}

static float render_scale = 0.1f;

static void draw_rect(
    float x, float y,
    float half_size_x, float half_size_y,
    u32 color
) {
    x *= ((float)render_state.width / render_state.height) * render_scale;
    y *= ((float)render_state.width / render_state.height) * render_scale;
    half_size_x *= ((float)render_state.width / render_state.height) * render_scale;
    half_size_y *= ((float)render_state.width / render_state.height) * render_scale;

    x += render_state.width / 2.f;
    y += render_state.height / 2.f;

    //Change to pixels
    int x0 = x - half_size_x;
    int x1 = x + half_size_x;
    int y0 = y - half_size_y;
    int y1 = y + half_size_y;

    draw_rect_in_pixels(x0, y0, x1, y1, color);
}

#endif // RENDERER_H
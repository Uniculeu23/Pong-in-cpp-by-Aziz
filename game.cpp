
#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

#include <cstdio>
#include <cmath>
#include "renderer.h"

float game_x0 = 3200, game_y0 = 0.f;
float game_x1 = -3200, game_y1 = 0.f;

float game_speed = 4000;

float game_v0 = 0;
float game_v1 = 0;

int game_arena_half_size_x = 3350, game_arena_half_size_y = 1750; 
float game_player_half_size_x = 100, game_player_half_size_y = 400;

float game_x2 = 0, game_y2 = 0;
float game_ball_half_size = 50;

float game_vx2 = 1000, game_vy2 = 750;

double game_time = 0;
bool game_paused = false;

int game_score_1 = 0, game_score_2 = 0;

void simulate_game(Input* input, float dt) {
    clear_screen(0x000000);

    
    if (pressed(BUTTON_SPACE)) {
        game_paused = !game_paused;
    }


    if (game_paused) {
        clear_screen(0x000000);
        draw_rect(0, 0, game_arena_half_size_x, game_arena_half_size_y, 0xffffff);
        // Maybe draw a "PAUSED" sign too?
        return;
    }    

    game_time += dt;

    draw_rect(0, 0, game_arena_half_size_x, game_arena_half_size_y, 0xffffff);    
    
    float acc0 = 0.f;
    float acc1 = 0.f;

    if (game_arena_half_size_y > game_y0 + game_player_half_size_y) {            
        if (is_down(BUTTON_UP)) {
            acc0 += game_speed + 3000;
        }
    }
    else {
        game_y0 = game_arena_half_size_y - game_player_half_size_y;
        game_v0 *= -2;
    }
    
    if (- game_arena_half_size_y < game_y0 - game_player_half_size_y) {            
        if (is_down(BUTTON_DOWN)) {
            acc0 -= game_speed + 3000;
        }    
    }
    else {
        game_y0 = - game_arena_half_size_y + game_player_half_size_y;
        game_v0 *= -2;
    }
    
    if (game_arena_half_size_y > game_y1 + game_player_half_size_y) {            
        if (is_down(BUTTON_W)) {
            acc1 += game_speed;
        }
    }
    else {
        game_y1 = game_arena_half_size_y - game_player_half_size_y;
        game_v1 *= -2;
    }
    

    if (- game_arena_half_size_y < game_y1 - game_player_half_size_y) {            
        if (is_down(BUTTON_S)) {
            acc1 -= game_speed;
        }    
    }
    else {
        game_y1 = - game_arena_half_size_y + game_player_half_size_y;
        game_v1 *= -2;
    }

    /*
    //bot medium

    int predict_y = game_y2 + game_vy2;
    
    if (predict_y > game_y1) {
        acc1 += game_speed;
    }
    else {
        acc1 -= game_speed;
    }
    */
    /*
    //bot 1 hard
    if (fabsf(game_vx2) > 0.01f) {
        float predict_time = fabsf((game_x2 - game_x0) / game_vx2);
        float sim_y = game_y2;
        float sim_vy = game_vy2;
        float time_remaining = predict_time;
    
        while (time_remaining > 0.f && fabsf(sim_vy) > 0.01f) {
            float time_to_top = (game_arena_half_size_y - game_ball_half_size - sim_y) / sim_vy;
            float time_to_bottom = (-game_arena_half_size_y + game_ball_half_size - sim_y) / sim_vy;
    
            float bounce_time = time_to_top * (sim_vy > 0) + time_to_bottom * (1 - (sim_vy > 0));
    
            bounce_time = fabsf(bounce_time);
    
            float will_bounce = bounce_time <= time_remaining;
    
            float used_time = will_bounce * bounce_time + (1 - will_bounce) * time_remaining;
            sim_y += sim_vy * used_time;
    
            float bounce = -1.01f;
            sim_vy = sim_vy * ((1 - will_bounce) + will_bounce * bounce);
            time_remaining -= used_time;
        }
    
        float predict_y = sim_y;
    
        acc0 += game_speed * (predict_y > game_y0);
        acc0 += game_speed * (predict_y < game_y0);
    }
    */
    //bot 2 hard
    if (fabsf(game_vx2) > 0.01f) {
        float predict_time = fabsf((game_x2 - game_x1) / game_vx2);
        float sim_y = game_y2;
        float sim_vy = game_vy2;
        float time_remaining = predict_time;
    
        while (time_remaining > 0.f && fabsf(sim_vy) > 0.01f) {
            float time_to_top = (game_arena_half_size_y - game_ball_half_size - sim_y) / sim_vy;
            float time_to_bottom = (-game_arena_half_size_y + game_ball_half_size - sim_y) / sim_vy;

            float bounce_time = time_to_top * (sim_vy > 0) + time_to_bottom * (1 - (sim_vy > 0));

            bounce_time = fabsf(bounce_time);
    
            float will_bounce = bounce_time <= time_remaining;

            float used_time = will_bounce * bounce_time + (1 - will_bounce) * time_remaining;
            sim_y += sim_vy * used_time;
            
            float bounce = -1.01f;
            sim_vy = sim_vy * ((1 - will_bounce) + will_bounce * bounce);
            time_remaining -= used_time;            
        }
    
        float predict_y = sim_y;
    
        if (predict_y > game_y1) {
            acc1 += game_speed;
        } else {
            acc1 -= game_speed;
        }
    }
    //player movement
    acc0 -= game_v0 * 5.f;
    acc1 -= game_v1 * 5.f;

    game_y0 = game_y0 + game_v0 * dt + acc0 * dt * dt / 2;
    game_v0 = game_v0 + acc0 * dt;

    game_y1 = game_y1 + game_v1 * dt + acc1 * dt * dt / 2;
    game_v1 = game_v1 + acc1 * dt;

    //ball movement
    int game_hit_wall_up = (game_y2 + game_ball_half_size > game_arena_half_size_y);

    int game_hit_wall_down = (game_y2 - game_ball_half_size < - game_arena_half_size_y);

    int game_hit_player_2 = ((game_x2 + game_ball_half_size > game_x0 - game_player_half_size_x) 
    && (game_x2 - game_ball_half_size < game_x0 + game_player_half_size_x)
    && (game_y2 > game_y0 - game_player_half_size_y)
    && (game_y2 < game_y0 + game_player_half_size_y));
    
    int game_hit_player_1 = ((game_x2 - game_ball_half_size < game_x1 + game_player_half_size_x) 
    && (game_x2 + game_ball_half_size > game_x1 - game_player_half_size_x)
    && (game_y2 > game_y1 - game_player_half_size_y)
    && (game_y2 < game_y1 + game_player_half_size_y));

    //wall up
    game_y2 = (1 - game_hit_wall_up) * game_y2 + game_hit_wall_up * (game_arena_half_size_y - game_ball_half_size);

    game_vy2 = (1 - game_hit_wall_up) * game_vy2 + game_hit_wall_up * game_vy2 * -1.01f;
    game_vx2 = (1 - game_hit_wall_up) * game_vx2 + game_hit_wall_up * game_vx2 * 1.01f;

    //wall down
    game_y2 = (1 - game_hit_wall_down) * game_y2 + game_hit_wall_down * (- game_arena_half_size_y + game_ball_half_size);

    game_vy2 = (1 - game_hit_wall_down) * game_vy2 + game_hit_wall_down * game_vy2 * -1.01f;
    game_vx2 = (1 - game_hit_wall_down) * game_vx2 + game_hit_wall_down * game_vx2 * 1.01f;

    //player2
    game_x2 = (1 - game_hit_player_2) * game_x2 + game_hit_player_2 * (game_x0 - game_player_half_size_x - game_ball_half_size);

    game_vx2 = (1 - game_hit_player_2) * game_vx2 + game_hit_player_2 * game_vx2 * -1.01f;
    game_vy2 = (1 - game_hit_player_2) * game_vy2 + game_hit_player_2 * game_vy2 *  1.01f;

    //player1
    game_x2 = (1 - game_hit_player_1) * game_x2 + game_hit_player_1 * (game_x1 + game_player_half_size_x + game_ball_half_size);
    
    game_vx2 = (1 - game_hit_player_1) * game_vx2 + game_hit_player_1 * game_vx2 * -1.01f;
    game_vy2 = (1 - game_hit_player_1) * game_vy2 + game_hit_player_1 * game_vy2 *  1.01f;

    game_x2 = game_x2 + game_vx2 * dt;
    game_y2 = game_y2 + game_vy2 * dt;

    if (game_x2 > game_arena_half_size_x) {
        game_score_2 += 1;
        game_x2 = 0; game_y2 = 0;
        game_vx2 = -1000; game_vy2 = -750;
    }

    if (game_x2 < - game_arena_half_size_x) {
        game_score_1 += 1;
        game_x2 = 0; game_y2 = 0;
        game_vx2 = 1000; game_vy2 = 750;
    }

    draw_rect(game_x2, game_y2, game_ball_half_size, game_ball_half_size, 0x000000);

    draw_rect(game_x0, game_y0, 100, 400, 0x000000);
    draw_rect(game_x1, game_y1, 100, 400, 0x000000);
}

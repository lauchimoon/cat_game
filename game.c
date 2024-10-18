#include <stdio.h>
#include "raylib.h"

#define P_SPEED 5.0f
#define N_PLAYERS  4
#define TILT_DIR_CHANGE 0.2f

#define XBOX_LEGACY "Xbox Controller"
#define XBOX        "Xbox 360 Controller"
#define PS3         "Sony PLAYSTATION(R)3 Controller"
#define NINTENDO    "Nintendo Co., Ltd. Pro Controller"

#define SHOOTING_COOLDOWN 5
#define BULLET_DURATION   15
#define N_BULLETS_PLAYER  30
#define BULLET_SPEED 70

#define WIN_WIDTH  1280
#define WIN_HEIGHT  720

enum {
    DIR_UP = 0,
    DIR_LEFT,
    DIR_DOWN,
    DIR_RIGHT,
};

typedef struct Bullet {
    Rectangle hitbox;
    int duration;
    int speed;
    bool active;
} Bullet;

typedef struct Player {
    bool ready;
    int id;
    int direction;
    float speed;
    bool shot;
    Rectangle hitbox;

    int shoot_button;
    int shoot_timer;
    int shot_bullet;
    Bullet bullet;
} Player;

void player_init(Player *, int);
void player_update(Player *);
void player_draw(Player *);

int main()
{
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "game");
    SetTargetFPS(60);

    Player players[N_PLAYERS] = {};
    for (int i = 0; i < N_PLAYERS; ++i)
        player_init(&players[i], i);

    while (!WindowShouldClose()) {
        for (int i = 0; i < N_PLAYERS; ++i) {
            players[i].ready = IsGamepadAvailable(i);
            player_update(&players[i]);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < N_PLAYERS; ++i)
            player_draw(&players[i]);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void player_init(Player *p, int id)
{
    p->ready = IsGamepadAvailable(id);
    p->id = id;
    p->direction = (p->id % 2 == 0)? DIR_RIGHT : DIR_LEFT;
    p->speed = P_SPEED;
    p->hitbox = (Rectangle){ 0.0f, 0.0f, 64.0f, 64.0f };
    p->shot_bullet = 0;

    if (TextIsEqual(GetGamepadName(p->id), XBOX_LEGACY) || TextIsEqual(GetGamepadName(p->id), XBOX) || TextIsEqual(GetGamepadName(p->id), PS3))
        p->shoot_button = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
    else if (TextIsEqual(GetGamepadName(p->id), NINTENDO))
        p->shoot_button = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;

    p->shoot_timer = 0;
    p->bullet = (Bullet){ (Rectangle){ p->hitbox.x, p->hitbox.y / 2.0f, 16.0f, 16.0f }, 0, BULLET_SPEED, false };
}

void player_update(Player *p)
{
    p->hitbox.x += p->speed*GetGamepadAxisMovement(p->id, GAMEPAD_AXIS_LEFT_X);
    p->hitbox.y += p->speed*GetGamepadAxisMovement(p->id, GAMEPAD_AXIS_LEFT_Y);

    // Determine direction
    if (GetGamepadAxisMovement(p->id, GAMEPAD_AXIS_LEFT_Y) != 0 ||
        GetGamepadAxisMovement(p->id, GAMEPAD_AXIS_LEFT_X) != 0) {
        if (GetGamepadAxisMovement(p->id, GAMEPAD_AXIS_LEFT_Y) > 2*TILT_DIR_CHANGE)
            p->direction = DIR_UP;
        if (GetGamepadAxisMovement(p->id, GAMEPAD_AXIS_LEFT_Y) < -2*TILT_DIR_CHANGE)
            p->direction = DIR_DOWN;
        if (GetGamepadAxisMovement(p->id, GAMEPAD_AXIS_LEFT_X) > 2*TILT_DIR_CHANGE)
            p->direction = DIR_RIGHT;
        if (GetGamepadAxisMovement(p->id, GAMEPAD_AXIS_LEFT_X) < -2*TILT_DIR_CHANGE)
            p->direction = DIR_LEFT;
    }

    // Shoot

    // Cooldown
    if (p->shot)
        ++p->shoot_timer;
    if (p->shoot_timer >= SHOOTING_COOLDOWN) {
        p->shot = false;
        p->shoot_timer = 0;
    }

    // Action
    Bullet *bullet = &p->bullet;
    if (IsGamepadButtonDown(p->id, p->shoot_button)) {
        p->shot = true;
        bullet->active = true;
    }

    if (bullet->active) {
        int sign_x = 0, sign_y = 0;
        if (p->direction == DIR_RIGHT) sign_x = 1;
        else if (p->direction == DIR_LEFT) sign_x = -1;

        if (p->direction == DIR_UP) sign_y = 1;
        else if (p->direction == DIR_DOWN) sign_y = -1;

        bullet->hitbox.x += sign_x*bullet->speed;
        bullet->hitbox.y += sign_y*bullet->speed;
        ++bullet->duration;
        if (bullet->duration >= BULLET_DURATION || bullet->hitbox.x + bullet->hitbox.width >= WIN_WIDTH ||
                bullet->hitbox.x <= 0 || bullet->hitbox.y + bullet->hitbox.height >= WIN_HEIGHT ||
                bullet->hitbox.y <= 0) {
            bullet->active = false;
            bullet->duration = 0;
            bullet->hitbox = (Rectangle){ p->hitbox.x, p->hitbox.y, 32.0f, 32.0f };
        }
    } else
        bullet->hitbox = (Rectangle){ p->hitbox.x, p->hitbox.y, 32.0f, 32.0f };
}

void player_draw(Player *p)
{
    Color color;
    switch (p->id) {
        case 0: color = RED; break;
        case 1: color = BLUE; break;
        case 2: color = YELLOW; break;
        case 3: color = GREEN; break;
    }

    if (p->ready) {
        DrawRectangleRec(p->hitbox, color);

        if (p->bullet.active)
            DrawRectangleRec(p->bullet.hitbox, GREEN);
    }
}

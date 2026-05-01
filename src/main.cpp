#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")

namespace {

constexpr char kWindowTitle[] = "FleshAndStoneCpp";
constexpr float kPi = 3.14159265358979323846f;
constexpr float kTau = kPi * 2.0f;

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct Rect {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
};

struct Color {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;
};

struct Backbuffer {
    BITMAPINFO info{};
    void* memory = nullptr;
    int width = 0;
    int height = 0;
    int pitch = 0;
};

struct Input {
    bool keys[256]{};
    bool keys_pressed[256]{};
    bool mouse_down = false;
    bool mouse_pressed = false;
    Vec2 mouse{};
};

struct Player {
    Vec2 p{};
    float r = 14.0f;
    int hp = 6;
    int max_hp = 6;
    int coins = 0;
    int kills = 0;
    float speed = 245.0f;
    float fire_cd = 0.0f;
    float invulnerable = 0.0f;
    bool dead = false;
};

struct Enemy {
    Vec2 p{};
    float r = 15.0f;
    float hp = 6.0f;
    float max_hp = 6.0f;
    float speed = 95.0f;
    float fire_cd = 0.0f;
    float hurt = 0.0f;
    int type = 0;
};

struct Bullet {
    Vec2 p{};
    Vec2 v{};
    float r = 5.0f;
    float damage = 3.0f;
    float life = 1.0f;
    bool enemy = false;
};

struct Particle {
    Vec2 p{};
    Vec2 v{};
    float r = 2.0f;
    float life = 1.0f;
    float max_life = 1.0f;
    Color color{};
};

struct Coin {
    Vec2 p{};
    Vec2 v{};
    float life = 10.0f;
};

struct Portal {
    Vec2 p{};
    float r = 34.0f;
    float pulse = 0.0f;
    bool active = false;
};

struct Game {
    int client_w = 1280;
    int client_h = 720;
    int floor = 1;
    int room = 1;
    float time = 0.0f;
    float shake = 0.0f;
    float message_time = 0.0f;
    std::string message;
    Player player;
    Portal portal;
    bool room_clear = false;
    std::vector<Rect> walls;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    std::vector<Particle> particles;
    std::vector<Coin> coins;
};

Backbuffer g_backbuffer;
Input g_input;
Game g_game;
bool g_running = true;

float clampf(float v, float a, float b) {
    return std::max(a, std::min(b, v));
}

float randf(float a, float b) {
    return a + (b - a) * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
}

int randi(int a, int b) {
    return a + (std::rand() % (b - a + 1));
}

float length(Vec2 v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

Vec2 normalize(Vec2 v) {
    const float len = length(v);
    if (len <= 0.0001f) return {0.0f, 0.0f};
    return {v.x / len, v.y / len};
}

float distance(Vec2 a, Vec2 b) {
    return length({a.x - b.x, a.y - b.y});
}

Color rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    return {r, g, b, 255};
}

std::uint32_t pack(Color c) {
    return (static_cast<std::uint32_t>(c.r) << 16) |
           (static_cast<std::uint32_t>(c.g) << 8) |
           static_cast<std::uint32_t>(c.b);
}

void resize_backbuffer(Backbuffer& buffer, int width, int height) {
    if (buffer.memory) {
        VirtualFree(buffer.memory, 0, MEM_RELEASE);
        buffer.memory = nullptr;
    }

    buffer.width = std::max(1, width);
    buffer.height = std::max(1, height);
    buffer.pitch = buffer.width * 4;

    buffer.info = {};
    buffer.info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    buffer.info.bmiHeader.biWidth = buffer.width;
    buffer.info.bmiHeader.biHeight = -buffer.height;
    buffer.info.bmiHeader.biPlanes = 1;
    buffer.info.bmiHeader.biBitCount = 32;
    buffer.info.bmiHeader.biCompression = BI_RGB;

    const std::size_t size = static_cast<std::size_t>(buffer.width) * static_cast<std::size_t>(buffer.height) * 4;
    buffer.memory = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

void clear(Color color) {
    if (!g_backbuffer.memory) return;

    const std::uint32_t packed = pack(color);
    auto* pixels = static_cast<std::uint32_t*>(g_backbuffer.memory);
    const int count = g_backbuffer.width * g_backbuffer.height;

    for (int i = 0; i < count; ++i) {
        pixels[i] = packed;
    }
}

void rect_i(int x, int y, int w, int h, Color color) {
    if (!g_backbuffer.memory) return;

    const int x0 = std::max(0, x);
    const int y0 = std::max(0, y);
    const int x1 = std::min(g_backbuffer.width, x + w);
    const int y1 = std::min(g_backbuffer.height, y + h);

    if (x1 <= x0 || y1 <= y0) return;

    const std::uint32_t packed = pack(color);

    for (int yy = y0; yy < y1; ++yy) {
        auto* row = reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(g_backbuffer.memory) + yy * g_backbuffer.pitch);
        for (int xx = x0; xx < x1; ++xx) {
            row[xx] = packed;
        }
    }
}

void rect(Rect r, Color color) {
    rect_i(static_cast<int>(r.x), static_cast<int>(r.y), static_cast<int>(r.w), static_cast<int>(r.h), color);
}

void circle(Vec2 p, float radius, Color color) {
    const int x0 = std::max(0, static_cast<int>(p.x - radius));
    const int y0 = std::max(0, static_cast<int>(p.y - radius));
    const int x1 = std::min(g_backbuffer.width - 1, static_cast<int>(p.x + radius));
    const int y1 = std::min(g_backbuffer.height - 1, static_cast<int>(p.y + radius));
    const float rr = radius * radius;
    const std::uint32_t packed = pack(color);

    for (int y = y0; y <= y1; ++y) {
        auto* row = reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(g_backbuffer.memory) + y * g_backbuffer.pitch);
        for (int x = x0; x <= x1; ++x) {
            const float dx = static_cast<float>(x) - p.x;
            const float dy = static_cast<float>(y) - p.y;
            if (dx * dx + dy * dy <= rr) {
                row[x] = packed;
            }
        }
    }
}

bool circle_rect(Vec2 p, float r, Rect rect) {
    const float nx = clampf(p.x, rect.x, rect.x + rect.w);
    const float ny = clampf(p.y, rect.y, rect.y + rect.h);
    const float dx = p.x - nx;
    const float dy = p.y - ny;
    return dx * dx + dy * dy <= r * r;
}

bool solid_at(Vec2 p, float r) {
    for (const Rect& wall : g_game.walls) {
        if (circle_rect(p, r, wall)) return true;
    }
    return false;
}

void move_circle(Vec2& p, Vec2 delta, float r) {
    if (!solid_at({p.x + delta.x, p.y}, r)) {
        p.x += delta.x;
    }
    if (!solid_at({p.x, p.y + delta.y}, r)) {
        p.y += delta.y;
    }
}

void say(const char* message, float seconds = 2.0f) {
    g_game.message = message;
    g_game.message_time = seconds;
}

void burst(Vec2 p, Color color, int count) {
    for (int i = 0; i < count; ++i) {
        const float a = randf(0.0f, kTau);
        const float s = randf(70.0f, 260.0f);
        g_game.particles.push_back({
            p,
            {std::cos(a) * s, std::sin(a) * s},
            randf(2.0f, 5.0f),
            randf(0.22f, 0.75f),
            0.75f,
            color
        });
    }
}

void spawn_enemy() {
    Enemy e;
    e.type = randi(0, 2);
    e.r = e.type == 1 ? 20.0f : 15.0f;
    e.hp = e.type == 1 ? 11.0f + g_game.floor * 2.0f : 6.0f + g_game.floor;
    e.max_hp = e.hp;
    e.speed = e.type == 1 ? 72.0f : e.type == 2 ? 85.0f : 128.0f;
    e.fire_cd = randf(0.5f, 1.6f);

    for (int tries = 0; tries < 200; ++tries) {
        e.p.x = randf(150.0f, static_cast<float>(g_game.client_w) - 150.0f);
        e.p.y = randf(150.0f, static_cast<float>(g_game.client_h) - 150.0f);

        if (distance(e.p, g_game.player.p) > 180.0f && !solid_at(e.p, e.r)) {
            break;
        }
    }

    g_game.enemies.push_back(e);
}

void create_room() {
    g_game.enemies.clear();
    g_game.bullets.clear();
    g_game.particles.clear();
    g_game.coins.clear();
    g_game.walls.clear();
    g_game.portal = Portal{};
    g_game.room_clear = false;

    const float w = static_cast<float>(g_game.client_w);
    const float h = static_cast<float>(g_game.client_h);
    const float m = 84.0f;

    g_game.walls.push_back({m, m, w - m * 2.0f, 24.0f});
    g_game.walls.push_back({m, h - m, w - m * 2.0f, 24.0f});
    g_game.walls.push_back({m, m, 24.0f, h - m * 2.0f});
    g_game.walls.push_back({w - m, m, 24.0f, h - m * 2.0f});

    const int obstacle_count = std::min(8, 2 + g_game.floor + g_game.room / 2);
    for (int i = 0; i < obstacle_count; ++i) {
        g_game.walls.push_back({
            randf(m + 100.0f, w - m - 220.0f),
            randf(m + 90.0f, h - m - 170.0f),
            randf(64.0f, 160.0f),
            randf(46.0f, 118.0f)
        });
    }

    g_game.player.p = {w * 0.5f, h * 0.5f};
    g_game.player.dead = false;
    g_game.player.fire_cd = 0.0f;
    g_game.player.invulnerable = 1.0f;

    const int enemy_count = std::min(15, 3 + g_game.floor + g_game.room);
    for (int i = 0; i < enemy_count; ++i) {
        spawn_enemy();
    }

    say("Clear the room. Teal portal opens after the last enemy.", 2.6f);
}

void restart_game() {
    g_game.floor = 1;
    g_game.room = 1;
    g_game.player = Player{};
    g_game.player.p = {g_game.client_w * 0.5f, g_game.client_h * 0.5f};
    create_room();
}

void enter_next_room() {
    ++g_game.room;
    if (g_game.room > 4) {
        ++g_game.floor;
        g_game.room = 1;
        ++g_game.player.max_hp;
        g_game.player.hp = g_game.player.max_hp;
    }
    create_room();
}

void shoot_player() {
    Player& p = g_game.player;
    if (p.dead || p.fire_cd > 0.0f) return;

    p.fire_cd = 0.14f;
    const Vec2 aim = normalize({g_input.mouse.x - p.p.x, g_input.mouse.y - p.p.y});
    g_game.bullets.push_back({
        {p.p.x + aim.x * 22.0f, p.p.y + aim.y * 22.0f},
        {aim.x * 680.0f, aim.y * 680.0f},
        5.0f,
        3.0f,
        0.95f,
        false
    });
    burst({p.p.x + aim.x * 24.0f, p.p.y + aim.y * 24.0f}, rgb(255, 231, 139), 4);
}

void shoot_enemy(const Enemy& e) {
    const Vec2 aim = normalize({g_game.player.p.x - e.p.x, g_game.player.p.y - e.p.y});
    g_game.bullets.push_back({
        e.p,
        {aim.x * 280.0f, aim.y * 280.0f},
        6.0f,
        1.0f,
        1.8f,
        true
    });
}

void damage_player(float amount, Vec2 source) {
    Player& p = g_game.player;
    if (p.dead || p.invulnerable > 0.0f) return;

    p.hp -= static_cast<int>(amount);
    p.invulnerable = 0.75f;
    g_game.shake = std::max(g_game.shake, 13.0f);
    burst(p.p, rgb(255, 101, 141), 22);

    const Vec2 away = normalize({p.p.x - source.x, p.p.y - source.y});
    move_circle(p.p, {away.x * 24.0f, away.y * 24.0f}, p.r);

    if (p.hp <= 0) {
        p.hp = 0;
        p.dead = true;
        say("You died. Press R to restart.", 9999.0f);
    }
}

void damage_enemy(Enemy& e, float amount, Vec2 source) {
    e.hp -= amount;
    e.hurt = 0.12f;

    const Vec2 away = normalize({e.p.x - source.x, e.p.y - source.y});
    move_circle(e.p, {away.x * 12.0f, away.y * 12.0f}, e.r);

    burst(e.p, rgb(255, 208, 164), 7);

    if (e.hp <= 0.0f) {
        ++g_game.player.kills;
        ++g_game.player.coins;
        g_game.coins.push_back({e.p, {randf(-90.0f, 90.0f), randf(-90.0f, 90.0f)}, 12.0f});
        burst(e.p, rgb(255, 231, 139), 24);
    }
}

void update_game(float dt) {
    Player& p = g_game.player;
    g_game.time += dt;
    g_game.shake = std::max(0.0f, g_game.shake - dt * 42.0f);
    g_game.message_time = std::max(0.0f, g_game.message_time - dt);

    if (g_input.keys_pressed['R']) {
        restart_game();
        return;
    }

    if (p.invulnerable > 0.0f) p.invulnerable -= dt;
    if (p.fire_cd > 0.0f) p.fire_cd -= dt;

    if (!p.dead) {
        Vec2 axis{};
        if (g_input.keys['W'] || g_input.keys[VK_UP]) axis.y -= 1.0f;
        if (g_input.keys['S'] || g_input.keys[VK_DOWN]) axis.y += 1.0f;
        if (g_input.keys['A'] || g_input.keys[VK_LEFT]) axis.x -= 1.0f;
        if (g_input.keys['D'] || g_input.keys[VK_RIGHT]) axis.x += 1.0f;
        axis = normalize(axis);

        move_circle(p.p, {axis.x * p.speed * dt, axis.y * p.speed * dt}, p.r);

        if (g_input.mouse_down || g_input.keys[VK_SPACE]) {
            shoot_player();
        }
    }

    for (int i = static_cast<int>(g_game.bullets.size()) - 1; i >= 0; --i) {
        Bullet& b = g_game.bullets[static_cast<std::size_t>(i)];
        b.life -= dt;
        b.p.x += b.v.x * dt;
        b.p.y += b.v.y * dt;

        bool remove = b.life <= 0.0f || solid_at(b.p, b.r);

        if (!remove && b.enemy) {
            if (!p.dead && distance(b.p, p.p) < b.r + p.r) {
                damage_player(1.0f, b.p);
                remove = true;
            }
        } else if (!remove) {
            for (Enemy& e : g_game.enemies) {
                if (e.hp <= 0.0f) continue;
                if (distance(b.p, e.p) < b.r + e.r) {
                    damage_enemy(e, b.damage, b.p);
                    remove = true;
                    break;
                }
            }
        }

        if (remove) {
            g_game.bullets.erase(g_game.bullets.begin() + i);
        }
    }

    for (Enemy& e : g_game.enemies) {
        if (e.hp <= 0.0f) continue;

        if (e.hurt > 0.0f) e.hurt -= dt;
        e.fire_cd -= dt;

        const Vec2 to_player = normalize({p.p.x - e.p.x, p.p.y - e.p.y});
        const float d = distance(e.p, p.p);

        if (e.type == 2) {
            const float desired = d < 270.0f ? -0.75f : 0.42f;
            move_circle(e.p, {to_player.x * e.speed * desired * dt, to_player.y * e.speed * desired * dt}, e.r);

            if (e.fire_cd <= 0.0f && !p.dead) {
                e.fire_cd = randf(0.8f, 1.5f);
                shoot_enemy(e);
            }
        } else {
            move_circle(e.p, {to_player.x * e.speed * dt, to_player.y * e.speed * dt}, e.r);
        }

        if (!p.dead && d < e.r + p.r) {
            damage_player(e.type == 1 ? 2.0f : 1.0f, e.p);
        }
    }

    g_game.enemies.erase(
        std::remove_if(g_game.enemies.begin(), g_game.enemies.end(), [](const Enemy& e) { return e.hp <= 0.0f; }),
        g_game.enemies.end()
    );

    for (int i = static_cast<int>(g_game.coins.size()) - 1; i >= 0; --i) {
        Coin& c = g_game.coins[static_cast<std::size_t>(i)];
        c.life -= dt;
        const float d = distance(c.p, p.p);

        if (d < 190.0f) {
            const Vec2 pull = normalize({p.p.x - c.p.x, p.p.y - c.p.y});
            c.v.x += pull.x * 780.0f * dt;
            c.v.y += pull.y * 780.0f * dt;
        }

        c.p.x += c.v.x * dt;
        c.p.y += c.v.y * dt;
        c.v.x *= 0.92f;
        c.v.y *= 0.92f;

        if (d < p.r + 12.0f) {
            ++p.coins;
            g_game.coins.erase(g_game.coins.begin() + i);
            continue;
        }

        if (c.life <= 0.0f) {
            g_game.coins.erase(g_game.coins.begin() + i);
        }
    }

    for (int i = static_cast<int>(g_game.particles.size()) - 1; i >= 0; --i) {
        Particle& part = g_game.particles[static_cast<std::size_t>(i)];
        part.life -= dt;
        part.p.x += part.v.x * dt;
        part.p.y += part.v.y * dt;
        part.v.x *= 0.90f;
        part.v.y *= 0.90f;

        if (part.life <= 0.0f) {
            g_game.particles.erase(g_game.particles.begin() + i);
        }
    }

    if (!g_game.room_clear && g_game.enemies.empty()) {
        g_game.room_clear = true;
        g_game.portal.active = true;
        g_game.portal.p = {static_cast<float>(g_game.client_w) - 140.0f, static_cast<float>(g_game.client_h) * 0.5f};
        say("Room clear. Enter the teal portal.", 2.4f);
    }

    if (g_game.portal.active) {
        g_game.portal.pulse += dt * 5.0f;
        if (!p.dead && distance(p.p, g_game.portal.p) < p.r + g_game.portal.r) {
            enter_next_room();
        }
    }

    std::fill(std::begin(g_input.keys_pressed), std::end(g_input.keys_pressed), false);
    g_input.mouse_pressed = false;
}

void draw_text(HDC hdc, int x, int y, const char* text, int size, Color color, bool bold = false) {
    HFONT font = CreateFontA(
        size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL,
        FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN,
        "Consolas"
    );

    HFONT old_font = static_cast<HFONT>(SelectObject(hdc, font));
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(color.r, color.g, color.b));
    TextOutA(hdc, x, y, text, static_cast<int>(std::strlen(text)));
    SelectObject(hdc, old_font);
    DeleteObject(font);
}

void render_game() {
    clear(rgb(8, 7, 10));

    const int grid = 32;
    for (int y = 0; y < g_backbuffer.height; y += grid) {
        for (int x = 0; x < g_backbuffer.width; x += grid) {
            if (((x / grid) + (y / grid)) % 2 == 0) {
                rect_i(x, y, grid - 3, grid - 3, rgb(20, 15, 21));
            }
        }
    }

    rect_i(84, 84, g_game.client_w - 168, g_game.client_h - 168, rgb(217, 204, 178));

    for (int i = 0; i < 90; ++i) {
        const int x = 112 + ((i * 73) % std::max(1, g_game.client_w - 240));
        const int y = 112 + ((i * 43) % std::max(1, g_game.client_h - 240));
        rect_i(x, y, 7, 7, rgb(198, 187, 164));
    }

    for (const Rect& wall : g_game.walls) {
        rect(wall, rgb(23, 17, 22));
        rect({wall.x + 4, wall.y + 4, std::max(0.0f, wall.w - 8), std::max(0.0f, wall.h - 8)}, rgb(39, 29, 36));
    }

    if (g_game.portal.active) {
        const float pulse = std::sin(g_game.portal.pulse) * 6.0f;
        circle(g_game.portal.p, g_game.portal.r + pulse, rgb(11, 240, 189));
        circle(g_game.portal.p, g_game.portal.r - 12.0f, rgb(16, 24, 28));
    }

    for (const Coin& c : g_game.coins) {
        circle(c.p, 8.0f, rgb(255, 212, 97));
    }

    for (const Bullet& b : g_game.bullets) {
        circle(b.p, b.r, b.enemy ? rgb(255, 115, 146) : rgb(255, 231, 139));
    }

    for (const Enemy& e : g_game.enemies) {
        const Color color = e.hurt > 0.0f
            ? rgb(255, 208, 164)
            : e.type == 1 ? rgb(142, 49, 86)
            : e.type == 2 ? rgb(217, 93, 114)
            : rgb(198, 61, 88);

        circle(e.p, e.r, color);
        rect_i(static_cast<int>(e.p.x - 18), static_cast<int>(e.p.y - e.r - 13), 36, 5, rgb(40, 20, 28));
        rect_i(static_cast<int>(e.p.x - 18), static_cast<int>(e.p.y - e.r - 13), static_cast<int>(36.0f * (e.hp / e.max_hp)), 5, rgb(255, 101, 141));
    }

    Player& p = g_game.player;
    if (!(p.invulnerable > 0.0f && (static_cast<int>(g_game.time * 20.0f) % 2 == 0))) {
        circle(p.p, p.r + 4.0f, rgb(214, 193, 175));
        circle(p.p, p.r, rgb(244, 233, 216));
        circle({p.p.x - 5.0f, p.p.y - 4.0f}, 2.5f, rgb(33, 23, 34));
        circle({p.p.x + 6.0f, p.p.y - 4.0f}, 2.5f, rgb(33, 23, 34));
    }

    for (const Particle& part : g_game.particles) {
        circle(part.p, part.r, part.color);
    }
}

void paint_to_window(HWND window) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(window, &ps);

    StretchDIBits(
        dc,
        0, 0, g_backbuffer.width, g_backbuffer.height,
        0, 0, g_backbuffer.width, g_backbuffer.height,
        g_backbuffer.memory,
        &g_backbuffer.info,
        DIB_RGB_COLORS,
        SRCCOPY
    );

    rect_i(0, 0, 0, 0, rgb(0, 0, 0)); // keeps compiler quiet if optimizers get spicy

    char line[256];

    draw_text(dc, 22, 18, kWindowTitle, 24, rgb(244, 233, 216), true);

    std::snprintf(line, sizeof(line), "Floor %d / Room %d", g_game.floor, g_game.room);
    draw_text(dc, 22, 50, line, 20, rgb(255, 231, 139), true);

    std::string hearts;
    for (int i = 0; i < g_game.player.max_hp; ++i) hearts += i < g_game.player.hp ? "<3 " : "-- ";
    draw_text(dc, 22, 78, hearts.c_str(), 19, rgb(255, 101, 141), true);

    std::snprintf(line, sizeof(line), "coins %d   kills %d", g_game.player.coins, g_game.player.kills);
    draw_text(dc, 22, 108, line, 18, rgb(185, 247, 255), true);

    draw_text(dc, 22, 134, "WASD/arrows move | mouse aim | click/space shoot | R restart", 16, rgb(244, 233, 216));

    if (g_game.message_time > 0.0f && !g_game.message.empty()) {
        draw_text(dc, 22, g_game.client_h - 54, g_game.message.c_str(), 19, rgb(255, 231, 139), true);
    }

    EndPaint(window, &ps);
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    switch (message) {
        case WM_CLOSE:
        case WM_DESTROY:
            g_running = false;
            PostQuitMessage(0);
            return 0;

        case WM_SIZE: {
            RECT rect{};
            GetClientRect(window, &rect);
            const int width = rect.right - rect.left;
            const int height = rect.bottom - rect.top;

            resize_backbuffer(g_backbuffer, width, height);
            g_game.client_w = width;
            g_game.client_h = height;

            if (g_game.player.hp <= 0 && !g_game.player.dead) {
                restart_game();
            }
            return 0;
        }

        case WM_MOUSEMOVE:
            g_input.mouse.x = static_cast<float>(GET_X_LPARAM(l_param));
            g_input.mouse.y = static_cast<float>(GET_Y_LPARAM(l_param));
            return 0;

        case WM_LBUTTONDOWN:
            g_input.mouse_down = true;
            g_input.mouse_pressed = true;
            SetCapture(window);
            return 0;

        case WM_LBUTTONUP:
            g_input.mouse_down = false;
            ReleaseCapture();
            return 0;

        case WM_KEYDOWN: {
            const int key = static_cast<int>(w_param);
            if (key >= 0 && key < 256) {
                if (!g_input.keys[key]) {
                    g_input.keys_pressed[key] = true;
                }
                g_input.keys[key] = true;
            }
            return 0;
        }

        case WM_KEYUP: {
            const int key = static_cast<int>(w_param);
            if (key >= 0 && key < 256) {
                g_input.keys[key] = false;
            }
            return 0;
        }

        case WM_PAINT:
            paint_to_window(window);
            return 0;
    }

    return DefWindowProcA(window, message, w_param, l_param);
}

double seconds_now() {
    static LARGE_INTEGER frequency{};
    static bool initialized = false;

    if (!initialized) {
        QueryPerformanceFrequency(&frequency);
        initialized = true;
    }

    LARGE_INTEGER counter{};
    QueryPerformanceCounter(&counter);
    return static_cast<double>(counter.QuadPart) / static_cast<double>(frequency.QuadPart);
}

} // namespace

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show_command) {
    timeBeginPeriod(1);
    std::srand(static_cast<unsigned>(GetTickCount()));

    WNDCLASSA wc{};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wc.lpszClassName = "GeneratedCppGameWindowClass";

    if (!RegisterClassA(&wc)) {
        MessageBoxA(nullptr, "RegisterClassA failed.", kWindowTitle, MB_ICONERROR);
        return 1;
    }

    RECT desired{0, 0, 1280, 720};
    AdjustWindowRect(&desired, WS_OVERLAPPEDWINDOW, FALSE);

    HWND window = CreateWindowExA(
        0,
        wc.lpszClassName,
        kWindowTitle,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        desired.right - desired.left,
        desired.bottom - desired.top,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!window) {
        MessageBoxA(nullptr, "CreateWindowExA failed.", kWindowTitle, MB_ICONERROR);
        return 1;
    }

    ShowWindow(window, show_command);
    UpdateWindow(window);

    RECT client{};
    GetClientRect(window, &client);
    resize_backbuffer(g_backbuffer, client.right - client.left, client.bottom - client.top);
    g_game.client_w = g_backbuffer.width;
    g_game.client_h = g_backbuffer.height;

    restart_game();

    double last = seconds_now();

    while (g_running) {
        MSG message{};
        while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                g_running = false;
            }
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }

        const double now = seconds_now();
        float dt = static_cast<float>(now - last);
        last = now;
        dt = clampf(dt, 0.0f, 1.0f / 20.0f);

        update_game(dt);
        render_game();

        HDC dc = GetDC(window);
        StretchDIBits(
            dc,
            0, 0, g_backbuffer.width, g_backbuffer.height,
            0, 0, g_backbuffer.width, g_backbuffer.height,
            g_backbuffer.memory,
            &g_backbuffer.info,
            DIB_RGB_COLORS,
            SRCCOPY
        );
        ReleaseDC(window, dc);

        InvalidateRect(window, nullptr, FALSE);
        Sleep(1);
    }

    timeEndPeriod(1);
    return 0;
}

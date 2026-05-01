
#include "raylib.h"
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

struct Bullet { Vector2 p, v; float r=5, life=1; bool enemy=false; };
struct Enemy { Vector2 p; float r=16, hp=5, cd=1; int type=0; };
struct Coin { Vector2 p, v; float life=10; };
struct Wall { Rectangle r; };

static const int W=1280, H=720;
static std::vector<Wall> walls;
static std::vector<Enemy> enemies;
static std::vector<Bullet> bullets;
static std::vector<Coin> coins;
static Vector2 player{W/2.0f,H/2.0f};
static int hp=6, maxHp=6, floorNo=1, roomNo=1, money=0, kills=0;
static float fireCd=0, inv=0, msgT=0, portalPulse=0;
static bool dead=false, portal=false;
static const char* msg="Clear the room. Teal portal opens after the last enemy.";

float len(Vector2 v){ return sqrtf(v.x*v.x+v.y*v.y); }
Vector2 add(Vector2 a,Vector2 b){ return {a.x+b.x,a.y+b.y}; }
Vector2 sub(Vector2 a,Vector2 b){ return {a.x-b.x,a.y-b.y}; }
Vector2 mul(Vector2 a,float s){ return {a.x*s,a.y*s}; }
Vector2 norm(Vector2 v){ float l=len(v); return l>0.001f?Vector2{v.x/l,v.y/l}:Vector2{0,0}; }
float dist(Vector2 a,Vector2 b){ return len(sub(a,b)); }
float rf(float a,float b){ return a+(b-a)*(GetRandomValue(0,10000)/10000.0f); }

bool circleRect(Vector2 p,float r,Rectangle q){
    float nx=fmaxf(q.x,fminf(q.x+q.width,p.x)), ny=fmaxf(q.y,fminf(q.y+q.height,p.y));
    float dx=p.x-nx, dy=p.y-ny; return dx*dx+dy*dy<=r*r;
}
bool solid(Vector2 p,float r){ for(auto&w:walls) if(circleRect(p,r,w.r)) return true; return false; }
void moveCircle(Vector2& p,Vector2 d,float r){ if(!solid({p.x+d.x,p.y},r)) p.x+=d.x; if(!solid({p.x,p.y+d.y},r)) p.y+=d.y; }

void spawnEnemy(){
    Enemy e; e.type=GetRandomValue(0,2); e.r=e.type==1?22:16; e.hp=e.type==1?10+floorNo*2:5+floorNo; e.cd=rf(.4f,1.5f);
    for(int i=0;i<200;i++){ e.p={rf(140,W-140),rf(140,H-140)}; if(dist(e.p,player)>180&&!solid(e.p,e.r)) break; }
    enemies.push_back(e);
}
void newRoom(){
    walls.clear(); enemies.clear(); bullets.clear(); coins.clear(); portal=false; dead=false; inv=1; player={W/2.0f,H/2.0f};
    const float m=84; walls.push_back({{m,m,W-m*2,24}}); walls.push_back({{m,H-m,W-m*2,24}});
    walls.push_back({{m,m,24,H-m*2}}); walls.push_back({{W-m,m,24,H-m*2}});
    for(int i=0;i<std::min(8,2+floorNo+roomNo/2);i++) walls.push_back({{rf(m+100,W-m-220),rf(m+90,H-m-170),rf(70,165),rf(48,118)}});
    for(int i=0;i<std::min(15,3+floorNo+roomNo);i++) spawnEnemy();
    msg="Clear the room. Teal portal opens after the last enemy."; msgT=3;
}
void restart(){ hp=maxHp=6; floorNo=roomNo=1; money=kills=0; newRoom(); }
void nextRoom(){ if(++roomNo>4){ roomNo=1; floorNo++; maxHp++; hp=maxHp; } newRoom(); }

void shootPlayer(){
    if(dead||fireCd>0) return; fireCd=.14f;
    Vector2 a=norm(sub(GetMousePosition(),player));
    bullets.push_back({add(player,mul(a,22)),mul(a,690),5,.9f,false});
}
void damagePlayer(int n,Vector2 src){
    if(dead||inv>0) return; hp-=n; inv=.75f; moveCircle(player,mul(norm(sub(player,src)),22),14);
    if(hp<=0){ hp=0; dead=true; msg="You died. Press R."; msgT=9999; }
}
void damageEnemy(Enemy& e,float n,Vector2 src){
    e.hp-=n; moveCircle(e.p,mul(norm(sub(e.p,src)),10),e.r);
    if(e.hp<=0){ kills++; money++; coins.push_back({e.p,{rf(-80,80),rf(-80,80)},10}); }
}
void update(float dt){
    if(IsKeyPressed(KEY_R)){ restart(); return; }
    if(inv>0) inv-=dt; if(fireCd>0) fireCd-=dt; if(msgT>0) msgT-=dt;
    if(!dead){
        Vector2 m{}; if(IsKeyDown(KEY_W)||IsKeyDown(KEY_UP))m.y--; if(IsKeyDown(KEY_S)||IsKeyDown(KEY_DOWN))m.y++;
        if(IsKeyDown(KEY_A)||IsKeyDown(KEY_LEFT))m.x--; if(IsKeyDown(KEY_D)||IsKeyDown(KEY_RIGHT))m.x++;
        moveCircle(player,mul(norm(m),245*dt),14);
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)||IsKeyDown(KEY_SPACE)) shootPlayer();
    }
    for(int i=(int)bullets.size()-1;i>=0;i--){
        auto&b=bullets[i]; b.life-=dt; b.p=add(b.p,mul(b.v,dt)); bool rem=b.life<=0||solid(b.p,b.r);
        if(!rem&&b.enemy&&dist(b.p,player)<b.r+14){ damagePlayer(1,b.p); rem=true; }
        if(!rem&&!b.enemy) for(auto&e:enemies) if(e.hp>0&&dist(b.p,e.p)<b.r+e.r){ damageEnemy(e,3,b.p); rem=true; break; }
        if(rem) bullets.erase(bullets.begin()+i);
    }
    for(auto&e:enemies) if(e.hp>0){
        Vector2 to=norm(sub(player,e.p)); float d=dist(e.p,player); e.cd-=dt;
        if(e.type==2){ moveCircle(e.p,mul(to,(d<270?-70:42)*dt),e.r); if(e.cd<=0&&!dead){ e.cd=rf(.8f,1.5f); bullets.push_back({e.p,mul(to,280),6,1.8f,true}); } }
        else moveCircle(e.p,mul(to,(e.type==1?75:128)*dt),e.r);
        if(!dead&&d<e.r+14) damagePlayer(e.type==1?2:1,e.p);
    }
    enemies.erase(std::remove_if(enemies.begin(),enemies.end(),[](Enemy&e){return e.hp<=0;}),enemies.end());
    for(int i=(int)coins.size()-1;i>=0;i--){ auto&c=coins[i]; c.life-=dt; float d=dist(c.p,player); if(d<190)c.v=add(c.v,mul(norm(sub(player,c.p)),780*dt)); c.p=add(c.p,mul(c.v,dt)); c.v=mul(c.v,.92f); if(d<26){money++; coins.erase(coins.begin()+i);} else if(c.life<=0) coins.erase(coins.begin()+i); }
    if(!portal&&enemies.empty()){ portal=true; portalPulse=0; msg="Room clear. Enter the teal portal."; msgT=3; }
    if(portal){ portalPulse+=dt*5; Vector2 p{W-140,H/2.0f}; if(!dead&&dist(player,p)<50) nextRoom(); }
}
void draw(){
    BeginDrawing(); ClearBackground({8,7,10,255});
    for(int y=0;y<H;y+=32)for(int x=0;x<W;x+=32) if(((x/32+y/32)&1)==0) DrawRectangle(x,y,29,29,{20,15,21,255});
    DrawRectangle(84,84,W-168,H-168,{217,204,178,255});
    for(auto&w:walls){ DrawRectangleRec(w.r,{23,17,22,255}); DrawRectangle(w.r.x+4,w.r.y+4,w.r.width-8,w.r.height-8,{39,29,36,255}); }
    if(portal){ Vector2 p{W-140,H/2.0f}; DrawCircleV(p,34+sinf(portalPulse)*6,{11,240,189,180}); DrawCircleV(p,22,{16,24,28,255}); }
    for(auto&c:coins) DrawCircleV(c.p,8,GOLD);
    for(auto&b:bullets) DrawCircleV(b.p,b.r,b.enemy?RED:GOLD);
    for(auto&e:enemies){ Color col=e.type==1?Color{142,49,86,255}:e.type==2?Color{217,93,114,255}:Color{198,61,88,255}; DrawCircleV(e.p,e.r,col); DrawRectangle(e.p.x-18,e.p.y-e.r-13,36,5,{40,20,28,255}); DrawRectangle(e.p.x-18,e.p.y-e.r-13,36*(e.hp/(e.type==1?10+floorNo*2:5+floorNo)),5,RED); }
    if(!(inv>0&&((int)(GetTime()*20)%2==0))){ DrawCircleV(player,18,{214,193,175,255}); DrawCircleV(player,14,{244,233,216,255}); DrawLineEx(player,add(player,mul(norm(sub(GetMousePosition(),player)),26)),4,GOLD); }
    DrawRectangle(14,14,620,146,Fade({11,8,14,255},.78f));
    DrawText("Flesh and Stone 2 - Raylib",28,28,24,RAYWHITE);
    DrawText(TextFormat("Floor %d / Room %d",floorNo,roomNo),28,62,22,GOLD);
    std::string hearts; for(int i=0;i<maxHp;i++) hearts += i<hp ? "<3 " : "-- "; DrawText(hearts.c_str(),28,92,22,RED);
    DrawText(TextFormat("coins %d   kills %d",money,kills),28,122,20,SKYBLUE);
    DrawText("WASD/arrows move | mouse aim | click/space shoot | R restart",28,H-36,18,RAYWHITE);
    if(msgT>0) DrawText(msg,34,H-78,20,GOLD);
    EndDrawing();
}
}
int main(){
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(W,H,"Flesh and Stone 2");
    SetTargetFPS(60);
    restart();
    while(!WindowShouldClose()){ update(GetFrameTime()); draw(); }
    CloseWindow();
    return 0;
}

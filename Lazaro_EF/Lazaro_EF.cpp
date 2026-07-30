#include <SDL3/SDL.h>

#include <SDL3/SDL_main.h>

#include <SDL3_image/SDL_image.h>

#include <SDL3_mixer/SDL_mixer.h>

#include <unordered_map>

#include <string>

#include <cstdint>



constexpr int TILE_SRC = 16;

constexpr int TILE_DST = 32;

constexpr int MAP_COLS = 25;

constexpr int MAP_ROWS = 18;

constexpr int WIN_W = 800;

constexpr int WIN_H = 600;

constexpr float GRAVITY = 1000.0f;

constexpr float MOVE_SPEED = 200.0f;

constexpr float JUMP_VEL = -450.0f;

constexpr int START_X = 32;

constexpr int START_Y = 32 * 2;



//------------- Estructura Mapas 0 = aire, 1 = solido, 2 = salida

static const char* g_map[MAP_ROWS] = {

  "0000000000000000000000000",

  "0000000000000000000000000",

  "0000000000000000000000000",

  "0000000000000000000000000",

  "0000000000000000000000000",

  "0000000000000000000222000",

  "0000000000000000000111000",

  "0000000000000000000000000",

  "0000000000000000011000000",

  "0000000000000000000000000",

  "0000000000000011000000000",

  "0000000000000000000000000",

  "0000000000110000000000000",

  "0000000000000000000000000",

  "0000001100000000000000000",

  "0000000000000000000000000",

  "1111111111111111111111111",

  "1111111111111111111111111"

};



enum PlayerState { IDLE, RUN, JUMP, FALL, HIT };



// ------------------Animaciones

struct Anim {

    SDL_Texture* tex = nullptr;

    int numFrames = 1;

};



struct Player {

    float x = START_X, y = START_Y;

    float vx = 0, vy = 0;

    bool onGround = false;

    bool facingRight = true;

    PlayerState state = IDLE;

    float animTimer = 0;

    int animFrame = 0;

};



bool isSolid(int col, int row) {

    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) return false;

    return g_map[row][col] == '1';

}



bool isGoal(int col, int row) {

    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) return false;

    return g_map[row][col] == '2';

}



bool rectHitsSolid(float x, float y, float w, float h) {

    int left = (int)(x) / TILE_DST;

    int right = (int)((x + w - 0.01f)) / TILE_DST;

    int top = (int)(y) / TILE_DST;

    int bottom = (int)((y + h - 0.01f)) / TILE_DST;

    for (int r = top; r <= bottom; r++)

        for (int c = left; c <= right; c++)

            if (isSolid(c, r)) return true;

    return false;

}



bool rectHitsGoal(float x, float y, float w, float h) {

    int left = (int)(x) / TILE_DST;

    int right = (int)((x + w - 0.01f)) / TILE_DST;

    int top = (int)(y) / TILE_DST;

    int bottom = (int)((y + h - 0.01f)) / TILE_DST;

    for (int r = top; r <= bottom; r++)

        for (int c = left; c <= right; c++)

            if (isGoal(c, r)) return true;

    return false;

}

//------------------------- Cargar animaciones

Anim LoadAnim(SDL_Renderer* renderer, const char* path) {

    Anim a;

    a.tex = IMG_LoadTexture(renderer, path);

    if (a.tex) {

        SDL_SetTextureScaleMode(a.tex, SDL_SCALEMODE_NEAREST);

        float tw, th;

        SDL_GetTextureSize(a.tex, &tw, &th);

        a.numFrames = (int)(tw / 32);

        if (a.numFrames < 1) a.numFrames = 1;

    }

    return a;

}

// Escribe tu código aquí arriba de main()



int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_Window* window = SDL_CreateWindow("Examen Final", 800, 600, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);



    //----------------- SONIDO



    SDL_Texture* bg = IMG_LoadTexture(renderer, "assets/Background/Blue.png");

    if (bg) SDL_SetTextureScaleMode(bg, SDL_SCALEMODE_NEAREST);

    SDL_Texture* terrain = IMG_LoadTexture(renderer, "assets/Terrain/Terrain (16x16).png");

    if (terrain) SDL_SetTextureScaleMode(terrain, SDL_SCALEMODE_NEAREST);

    std::unordered_map<int, Anim> anims;

    anims[IDLE] = LoadAnim(renderer, "assets/Character/Idle (32x32).png");

    anims[RUN] = LoadAnim(renderer, "assets/Character/Run (32x32).png");

    anims[JUMP] = LoadAnim(renderer, "assets/Character/Jump (32x32).png");

    anims[FALL] = LoadAnim(renderer, "assets/Character/Fall (32x32).png");

    anims[HIT] = LoadAnim(renderer, "assets/Character/Hit (32x32).png");



    MIX_Init();

    MIX_Mixer* mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);



    MIX_Audio* jumpSfx = MIX_LoadAudio(mixer, "assets/sounds/jump.wav", true);

    MIX_Audio* dieSfx = MIX_LoadAudio(mixer, "assets/sounds/die.wav", true);

    MIX_Audio* music = MIX_LoadAudio(mixer, "assets/sounds/music.mp3", false);



    MIX_Track* sfxTrack = MIX_CreateTrack(mixer);

    MIX_Track* musicTrack = MIX_CreateTrack(mixer);



    MIX_SetTrackAudio(musicTrack, music);

    MIX_PlayTrack(musicTrack, -1);





    Player player;

    int lives = 3;

    bool won = false;

    bool lost = false;

    float elapsedTime = 0;



    const bool* keys = SDL_GetKeyboardState(nullptr);



    Uint64 lastTick = SDL_GetTicks();

    bool running = true;

    SDL_Event ev;





    while (running) {

        Uint64 now = SDL_GetTicks();

        float dt = (now - lastTick) / 1000.0f;

        lastTick = now;

        if (dt > 0.05f) dt = 0.05f;



        while (SDL_PollEvent(&ev)) {

            if (ev.type == SDL_EVENT_QUIT) running = false;

            if (ev.type == SDL_EVENT_KEY_DOWN) {

                if (ev.key.scancode == SDL_SCANCODE_ESCAPE) running = false;

                if (ev.key.scancode == SDL_SCANCODE_SPACE || ev.key.scancode == SDL_SCANCODE_W || ev.key.scancode == SDL_SCANCODE_UP) {

                    if (!won && !lost && player.onGround) {

                        player.vy = JUMP_VEL;

                        player.onGround = false;

                        MIX_SetTrackAudio(sfxTrack, jumpSfx);

                        MIX_PlayTrack(sfxTrack, 0);

                    }

                }

            }

            // Agrega tus eventos aquí

        }

        if (!won && !lost) {

            elapsedTime += dt;



            player.vx = 0;

            if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) {

                player.vx = -MOVE_SPEED;

                player.facingRight = false;

            }

            if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) {

                player.vx = MOVE_SPEED;

                player.facingRight = true;

            }



            player.vy += GRAVITY * dt;



            // Mover en X, corrigiendo colisión

            float newX = player.x + player.vx * dt;

            if (!rectHitsSolid(newX, player.y, TILE_DST, TILE_DST)) {

                player.x = newX;

            }



            // Mover en Y, corrigiendo colisión

            float newY = player.y + player.vy * dt;



            if (player.vy >= 0) {

                // cayendo o en reposo: revisar el centro de los pies

                int row = (int)((newY + TILE_DST) / TILE_DST);

                int centerCol = (int)((player.x + TILE_DST / 2.0f) / TILE_DST);

                bool hitGround = isSolid(centerCol, row);

                if (hitGround) {

                    player.y = (float)(row * TILE_DST - TILE_DST);

                    player.vy = 0;

                    player.onGround = true;

                }

                else {

                    player.y = newY;

                    player.onGround = false;

                }

            }

            else {

                // subiendo: revisar techo

                if (rectHitsSolid(player.x, newY, TILE_DST, TILE_DST)) {

                    int row = (int)(newY / TILE_DST) + 1;

                    player.y = (float)(row * TILE_DST);

                    player.vy = 0;

                }

                else {

                    player.y = newY;

                }

                player.onGround = false;

            }



            // Ganar

            if (rectHitsGoal(player.x, player.y, TILE_DST, TILE_DST)) {

                won = true;

            }



            // Caer fuera del mapa

            if (player.y > WIN_H) {

                lives--;

                MIX_SetTrackAudio(sfxTrack, dieSfx);

                MIX_PlayTrack(sfxTrack, 0);

                if (lives <= 0) {

                    lost = true;

                }

                else {

                    player.x = START_X;

                    player.y = START_Y;

                    player.vx = 0;

                    player.vy = 0;

                    player.onGround = false;

                }

            }



            // Estado de animación

            if (player.onGround) {

                player.state = (player.vx != 0) ? RUN : IDLE;

            }

            else {

                player.state = (player.vy < 0) ? JUMP : FALL;

            }



            // Animación (frame timer)

            Anim& curAnim = anims[player.state];

            player.animTimer += dt;

            if (player.animTimer >= 0.1f) {

                player.animTimer = 0;

                player.animFrame = (player.animFrame + 1) % curAnim.numFrames;

            }

        }



        SDL_SetRenderDrawColor(renderer, 15, 20, 30, 255);

        SDL_RenderClear(renderer);



        // Agrega tu render aquí



        // Fondo repetido (baldosa de 64x64)

        if (bg) {

            for (int y = 0; y < WIN_H; y += 64) {

                for (int x = 0; x < WIN_W; x += 64) {

                    SDL_FRect dst{ (float)x, (float)y, 64.0f, 64.0f };

                    SDL_RenderTexture(renderer, bg, nullptr, &dst);

                }

            }

        }



        // Mapa

        for (int r = 0; r < MAP_ROWS; r++) {

            for (int c = 0; c < MAP_COLS; c++) {

                char t = g_map[r][c];

                if (t == '1' && terrain) {

                    SDL_FRect src{ 0, 0, (float)TILE_SRC, (float)TILE_SRC };

                    SDL_FRect dst{ (float)(c * TILE_DST), (float)(r * TILE_DST), (float)TILE_DST, (float)TILE_DST };

                    SDL_RenderTexture(renderer, terrain, &src, &dst);

                }

                else if (t == '2') {

                    SDL_SetRenderDrawColor(renderer, 60, 220, 90, 255);

                    SDL_FRect dst{ (float)(c * TILE_DST), (float)(r * TILE_DST), (float)TILE_DST, (float)TILE_DST };

                    SDL_RenderFillRect(renderer, &dst);

                    SDL_SetRenderDrawColor(renderer, 15, 20, 30, 255);

                }

            }

        }



        // Personaje

        Anim& drawAnim = anims[player.state];

        if (drawAnim.tex) {

            SDL_FRect src{ (float)(player.animFrame * 32), 0, 32, 32 };

            SDL_FRect dst{ player.x, player.y, (float)TILE_DST, (float)TILE_DST };

            SDL_FlipMode flip = player.facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

            SDL_RenderTextureRotated(renderer, drawAnim.tex, &src, &dst, 0, nullptr, flip);

        }



        // HUD

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_RenderDebugTextFormat(renderer, 10, 10, "Vidas: %d", lives);

        SDL_RenderDebugTextFormat(renderer, 10, 30, "Tiempo: %.1f s", elapsedTime);



        if (won || lost) {

            float scale = 4.0f;

            SDL_SetRenderScale(renderer, scale, scale);

            if (won) {

                SDL_SetRenderDrawColor(renderer, 60, 220, 90, 255);

                SDL_RenderDebugTextFormat(renderer, (WIN_W / scale) / 2 - 60, 40, "GANASTE");

            }

            if (lost) {

                SDL_SetRenderDrawColor(renderer, 220, 60, 60, 255);

                SDL_RenderDebugTextFormat(renderer, (WIN_W / scale) / 2 - 70, 40, "PERDISTE");

            }

            SDL_SetRenderScale(renderer, 1.0f, 1.0f);

        }



        SDL_RenderPresent(renderer);

    }



    // Agrega tu limpieza aquí



    MIX_DestroyTrack(musicTrack);

    MIX_DestroyTrack(sfxTrack);

    MIX_DestroyAudio(music);

    MIX_DestroyAudio(dieSfx);

    MIX_DestroyAudio(jumpSfx);

    MIX_DestroyMixer(mixer);

    MIX_Quit();



    for (auto& p : anims) {

        if (p.second.tex) SDL_DestroyTexture(p.second.tex);

    }

    if (bg) SDL_DestroyTexture(bg);

    if (terrain) SDL_DestroyTexture(terrain);



    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;



}
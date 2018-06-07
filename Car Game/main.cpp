//
//  main.cpp
//  Game
//
//  Created by Pepe Becker on 3/8/15.
//  Copyright (c) 2015 Pepe Apps. All rights reserved.
//

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_mixer/SDL_mixer.h>
#include <SDL2_ttf/SDL_ttf.h>

#include <iostream>
#include <vector>
#include <math.h>
#include <string>

#include "Car.h"

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 512;
const int FPS = 64;
const int countDownTime = 3;
int total_frames = 0;

SDL_Window *window;
SDL_Renderer *renderer;

bool running = true;
bool spawnEnemies = false;
bool _pause = true;
bool gameOver = false;
int mode = 0;

int score = 0;

std::vector<SDL_Texture*> menuFrames;
int menuState = 1;

SDL_Texture *road;
SDL_Rect roadRect;
Car *player;
std::vector<Car*> enemies;
SDL_Texture *explosion;
SDL_Rect explosionFrame;
SDL_Rect explosionRect;
Mix_Chunk *explosionSound;

void LoadResources();
void Start();
void Update();
void Render();

int main(int argc, const char * argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL failed to initialize!\n");
        return EXIT_FAILURE;
    }
    
    if (IMG_Init(IMG_INIT_PNG) < 0) {
        printf("SDL_image failed to initialize!\n");
        return EXIT_FAILURE;
    }
    
    if (Mix_Init(MIX_INIT_MP3) < 0) {
        printf("SDL_mixer failed to initialize!\n");
        return EXIT_FAILURE;
    }
    
    if (TTF_Init() < 0) {
        printf("SDL_ttf failed to initialize!\n");
        return EXIT_FAILURE;
    }
    
    window = SDL_CreateWindow("Car Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
    
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
    
    LoadResources();
    
    Start();
    
    Uint32 start;
    
    while (running) {
        start = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (menuState == 0) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        _pause = true;
                        menuState = 6;
                    }
                    if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                        player->SwitchLane();
                    }
                } else if (menuState > 0) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (menuState == 3 || menuState == 4 || menuState == 5) {
                            menuState = 1;
                        }
                        if (menuState == 6 || menuState == 7) {
                            menuState = 0;
                            _pause = false;
                        }
                    }
                    if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                        switch (menuState) {
                            case 2:
                                menuState = 1;
                                break;
                            case 4:
                                menuState = 3;
                                break;
                            case 5:
                                menuState = 4;
                                break;
                            case 7:
                                menuState = 6;
                                break;
                            case 9:
                                menuState = 8;
                                break;
                                
                            default:
                                break;
                        }
                    }
                    if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                        switch (menuState) {
                            case 1:
                                menuState = 2;
                                break;
                            case 3:
                                menuState = 4;
                                break;
                            case 4:
                                menuState = 5;
                                break;
                            case 6:
                                menuState = 7;
                                break;
                            case 8:
                                menuState = 9;
                                break;
                                
                            default:
                                break;
                        }
                    }
                    if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                        switch (menuState) {
                            case 1:
                                menuState = 4;
                                break;
                            case 2:
                                running = false;
                                break;
                            case 3:
                                menuState = 0;
                                _pause = false;
                                mode = 0;
                                break;
                            case 4:
                                menuState = 0;
                                _pause = false;
                                mode = 1;
                                break;
                            case 5:
                                menuState = 0;
                                _pause = false;
                                mode = 2;
                                break;
                            case 6:
                                menuState = 0;
                                _pause = false;
                                break;
                            case 7:
                                menuState = 1;
                                Start();
                                break;
                            case 8:
                                menuState = 0;
                                Start();
                                _pause = false;
                                break;
                            case 9:
                                menuState = 1;
                                Start();
                                break;
                                
                            default:
                                break;
                        }
                    }
                }
            }
        }
        
        if (total_frames / FPS == countDownTime) {
            spawnEnemies = true;
        }
        
        if (!_pause) {
            Update();
        }
        
        Render();
        
        total_frames ++;
        
        if (1000/FPS > SDL_GetTicks() - start) {
            SDL_Delay(1000/FPS-(SDL_GetTicks() - start));
        }
    }
    
    enemies.clear();
    Mix_FreeChunk(explosionSound);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
    
    return EXIT_SUCCESS;
}

void LoadResources() {
    SDL_Surface *surface;
    surface = IMG_Load("Play.png");
    menuFrames.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    surface = IMG_Load("Quit.png");
    menuFrames.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    surface = IMG_Load("Easy.png");
    menuFrames.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    surface = IMG_Load("Medium.png");
    menuFrames.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    surface = IMG_Load("Hard.png");
    menuFrames.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    surface = IMG_Load("Resume.png");
    menuFrames.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    surface = IMG_Load("End.png");
    menuFrames.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    surface = IMG_Load("Restart.png");
    menuFrames.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    surface = IMG_Load("End2.png");
    menuFrames.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    
    surface = IMG_Load("Road.png");
    road = SDL_CreateTextureFromSurface(renderer, surface);
    
    surface = IMG_Load("Explosion.png");
    explosion = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    explosionSound = Mix_LoadWAV("Explosion.wav");
}

void Start() {
    total_frames = 0;
    score = 0;
    gameOver = false;
    spawnEnemies = false;
    
    roadRect.x = 256;
    roadRect.y = -64;
    roadRect.w = 128;
    roadRect.h = 576;
    
    explosionFrame.x = 0;
    explosionFrame.y = 0;
    explosionFrame.w = 64;
    explosionFrame.h = 64;
    
    explosionRect.x = 0;
    explosionRect.y = 0;
    explosionRect.w = 128;
    explosionRect.h = 128;
    
    player = new Car(320, 384, renderer, CarColorRed);
    
    enemies.clear();
}

void SpawnEnemy() {
    int color = rand() % 2;
    int lane = rand() % 2;
    Car *car;
    
    if (color == 0)
        car = new Car(256 + (64 * lane), -128, renderer, CarColorBlue);
    else
        car = new Car(256 + (64 * lane), -128, renderer, CarColorGreen);
    
    car->SetDirection(-1);
    car->SetSpeed(8);
    enemies.push_back(car);
}

void Update() {
    if (!gameOver) {
        roadRect.y += 4;
        if (roadRect.y >= 0) {
            roadRect.y = -64;
        }
        
        player->Update();
        
        for (Car *enemy : enemies) {
            enemy->Update();
            
            if (abs(enemy->rect.x - player->rect.x) < 18) {
                if (enemy->rect.y < player->rect.y + 64 && enemy->rect.y + 64 > player->rect.y) {
                    gameOver = true;
                    explosionRect.x = (player->rect.x + enemy->rect.x) / 2 - 32;
                    explosionRect.y = (player->rect.y + enemy->rect.y) / 2 - 32;
                    Mix_PlayChannel(MIX_DEFAULT_CHANNELS, explosionSound, 0);
                }
            }
            
            if (enemy->rect.y > SCREEN_HEIGHT*2) {
                enemies.erase(enemies.begin());
                score ++;
                printf("Score: %d\n", score);
            }
        }
    }
    
    if (gameOver) {
        explosionFrame.x += 64;
        if (explosionFrame.x >= 1472) {
            _pause = true;
            menuState = 8;
        }
    }
    
    int devider = (mode * 2 == 0?1:mode * 2);
    
    if (spawnEnemies && total_frames % (FPS / devider) == 0) {
        SpawnEnemy();
    }
}

void Render() {
    SDL_RenderClear(renderer);
    
    SDL_RenderCopy(renderer, road, NULL, &roadRect);
    
    player->Draw(renderer);
    
    for (Car *enemy : enemies) {
        enemy->Draw(renderer);
    }
    
    if (gameOver) {
        SDL_RenderCopy(renderer, explosion, &explosionFrame, &explosionRect);
    }
    
    if (menuState > 0) {
        SDL_RenderCopy(renderer, menuFrames[menuState-1], NULL, NULL);
    }
    
    SDL_RenderPresent(renderer);
}














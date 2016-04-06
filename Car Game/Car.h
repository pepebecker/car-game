//
//  Car.h
//  Game
//
//  Created by Pepe Becker on 3/8/15.
//  Copyright (c) 2015 Pepe Apps. All rights reserved.
//

#ifndef __Game__Car__
#define __Game__Car__

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_mixer/SDL_mixer.h>

enum CarColor {
    CarColorRed,
    CarColorBlue,
    CarColorGreen,
    CarColorBrown,
    CarColorBlack,
};

class Car {
public:
    SDL_Texture *texture;
    SDL_Rect rect;
    Car(int x, int y, SDL_Renderer *renderer, CarColor color);
    ~Car();
    void Draw(SDL_Renderer *renderer);
    void SetSpeed(float speed);
    void SetDirection(int dir);
    void SwitchLane();
    void Update();
private:
    int rotation = 0;
    int direction = 1;
    int speed = 0;
    int lane = 1;
    int defaultLanePos = 0;
    bool switchingLane = false;
};

#endif /* defined(__Game__Car__) */

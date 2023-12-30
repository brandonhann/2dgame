#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>

class Camera {
public:
    Camera(int x, int y, int width, int height);
    void update(int playerX, int playerY);
    SDL_Rect& getCameraRect();

private:
    SDL_Rect cameraRect;
};

#endif

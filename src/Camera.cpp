#include "Camera.h"

Camera::Camera(int x, int y, int width, int height) {
    cameraRect = {x, y, width, height};
}

void Camera::update(int playerX, int playerY) {
    cameraRect.x = playerX - cameraRect.w / 2;
    cameraRect.y = playerY - cameraRect.h / 2;
}

SDL_Rect& Camera::getCameraRect() {
    return cameraRect;
}

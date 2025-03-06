#pragma once
#include <SDL.h>

class map {
public:
    map();
    void loadMap(int arr[30][50]);  // Đã thêm dấu chấm phẩy ở cuối
    void drawMap();

    // Các hàm getter để lấy dữ liệu của map (dùng cho việc kiểm tra va chạm, v.v.)
    const int (*getMapMatrix() const)[50] { return grid; }
    int getRows() const { return 30; }
    int getCols() const { return 50; }
    int getTileWidth() const { return destRect.w; }
    int getTileHeight() const { return destRect.h; }

private:
    SDL_Texture *dirt;
    SDL_Texture *grassBlock;
    SDL_Texture *grass;
    SDL_Rect srcRect, destRect;
    int grid[30][50];
};

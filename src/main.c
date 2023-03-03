#include <stdio.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <time.h>
#ifdef main
#undef main
#endif
struct MiddleBlock {
    int x1,y1,y2;
};
struct Ball {
    int x, y, r;
    Uint32 color;
};
const int FPS = 60;
const int SCN_WIDTH = 840;
const int SCN_HEIGHT = 720;
const int V = 3;
SDL_Texture *getImageTexture(SDL_Renderer *sdlRenderer, char *image_path);
void drawStartPage(SDL_Renderer *sdlRenderer);
void drawExitPage();
void UpdateBest(int * distance, int * best);
void drawGameOverPage(SDL_Renderer *sdlRenderer);
void firstGenerating(int downBlocks[29], int upBlocks[29]);
void initializeAgain(int *distance, int *passedFrames,int *scene, int *v, struct Ball *ball, int* passedDis, int* middleGenerated, struct MiddleBlock middleBlocks[3], int upBlocks[29], int downBlocks[29], int passedWay[3]);
int randNum(int min, int max);
void drawBack(SDL_Renderer *sdlRenderer,  int * scene, int downBlocks[29], int upBlocks[29], struct MiddleBlock middleBlocks[3],
        int *passedFrames, struct Ball* ball, int passedWay[8], int* best, int *distance, int * v, int* passedDis, int *middleGenerated);
void generatingBlocks(int downBlocks[29], int upBlocks[29], int bool, struct MiddleBlock middleBlocks[3]);
int doseCrashed(int x, int y, int r, int yB1, int yB2, int xB3, int yB3, int xB4, int yB4);
void UpdatePassedWay(int passedWay[8]);
void UpdateDownVUpBlocks(int downBlocks[29], int upBlocks[29]);
void UpdateMiddleBlocks(struct MiddleBlock middleBlocks[3]);
int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    srand(time(NULL));
    SDL_Window *sdlWindow = SDL_CreateWindow("Bounce Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCN_WIDTH, SCN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    SDL_bool shallExit = SDL_FALSE;
    SDL_Event sdlEvent;
    int upBlocks[29];
    int downBlocks[29];
    int PassedWay[8];
    for (int i = 0; i < 8; i++)
        PassedWay[i] = -1;
    struct MiddleBlock middleBlocks[3];
    struct Ball ball;
    ball.y = 360, ball.x = 250, ball.r = 30, ball.color = 0xff821DC4;
    int best = 0, distance = 0, passedFrames = 0, scene = 1, v = V, passedDis = 0, middleGenerated = 0;
    drawStartPage(sdlRenderer);
    firstGenerating(downBlocks, upBlocks);
    while(!shallExit) {
        if (scene == 1) {
            drawBack(sdlRenderer, &scene, downBlocks, upBlocks, middleBlocks, &passedFrames,
                     &ball, PassedWay, &best, &distance, &v, &passedDis, &middleGenerated);
        } else if (scene == 2) {
            drawGameOverPage(sdlRenderer);
        }
        while(SDL_PollEvent(&sdlEvent)) {
            switch (sdlEvent.type) {
                case SDL_QUIT :
                    if (scene == 1) {
                        drawGameOverPage(sdlRenderer);
                        scene = 2;
                    }
                    else if (scene == 2)
                        shallExit = SDL_TRUE;
                    break;
                case SDL_MOUSEBUTTONDOWN :
                    if (scene == 1) {
                        switch (sdlEvent.button.button) {
                            case SDL_BUTTON_LEFT :
                                ball.y -= 10;
                                break;
                            case SDL_BUTTON_RIGHT:
                                ball.y += 10;
                        }
                        break;
                    }
                    else if (scene == 2) {
                        if (sdlEvent.button.x <= 125 && sdlEvent.button.x >= 40 && sdlEvent.button.y >= 680 && sdlEvent.button.y <= 695) {
                            shallExit = SDL_TRUE;
                        }
                        else if (sdlEvent.button.x <= 810 && sdlEvent.button.x >= 710 && sdlEvent.button.y >= 680 && sdlEvent.button.y <= 695) {
                            scene = 1;
                            UpdateBest(&distance, &best);
                            initializeAgain(&distance, &passedFrames, &scene, &v, &ball, &passedDis, &middleGenerated, middleBlocks, upBlocks, downBlocks, PassedWay);
                        }
                        break;
                    }
                case SDL_KEYDOWN:
                    switch (sdlEvent.key.keysym.scancode) {
                        case SDL_SCANCODE_DOWN :
                            ball.y += 10;
                            break;
                        case SDL_SCANCODE_UP:
                            ball.y -= 10;
                    }
            }
        }
    }
    SDL_DestroyWindow(sdlWindow);
    SDL_DestroyRenderer(sdlRenderer);
    drawExitPage();
    printf("THE END");
    return 0;
}

void generatingBlocks(int downBlocks[29], int upBlocks[29], int bool, struct MiddleBlock middleBlock[3]) {
    while (1) {
        int yBU;
        int yBD;
        int yU = randNum(100, 300);
        int yD = randNum(510, 740);
        if (bool) {
            while(1) {
                yBU = randNum(yU, yD);
                yBD = yBU + 100;
                if (yBU - yU >= 80 || yD - yBD >= 80) {
                    middleBlock[2].y1 = yBU;
                    middleBlock[2].y2 = yBD;
                    middleBlock[2].x1 = 840;
                    upBlocks[28] = yU;
                    downBlocks[28] = yD;
                    return;
                }
            }
        }
        else {
            if (yD - yU >= 80) {
                upBlocks[28] = yU;
                downBlocks[28] = yD;
                return;
            }
        }
    }
}

int doseCrashed(int x, int y, int r, int yB1, int yB2, int xB3, int yB3, int xB4, int yB4) {
    if (xB4 > 0) {
        return y + r >= yB1 || y - r <= yB2 || (xB3 - x) * (xB3 - x) + (yB3 - y) * (yB3 - y) <= r * r ||
               (xB4 - x) * (xB4 - x) + (yB4 - y) * (yB4 - y) <= r * r
               || (xB4 - x) * (xB4 - x) + (yB4 + 100 - y) * (yB4 + 100 - y) <= r * r ||
               (xB4 <= x + 30 && xB4 >= x + 35 && y >= yB4 && y <= yB4 + 100);
    }
        return y + r >= yB1 || y - r <= yB2 || (xB3 - x) * (xB3 - x) + (yB3 - y) * (yB3 - y) <= r * r;
}
void drawBack(SDL_Renderer *sdlRenderer, int* scene, int downBlocks[29], int upBlocks[29], struct MiddleBlock middleBlocks[3],
        int *passedFrames, struct Ball *ball, int passedWay[8], int* best, int *distance, int *v, int* passedDis, int* middleGenerated) {
    SDL_SetRenderDrawColor(sdlRenderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(sdlRenderer);
    filledCircleColor(sdlRenderer,ball->x, ball->y, ball->r, ball->color);
    if(*passedDis % 30 == 0) {
        UpdatePassedWay(passedWay);
        passedWay[7] = ball->y;
        int bool = 0;
        if (*passedDis % (900 - 30 * (*v)) == 0) {
            bool = 1;
            UpdateMiddleBlocks(middleBlocks);
            ++ *middleGenerated;
        }
        UpdateDownVUpBlocks(downBlocks, upBlocks);
        generatingBlocks(downBlocks, upBlocks, bool, middleBlocks);
        ++ *distance;
    }
    for (int i = 0; i < 29; i++) {
        boxColor(sdlRenderer, 30 * i - (*passedDis % 30), 0, 30 * i + 30 - (*passedDis % 30),
                 upBlocks[i], 0xffde5b09);
        boxColor(sdlRenderer, 30 * i - (*passedDis % 30), downBlocks[i], 30 * i + 30 - (*passedDis % 30), 720, 0xffde5b09);
    }
    for (int i = 0; i < 8; i++) {
        if (passedWay[i] != -1)
            filledCircleColor(sdlRenderer, 30 * i + 5, passedWay[i], 5, 0xffbdbdbd);
    }
    if (doseCrashed(ball->x, ball->y, ball->r, downBlocks[8], upBlocks[8], downBlocks[9], upBlocks[9], middleBlocks[2].x1, middleBlocks[2].y1)) {
        *scene = 2;
        circleColor(sdlRenderer, ball->x, ball->y, 2 * (ball->r), 0xff1d09d1);
    }
    for(int i = 0; i < 3; i++) {
        if (middleBlocks[i].x1 >= 0) {
            boxColor(sdlRenderer, middleBlocks[i].x1, middleBlocks[i].y1, middleBlocks[i].x1 + 30, middleBlocks[i].y2, 0xffde5b09);
            middleBlocks[i].x1 -= *v;
        }
    }
    boxColor(sdlRenderer, 30, 690, 150, 710, 0xff000000);
    boxColor(sdlRenderer, 710, 690, 790, 710, 0xff000000);
    char strDis[5];
    char strBst[5];
    stringColor(sdlRenderer, 40, 695, "DISTANCE", 0xffffffff);
    stringColor(sdlRenderer, 720, 695, "BEST", 0xffffffff);
    itoa(*best, strBst, 10);
    itoa(*distance, strDis, 10);
    stringColor(sdlRenderer, 120, 695, strDis, 0xffffffff);
    stringColor(sdlRenderer, 760, 695, strBst, 0xffffffff);
    *passedDis += *v;
    if (*passedFrames % 10000 == 0 && *passedFrames != 0) {
        (* v) += 1;
    }
    ++ *passedFrames;
    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(1000/FPS);
    if (*scene == 2)
        SDL_Delay(1000);

}

void UpdateMiddleBlocks(struct MiddleBlock middleBlock[3]) {
    for (int i = 0; i < 2; i++)
        middleBlock[i] = middleBlock[i + 1];
}

void UpdateDownVUpBlocks(int downBlocks[29], int upBlocks[29]) {
    for (int i = 0; i < 28; i++) {
        downBlocks[i] = downBlocks[i + 1];
        upBlocks[i] = upBlocks[i + 1];
    }
}

void UpdatePassedWay(int passedWay[8]) {
    for (int i = 0; i < 7; i++ )
        passedWay[i] = passedWay[i + 1];
}
void drawGameOverPage(SDL_Renderer *sdlRenderer) {
    SDL_SetRenderDrawColor(sdlRenderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(sdlRenderer);
    SDL_Texture *sdlTexture = getImageTexture(sdlRenderer, "GameOver.bmp");
    SDL_Rect rect;
    rect.x = 0, rect.y = 0, rect.w = 840, rect.h = 720;
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &rect);
    boxColor(sdlRenderer, 40, 680, 125, 695, 0xffffffff);
    stringColor(sdlRenderer, 65, 685, "Exit", 0xff000000);
    boxColor(sdlRenderer, 710, 680, 810, 695, 0xffffffff);
    stringColor(sdlRenderer, 726, 685, "Try Again", 0xff000000);
    SDL_RenderPresent(sdlRenderer);
    SDL_DestroyTexture(sdlTexture);
    SDL_Delay(1000 / FPS);
}
void initializeAgain(int *distance, int *passedFrames,int *scene, int *v, struct Ball *ball, int* passedDis,
        int* middleGenerated, struct MiddleBlock middleBlocks[3], int upBlocks[29], int downBlocks[29], int passedWay[8]) {
    *distance = 0;
    *passedFrames = 0;
    *scene = 1;
    *v = V;
    (*ball).y = 360;
    *passedDis = 0;
    *middleGenerated = 0;
    for (int i = 0; i < 3; i++)
        middleBlocks[i].x1 = -1;
    for (int i = 0; i < 8; i++)
        passedWay[i] = -1;
    firstGenerating(downBlocks, upBlocks);
}
int randNum(int min, int max) {
    return rand() % (max - min) + min ;
}
void firstGenerating(int downBlocks[29], int upBlocks[29]) {
    for (int i = 0; i < 28; i++) {
        upBlocks[i] = 5 * i + 20;
        downBlocks[i] = 700 - 5 * i;
    }
}
void UpdateBest(int * distance, int * best) {
    if (*distance > *best)
        *best = *distance;
}
SDL_Texture* getImageTexture(SDL_Renderer *sdlRenderer, char *image_path) {
    SDL_Surface *image = SDL_LoadBMP(image_path);
    if (image == NULL) {
        printf("Failed to load image at %s because %s\n", image_path, SDL_GetError());
        return 0;
    }
    SDL_Texture *sdlTexture = SDL_CreateTextureFromSurface(sdlRenderer, image);
    SDL_FreeSurface(image);
    return sdlTexture;
}
void drawStartPage(SDL_Renderer *sdlRenderer) {
    char strLoad[3];
    int x2 = 440, load = 0;
    for (int i = 0; i < 101; i++) {
        //SDL_SetRenderDrawColor(sdlRenderer, 0xb1, 0xed, 0xe6, 0xff);
        SDL_SetRenderDrawColor(sdlRenderer, 0x3f, 0xda, 0xf0, 0xff);
        //SDL_SetRenderDrawColor(sdlRenderer, 0x36, 0xf0, 0x57, 0xff);
        SDL_RenderClear(sdlRenderer);
        SDL_Texture *textureStart = getImageTexture(sdlRenderer, "start.bmp");
        SDL_Rect rect;
        rect.x = 164, rect.y = 0, rect.w = 512, rect.h= 512;
        itoa(load, strLoad,  10);
        SDL_RenderCopy(sdlRenderer, textureStart, NULL, &rect);
        stringColor(sdlRenderer, 330, 520, "Loading ", 0xffde5b09);
        stringColor(sdlRenderer, 395, 520, strLoad, 0xffde5b09);
        stringColor(sdlRenderer, 420, 520, "%", 0xffde5b09);
        boxColor(sdlRenderer, 440, 520, x2, 530, 0xffde5b09);
        SDL_RenderPresent(sdlRenderer);
        load++;
        x2++;
        SDL_Delay(1000/FPS);
    }
}
void drawExitPage() {
    SDL_Window *sdlWindow = SDL_CreateWindow("Exit page", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 612, 408, SDL_WINDOW_OPENGL);
    SDL_Surface *Img = SDL_LoadBMP("The-End.bmp");
    SDL_Surface *sdlSurface = SDL_GetWindowSurface(sdlWindow);
    SDL_BlitSurface(Img, NULL, sdlSurface, NULL);
    SDL_UpdateWindowSurface(sdlWindow);
    SDL_Delay(900);
}
/*
 *  boxColor(sdlRenderer, 0, 0, 30 - (*passedDis) % 30, upBlocks[0], 0xffde5b09);
    boxColor(sdlRenderer, 0, downBlocks[0],  30 - (*passedDis) % 30, 720, 0xffde5b09);
    boxColor(sdlRenderer, 840 - (*passedDis) % 30, 0, 840, upBlocks[27], 0xffde5b09);
    boxColor(sdlRenderer, 840 - (*passedDis) % 30, downBlocks[27], 840, 720, 0xffde5b09);
 */

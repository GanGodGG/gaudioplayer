#include <iostream>
#include <gangod/reciver.h>
#include <gangod/server.h>
#include <gangod/graphics.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <future>
void quit(WindowHandler& winh){
    SDL_DestroyRenderer(winh.render);
    SDL_DestroyWindow(winh.window);
    SDL_Quit();
}

int main() {
    //gserver::startServer();
    std::cout << "Starting..." << std::endl;
    // Set up locals
    std::setlocale(LC_ALL, "Russian");
    std::locale::global(std::locale(""));
    // connect to the service
    Reciver::init(Reciver::ReciverConnect::YoutubeMusic);
    std::future<Player> asyncp = std::async(Reciver::connect, Reciver::ReciverConnect::YoutubeMusic);
    //SDL start
    bool running;
    if(!SDL_Init(SDL_INIT_VIDEO)){
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "Error while starting sdl3, check is there SDL3.dll file with .exe file in programm, love, GANGOD.", nullptr);
        return -1;
    }
    if(!TTF_Init()){
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "Error while starting sdl3 ttf, check is there SDL3.dll (ttf) file with .exe file in programm, love, GANGOD.", nullptr);
        return -1;
    }
    //window with 800x600 resolution!
    WindowHandler wh;
    wh.window = SDL_CreateWindow("GAPLAYER!", 800, 600, NULL);
    if(!wh.window){
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "Fail with WIN start, love, GANGOD.", nullptr);
        quit(wh);
    }
    wh.render = SDL_CreateRenderer(wh.window, nullptr);
    running = true;

    

    Image img("img1", wh.render);
    Text tx("txt1", wh.render);
    Button play("play");
    Button next("next");
    Button prev("prev");
    Player p = asyncp.get();

    play.onclick = [&p](){
        p.Play();
    };

    UIVerticalArray<Button> bva("bva1", p.ListSongs().size(), true, "but1");
    UIVerticalArray<Text> bva2("bva1", p.ListSongs().size(), true, "txt1", wh.render);
    for (int i = 0; i < bva.size(); ++i){
        Button* b = bva[i];
        b->onclick = [&p, &wh, &img, &tx, i](){
            p.Playsong(i);
            img.gLoadImage(wh.render, 
                p.GetCurrentSong().thumb.width, 
                p.GetCurrentSong().thumb.height, 
                p.GetCurrentSong().thumb.url
            );
            tx.ChangeText(p.GetCurrentSong().title.c_str());
        };
    }
    for (int i = 0; i < bva2.size(); ++i){
        Text* b = bva2[i];
        b->ChangeText(wh.render, p.ListSongs()[i].title.c_str());
    }

    bool debug = true;
    while(running){
        SDL_Event event { 0 };
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_EVENT_QUIT:
                    running = false;
                break;

                case SDL_EVENT_MOUSE_MOTION:
                    uiResponse::OnMouseMove(event.motion.x, event.motion.y);
                break;

                case SDL_EVENT_MOUSE_WHEEL:
                    uiResponse::onScroll(event.wheel.x, event.wheel.y);
                break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        ObjectParenter::updButtonClick();
                    }
                break;

                case SDL_EVENT_KEY_DOWN:

                    if(event.key.key == SDLK_F5){
                        std::cout << "reload!" << std::endl;
                        ObjectParenter::reload();
                    }
                break;
            }
        }


        // rendering
        SDL_SetRenderDrawColor(wh.render, 38, 38, 38, 255);
        SDL_RenderClear(wh.render);
        p.Update();
        uiResponse::update();
        ObjectParenter::updAll(wh.render);

        // swap buffers
        SDL_RenderPresent(wh.render);

        SDL_Delay(16);
    }

    quit(wh);
    return 0;
}
#include <iostream>
#include <gangod/reciver.h>
#include <gangod/server.h>

int main() {
    //gserver::startServer();
    std::cout << "Starting Reciver..." << std::endl;

    std::setlocale(LC_ALL, "Russian");
    std::locale::global(std::locale(""));
  
    Reciver::init(Reciver::ReciverConnect::YoutubeMusic);
    Player pl = Reciver::connect(Reciver::ReciverConnect::YoutubeMusic);
    std::cout << pl << std::endl;
    while(true){
        std::string song;
        std::cin >> song;
        pl.Playsong(song);
        if(pl.GetSongAudio(pl.GetCurrentSong())){
            std::cout << "mp3 extracted!" << std::endl;
        }
        std::cout << pl << std::endl;
    }
    return 0;
}
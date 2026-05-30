#pragma once
// The main rule is to keep every class, method, variable, etc. as simple as possible, and to keep it independent of other classes, methods, variables, etc. This is to make the code more readable, maintainable, and testable. Also, to keep the code as simple as possible, we should avoid using complex data structures, algorithms, and design patterns unless they are absolutely necessary.
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <curl/curl.h>
#include <stdexcept>
#include <json.hpp>
#include <windows.h>
#include <shellapi.h>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ostream>
#include <gangod/filemgmt.h>
#include <gangod/server.h>
#include <FMOD/fmod.hpp>
#include <regex>
class Reciver;


// handler for errors
class Reciver_Error : public std::exception {
private:
// The message of the error
    const char* message;
public:
// Error, lol.
    Reciver_Error(char const* const message) throw() : message(message) {};
    const char* what() const noexcept override {
        return message;
    }
};

class Information{
public:
    // useless for now
    struct UserInfo{
        std::string username;
        std::string email;
    };
    // thumbnail
    struct ThumbnailInfo{
        std::string url;
        int width;
        int height;

        ThumbnailInfo(std::string url_in, int w, int h) : url(url_in), width(w), height(h) {}
    };
    // song
    struct SongInfo{
        std::string title;

        std::string author;
        std::string description;
        std::string data;

        std::string id;

        int length;
        int likes;
        int dislikes;

        ThumbnailInfo thumb;

        SongInfo(ThumbnailInfo t) : thumb(t){
        }

        SongInfo() : thumb({"", 0, 0}){
        }

        bool operator == (SongInfo& other){
            return id == other.id; 
        }

        bool operator != (SongInfo& other){
            return id != other.id; 
        }
    };
};

class Player{
private:
// current selected playing song
    bool isPlaying = false;
    bool isPaused = false;

    bool isDownloading = false;
    bool isReadyToPlay = false;

    std::string ds = "";

    Information::SongInfo currentSong;
    Information::SongInfo playingSong;
    std::vector<Information::SongInfo> all_songs;
    FMOD::System* sys = nullptr;
    FMOD::Sound* snd = nullptr;
    FMOD::Channel* chan = nullptr;
    FMOD_RESULT result;
public:
    Information::SongInfo GetCurrentSong();
    // -------------- all songs --------------
    Information::SongInfo GetSongFromList(int index);
    std::vector<Information::SongInfo> ListSongs();
    inline void PushSong(Information::SongInfo song);
    int GetSongIndex(Information::SongInfo song);
    void Play();
    void AsyncPlaySong(Information::SongInfo song);
    void Playsong(Information::SongInfo song);
    void Playsong(std::string songName);
    void Playsong(int songID);
    void Update();

    void Next();
    std::string GetSongAudio(Information::SongInfo song);

    Player();

    friend std::ostream& operator<<(std::ostream& os, const Player& pl) // returns current song
    {
        os << "------------" << pl.currentSong.title << "------------" << std::endl;

        os << "Author \t" << pl.currentSong.author << std::endl;

        os << "------------" << "Description" << "------------" << "\n" << pl.currentSong.description  << std::endl;

        os << "Data of creation \t" << pl.currentSong.data << std::endl;

        os << "ID \t" << pl.currentSong.id << std::endl;

        os << "urlimg: \t" << pl.currentSong.thumb.url << std::endl;
        return os;
    }
};

// Absolutely static class, no need to create an instance of it, just call the static methods
class Reciver {
public:
    static std::string token;
    enum ReciverConnect{
        // ------------------------------- supported services --------------------------------
        YoutubeMusic,
        // ------------------------------- unsupported services --------------------------------
        Spotify,
        AppleMusic,
        AmazonMusic,
        SoundCloud
    };
// Constructor and Destructor is deleted to prevent creating sample of it
    Reciver() = delete;
    ~Reciver() = delete;
    Reciver(const Reciver&) = delete;
    Reciver& operator=(const Reciver&) = delete;
// init. The init will read token from a file!!!..>!>!>!>!>!>
    static void init(ReciverConnect service);
// ---------------------------------- User Mgmt ----------------------------------
// Trying to register a user, if the user registered (got it on current service) returns 1, else 0
// Declareted in build/RECIVER.CPP, defined in this header.
    // static int registerUser(UserInfo user);
// Connecting to current service
// I'll block fkng forsaken music, 
// cuz its the worst music in the world, and I don't want to support it, you'll get an error message.
    static Player connect(ReciverConnect service);
// Get songs list (youtube)
    static std::string YT_getSongsList(std::string playlist_id, std::string token);
};
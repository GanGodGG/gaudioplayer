#include <gangod/reciver.h>
std::string Reciver::token = "";
// --------------------------------- class Reciver method definitions ---------------------------------
// Callback function for curl. Write 
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    } catch (std::bad_alloc& e) {
        throw Reciver_Error("Failed to allocate memory for curl response.");
    }
    return newLength;
}

void Reciver::init(ReciverConnect service) {
    // Here we can add the code to initialize the reciver, but for now, we'll just print a message.
    std::string username = std::getenv("USERPROFILE");
    if(service == ReciverConnect::YoutubeMusic) {

        // ------------------ GETTING REFRESH TOKEN ------------------
        std::filesystem::path secretPath = std::filesystem::path(username) / "Roaming" / "GANGOD_ENC" / "secret.dat";
        std::string reftoken = fileRead::decrypt_token(fileRead::binOpenFILE(secretPath.string())); // refresh token result
         // ------------------ CURL INIT ------------------
        CURL* curl = curl_easy_init();
        // if curl not initialized
        if(!curl) {
            throw Reciver_Error("Failed to initialize curl in reciver init!");
            return;
        }
        std::string post_fields; // post fields

        std::string cliID = fileRead::OpenFILE(std::filesystem::path("./PSST.json").string());
        auto parsed = nlohmann::json::parse(cliID);
        // ----------------------------------------- CLIENT INFO -----------------------------------------
        std::string client_id = parsed["installed"]["client_id"];
        std::string client_secret = parsed["installed"]["client_secret"];
        std::string url = "https://oauth2.googleapis.com/token";

        std::string reurl = "http://127.0.0.1:8080";
        
        std::string response_string; //response

        // open browser with url (google. CLIENT ID NEED TO BE HIDDEN)
        if(reftoken.size() < 3){
            ShellExecute(0, 0, 
            ("https://accounts.google.com/o/oauth2/v2/auth?client_id=" + 
                client_id 
                + 
                "&redirect_uri=" + reurl +"&response_type=code&scope=https://www.googleapis.com/auth/youtube.readonly").c_str(), 0, 0, SW_SHOW );
            std::string auth_code = "";
            gserver::startServer(auth_code);    
            post_fields = "code=" + auth_code +
                              "&client_id=" + client_id +
                              "&client_secret=" + client_secret +
                              "&redirect_uri=" + reurl +
                              "&grant_type=authorization_code";
        }
        else // we already registered user.
        {
            // --------------- CHANGING POST-FIELD ---------------
            post_fields = "client_id=" + client_id +
                        "&client_secret=" + client_secret +
                        "&refresh_token=" + reftoken +
                        "&grant_type=refresh_token";
        
        }
        // -------------------------- SEND --------------------------
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            
            auto j = nlohmann::json::parse(response_string);
        
            // if token is here?
            if (j.contains("access_token")) {
                token = j["access_token"];
                if(reftoken.size() < 3){
                    std::vector<BYTE> crypt = fileRead::encrypt_token(((std::string)j["refresh_token"]));
                    fileRead::Createfile(secretPath,
                    reinterpret_cast<char*>(crypt.data()), crypt.size());
                }
            } 
            else 
            {
                throw Reciver_Error("Failed to obtain access token.");
            }

        } 
        else 
        {
            std::cerr << response_string << std::endl;
            throw Reciver_Error("Failed to perform cURL request.");
        }
    }
}

Player Reciver::connect(ReciverConnect service) {
    Player pl;
    // I'll block fkng forsaken music.
    if (service == ReciverConnect::Spotify 
        || service == ReciverConnect::AppleMusic 
        || service == ReciverConnect::AmazonMusic 
        || service == ReciverConnect::SoundCloud) {
        throw Reciver_Error("This music service is not supported yet, sorry for the inconvenience.");
    }
    // Connecting to youtube music, which is the only supported service for now.
    if(service == ReciverConnect::YoutubeMusic) {
        //std::cout << "Connecting to Youtube Music..." << std::endl;
        // Here we can add the code to connect to youtube music, but for now, we'll just print a message.
        CURL* curl = curl_easy_init();
        // if curl_easy_init returns NULL, it means that it failed to initialize curl, so we throw an error.
        if(!curl) {
            throw Reciver_Error("Failed to initialize curl.");
        }
        // Readbuffer will store the response from the server, which is the list of playlists of the user.
        std::string readBuffer;
        struct curl_slist* headers = NULL;
        std::string auth_header = "Authorization: Bearer " + token;
        // Here we can add the code to set the headers for the request, but for now, we'll just set the authorization header with a dummy token.
        headers = curl_slist_append(headers, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.googleapis.com/youtube/v3/channels?part=contentDetails&mine=true");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        CURLcode res = curl_easy_perform(curl);
        std::string playlist_id;
        if(res != CURLE_OK) {
            // CRITICAL: MEMORY LEAK!
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
            throw Reciver_Error("Failed to connect to Youtube Music.");
        }
        else{
            // std::cout << "Response from Youtube Music: " << readBuffer << std::endl;
            //auto, cuz i dont want tto write the type
            auto parsed = nlohmann::json::parse(readBuffer);
            for (const auto& item : parsed["items"]) {
                if(item["contentDetails"]["relatedPlaylists"]["likes"].get<std::string>().find("LL") != std::string::npos){
                    //std::cout << "Found liked songs playlist!" << std::endl;
                    playlist_id = item["contentDetails"]["relatedPlaylists"]["likes"];
                }
            }
        }
        // ----------------------------------------- music list -----------------------------------------


        std::string pgtoken = ""; // now there's no shadowing
        while(true){
            //std::cout << "Response from Music: " << readBuffer << std::endl;
            
            readBuffer = YT_getSongsList(playlist_id, pgtoken);
            if(readBuffer == ""){
                break;
            }
            auto parsed = nlohmann::json::parse(readBuffer);
            for (const auto& item : parsed["items"]) {

                if(!item.contains("snippet")) { continue; }

                // std::cout << "Music: " << item["snippet"]["title"] << std::endl;
                //State of the song
                if(item["snippet"]["title"] == "Private video" || item["snippet"]["title"] == "Deleted video") {continue;}
                Information::SongInfo song;
                // thumbnail info
                if(item["snippet"].contains("thumbnails") && item["snippet"]["thumbnails"].contains("high")) {// check if thumbnail song exist (fckin omori song)
                    song.thumb = Information::ThumbnailInfo(
                        item["snippet"]["thumbnails"]["high"]["url"], 
                        item["snippet"]["thumbnails"]["high"]["width"],
                        item["snippet"]["thumbnails"]["high"]["height"]
                    );
                }
                else{
                    continue;
                }
                
                // song info
                song.title = item["snippet"].contains("title") ? item["snippet"]["title"] : "unnamed :/";
                song.author = item["snippet"].contains("videoOwnerChannelTitle") ? item["snippet"]["videoOwnerChannelTitle"] : "Cool author";
                song.id = item["snippet"]["resourceId"]["videoId"]; // IMPORTANT!
                song.description = item["snippet"].contains("description") ? item["snippet"]["description"] : "Beautiful song!";
                song.data = item["snippet"].contains("publishedAt") ? item["snippet"]["publishedAt"]  : "12.11.09";

                pl.PushSong(song);
            }
            if(parsed.contains("nextPageToken")){
                pgtoken = parsed["nextPageToken"];
            }
            else{
                break;
            }

        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    return pl;
}

std::string Reciver::YT_getSongsList(std::string playlist_id, std::string next_page_token) {

    std::string readBuffer;
    struct curl_slist* headers = NULL;
    std::string auth_header = "Authorization: Bearer " + token;
    CURL* curl;
    CURLcode res;
    curl = curl_easy_init();
    headers = NULL;
    headers = curl_slist_append(headers, auth_header.c_str());
    readBuffer = "";
    std::string url = "https://www.googleapis.com/youtube/v3/playlistItems"
                      "?part=snippet"
                      "&maxResults=50"
                      "&playlistId=" "LM";
    if(next_page_token.size() > 3){
        //std::cout << "page token found!" << std::endl;
        url += "&pageToken=" + next_page_token;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    if(res != CURLE_OK) {
        return "";
    }
    return readBuffer;
}



// --------------------------------- class Player method definitions ---------------------------------

Player::Player(){ // js not iinit
    result = FMOD::System_Create(&sys);
    if(result != FMOD_OK){
        throw Reciver_Error("Error while creating an system!");
    }
    result = sys->init(32, FMOD_INIT_NORMAL, nullptr);
    if(result != FMOD_OK){
        throw Reciver_Error("Error while creating an system!");
    }

}

int Player::GetSongIndex(Information::SongInfo song){
    for(int i = 0; i < all_songs.size(); ++i){
        if(all_songs[i] == song){
            return i;
        }
    }
    return -1;
}

Information::SongInfo Player::GetCurrentSong()
{
    return currentSong;
}

Information::SongInfo Player::GetSongFromList(int index)
{
    return all_songs[index];
}

std::vector<Information::SongInfo> Player::ListSongs()
{
    return all_songs;
}

inline void Player::PushSong(Information::SongInfo song)
{
    all_songs.push_back(song);
}

void Player::Playsong(Information::SongInfo song)
{
    currentSong = song;
}

void Player::Play(){
    if(!isPlaying || playingSong != currentSong){
        if(chan){
            chan->stop();
            chan->setPaused(false);
        }
        AsyncPlaySong(currentSong);
        return;
    }
    chan->getPaused(&isPaused);
    chan->setPaused(!isPaused);
}

void Player::AsyncPlaySong(Information::SongInfo song)
{
    if(isDownloading) { return; }

    isDownloading = true;

    std::thread([this, song](){
        std::string audpth = GetSongAudio(song);
        this->ds = audpth;
        this->isReadyToPlay = true;
    }).detach();
}

void Player::Update(){
    if(isReadyToPlay){
        isReadyToPlay = false;
        isDownloading = false;
        result = sys->createSound(ds.c_str(), FMOD_DEFAULT, nullptr, &snd);
        std::cout << ds << std::endl;
        if(result != FMOD_OK){
            std::cerr << "Failed to play sound: " << result << std::endl;
            sys->release();
            throw Reciver_Error("Error while creating an system!");
        }
        result = sys->playSound(snd, nullptr, false, &chan);
        if (result != FMOD_OK) {
            std::cerr << "Failed to play sound: " << result << std::endl;
            snd->release();
            sys->release();
            return;
        }
        playingSong = currentSong;
        isPlaying = true;
    }
    if(isPlaying){
        sys->update();
        if (chan) {
            chan->isPlaying(&isPlaying);
        }
        if(!isPlaying){
            Next();
        }
    }
}

void Player::Next(){
    Playsong(GetSongIndex(currentSong) + 1);
    Play();
}

void Player::Playsong(std::string songName)
{
    for(const auto& song : all_songs) {
        if(song.title.find(songName) != std::string::npos) {
            currentSong = song;
            return;
        }
    }

}

void Player::Playsong(int songID)
{
    if (songID >= 0 && songID < all_songs.size()) {
        currentSong = all_songs[songID];
    } else {
        std::cerr << "no such song (id)" << std::endl;
    }
}

std::string Player::GetSongAudio(Information::SongInfo song){
    std::string exit = std::string("./Downloads/" + song.id + ".mp3");
    if(fileRead::isFileExist("./Downloads/" + song.id + ".mp3")) { return exit; }
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    std::regex ytRegex("^[a-zA-Z0-9_-]{11}$");
    if(!std::regex_match(song.id, ytRegex)){
        return std::string("./no, my dear hackers ;).mp3");
    }

    std::string downloadPath = "./Downloads/%(id)s.%(ext)s";
    std::string command = "yt-dlp.exe -x --audio-format mp3 -o \"" + downloadPath + "\" https://www.youtube.com/watch?v=" + song.id;
    CreateProcessA("yt-dlp.exe", command.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi); 
    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return exit;
}
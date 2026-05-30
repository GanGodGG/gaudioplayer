#include <gangod/graphics.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
float uiResponse::mouseX = 0.0f;
float uiResponse::mouseY = 0.0f;

float uiResponse::scrollX = 0.0f;
float uiResponse::scrollY = 0.0f;

std::vector<Object*> ObjectParenter::alls = {};

void ObjectParenter::push(Object* o){
    alls.push_back(o);
}

void ObjectParenter::updButtonClick(){
    for(auto& o : alls){
        auto but = dynamic_cast<Button*>(o);
        if(but){
            but->Click(); 
        }
           
    }
}

void ObjectParenter::updAll(SDL_Renderer* rend){
    for(auto& o : alls){
        o->Update(rend); 
    }
}

void ObjectParenter::reload(){
    for(auto& o : alls){
        if(!o->name.empty()){
            o->ConstructFile(o->name.c_str());
        }
    }
}

Object::Object(const char* name){
    ObjectParenter::push(this);
    ConstructFile(name);
    BasicUpdate();
}

Object::Object(){
    ObjectParenter::push(this);
}

void Object::ConstructFile(const char* name){
    std::string file = fileRead::OpenFILE("jsons/interface.json");
    auto parse = nlohmann::json::parse(file);
    for(const auto& f : parse["ui_elements"]){
        if(!f.contains("name")) {continue;}
        if(f["name"].get<std::string>() == name){
            jfile = f;
            this->name = f["name"].get<std::string>();
            
            ChangeByJsonFile();

            if(onReload){
                onReload();
            }
            return;
        }
    }

    std::cerr << "no such object! " << name << std::endl;
}

void Image::gLoadImage(SDL_Renderer* rnd, int width, int height, std::string url) {
    // 1. ИСПРАВЛЕНО: Безопасный захват по значению [=], чтобы переменные не стерлись
    // И заменено rendptr на rnd
    onReload = [this, rnd, width, height, url]() {
        gLoadImage(rnd, width, height, url);
    };

    if (tex) {
        SDL_DestroyTexture(tex);
        tex = nullptr;
    }

    std::vector<char> img = findImageFromUrl(url);
    if (img.empty()) {
        std::cout << "Url вернул пустой массив данных!" << std::endl;
        return;
    }

    int nrChannels, x, y;
    unsigned char* data = stbi_load_from_memory(
        (const stbi_uc*)img.data(), 
        img.size(), 
        &x, &y, &nrChannels, STBI_rgb_alpha
    );

    if (!data) {
        std::cout << "stbi_load_from_memory failed!" << std::endl;
        return;
    }

    SDL_Surface* suf = SDL_CreateSurfaceFrom(
        x, y, 
        SDL_PIXELFORMAT_RGBA32, 
        data, 
        x * 4
    );
    origX = width;
    origY = height;

    if (!suf) {
        std::cout << "Error while creating surface: " << SDL_GetError() << std::endl;
        stbi_image_free(data);
        return;
    }

    // Создаем текстуру из поверхности, пока data еще жива!
    tex = SDL_CreateTextureFromSurface(rnd, suf);
    
    // 2. ИСПРАВЛЕНО: Освобождаем память СТРОГО после того, как текстура создана
    SDL_DestroySurface(suf);
    stbi_image_free(data); 

    if (!tex) {
        std::cout << "Cannot load texture: " << SDL_GetError() << std::endl;
    }
}


// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "Project_SDL1.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <numeric>
#include <random>
#include <string>

void init() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0)
    throw std::runtime_error("init():" + std::string(SDL_GetError()));

  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
    throw std::runtime_error("init(): SDL_image could not initialize! "
                             "SDL_image Error: " +
                             std::string(IMG_GetError()));
}

namespace {
  // Defining a namespace without a name -> Anonymous workspace
  // Its purpose is to indicate to the compiler that everything
  // inside of it is UNIQUELY used within this source file.

  SDL_Surface* load_surface_for(const std::string& path, SDL_Surface* window_surface_ptr)
  {

    // Helper function to load a png for a specific surface
    // See SDL_ConvertSurface
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());

    if (!loadedSurface)
    {
      throw std::runtime_error("Unable to load image");
    }

    auto optimizedSurface = SDL_ConvertSurface(loadedSurface, window_surface_ptr->format, 0);
    
    if (optimizedSurface == NULL)
    {
      throw std::runtime_error("Unable to optimize image");
    }

    SDL_FreeSurface(loadedSurface);
    
    return optimizedSurface;
  }
} // namespace


//Application
application::application(unsigned n_sheep, unsigned n_wolf)
{
  std::cout << "Constructor application" << "\n";
  
  window_ptr_= SDL_CreateWindow(
    "SDL Project", 
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    frame_width,
    frame_height,
    SDL_WINDOW_SHOWN
  );

  if (!window_ptr_) {
    throw std::runtime_error("Unable to create window");
  }

  window_surface_ptr_ = SDL_GetWindowSurface(window_ptr_);

  if (!window_surface_ptr_) {
    throw std::runtime_error("Unable to create surface");
  }

  SDL_FillRect(window_surface_ptr_, NULL, SDL_MapRGB(window_surface_ptr_->format, 143, 233, 50));
  
  _ground = new ground(window_surface_ptr_);
  
  std::srand(time(nullptr));
  for (unsigned i = 0; i < n_sheep; ++i)
  {
    _ground->add_animal(new sheep(window_surface_ptr_));
  }
  for (unsigned i = 0; i < n_wolf; ++i)
  {
    _ground->add_animal(new wolf(window_surface_ptr_));
  }
}

int application::loop(unsigned period)
{
  bool running = true;
  unsigned lastTime = 0;
  unsigned startTicks;

  while(running && SDL_GetTicks() - lastTime <= period * 1000)
  {
    startTicks = SDL_GetTicks();

    SDL_Event e;
    while ( SDL_PollEvent( &e ) != 0 ) {
			if (e.type == SDL_QUIT) {
        running = false;
        break;
			}
		}
    _ground->update();
    SDL_Delay(50);
    SDL_UpdateWindowSurface(window_ptr_);

  }
  return 0;
}

application::~application()
{

}


//Ground
ground::ground(SDL_Surface* window_surface_ptr)
{
  window_surface_ptr_ = window_surface_ptr;
}

void ground::add_animal(animal *animal)
{
  zoo.push_back(animal);
}

void ground::update()
{
  for (animal * animal : zoo)
  {
    animal->move();
  }
  SDL_FillRect(window_surface_ptr_, nullptr,
                 SDL_MapRGB(window_surface_ptr_->format, 143, 233, 50));
  for (animal * animal : zoo)
  {
    animal->draw();
  }
}

//Animal
int randomPosition(int min, int max)
{
  std::random_device rd;
  std::mt19937::result_type seed = rd() ^ (
    (std::mt19937::result_type)
    std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()
      ).count() +
      (std::mt19937::result_type)
      std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
      ).count() );
    std::mt19937 gen(seed);
    std::uniform_int_distribution<unsigned> distrib(min, max);

  return distrib(gen);
}

animal::animal(const std::string& file_path, SDL_Surface* window_surface_ptr)
{
  window_surface_ptr_ = window_surface_ptr;
  image_ptr_ = load_surface_for(file_path, window_surface_ptr_);
  
  x = randomPosition(frame_boundary, frame_width - frame_boundary - image_ptr_->w);
  y = randomPosition(frame_boundary, frame_height - frame_boundary - image_ptr_->h);
}

void animal::draw()
{
  auto rect = SDL_Rect{x, y, image_ptr_->w, image_ptr_->h};
  if (SDL_BlitSurface(image_ptr_, NULL, window_surface_ptr_, &rect))
    throw std::runtime_error("Bug texture");

}

//Sheep

sheep::sheep(SDL_Surface *window_surface_ptr):animal(sheep_path, window_surface_ptr)
{
  speed = 5;
}

sheep::~sheep()
{
}

void sheep::move()
{
  //TODO if wolf go other side
  if (x+speed < frame_width - frame_boundary) {
    x+=speed;
  }
  
  if (y+speed < frame_height - frame_boundary) {
    y+=speed;
  }
}


//Wolf

wolf::wolf(SDL_Surface *window_surface_ptr):animal(wolf_path, window_surface_ptr)
{
  speed = 10;
}

wolf::~wolf()
{


}

void wolf::move()
{
  if (x+speed < frame_width - frame_boundary) {
    x+=speed;
  }
  if (y+speed < frame_height - frame_boundary) {
    y+=speed;
  }
}

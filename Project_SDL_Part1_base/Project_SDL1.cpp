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

  SDL_Texture* load_surface_for(const std::string& path, SDL_Renderer* window_renderer_ptr, SDL_Rect *rect)
  {

    // Helper function to load a png for a specific surface
    // See SDL_ConvertSurface
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());

    if (!loadedSurface)
    {
      throw std::runtime_error("Unable to load image");
    }

    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(window_renderer_ptr, loadedSurface);
    if (texture == NULL)
    {
      throw std::runtime_error("Unable to optimize image");
    }
    rect->w = loadedSurface->w;
    rect->h = loadedSurface->h;
    SDL_FreeSurface(loadedSurface);
    
    return texture;
  }
} // namespace 


//Application
application::application(unsigned n_sheep, unsigned n_wolf)
{
  std::cout << "Constructor application" << "\n";
  
  if (SDL_CreateWindowAndRenderer(frame_width, frame_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE, &window_ptr_, &window_renderer_ptr_) == -1) {
    throw std::runtime_error("Error create window and renderer");
  }

  SDL_SetWindowPosition(window_ptr_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  
  SDL_SetWindowTitle(window_ptr_, "Title window");
  
  _ground = new ground(window_renderer_ptr_);
  
  std::srand(time(nullptr));
  for (unsigned i = 0; i < n_sheep; ++i)
  {
    _ground->add_animal(new sheep(window_renderer_ptr_));
  }
  for (unsigned i = 0; i < n_wolf; ++i)
  {
    _ground->add_animal(new wolf(window_renderer_ptr_));
  }
}

int application::loop(unsigned period)
{
  bool running = true;

  while(running && SDL_GetTicks() <= period * 1000)
  {
    
    Uint32 start = SDL_GetTicks();
    SDL_Event e;
    while ( SDL_PollEvent( &e ) != 0 ) {
			if (e.type == SDL_QUIT) {
        running = false;
        break;
			}
		}

    SDL_RenderClear(window_renderer_ptr_);

    _ground->update();

    SDL_RenderPresent(window_renderer_ptr_);

    Uint32 elapsedTime = SDL_GetTicks() - start;
    
    if (elapsedTime < frame_rate) {
      SDL_Delay(frame_rate - elapsedTime);
    }
  }
  return 0;
}

application::~application()
{
  delete this->_ground;

  // Destroy current renderer before window
  SDL_DestroyWindow(this->window_ptr_);
  IMG_Quit();
}


//Ground
ground::ground(SDL_Renderer* window_renderer_ptr_)
{
  this->window_renderer_ptr_ = window_renderer_ptr_;
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
   if (SDL_SetRenderDrawColor(window_renderer_ptr_, 143, 233, 50, SDL_ALPHA_OPAQUE) != 0)
            throw std::runtime_error("SDL_SetRenderDrawColor"
                                    + std::string(SDL_GetError()));
  for (animal * animal : zoo)
  {
    animal->draw();
  }
}

ground::~ground()
{
  for (animal * animal : zoo)
  {
    delete animal;
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

animal::animal(const std::string& file_path, SDL_Renderer* window_renderer_ptr)
{
  window_renderer_ptr_ = window_renderer_ptr;
  image_ptr_ = load_surface_for(file_path, window_renderer_ptr_, &rect);
  
  rect.x = randomPosition(frame_boundary, frame_width - frame_boundary - rect.w);
  rect.y = randomPosition(frame_boundary, frame_height - frame_boundary - rect.h);
}

void animal::draw()
{
  if (SDL_RenderCopy(window_renderer_ptr_, image_ptr_, NULL, &rect))
    throw std::runtime_error("Error SDL Render Copy");

}

SDL_Rect animal::get_rect()
{
  return rect;
}

void random_move(int *position, int *speed, int size, int frame)
{
  if (*position <= frame_boundary) {
    *position = frame_boundary;
    *speed *= -1;
  }

  if (*position >= frame - frame_boundary - size) {
    *position = frame - frame_boundary - size;
    *speed *= -1;
  }
  *position += *speed;
}

//Sheep

sheep::sheep(SDL_Renderer *window_renderer_ptr_):animal(sheep_path, window_renderer_ptr_)
{
  speedX = 5;
  speedY = 5;
}

sheep::~sheep()
{
}

void sheep::move()
{ 
  random_move(&rect.x, &speedX, rect.w, frame_width);
  random_move(&rect.y, &speedY, rect.h, frame_height);
}


//Wolf

wolf::wolf(SDL_Renderer *window_renderer_ptr_):animal(wolf_path, window_renderer_ptr_)
{
  speedX = 6;
  speedY = 6;
}

wolf::~wolf()
{


}

void wolf::move()
{
  random_move(&rect.x, &speedX, rect.w, frame_width);
  random_move(&rect.y, &speedY, rect.h, frame_height);
}


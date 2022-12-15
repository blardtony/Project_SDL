// SDL_Test.h: Includedatei für Include-Standardsystemdateien
// oder projektspezifische Includedateien.

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <vector>

// Defintions
constexpr double frame_rate = 60.0; // refresh rate
constexpr double frame_time = 1. / frame_rate;
constexpr unsigned frame_width = 1400; // Width of window in pixel
constexpr unsigned frame_height = 900; // Height of window in pixel
constexpr char sheep_path[] = "../../media/sheep.png";
constexpr char wolf_path[] = "../../media/wolf.png";
// Minimal distance of animals to the border
// of the screen
constexpr unsigned frame_boundary = 100;

// Helper function to initialize SDL
void init();

class animal {
  protected:
    int speedX;
    int speedY;
    SDL_Rect rect;
    // SDL_Renderer* window_renderer_ptr_;
  private:
    SDL_Renderer* window_renderer_ptr_; // ptr to the surface on which we want the
                                      // animal to be drawn, also non-owning
    SDL_Texture* image_ptr_; // The texture of the sheep (the loaded image), use
                            // load_surface_for
    // todo: Attribute(s) to define its position
  public:
    animal(const std::string& file_path, SDL_Renderer* window_renderer_ptr_);
    // todo: The constructor has to load the sdl_surface that corresponds to the
    // texture
    ~animal(){}; // todo: Use the destructor to release memory and "clean up
                // behind you"

    void draw(); // todo: Draw the animal on the screen <-> window_surface_ptr.
                  // Note that this function is not virtual, it does not depend
                  // on the static type of the instance

    virtual void move(){}; // todo: Animals move around, but in a different
                              // fashion depending on which type of animal
    SDL_Rect get_rect();
    
};

// Insert here:
// class sheep, derived from animal
class sheep : public animal {
  public:
    sheep(SDL_Renderer *window_renderer_ptr_);
    virtual ~sheep();
    virtual void move();
};

class wolf : public animal {
  public:
    wolf(SDL_Renderer *window_renderer_ptr_);
    virtual ~wolf();
    virtual void move();
};

// Insert here:
// class wolf, derived from animal
// Use only sheep at first. Once the application works
// for sheep you can add the wolves

// The "ground" on which all the animals live (like the std::vector
// in the zoo example).
class ground {
  private:
    // Attention, NON-OWNING ptr, again to the screen
    SDL_Renderer* window_renderer_ptr_;

    // Some attribute to store all the wolves and sheep
    // here
    std::vector<wolf *> vect_wolf;
    std::vector<sheep *> vect_sheep;
    std::vector<animal *> zoo;

  public:
    ground(SDL_Renderer* window_renderer_ptr_); // todo: Ctor
    ~ground(); // todo: Dtor, again for clean up (if necessary)
    void add_animal(animal *animal); // todo: Add an animal
    void update(); // todo: "refresh the screen": Move animals and draw them
    // Possibly other methods, depends on your implementation
};

// The application class, which is in charge of generating the window
class application {
  private:
    // The following are OWNING ptrs
    SDL_Window* window_ptr_;
    SDL_Renderer* window_renderer_ptr_;
    SDL_Event window_event_;

    // Other attributes here, for example an instance of ground
    ground *_ground;

  public:
    application(unsigned n_sheep, unsigned n_wolf); // Ctor
    ~application();                                 // dtor

    int loop(unsigned period); // main loop of the application.
                              // this ensures that the screen is updated
                              // at the correct rate.
                              // See SDL_GetTicks() and SDL_Delay() to enforce a
                              // duration the application should terminate after
                              // 'period' seconds
};
/*
 *  PECS - engine.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/21/24
 */

#ifndef pecs_engine_hpp
#define pecs_engine_hpp

#include "src/include/device.hpp"

namespace pecs
{

class Engine : public Singular
{
  public:
    Engine();
    Engine(const Settings&);

    ~Engine();

    void run();

  private:
    void initialize();
    void createInstance();

  private:
    Settings::Engine settings;
    std::vector<const char *> layers;
    
    GUI * gui = nullptr;
    Device * device = nullptr;

    vk::raii::Instance vk_instance = nullptr;
};

} // namespace pecs

#endif // pecs_engine_hpp
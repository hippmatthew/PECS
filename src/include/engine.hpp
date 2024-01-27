/*
 *  PECS - engine.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/26/24
 */

#ifndef pecs_engine_hpp
#define pecs_engine_hpp

#include "src/include/loop.hpp"

namespace pecs
{

class Engine : public Singular
{
  public:
    Engine();
    Engine(const Settings&);

    ~Engine();

    const Device& getDevice() const;
    const ViewportInfo viewportInfo() const;

    void run(Loop&);

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
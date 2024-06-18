#ifndef vecs_core_entitycontroller_hpp
#define vecs_core_entitycontroller_hpp

#include <list>

namespace vecs
{

class Entity
{
  public:
    Entity() = default;
    Entity(unsigned int);
    Entity(const Entity&) = default;
    Entity(Entity&&) = default;

    ~Entity() = default;

    Entity& operator = (const Entity&) = default;
    Entity& operator = (Entity&&) = default;

    unsigned int id() const;

    Entity& addSystems(unsigned int);

  private:
    unsigned int e_id = 0x0u;
};

class EntityController
{
  public:
    EntityController(const EntityController&) = delete;
    EntityController(EntityController&&) = delete;

    ~EntityController();

    EntityController& operator = (const EntityController&) = delete;
    EntityController& operator = (EntityController&&) = delete;

    static EntityController& instance();
    static void destroy();

    const std::list<std::shared_ptr<Entity>>& entities() const;

    void add_entities(std::list<std::shared_ptr<Entity>>&);

  private:
    EntityController() = default;
  
  private:
    static EntityController * p_controller;
    std::list<std::shared_ptr<Entity>> e_list;
};

} //namespace vecs

#endif // vecs_core_entitycontroller_hpp
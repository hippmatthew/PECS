#include "src/core/include/entitycontroller.hpp"

namespace vecs
{

EntityController * EntityController::p_controller = nullptr;

Entity::Entity(unsigned int bits)
{
  e_id = bits;
}

unsigned int Entity::id() const
{
  return e_id;
}

Entity& Entity::addSystems(unsigned int bits)
{
  e_id ^= bits;
  return *this;
}

EntityController::~EntityController()
{
  destroy();
}

EntityController& EntityController::instance()
{
  if (p_controller == nullptr)
    p_controller = new EntityController;

  return *p_controller;
}

void EntityController::destroy()
{
  if (p_controller == nullptr) return;
  
  delete p_controller;
  p_controller = nullptr;
}

const std::list<std::shared_ptr<Entity>>& EntityController::entities() const
{
  return e_list;
}

void EntityController::add_entities(std::list<std::shared_ptr<Entity>>& list)
{
  for (auto& entity : list)
    e_list.emplace_back(entity);
}

} // namespace vecs
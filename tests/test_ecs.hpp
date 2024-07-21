#ifndef vecs_tests_test_ecs_hpp
#define vecs_tests_test_ecs_hpp

#include "vecs/vecs.hpp"

namespace TEST
{

struct ComponentA
{
  int number = 0;
};

struct ComponentB
{
  int multiplier = 0;
};

class SystemA : public vecs::System
{
  public:
    void update(const std::shared_ptr<vecs::ComponentManager>& c_manager, std::set<unsigned long> e_ids)
    {
      for (const auto& e_id : e_ids)
      {
        auto data_opt = c_manager->retrieve<ComponentA>(e_id);
        if (data_opt == std::nullopt) continue;
        
        auto data = data_opt.value();
        data.number += 1;

        c_manager->update_data<ComponentA>(e_id, data);
      }
    }
};

class SystemB : public vecs::System
{
  public:
    void update(const std::shared_ptr<vecs::ComponentManager>& c_manager, std::set<unsigned long> e_ids)
    {
      for (const auto& e_id : e_ids)
      {
        auto dataA_opt = c_manager->retrieve<ComponentA>(e_id);
        auto dataB_opt = c_manager->retrieve<ComponentB>(e_id);
        if (dataA_opt == std::nullopt || dataB_opt == std::nullopt) continue;
        
        auto dataA = dataA_opt.value();
        auto dataB = dataB_opt.value();
        
        dataA.number *= dataB.multiplier;

        c_manager->update_data<ComponentA>(e_id, dataA);
      }
    }
};

class Engine : public vecs::Engine
{
  public:
    const std::pair<int, int> final_values() const
    {
      auto data0 = component_manager->retrieve<ComponentA>(0);
      auto data1 = component_manager->retrieve<ComponentA>(1);

      return std::make_pair(data0.value().number, data1.value().number);
    }
    
    void load() override
    {
      system_manager->emplace<SystemA, SystemB>();
      system_manager->add_components<SystemA, ComponentA>();
      system_manager->add_components<SystemB, ComponentA, ComponentB>();

      component_manager->register_components<ComponentA, ComponentB>();
      component_manager->update_data<ComponentA>(0, { 1 });
      component_manager->update_data<ComponentA>(1, { 1 });
      component_manager->update_data<ComponentB>(1, { 2 });

      entity_manager->new_entity();
      entity_manager->add_components<ComponentA>(0);
      
      entity_manager->new_entity();
      entity_manager->add_components<ComponentA, ComponentB>(1);

      initialize();
    }
    
    void run() override
    {
      auto systemA = system_manager->system<SystemA>().value();
      auto systemB = system_manager->system<SystemB>().value();
      
      while (!close_condition())
      {
        poll_gui();

        systemA->update(component_manager, entity_manager->retrieve<ComponentA>());
        systemB->update(component_manager, entity_manager->retrieve<ComponentA, ComponentB>(true));

        ++loopCounter;
      }
    }
  
  private:
    bool close_condition() override
    { return !(loopCounter < 10); }

  private:
    unsigned int loopCounter = 0;
};

} // namespace TEST

#endif // vecs_tests_test_ecs_hpp
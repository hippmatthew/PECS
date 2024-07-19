#ifndef vecs_core_components_hpp
#define vecs_core_components_hpp

#include <map>
#include <optional>
#include <vector>

namespace vecs
{

class IComponentArray
{
  public:
    IComponentArray() = default;
    IComponentArray(const IComponentArray&) = default;
    IComponentArray(IComponentArray&&) = default;

    virtual ~IComponentArray() = default;

    IComponentArray& operator = (const IComponentArray&) = default;
    IComponentArray& operator = (IComponentArray&&) = default;
};

template <typename T>
class ComponentArray : public IComponentArray
{
  public:
    ComponentArray() = default;
    ComponentArray(const ComponentArray&) = default;
    ComponentArray(ComponentArray&&) = default;

    ~ComponentArray() = default;

    ComponentArray& operator = (const ComponentArray&) = default;
    ComponentArray& operator = (ComponentArray&&) = default;
    
    std::optional<T> at(unsigned long) const;
    
    void emplace(unsigned long, T&);
    void erase(unsigned long);

  private:
    bool valid(unsigned long) const;

  private:
    std::vector<T> data;
    std::map<unsigned long, unsigned long> indexMap;
};

class ComponentManager
{
  public:
    ComponentManager() = default;
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager(ComponentManager&&) = delete;

    ~ComponentManager() = default;

    ComponentManager& operator = (const ComponentManager&) = delete;
    ComponentManager& operator = (ComponentManager&&) = delete;
    
    template <typename... Tps>
    void registerComponents();

    template <typename... Tps>
    void unregisterComponents();

    template <typename... Tps>
    void updateData(unsigned long, Tps&...);

    template <typename... Tps>
    void removeData(unsigned long);

    template <typename T>
    std::optional<T> retrieve(unsigned long);

    template <typename T>
    bool registered() const;
  
  private:
    template <typename T>
    std::shared_ptr<ComponentArray<T>> array() const;

    template <typename T>
    void registerComponent();

    template <typename T>
    void unregisterComponent();

    template <typename T>
    void update(unsigned long, T&);

    template <typename T>
    void remove(unsigned long);

  private:
    std::map<const char *, std::shared_ptr<IComponentArray>> componentMap;

};

} // namespace vecs

#include "src/core/include/components_templates.hpp"

#endif // vecs_core_components_hpp
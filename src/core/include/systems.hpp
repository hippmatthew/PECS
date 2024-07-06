#ifndef vecs_core_systems_hpp
#define vecs_core_systems_hpp

#include "src/core/include/components.hpp"
#include "src/core/include/signature.hpp"

#include <map>
#include <memory>
#include <optional>

namespace vecs
{

class System
{
  public:
    System() = default;
    System(const System&) = default;
    System(System&&) = default;

    virtual ~System() = default;

    System& operator = (const System&) = default;
    System& operator = (System&&) = default;

    virtual void update(const std::shared_ptr<ComponentManager>&, std::set<unsigned long>) = 0;
    
    const Signature& signature() const;
    
    template <typename... Tps>
    void addComponents();

    template <typename... Tps>
    void removeComponents();
  
  private:
    Signature sys_signature;
};

class SystemManager
{
  public:
    SystemManager() = default;
    SystemManager(const SystemManager&) = default;
    SystemManager(SystemManager&&) = default;

    ~SystemManager() = default;

    SystemManager& operator = (const SystemManager&) = default;
    SystemManager& operator = (SystemManager&&) = default;

    template <typename T>
    std::optional<std::shared_ptr<T>> system() const;
    
    template <typename... Tps>
    void emplace();

    template <typename... Tps>
    void erase();

    template <typename T, typename... Tps>
    void addComponents();

    template <typename T, typename... Tps>
    void removeComponents();

  private:
    template <typename T>
    bool registered() const;

    template <typename T>
    void add();

    template <typename T>
    void remove();

  private:
    std::map<const char *, std::shared_ptr<System>> systemMap;
};

} // namespace vecs

#include "src/core/include/systemtemplates.hpp"

#endif // vecs_core_systems_hpp
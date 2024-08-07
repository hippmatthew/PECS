#ifndef vecs_core_extras_hpp
#define vecs_core_extras_hpp

namespace vecs
{

class IComponentArray;
template <typename T> class ComponentArray;
class ComponentManager;
class Device;
class Engine;
class EntityManager;
class GUI;
class Settings;
class Signature;
class System;
class SystemManager;

enum QueueType
{
  Graphics,
  Present,
  SyncCompute,
  SyncTransfer,
  AsyncCompute,
  AsyncTransfer,
  SparseBinding
};

enum FamilyType
{
  All,
  Compute,
  Transfer,
  Async,
  Sparse
};

}

#endif // vecs_core_extras_hpp
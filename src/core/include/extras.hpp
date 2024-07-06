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
class Material;
class GUI;
class Settings;
class Signature;
class Synchronization;
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

enum ShaderType
{
  Vertex,
  Tesselation1,
  Tesselation2,
  Geometry,
  Fragment,
  ComputeShader
};  

}

#endif // vecs_core_extras_hpp
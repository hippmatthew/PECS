#ifndef vecs_core_extras_hpp
#define vecs_core_extras_hpp

namespace vecs
{

class Settings;
class Material;
class GUI;
class Device;
class Synchronization;
class Engine;

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
namespace vecs
{

template <typename T>
unsigned short Settings::component_id()
{
  const char * typeName = typeid(T).name();
  
  if (s_idMap.find(typeName) == s_idMap.end() && nextID != s_maxComponents)
    s_idMap.emplace(std::make_pair(typeName, nextID++));

  return s_idMap.at(typeName);
}

} // namespace vecs
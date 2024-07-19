namespace vecs
{

template <typename... Tps>
void Signature::set()
{
  ( add<Tps>(), ... );
}

template <typename... Tps>
void Signature::unset()
{
  ( remove<Tps>(), ... );
}

template <typename T>
void Signature::add()
{
  bits.set(VECS_SETTINGS.component_id<T>());
}

template <typename T>
void Signature::remove()
{
  bits.set(VECS_SETTINGS.component_id<T>(), false);
}

} // namespace vecs
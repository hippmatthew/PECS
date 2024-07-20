#ifndef vecs_tests_utils_hpp
#define vecs_tests_utils_hpp

#include <string>
#include <vector>

namespace TEST
{

bool compare(const std::vector<const char *>& vec1, const std::vector<const char *>& vec2)
{
  if (vec1.size() != vec2.size()) return false;

  for (unsigned long i = 0; i < vec1.size(); ++i)
    if (std::string(vec1[i]) != vec2[i]) return false;

  return true;
}

} // namespace TEST

#endif // vecs_tests_utils_hpp
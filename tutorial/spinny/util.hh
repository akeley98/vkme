#ifndef UTIL_HH_
#define UTIL_HH_

#include "glm/glm.hpp"
#include <string>

// Name a file that is in the data directory, and return its absolute path.
// Filenames that are already absolute paths (start with /) are unchanged.
std::string expand_filename(const std::string& in);

inline bool is_real(float v)
{
    return v - v == 0;
}

inline bool is_real(double v)
{
    return v - v == 0;
}

inline bool is_real(glm::vec3 v)
{
    return v - v == glm::vec3(0, 0, 0);
}

inline bool is_real(glm::dvec3 v)
{
    return v - v == glm::dvec3(0, 0, 0);
}

#endif

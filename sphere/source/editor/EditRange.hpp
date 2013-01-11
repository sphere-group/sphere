#ifndef EDIT_RANGE_HPP
#define EDIT_RANGE_HPP
#include <string>
#include <vector>
class EditRange {
public:
  static bool OnEditRange(const std::string type, unsigned int id, const bool allow_duplicates, void* data, const int pos);
};
#endif // EDIT_RANGE_HPP

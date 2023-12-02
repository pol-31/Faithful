#ifndef FAITHFUL_LOADPATHMANAGER_H
#define FAITHFUL_LOADPATHMANAGER_H

#include "../../Span.h"

#include <map>

namespace faithful {

// TODO: replace Span by std::string

template <typename T>
class LoadPathManager {
 protected:
  using FileNameType = utility::Span<const char>;
  using FileNameCmpType = utility::SpanSizeComparator<const char>;


  static T* Find(const char *path) {
    std::size_t path_size = 0;
    for (auto i = path; *i != '\0'; ++i) {
      ++path_size;
    }
    utility::Span<const char> new_file(path_size, path);
    auto founded_file = loaded_files_->equal_range(new_file);
    if (founded_file.first == loaded_files_->end())
      return nullptr;
    bool found = false;

    for (auto entry = founded_file.first; entry != founded_file.second; ++entry) {
      for (auto it1 = entry->first.cbegin(), it2 = new_file.cbegin(); it1 != entry->first.cend(); ++it1, ++it2) {
        if (*it1 != *it2) {
          found = false;
          break;
        }
        found = true;
      }
      if (found) return &entry->second;
    }
    return nullptr;
  }

  static std::multimap<FileNameType, T, FileNameCmpType>* loaded_files_;
};

template <typename T>
std::multimap<typename LoadPathManager<T>::FileNameType,
  T,
  typename LoadPathManager<T>::FileNameCmpType>*
  LoadPathManager<T>::loaded_files_ = new std::multimap<
    LoadPathManager<T>::FileNameType,
    T,
    LoadPathManager<T>::FileNameCmpType>;

} // namespace faithful

#endif // FAITHFUL_LOADPATHMANAGER_H

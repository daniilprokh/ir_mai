#ifndef DOCUMENT_H_IR_MAI
#define DOCUMENT_H_IR_MAI

#include <boost/serialization/access.hpp>

#include <filesystem>
#include <functional>
#include <string>

class Document{
  public:
    Document() = default;

    Document(const std::filesystem::path &path) 
        : filename_str_(path.filename().string()) {
      std::string stem = path.stem();
      id_ = std::stoi(stem);
    }

    operator int() const { return id_;} 

    bool operator==(const Document &oth) {
      return id_ == oth.id_;
    }

    bool operator<(const Document &oth) {
      return id_ < oth.id_;
    }

    int Id() const {
      return id_;
    } 

    std::filesystem::path Filename() const {
      return std::filesystem::path(filename_str_);
    }
    
  private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
      ar & filename_str_;
      ar & id_;
    }

    std::string filename_str_;
    int id_{0};
};

inline bool operator==(const Document &lhs, const Document &rhs) {
  return lhs.Id() == rhs.Id();
}

inline bool operator!=(const Document &lhs, const Document &rhs) {
  return !operator==(lhs, rhs);
}

template<> struct std::hash<Document>
{
  std::size_t operator()(const Document &d) const
  {
    return std::hash<int>{}(d.Id());
  }
};

#endif

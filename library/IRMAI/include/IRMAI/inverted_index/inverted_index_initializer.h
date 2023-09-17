#ifndef INVERTED_INDEX_INITIALIZER_H_IR_MAI
#define INVERTED_INDEX_INITIALIZER_H_IR_MAI

#include <IRMAI/inverted_index/inverted_index.h>

#include <optional>

class InvertedIndexInitializer {
  public:
    static InvertedIndex Build(const std::filesystem::path &corpusPath);
    static std::optional<InvertedIndex> Load(
        const std::filesystem::path &indexPath);
    static InvertedIndex LoadOrBuild(const std::filesystem::path &indexPath,
                                     const std::filesystem::path &corpusPath);
};

#endif

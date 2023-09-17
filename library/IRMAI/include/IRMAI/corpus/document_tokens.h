#ifndef DOCUMENT_TOKENS_H_IR_MAI
#define DOCUMENT_TOKENS_H_IR_MAI

#include <IRMAI/corpus/document.h>

#include <vector>

struct DocumentTokens {
  DocumentTokens() = default;

  DocumentTokens(const DocumentTokens&) = default;
  DocumentTokens& operator=(const DocumentTokens&) = default;

  DocumentTokens(DocumentTokens&&) = default;
  DocumentTokens& operator=(DocumentTokens&&) = default;

  DocumentTokens(
      const std::filesystem::path& path,
      std::vector<std::string> tokens)
    : document_(path),
      tokens_(tokens)
  {}

  Document document_;
  std::vector<std::string> tokens_;
};

#endif

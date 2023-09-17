#ifndef CORPUS_HANDLER_H_IR_MAI
#define CORPUS_HANDLER_H_IR_MAI

#include <filesystem>
#include <functional>

#ifdef DEBUG
#include <iostream>
#endif //! DEBUG

struct CorpusHandler {
  using FunctionForHandlingCorpusDocument =
   std::function<void (const std::filesystem::directory_entry &)>;

  static void Pass(
      const std::filesystem::path &corpusPath,
      const FunctionForHandlingCorpusDocument &handlingCorpusDocument) {
    if (!std::filesystem::is_directory(corpusPath)) {
#ifdef DEBUG
      const char *message = "[DEBUG] "
                            "The path of corpus "
                            "doesn't correspond to directory: ";
      std::cout << message << corpusPath << std::endl;
#endif //! DEBUG
      return;
    }

    for (const auto &entry : std::filesystem::directory_iterator{corpusPath}) {
      if (entry.is_regular_file() ||
          entry.path().extension().c_str() != ".txt") {
        handlingCorpusDocument(entry);
      }
    }
  }
};

#endif

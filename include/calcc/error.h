//
// Created by joe on 10/19/16.
//

#ifndef CALCCOMPILER_ERROR_H
#define CALCCOMPILER_ERROR_H

namespace calcc {
  namespace error {
    //! Generic parser error
    struct parser : public std::runtime_error {
      parser(const std::string &msg) : std::runtime_error(msg) {}
    };

    //! Generic scanner error
    struct scanner : public std::runtime_error {
      scanner(const std::string &msg) : std::runtime_error(msg) {}
    };
  }
}

#endif //CALCCOMPILER_ERROR_H

#ifndef DALI_PARSER_H
#define DALI_PARSER_H
#include "nlohmann/json.hpp"
#include "Dali/Dali.hpp"

namespace Dali {

  class Parser {
    public:
      static std::pair<std::shared_ptr<Layout>, std::string> parse(
        const nlohmann::json& m_json);
  };
}

#endif

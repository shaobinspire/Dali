#ifndef DALI_PARSER_H
#define DALI_PARSER_H
#include "nlohmann/json.hpp"
#include "Dali/Dali.hpp"

namespace Dali {
  class Parser {
    public:

      std::shared_ptr<Layout> parse(const std::string& name);

      std::shared_ptr<Layout> parse();

    private:
      nlohmann::json m_json;
  };
}

#endif

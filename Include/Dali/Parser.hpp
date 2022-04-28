#ifndef DALI_PARSER_H
#define DALI_PARSER_H
#include "nlohmann/json.hpp"
#include "valijson_nlohmann_bundled.hpp"
#include "Dali/Dali.hpp"

namespace Dali {

  class Parser {
    public:
      Parser();

      std::pair<std::shared_ptr<Layout>, std::string> parse();

      std::pair<bool, std::string> verify(const std::string& text);

    private:
      nlohmann::json m_json;
      valijson::Schema m_schema;
  };
}

#endif

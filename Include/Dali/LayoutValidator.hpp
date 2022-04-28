#ifndef DALI_LAYOUT_VALIDATOR_H
#define DALI_LAYOUT_VALIDATOR_H
#include "nlohmann/json.hpp"
#include "valijson_nlohmann_bundled.hpp"
#include "Dali/Dali.hpp"

namespace Dali {

  class LayoutValidator {
    public:
      LayoutValidator();

      std::pair<bool, std::string> validate(const nlohmann::json& json);

    private:
      valijson::Schema m_schema;
  };
}

#endif

#ifndef DALI_HPP
#define DALI_HPP
#ifdef _DEBUG
  #include <QDebug>
#endif

namespace Dali {
  const auto LAYOUT_NAME = "layout";

  const auto MAX_LAYOUT_SIZE = (1 << 24) - 1;

  enum class SizePolicy {
    Expanding,
    Fixed
  };

  class Constraint;
  class Constraints;
  class Layout;
  class LayoutBox;
  class LayoutWidget;
  class Parser;
  class Solver;
}

#endif

#ifndef DALI_HPP
#define DALI_HPP
#ifdef _DEBUG
  #include <QDebug>
#endif

namespace Dali {
  const auto layout_name = "layout";

  enum class SizePolicy {
    Expanding,
    Fixed
  };

  class ConstraintGraph;
  class Layout;
  class LayoutBox;
  class LayoutWidget;
}

#endif

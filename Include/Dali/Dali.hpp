#ifndef DALI_HPP
#define DALI_HPP
#ifdef _DEBUG
  #include <QDebug>
#endif

namespace Dali {
  enum class SizePolicy {
    Expanding,
    Fixed
  };

  class ConstraintExpression;
  class ConstraintGraph;
  class Layout;
  class LayoutBox;
  class LayoutWidget;
}

#endif

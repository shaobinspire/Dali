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

  struct LayoutTree;
  struct LayoutTreeNode;
  class Layout;
  class LayoutItem;
  class LayoutWidget;
}

#endif

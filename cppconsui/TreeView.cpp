// Copyright (C) 2007 Mark Pustjens <pustjens@dds.nl>
// Copyright (C) 2010-2015 Petr Pavlu <setup@dagobah.cz>
//
// This file is part of CenterIM.
//
// CenterIM is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// CenterIM is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CenterIM.  If not, see <http://www.gnu.org/licenses/>.

#include "TreeView.h"

#include "ColorScheme.h"

#include <cassert>

namespace CppConsUI {

TreeView::ToggleCollapseButton::ToggleCollapseButton(
  int w, int h, const char *text)
  : Button(w, h, text)
{
}

TreeView::ToggleCollapseButton::ToggleCollapseButton(const char *text)
  : Button(text)
{
}

void TreeView::ToggleCollapseButton::setParent(Container &parent)
{
  TreeView *tree = dynamic_cast<TreeView *>(&parent);
  assert(tree != nullptr);

  Button::setParent(parent);
  signal_activate.connect(
    sigc::hide(sigc::mem_fun(tree, &TreeView::actionToggleCollapsed)));
}

TreeView::TreeView(int w, int h) : Container(w, h)
{
  // Allow fast focus changing (paging) using PageUp/PageDown keys.
  page_focus_ = true;

  // Initialise the tree.
  TreeNode root;
  root.treeview = this;
  root.collapsed = false;
  root.style = STYLE_NORMAL;
  root.widget = nullptr;
  thetree_.set_head(root);
  focus_node_ = thetree_.begin();

  declareBindables();
}

TreeView::~TreeView()
{
  clear();
}

int TreeView::draw(Curses::ViewPort area, Error &error)
{
  area.scroll(scroll_xpos_, scroll_ypos_);

  int attrs;
  DRAW(
    getAttributes(ColorScheme::PROPERTY_CONTAINER_BACKGROUND, &attrs, error));
  DRAW(area.fill(attrs, error));

  int height;
  DRAW(drawNode(thetree_.begin(), &height, area, error));

  return 0;
}

void TreeView::cleanFocus()
{
  Container::cleanFocus();
  focus_node_ = thetree_.begin();
}

bool TreeView::grabFocus()
{
  for (TheTree::pre_order_iterator i = ++thetree_.begin(); i != thetree_.end();
       ++i)
    if (i->widget->grabFocus())
      return true;
  return false;
}

void TreeView::clear()
{
  TheTree::pre_order_iterator root = thetree_.begin();
  while (root.number_of_children() != 0)
    deleteNode(++thetree_.begin(), false);

  // Stay sane.
  assert(children_.empty());
}

bool TreeView::isWidgetVisible(const Widget &child) const
{
  if (parent_ == nullptr || !visible_)
    return false;

  NodeReference node = findNode(child);
  if (!isNodeVisible(node))
    return false;

  return parent_->isWidgetVisible(*this);
}

bool TreeView::setFocusChild(Widget &child)
{
  NodeReference node = findNode(child);
  if (!isNodeVisible(node))
    return false;

  bool res = Container::setFocusChild(child);
  focus_node_ = node;
  return res;
}

void TreeView::getFocusChain(
  FocusChain &focus_chain, FocusChain::iterator parent)
{
  // It is possible that the predecessor of focused node was just made invisible
  // and moveFocus() is called so other widget can take the focus. In this case
  // we find top invisible predecessor of focused node and later the focused
  // node is placed into the focus chain when this predecessor is reached.
  NodeReference act = focus_node_;
  NodeReference top = thetree_.begin();
  while (act != thetree_.begin()) {
    if (!act->widget->isVisible())
      top = act;
    act = thetree_.parent(act);
  }

  // The preorder iterator starts with the root so we must skip it.
  for (TheTree::pre_order_iterator i = ++thetree_.begin(); i != thetree_.end();
       ++i) {
    Widget *widget = i->widget;
    Container *container = dynamic_cast<Container *>(widget);

    if (container != nullptr && container->isVisible()) {
      // The widget is a container so add its widgets as well.
      FocusChain::pre_order_iterator iter =
        focus_chain.append_child(parent, container);
      container->getFocusChain(focus_chain, iter);

      // If this is not a focusable widget and it has no focusable children,
      // remove it from the chain.
      if (focus_chain.number_of_children(iter) == 0)
        focus_chain.erase(iter);
    }
    else if (widget->canFocus() && widget->isVisible()) {
      // Widget can be focused.
      focus_chain.append_child(parent, widget);
    }
    else if (i == top) {
      // This node is the focused node or the focused node is in a subtree of
      // this node.

      Container *focus_cont = dynamic_cast<Container *>(focus_child_);
      if (focus_cont != nullptr) {
        // The focused node is actually a Container. First add the Container,
        // then the focused widget.
        FocusChain::pre_order_iterator iter =
          focus_chain.append_child(parent, focus_cont);
        focus_chain.append_child(iter, focus_cont->getFocusWidget());
      }
      else
        focus_chain.append_child(parent, focus_child_);
    }

    if (i->collapsed || !widget->isVisible())
      i.skip_children();
  }
}

void TreeView::onChildMoveResize(
  Widget &activator, const Rect &oldsize, const Rect &newsize)
{
  // Sanity check.
  assert(newsize.getLeft() == UNSETPOS && newsize.getTop() == UNSETPOS);

  assert(activator.getParent() == this);

  // Only interested in external height changes.
  if (oldsize.getHeight() == newsize.getHeight())
    return;

  updateArea();
}

void TreeView::onChildWishSizeChange(
  Widget &activator, const Size &oldsize, const Size &newsize)
{
  assert(activator.getParent() == this);

  if (activator.getHeight() != AUTOSIZE)
    return;

  if (oldsize.getHeight() == newsize.getHeight())
    return;

  updateArea();
}

void TreeView::onChildVisible(Widget &activator, bool /*visible*/)
{
  assert(activator.getParent() == this);

  // TreeView children should get rarely hidden.
  updateArea();
}

void TreeView::setCollapsed(NodeReference node, bool collapsed)
{
  assert(node->treeview == this);

  if (node->collapsed == collapsed)
    return;

  node->collapsed = collapsed;
  fixFocus();
  updateArea();
  redraw();
}

void TreeView::toggleCollapsed(NodeReference node)
{
  assert(node->treeview == this);

  node->collapsed = !node->collapsed;
  fixFocus();
  updateArea();
  redraw();
}

void TreeView::actionToggleCollapsed()
{
  toggleCollapsed(focus_node_);
}

TreeView::NodeReference TreeView::insertNode(
  NodeReference position, Widget &widget)
{
  assert(position->treeview == this);

  TreeNode node = addNode(widget);
  NodeReference iter = thetree_.insert(position, node);
  addWidget(widget, UNSETPOS, UNSETPOS);
  updateArea();
  return iter;
}

TreeView::NodeReference TreeView::insertNodeAfter(
  NodeReference position, Widget &widget)
{
  assert(position->treeview == this);

  TreeNode node = addNode(widget);
  NodeReference iter = thetree_.insert_after(position, node);
  addWidget(widget, UNSETPOS, UNSETPOS);
  updateArea();
  return iter;
}

TreeView::NodeReference TreeView::prependNode(
  NodeReference parent, Widget &widget)
{
  assert(parent->treeview == this);

  TreeNode node = addNode(widget);
  NodeReference iter = thetree_.prepend_child(parent, node);
  addWidget(widget, UNSETPOS, UNSETPOS);
  updateArea();
  return iter;
}

TreeView::NodeReference TreeView::appendNode(
  NodeReference parent, Widget &widget)
{
  assert(parent->treeview == this);

  TreeNode node = addNode(widget);
  NodeReference iter = thetree_.append_child(parent, node);
  addWidget(widget, UNSETPOS, UNSETPOS);
  updateArea();
  return iter;
}

void TreeView::deleteNode(NodeReference node, bool keepchildren)
{
  assert(node->treeview == this);

  // If we want to keep child nodes we should flatten the tree.
  if (keepchildren)
    thetree_.flatten(node);

  int shrink = 0;
  if (node->widget) {
    int h = node->widget->getHeight();
    if (h == AUTOSIZE) {
      h = node->widget->getWishHeight();
      if (h == AUTOSIZE)
        h = 1;
    }
    shrink += h;
  }

  while (thetree_.number_of_children(node) != 0) {
    TheTree::pre_order_iterator i = thetree_.begin_leaf(node);
    int h = i->widget->getHeight();
    if (h == AUTOSIZE) {
      h = i->widget->getWishHeight();
      if (h == AUTOSIZE)
        h = 1;
    }
    shrink += h;

    // Remove the widget and instantly remove it from the tree.
    removeWidget(*i->widget);
    thetree_.erase(i);
  }

  if (node->widget != nullptr)
    removeWidget(*node->widget);

  thetree_.erase(node);
  updateArea();
  redraw();
}

void TreeView::deleteNodeChildren(NodeReference node, bool keepchildren)
{
  assert(node->treeview == this);

  SiblingIterator i;
  while ((i = node.begin()) != node.end())
    deleteNode(i, keepchildren);
}

TreeView::NodeReference TreeView::getSelectedNode() const
{
  return focus_node_;
}

int TreeView::getNodeDepth(NodeReference node) const
{
  assert(node->treeview == this);

  return thetree_.depth(node);
}

void TreeView::moveNodeBefore(NodeReference node, NodeReference position)
{
  assert(node->treeview == this);
  assert(position->treeview == this);

  if (thetree_.previous_sibling(position) == node)
    return;

  thetree_.move_before(position, node);
  fixFocus();
  updateArea();
  redraw();
}

void TreeView::moveNodeAfter(NodeReference node, NodeReference position)
{
  assert(node->treeview == this);
  assert(position->treeview == this);

  if (thetree_.next_sibling(position) == node)
    return;

  thetree_.move_after(position, node);
  fixFocus();
  updateArea();
  redraw();
}

void TreeView::setNodeParent(NodeReference node, NodeReference newparent)
{
  assert(node->treeview == this);
  assert(newparent->treeview == this);

  if (thetree_.parent(node) == newparent)
    return;

  thetree_.move_ontop(thetree_.append_child(newparent), node);
  fixFocus();
  updateArea();
  redraw();
}

void TreeView::setNodeStyle(NodeReference node, Style s)
{
  assert(node->treeview == this);

  if (node->style == s)
    return;

  node->style = s;
  updateArea();
  redraw();
}

TreeView::Style TreeView::getNodeStyle(NodeReference node) const
{
  assert(node->treeview == this);

  return node->style;
}

void TreeView::updateArea()
{
  repositionChildren(thetree_.begin(), 0, true);

  // Make sure that the currently focused widget is visible.
  updateScroll();
}

int TreeView::drawNode(
  SiblingIterator node, int *out_height, Curses::ViewPort &area, Error &error)
{
  assert(out_height != nullptr);

  int top = 0;
  *out_height = 0;

  // Draw the node Widget first.
  if (node->widget) {
    if (!node->widget->isVisible())
      return 0;

    DRAW(drawChild(*node->widget, area, error));

    top = node->widget->getRealTop();
    int h = node->widget->getRealHeight();
    assert(h != AUTOSIZE);
    *out_height += h;
  }

  // If the node is collapsed or not openable then everything is done.
  if (node->collapsed || !isNodeOpenable(node))
    return 0;

  int depthoffset = thetree_.depth(node) * 2;
  SiblingIterator i;
  int j;

  int attrs;
  DRAW(getAttributes(ColorScheme::PROPERTY_TREEVIEW_LINE, &attrs, error));
  DRAW(area.attrOn(attrs, error));

  for (j = 1; j < *out_height; ++j)
    DRAW(area.addLineChar(depthoffset, top + j, Curses::LINE_VLINE, error));

  // Note: It would be better to start from the end towards the beginning but
  // for some reason it does not seem to work.
  SiblingIterator last = node.begin();
  for (i = node.begin(); i != node.end(); ++i) {
    if (i->widget == nullptr)
      continue;

    if (!i->widget->isVisible())
      continue;

    int h = i->widget->getRealHeight();
    assert(h != AUTOSIZE);
    if (h > 0)
      last = i;
  }
  SiblingIterator end = last;
  ++end;
  for (i = node.begin(); i != end; ++i) {
    if (i != last)
      DRAW(area.addLineChar(
        depthoffset, top + *out_height, Curses::LINE_LTEE, error));
    else
      DRAW(area.addLineChar(
        depthoffset, top + *out_height, Curses::LINE_LLCORNER, error));

    if (i->style == STYLE_NORMAL && isNodeOpenable(i)) {
      const char *c = i->collapsed ? "[+]" : "[-]";
      DRAW(area.addString(depthoffset + 1, top + *out_height, c, error));
    }
    else
      DRAW(area.addLineChar(
        depthoffset + 1, top + *out_height, Curses::LINE_HLINE, error));

    DRAW(area.attrOff(attrs, error));
    int oldh = *out_height;
    int child_height;
    DRAW(drawNode(i, &child_height, area, error));
    *out_height += child_height;
    DRAW(area.attrOn(attrs, error));

    if (i != last)
      for (j = oldh + 1; j < *out_height; ++j)
        DRAW(area.addLineChar(depthoffset, top + j, Curses::LINE_VLINE, error));
  }
  DRAW(area.attrOff(attrs, error));

  return 0;
}

TreeView::TreeNode TreeView::addNode(Widget &widget)
{
  // Make room for this widget.
  int h = widget.getHeight();
  if (h == AUTOSIZE) {
    h = widget.getWishHeight();
    if (h == AUTOSIZE)
      h = 1;
  }

  // Construct the new node.
  TreeNode node;
  node.treeview = this;
  node.collapsed = false;
  node.style = STYLE_NORMAL;
  node.widget = &widget;

  return node;
}

void TreeView::fixFocus()
{
  // This function is called when a widget tree is reorganized (a node was moved
  // in another position in the tree). In this case, it is possible that there
  // can be revealed a widget that could grab the focus (if there is no focused
  // widget yet) or it is also possible that the currently focused widget was
  // hidden by this reorganization (then the focus has to be handled to another
  // widget).

  updateFocusChain();

  Container *t = getTopContainer();
  Widget *focus = t->getFocusWidget();
  if (focus == nullptr) {
    // Try to grab the focus.
    t->moveFocus(FOCUS_DOWN);
  }
  else if (!focus->isVisibleRecursive()) {
    // Move focus.
    t->moveFocus(FOCUS_DOWN);
  }
}

TreeView::NodeReference TreeView::findNode(const Widget &child) const
{
  /// @todo Speed up this algorithm.
  TheTree::pre_order_iterator i;
  for (i = thetree_.begin(); i != thetree_.end(); ++i)
    if (i->widget == &child)
      break;
  assert(i != thetree_.end());
  return i;
}

bool TreeView::isNodeOpenable(SiblingIterator &node) const
{
  for (SiblingIterator i = node.begin(); i != node.end(); ++i) {
    if (!i->widget)
      continue;
    int h = i->widget->getHeight();
    if (h == AUTOSIZE) {
      h = i->widget->getWishHeight();
      if (h == AUTOSIZE)
        h = 1;
    }
    if (h > 0 && i->widget->isVisible())
      return true;
  }
  return false;
}

bool TreeView::isNodeVisible(NodeReference &node) const
{
  // Node is visible if all predecessors are visible and open.
  NodeReference act = node;
  bool first = true;
  while (act != thetree_.begin()) {
    if (!act->widget->isVisible() || (!first && act->collapsed))
      return false;
    first = false;
    act = thetree_.parent(act);
  }
  return true;
}

int TreeView::repositionChildren(SiblingIterator node, int top, bool in_visible)
{
  int height = 0;

  // Position the node Widget first.
  Widget *widget = node->widget;
  if (widget != nullptr) {
    int l = thetree_.depth(node) * 2;
    l += (node->style == STYLE_NORMAL && isNodeOpenable(node)) ? 3 : 1;
    widget->setRealPosition(l, top);

    // Calculate the real width.
    int w = widget->getWidth();
    if (w == AUTOSIZE) {
      w = widget->getWishWidth();
      if (w == AUTOSIZE)
        w = real_width_ - l;
    }
    if (w > real_width_)
      w = real_width_;

    // Calculate the real height.
    int h = widget->getHeight();
    if (h == AUTOSIZE) {
      h = widget->getWishHeight();
      if (h == AUTOSIZE)
        h = 1;
    }

    widget->setRealSize(w, h);

    if (in_visible && widget->isVisible())
      height += h;
  }

  in_visible = in_visible && !node->collapsed && isNodeOpenable(node);

  // Position child nodes.
  int children_height = height;
  for (SiblingIterator i = node.begin(); i != node.end(); ++i)
    children_height += repositionChildren(i, top + children_height, in_visible);

  if (!in_visible)
    assert(children_height == height);

  return children_height;
}

void TreeView::actionCollapse()
{
  setCollapsed(focus_node_, true);
}

void TreeView::actionExpand()
{
  setCollapsed(focus_node_, false);
}

void TreeView::declareBindables()
{
  declareBindable("treeview", "fold-subtree",
    sigc::mem_fun(this, &TreeView::actionCollapse),
    InputProcessor::BINDABLE_NORMAL);
  declareBindable("treeview", "unfold-subtree",
    sigc::mem_fun(this, &TreeView::actionExpand),
    InputProcessor::BINDABLE_NORMAL);
}

} // namespace CppConsUI

// vim: set tabstop=2 shiftwidth=2 textwidth=80 expandtab:

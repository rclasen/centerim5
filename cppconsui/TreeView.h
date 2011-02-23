/*
 * Copyright (C) 2007 by Mark Pustjens <pustjens@dds.nl>
 * Copyright (C) 2010-2011 by CenterIM developers
 *
 * This file is part of CenterIM.
 *
 * CenterIM is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * CenterIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * */

#ifndef __TREEVIEW_H__
#define __TREEVIEW_H__

#include "Button.h"
#include "ScrollPane.h"
#include "LineStyle.h"

#include "tree.hh"

class TreeView
: public ScrollPane
{
protected:
  class TreeNode;

public:
  class ToggleCollapseButton
  : public Button
  {
  public:
    ToggleCollapseButton(int w, int h, const char *text_ = NULL);
    explicit ToggleCollapseButton(const char *text_ = NULL);
    virtual ~ToggleCollapseButton() {}

    // Widget
    virtual void SetParent(Container& parent);

  protected:

  private:
    ToggleCollapseButton(const ToggleCollapseButton&);
    ToggleCollapseButton& operator=(const ToggleCollapseButton&);
  };

  /**
   * Node drawing style.
   */
  enum Style {
    STYLE_NORMAL, ///< Draw "[+]" if the node is collapsed.
    STYLE_VOID ///< Don't draw any extra information.
  };

  typedef tree<TreeNode> TheTree;
  typedef TheTree::pre_order_iterator NodeReference;
  typedef TheTree::sibling_iterator SiblingIterator;

  TreeView(int w, int h, LineStyle::Type ltype = LineStyle::DEFAULT);
  virtual ~TreeView();

  // Widget
  virtual void Draw();
  virtual void CleanFocus();
  virtual bool GrabFocus();

  // Container
  virtual bool IsWidgetVisible(const Widget& widget) const;
  virtual bool SetFocusChild(Widget& child);
  virtual void GetFocusChain(FocusChain& focus_chain,
      FocusChain::iterator parent);
  virtual Curses::Window *GetSubPad(const Widget& child, int begin_x,
      int begin_y, int ncols, int nlines);

  /**
   * Folds given node.
   */
  void Collapse(const NodeReference node);
  /**
   * Unfolds given node.
   */
  void Expand(const NodeReference node);
  /**
   * Toggles folding for given node.
   */
  void ToggleCollapsed(const NodeReference node);
  /**
   * Convenient method to toggle folding of the current active node.
   */
  void ActionToggleCollapsed();

  /**
   * Returns root node reference.
   */
  NodeReference Root() const { return thetree.begin(); }

  /**
   * Inserts a widget before a specified position. TreeView takes ownership of
   * the widget.
   */
  NodeReference InsertNode(const NodeReference position, Widget& widget);
  /**
   * Inserts a widget after a specified position. TreeView takes ownership of
   * the widget.
   */
  NodeReference InsertNodeAfter(const NodeReference position, Widget& widget);
  /**
   * Prepends a widget to a specified parent. TreeView takes ownership of the
   * widget.
   */
  NodeReference PrependNode(const NodeReference parent, Widget& widget);
  /**
   * Appends a widget to a specified parent. TreeView takes ownership of the
   * widget.
   */
  NodeReference AppendNode(const NodeReference parent, Widget& widget);

  /**
   * Deletes given node.
   */
  void DeleteNode(const NodeReference node, bool keepchildren);
  /**
   * Deletes all children of given node.
   */
  void DeleteChildren(const NodeReference node, bool keepchildren);

  /**
   * Returns reference to currently focused node/widget.
   */
  const NodeReference GetSelected() const;

  /**
   * Returns node depth.
   */
  int GetDepth(const NodeReference node) const;

  /**
   * Detaches a given node from its current location and moves it before
   * a given position.
   */
  void MoveBefore(const NodeReference node, const NodeReference position);
  /**
   * Detaches a given node from its current location and moves it after
   * a given position.
   */
  void MoveAfter(const NodeReference node, const NodeReference position);
  /**
   * Detaches a given node from its current location and appents it to a new
   * parent.
   */
  void SetParent(const NodeReference node, const NodeReference newparent);
  void SetStyle(const NodeReference node, Style s);
  Style GetStyle(const NodeReference node) const;

protected:
  class TreeNode
  {
  /* Note: If TreeNode is just protected/private and all its variables are
   * public, then variables can be accessed from outside using NodeReference.
   */
  friend class TreeView;

  public:
    TreeView *GetTreeView() const { return treeview; }
    bool GetOpen() const { return open; }
    Style GetStyle() const { return style; }
    Widget *GetWidget() const { return widget; }

  protected:

  private:
    /**
     * Pointer to TreeView this node belongs to.
     */
    TreeView *treeview;

    /**
     * Flag whether the subtree is unfolded.
     */
    bool open;

    /**
     * Selected node drawing style.
     */
    Style style;

    /**
     * Widget to show. Not const because width is changed to fit. E.g. labels
     * can show `...' when the text does not fit in the given space.
     */
    Widget *widget;
  };

  TheTree thetree;
  NodeReference focus_node;
  LineStyle linestyle;

  // Container
  virtual void AddWidget(Widget& widget, int x, int y);
  virtual void RemoveWidget(Widget& widget);
  virtual void Clear(); ///< @todo Implemement.

  int DrawNode(SiblingIterator node, int top);

  TreeNode AddNode(Widget& widget);

  void FixFocus();

  NodeReference FindNode(const Widget& child) const;

  bool IsNodeOpenable(const SiblingIterator& node) const;
  bool IsNodeVisible(const NodeReference& node) const;

  // handlers of signals
  virtual void OnChildMoveResize(Widget& activator, const Rect& oldsize,
      const Rect& newsize);

private:
  TreeView(const TreeView&);
  TreeView& operator=(const TreeView&);

  void ActionCollapse();
  void ActionExpand();

  void DeclareBindables();
};

#endif // __TREEVIEW_H__

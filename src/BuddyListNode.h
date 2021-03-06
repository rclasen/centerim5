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

#ifndef BUDDYLISTNODE_H
#define BUDDYLISTNODE_H

#include <cppconsui/Button.h>
#include <cppconsui/InputDialog.h>
#include <cppconsui/MenuWindow.h>
#include <cppconsui/MessageDialog.h>
#include <cppconsui/TreeView.h>
#include <libpurple/purple.h>

class BuddyListNode : public CppConsUI::Button {
public:
  static BuddyListNode *createNode(PurpleBlistNode *node);

  // Widget
  virtual void setParent(CppConsUI::Container &parent) override;

  virtual bool lessOrEqual(const BuddyListNode &other) const = 0;
  virtual void update();
  virtual void onActivate(CppConsUI::Button &activator) = 0;
  // Debugging method.
  virtual const char *toString() const = 0;

  virtual void setRefNode(CppConsUI::TreeView::NodeReference n);
  virtual CppConsUI::TreeView::NodeReference getRefNode() const { return ref_; }

  PurpleBlistNode *getPurpleBlistNode() const { return blist_node_; }

  // Sorts in this node.
  void sortIn();

  BuddyListNode *getParentNode() const;

protected:
  class ContextMenu : public CppConsUI::MenuWindow {
  public:
    ContextMenu(BuddyListNode &parent_node);
    virtual ~ContextMenu() override {}

  protected:
    BuddyListNode *parent_node_;

    void onMenuAction(Button &activator, PurpleCallback callback, void *data);
    void appendMenuAction(MenuWindow &menu, PurpleMenuAction *act);
    void appendProtocolMenu(PurpleConnection *gc);
    void appendExtendedMenu();

  private:
    CONSUI_DISABLE_COPY(ContextMenu);
  };

  CppConsUI::TreeView *treeview_;
  CppConsUI::TreeView::NodeReference ref_;

  PurpleBlistNode *blist_node_;

  // Cached value of purple_blist_node_get_int(blist_node, "last_activity").
  int last_activity_;

  BuddyListNode(PurpleBlistNode *node_);
  virtual ~BuddyListNode();

  virtual void openContextMenu() = 0;

  bool lessOrEqualByType(const BuddyListNode &other) const;
  bool lessOrEqualByBuddySort(PurpleBuddy *left, PurpleBuddy *right) const;

  // Called by BuddyListBuddy and BuddyListContact to get presence status char.
  // Returned value should be used as a prefix of buddy/contact name.
  const char *getBuddyStatus(PurpleBuddy *buddy) const;

  // Returns weight of buddy status (available > away > offline...). Used for
  // sorting.
  int getBuddyStatusWeight(PurpleBuddy *buddy) const;

  int getColorSchemeByBuddy(int base_scheme, PurpleBuddy *buddy);

  void updateFilterVisibility(const char *name);

  void retrieveUserInfoForName(PurpleConnection *gc, const char *name) const;

private:
  CONSUI_DISABLE_COPY(BuddyListNode);

  void actionOpenContextMenu();
  void declareBindables();
};

class BuddyListBuddy : public BuddyListNode {
  friend class BuddyListNode;

public:
  // BuddyListNode
  virtual bool lessOrEqual(const BuddyListNode &other) const override;
  virtual void update() override;
  virtual void onActivate(Button &activator) override;
  virtual const char *toString() const override;

  PurpleBuddy *getPurpleBuddy() const { return buddy_; }
  void retrieveUserInfo();

protected:
  class BuddyContextMenu : public ContextMenu {
  public:
    BuddyContextMenu(BuddyListBuddy &parent_buddy);
    virtual ~BuddyContextMenu() override {}

  protected:
    BuddyListBuddy *parent_buddy_;

    void onInformation(Button &activator);

    void changeAliasResponseHandler(CppConsUI::InputDialog &activator,
      CppConsUI::AbstractDialog::ResponseType response);
    void onChangeAlias(Button &activator);

    void removeResponseHandler(CppConsUI::MessageDialog &activator,
      CppConsUI::AbstractDialog::ResponseType response);
    void onRemove(Button &activator);

  private:
    CONSUI_DISABLE_COPY(BuddyContextMenu);
  };

  PurpleBuddy *buddy_;

  // Widget
  virtual int getAttributes(int property, int subproperty, int *attrs,
    CppConsUI::Error &error) const override;

  // BuddyListNode
  virtual void openContextMenu() override;

  void updateColorScheme();

private:
  BuddyListBuddy(PurpleBlistNode *node);
  virtual ~BuddyListBuddy() override {}
  CONSUI_DISABLE_COPY(BuddyListBuddy);
};

class BuddyListChat : public BuddyListNode {
  friend class BuddyListNode;

public:
  // BuddyListNode
  virtual bool lessOrEqual(const BuddyListNode &other) const override;
  virtual void update() override;
  virtual void onActivate(Button &activator) override;
  virtual const char *toString() const override;

  PurpleChat *getPurpleChat() const { return chat_; }

protected:
  class ChatContextMenu : public ContextMenu {
  public:
    ChatContextMenu(BuddyListChat &parent_chat);
    virtual ~ChatContextMenu() override {}

  protected:
    BuddyListChat *parent_chat_;

    void changeAliasResponseHandler(CppConsUI::InputDialog &activator,
      CppConsUI::AbstractDialog::ResponseType response);
    void onChangeAlias(Button &activator);

    void removeResponseHandler(CppConsUI::MessageDialog &activator,
      CppConsUI::AbstractDialog::ResponseType response);
    void onRemove(Button &activator);

  private:
    CONSUI_DISABLE_COPY(ChatContextMenu);
  };

  PurpleChat *chat_;

  // BuddyListNode
  virtual void openContextMenu() override;

private:
  BuddyListChat(PurpleBlistNode *node);
  virtual ~BuddyListChat() override {}
  CONSUI_DISABLE_COPY(BuddyListChat);
};

class BuddyListContact : public BuddyListNode {
  friend class BuddyListNode;

public:
  // BuddyListNode
  virtual bool lessOrEqual(const BuddyListNode &other) const override;
  virtual void update() override;
  virtual void onActivate(Button &activator) override;
  virtual const char *toString() const override;
  virtual void setRefNode(CppConsUI::TreeView::NodeReference n) override;

  PurpleContact *getPurpleContact() const { return contact_; }
  void retrieveUserInfo();

  bool isCollapsed() const { return ref_->isCollapsed(); }
  void setCollapsed(bool collapsed)
  {
    treeview_->setCollapsed(ref_, collapsed);
  }

protected:
  class ContactContextMenu : public ContextMenu {
  public:
    ContactContextMenu(BuddyListContact &parent_contact);
    virtual ~ContactContextMenu() override {}

  protected:
    BuddyListContact *parent_contact_;

    void onExpandRequest(Button &activator, bool expand);
    void onInformation(Button &activator);

    void changeAliasResponseHandler(CppConsUI::InputDialog &activator,
      CppConsUI::AbstractDialog::ResponseType response);
    void onChangeAlias(Button &activator);

    void removeResponseHandler(CppConsUI::MessageDialog &activator,
      CppConsUI::AbstractDialog::ResponseType response);
    void onRemove(Button &activator);

    void onMoveTo(Button &activator, PurpleGroup *group);

  private:
    CONSUI_DISABLE_COPY(ContactContextMenu);
  };

  PurpleContact *contact_;

  // Widget
  virtual int getAttributes(int property, int subproperty, int *attrs,
    CppConsUI::Error &error) const override;

  // BuddyListNode
  virtual void openContextMenu() override;

  void updateColorScheme();

private:
  BuddyListContact(PurpleBlistNode *node);
  virtual ~BuddyListContact() override {}
  CONSUI_DISABLE_COPY(BuddyListContact);
};

class BuddyListGroup : public BuddyListNode {
  friend class BuddyListNode;

public:
  // BuddyListNode
  virtual bool lessOrEqual(const BuddyListNode &other) const override;
  virtual void update() override;
  virtual void onActivate(Button &activator) override;
  virtual const char *toString() const override;
  virtual void setRefNode(CppConsUI::TreeView::NodeReference n) override;

  PurpleGroup *getPurpleGroup() const { return group_; }

  void initCollapsedState();

protected:
  class GroupContextMenu : public ContextMenu {
  public:
    GroupContextMenu(BuddyListGroup &parent_group);
    virtual ~GroupContextMenu() override {}

  protected:
    BuddyListGroup *parent_group_;

    void renameResponseHandler(CppConsUI::InputDialog &activator,
      CppConsUI::AbstractDialog::ResponseType response);
    void onRename(Button &activator);

    void removeResponseHandler(CppConsUI::MessageDialog &activator,
      CppConsUI::AbstractDialog::ResponseType response);
    void onRemove(Button &activator);

    void onMoveAfter(Button &activator, PurpleGroup *group);

  private:
    CONSUI_DISABLE_COPY(GroupContextMenu);
  };

  PurpleGroup *group_;

  // BuddyListNode
  virtual void openContextMenu() override;

private:
  BuddyListGroup(PurpleBlistNode *node);
  virtual ~BuddyListGroup() override {}
  CONSUI_DISABLE_COPY(BuddyListGroup);
};

#endif // BUDDYLISTNODE_H

// vim: set tabstop=2 shiftwidth=2 textwidth=80 expandtab:

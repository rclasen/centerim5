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

#ifndef GENERALMENU_H
#define GENERALMENU_H

#include "config.h"
#include <cppconsui/MenuWindow.h>
#include <libpurple/purple.h>

class GeneralMenu : public CppConsUI::MenuWindow {
public:
  GeneralMenu();
  virtual ~GeneralMenu() override {}

  // FreeWindow
  virtual void onScreenResized() override;

private:
  CONSUI_DISABLE_COPY(GeneralMenu);

  void openStatusWindow(CppConsUI::Button &activator);
  void openAccountWindow(CppConsUI::Button &activator);
  void openAddBuddyRequest(CppConsUI::Button &activator);
  void openAddChatRequest(CppConsUI::Button &activator);
  void openAddGroupRequest(CppConsUI::Button &activator);
  void openPendingRequests(CppConsUI::Button &activator);
  void openOptionWindow(CppConsUI::Button &activator);
  void openPluginWindow(CppConsUI::Button &activator);

#ifdef DEBUG
  void openRequestInputTest(CppConsUI::Button &activator);
  void openRequestChoiceTest(CppConsUI::Button &activator);
  void openRequestActionTest(CppConsUI::Button &activator);
  void openRequestFieldsTest(CppConsUI::Button &activator);

  static void input_ok_cb_(void *data, const char *text)
  {
    reinterpret_cast<GeneralMenu *>(data)->input_ok_cb(text);
  }
  void input_ok_cb(const char *text);

  static void choice_ok_cb_(void *data, int selected)
  {
    reinterpret_cast<GeneralMenu *>(data)->choice_ok_cb(selected);
  }
  void choice_ok_cb(int selected);

  static void action_cb_(void *data, int action)
  {
    reinterpret_cast<GeneralMenu *>(data)->action_cb(action);
  }
  void action_cb(int action);

  static void fields_ok_cb_(void *data, PurpleRequestFields *fields)
  {
    reinterpret_cast<GeneralMenu *>(data)->fields_ok_cb(fields);
  }
  void fields_ok_cb(PurpleRequestFields *fields);
#endif // DEBUG
};

#endif // GENERALMENU_H

// vim: set tabstop=2 shiftwidth=2 textwidth=80 expandtab:

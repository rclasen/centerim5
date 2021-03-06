// Copyright (C) 2011-2015 Petr Pavlu <setup@dagobah.cz>
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

/// @file
/// AbstractDialog class implementation.
///
/// @ingroup cppconsui

#include "AbstractDialog.h"

namespace CppConsUI {

AbstractDialog::AbstractDialog(int x, int y, int w, int h, const char *title)
  : Window(x, y, w, h, title, TYPE_TOP)
{
  initLayout();
}

AbstractDialog::AbstractDialog(const char *title)
  : Window(10, 10, 60, 12, title, TYPE_TOP)
{
  initLayout();
}

void AbstractDialog::close()
{
  response(RESPONSE_CANCEL);
}

void AbstractDialog::addButton(
  const char *text, AbstractDialog::ResponseType response)
{
  buttons_->appendItem(
    text, sigc::bind(
            sigc::mem_fun(this, &AbstractDialog::onButtonResponse), response));
}

void AbstractDialog::addSeparator()
{
  buttons_->appendSeparator();
}

void AbstractDialog::response(ResponseType response_type)
{
  emitResponse(response_type);
  Window::close();
}

void AbstractDialog::initLayout()
{
  layout_ = new ListBox(AUTOSIZE, AUTOSIZE);
  addWidget(*layout_, 1, 1);

  separator_ = new HorizontalLine(AUTOSIZE);
  layout_->appendWidget(*separator_);
  buttons_ = new HorizontalListBox(AUTOSIZE, 1);
  layout_->appendWidget(*buttons_);
}

void AbstractDialog::onButtonResponse(
  Button & /*activator*/, ResponseType response_type)
{
  response(response_type);
}

} // namespace CppConsUI

// vim: set tabstop=2 shiftwidth=2 textwidth=80 expandtab:

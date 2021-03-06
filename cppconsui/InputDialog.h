// Copyright (C) 2008 Mark Pustjens <pustjens@dds.nl>
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

/// @file
/// InputDialog class.
///
/// @ingroup cppconsui

#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include "AbstractDialog.h"
#include "TextEntry.h"

namespace CppConsUI {

class InputDialog : public AbstractDialog {
public:
  InputDialog(const char *title, const char *defaultvalue);
  virtual ~InputDialog() override {}

  virtual void setText(const char *new_text) { entry_->setText(new_text); }
  virtual const char *getText() const { return entry_->getText(); }

  virtual void setFlags(int new_flags) { entry_->setFlags(new_flags); }
  virtual int getFlags() const { return entry_->getFlags(); }

  virtual void setMasked(bool new_masked) { entry_->setMasked(new_masked); }
  virtual bool isMasked() const { return entry_->isMasked(); }

  /// Signal emitted when user closes the dialog.
  sigc::signal<void, InputDialog &, ResponseType> signal_response;

protected:
  TextEntry *entry_;

  // AbstractDialog
  virtual void emitResponse(ResponseType response) override;

private:
  CONSUI_DISABLE_COPY(InputDialog);
};

} // namespace CppConsUI

#endif // INPUTDIALOG_H

// vim: set tabstop=2 shiftwidth=2 textwidth=80 expandtab:

/*
 * Copyright (C) 2007 by Mark Pustjens <pustjens@dds.nl>
 * Copyright (C) 2010 by CenterIM developers
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

/**
 * @file
 * Dialog class implementation.
 *
 * @ingroup cppconsui
 */

#include "Dialog.h"

Dialog::Dialog(int x, int y, int w, int h, LineStyle::Type ltype)
: Window(x, y, w, h, TYPE_TOP, ltype)
{
	AddWidgets();
}

Dialog::Dialog()
: Window(0, 0, 1, 1, TYPE_TOP)
{
	//TODO set correct position.
	MoveResize(10, 10, 60, 12);

	AddWidgets();
}

void Dialog::AddWidgets()
{
	layout = new ListBox(AUTOSIZE, AUTOSIZE);
	AddWidget(*layout, 0, 0);

	separator = new HorizontalLine(AUTOSIZE);
	layout->AppendWidget(*separator);
	buttons = new HorizontalListBox(AUTOSIZE, 1);
	layout->AppendWidget(*buttons);
}

void Dialog::AddButton(const gchar *text, Dialog::ResponseType response)
{
	buttons->AppendItem(text, sigc::bind(sigc::mem_fun(this, &Dialog::Response), response));
}

void Dialog::Close()
{
	Response(RESPONSE_CANCEL);
}

void Dialog::Response(Dialog::ResponseType response)
{
	signal_response(response);

	Window::Close();
}

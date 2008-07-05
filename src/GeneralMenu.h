/*
 * Copyright (C) 2007 by Mark Pustjens <pustjens@dds.nl>
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

#ifndef __GENERALMENU_H__
#define __GENERALMENU_H__

#include <cppconsui/MenuWindow.h>

class GeneralMenu
: public MenuWindow
{
	public:
		GeneralMenu(int x, int y, int w, int h, LineStyle *linestyle);
		virtual ~GeneralMenu();

		void Dummy(void) { ; } //TODO remove
	protected:

	private:
		GeneralMenu();
		GeneralMenu(const GeneralMenu&);

		GeneralMenu& operator=(const GeneralMenu&);

		void OpenAccountsWindow(void);
		void OpenTestWindow(void);
};

#endif /* __GENERALMENU_H__ */
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

#ifndef __TEXTBUFFER_H__
#define __TEXTBUFFER_H__

#include "config.h"

#include "TextRBTree.h"

class TextBuffer
{
	public:
		typedef TextRBTree::char_iterator iterator;

		TextBuffer();
		~TextBuffer();

		iterator insert (const iterator iter, const char *text, int len);
		void insert (const char *text, int len);

	protected:

	private:
		TextBuffer(const TextBuffer &);

		TextBuffer& operator=(const TextBuffer&);

		TextRBTree tree;
		iterator cursor;
};

#endif /* __TEXTBUFFER_H__ */

/*
 * Extended BList Sort
 * Copyright (C) 2008 Konrad Gräfe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1301, USA.
 */

#ifndef _SORT_METHODS_H
#define _SORT_METHODS_H

enum sort_methods {
	SORT_METHOD_NOTHING,
	SORT_METHOD_NAME,
	SORT_METHOD_STATUS,
	SORT_METHOD_ONOFFLINE,
	SORT_METHOD_PROTOCOL,
	SORT_METHOD_PRIORITY,
	SORT_METHOD_ONOFFLINETIME,
	SORT_METHOD_LOGSIZE,
	SORT_METHOD_ACCOUNT
};

enum priorities{
	PRIORITY_UNDEFINED,
	PRIORITY_VERY_LOW,
	PRIORITY_LOW,
	PRIORITY_NORMAL,
	PRIORITY_HIGH,
	PRIORITY_VERY_HIGH
};

void init_sort_methods(void);
void uninit_sort_methods(void);

#endif /* _SORT_METHODS_H */


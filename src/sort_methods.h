/* Copyright (C) 2008-2019 Konrad Gräfe <konradgraefe@aol.com>
 *
 * This software may be modified and distributed under the terms
 * of the GPL2.0 license. See the COPYING file for details.
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
	SORT_METHOD_ACCOUNT,
	SORT_METHOD_LAST_NAME
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


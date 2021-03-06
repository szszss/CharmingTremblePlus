/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        wcsicmp.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#ifdef TB_CONFIG_LIBC_HAVE_WCSICMP
#   include <wchar.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */
#ifdef TB_CONFIG_LIBC_HAVE_WCSICMP
tb_long_t tb_wcsicmp(tb_wchar_t const* s1, tb_wchar_t const* s2)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, 0);
#   ifdef TB_COMPILER_IS_MSVC
    return _wcsicmp(s1, s2);
#   else
    return wcscasecmp(s1, s2);
#   endif
}
#else
tb_long_t tb_wcsicmp(tb_wchar_t const* s1, tb_wchar_t const* s2)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, 0);
    tb_check_return_val(s1 != s2, 0);

    // done
    while ((*s1 == *s2) || (tb_tolower(*s1) == tb_tolower(*s2)))
    {
        if (!*s1++) return 0;
        ++s2;
    }
    return (((tb_wchar_t)tb_tolower(*s1)) < ((tb_wchar_t)tb_tolower(*s2))) ? -1 : 1;
}
#endif

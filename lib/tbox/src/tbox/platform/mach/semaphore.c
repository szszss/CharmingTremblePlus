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
 * @file        semaphore.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <time.h>
#include <errno.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#include <mach/mach.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the semaphore type
typedef struct __tb_semaphore_impl_t
{
    // the semaphore
    semaphore_t         semaphore;

    // the value
    tb_atomic_t         value;

}tb_semaphore_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_semaphore_ref_t tb_semaphore_init(tb_size_t init)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_semaphore_impl_t*    impl = tb_null;
    do
    {
        // make semaphore
        impl = tb_malloc0_type(tb_semaphore_impl_t);
        tb_assert_and_check_break(impl);

        // init semaphore 
        if (KERN_SUCCESS != semaphore_create(mach_task_self(), &(impl->semaphore), SYNC_POLICY_FIFO, init)) break;

        // init value
        impl->value = init;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_semaphore_exit((tb_semaphore_ref_t)impl);
        impl = tb_null;
    }

    // ok
    return (tb_semaphore_ref_t)impl;
}
tb_void_t tb_semaphore_exit(tb_semaphore_ref_t semaphore)
{
    tb_semaphore_impl_t* impl = (tb_semaphore_impl_t*)semaphore;
    if (semaphore) 
    {
        // exit semaphore
        semaphore_destroy(mach_task_self(), impl->semaphore);

        // exit it
        tb_free(semaphore);
    }
}
tb_bool_t tb_semaphore_post(tb_semaphore_ref_t semaphore, tb_size_t post)
{
    // check
    tb_semaphore_impl_t* impl = (tb_semaphore_impl_t*)semaphore;
    tb_assert_and_check_return_val(semaphore && post, tb_false);

    // post
    while (post--)
    {
        // +2 first
        tb_atomic_fetch_and_add(&impl->value, 2);

        // signal
        if (KERN_SUCCESS != semaphore_signal(impl->semaphore)) 
        {
            // restore
            tb_atomic_fetch_and_sub(&impl->value, 2);
            return tb_false;
        }

        // -1
        tb_atomic_fetch_and_dec(&impl->value);
    }

    // ok
    return tb_true;
}
tb_long_t tb_semaphore_value(tb_semaphore_ref_t semaphore)
{
    // check
    tb_semaphore_impl_t* impl = (tb_semaphore_impl_t*)semaphore;
    tb_assert_and_check_return_val(semaphore, -1);

    // get value
    return (tb_long_t)tb_atomic_get(&impl->value);
}
tb_long_t tb_semaphore_wait(tb_semaphore_ref_t semaphore, tb_long_t timeout)
{
    // check
    tb_semaphore_impl_t* impl = (tb_semaphore_impl_t*)semaphore;
    tb_assert_and_check_return_val(semaphore, -1);

    // init timespec
    mach_timespec_t spec = {0};
    if (timeout > 0)
    {
        spec.tv_sec += timeout / 1000;
        spec.tv_nsec += (timeout % 1000) * 1000000;
    }
    else if (timeout < 0) spec.tv_sec += 12 * 30 * 24 * 3600; // infinity: one year

    // wait
    tb_long_t ok = semaphore_timedwait(impl->semaphore, spec);

    // timeout?
    tb_check_return_val(ok != KERN_OPERATION_TIMED_OUT, 0);

    // ok?
    tb_check_return_val(ok == KERN_SUCCESS, -1);

    // check value
    tb_assert_and_check_return_val((tb_long_t)tb_atomic_get(&impl->value) > 0, -1);
    
    // value--
    tb_atomic_fetch_and_dec(&impl->value);
    
    // ok
    return 1;
}



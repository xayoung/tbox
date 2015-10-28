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
 * @file        static_pool.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "static_pool"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_pool.h"
#include "impl/static_large_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_static_pool_ref_t tb_static_pool_init(tb_byte_t* data, tb_size_t size)
{
    // init pool
    return tb_static_large_pool_init(data, size, 8);
}
tb_void_t tb_static_pool_exit(tb_static_pool_ref_t pool)
{
    // check
    tb_assert_and_check_return(pool);

    // exit pool
    tb_static_large_pool_exit(pool);
}
tb_void_t tb_static_pool_clear(tb_static_pool_ref_t pool)
{
    // check
    tb_assert_and_check_return(pool);

    // clear pool
    tb_static_large_pool_clear(pool);
}
tb_pointer_t tb_static_pool_malloc_(tb_static_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // malloc data
    tb_pointer_t data = tb_static_large_pool_malloc(pool, size, tb_null __tb_debug_args__);
    tb_assertf_abort(data, "malloc(%lu) failed!", size);
    tb_assertf_abort(!(((tb_size_t)data) & (TB_POOL_DATA_ALIGN - 1)), "malloc(%lu): unaligned data: %p", size, data);

    // ok
    return data;
}
tb_pointer_t tb_static_pool_malloc0_(tb_static_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // malloc0 data
    tb_pointer_t data = tb_static_large_pool_malloc(pool, size, tb_null __tb_debug_args__);
    tb_assertf_abort(data, "malloc0(%lu) failed!", size);
    tb_assertf_abort(!(((tb_size_t)data) & (TB_POOL_DATA_ALIGN - 1)), "malloc0(%lu): unaligned data: %p", size, data);

    // clear it
    tb_memset_(data, 0, size);

    // ok
    return data;
}
tb_pointer_t tb_static_pool_nalloc_(tb_static_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && size, tb_null);
    tb_assert_and_check_return_val((item * size) <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // nalloc data
    tb_pointer_t data = tb_static_large_pool_malloc(pool, item * size, tb_null __tb_debug_args__);
    tb_assertf_abort(data, "nalloc(%lu, %lu) failed!", item, size);
    tb_assertf_abort(!(((tb_size_t)data) & (TB_POOL_DATA_ALIGN - 1)), "nalloc(%lu, %lu): unaligned data: %p", item, size, data);

    // ok
    return data;
}
tb_pointer_t tb_static_pool_nalloc0_(tb_static_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && size, tb_null);
    tb_assert_and_check_return_val((item * size) <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // nalloc0 data
    tb_pointer_t data = tb_static_large_pool_malloc(pool, item * size, tb_null __tb_debug_args__);
    tb_assertf_abort(data, "nalloc0(%lu, %lu) failed!", item, size);
    tb_assertf_abort(!(((tb_size_t)data) & (TB_POOL_DATA_ALIGN - 1)), "nalloc0(%lu, %lu): unaligned data: %p", item, size, data);

    // clear it
    tb_memset_(data, 0, item * size);

    // ok
    return data;
}
tb_pointer_t tb_static_pool_ralloc_(tb_static_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && data && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // ralloc data
    tb_pointer_t data_new = tb_static_large_pool_ralloc(pool, data, size, tb_null __tb_debug_args__);

    // failed? dump it
#ifdef __tb_debug__
    if (!data_new) 
    {
        // trace
        tb_trace_e("ralloc(%p, %lu) failed! at %s(): %lu, %s", data, size, func_, line_, file_);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)data, tb_true, "[static_pool]: [error]: ");

        // abort
        tb_abort();
    }
#endif

    // check
    tb_assertf_abort(!(((tb_size_t)data_new) & (TB_POOL_DATA_ALIGN - 1)), "ralloc(%lu): unaligned data: %p", size, data);

    // ok
    return data_new;
}
tb_bool_t tb_static_pool_free_(tb_static_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && data, tb_false);

    // free data
    tb_bool_t ok = tb_static_large_pool_free(pool, data __tb_debug_args__);

    // failed? dump it
#ifdef __tb_debug__
    if (!ok) 
    {
        // trace
        tb_trace_e("free(%p) failed! at %s(): %lu, %s", data, func_, line_, file_);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)data, tb_true, "[static_pool]: [error]: ");

        // abort
        tb_abort();
    }
#endif

    // ok
    return ok;
}
#ifdef __tb_debug__
tb_void_t tb_static_pool_dump(tb_static_pool_ref_t pool)
{
    // check
    tb_assert_and_check_return(pool);

    // dump the pool
    return tb_static_large_pool_dump(pool);
}
#endif

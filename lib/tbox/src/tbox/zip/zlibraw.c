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
 * @file        zlibraw.c
 * @ingroup     zip
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "zlibraw"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "zlibraw.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implements
 */
static __tb_inline__ tb_zip_zlibraw_t* tb_zip_zlibraw_cast(tb_zip_ref_t zip)
{
    tb_assert_and_check_return_val(zip && zip->algo == TB_ZIP_ALGO_ZLIBRAW, tb_null);
    return (tb_zip_zlibraw_t*)zip;
}
static tb_long_t tb_zip_zlibraw_spak_deflate(tb_zip_ref_t zip, tb_static_stream_ref_t ist, tb_static_stream_ref_t ost, tb_long_t sync)
{
    tb_zip_zlibraw_t* zlibraw = tb_zip_zlibraw_cast(zip);
    tb_assert_and_check_return_val(zlibraw && ist && ost, -1);

    // the input stream
    tb_byte_t* ip = ist->p;
    tb_byte_t* ie = ist->e;
    tb_check_return_val(ip && ip < ie, 0);

    // the output stream
    tb_byte_t* op = ost->p;
    tb_byte_t* oe = ost->e;
    tb_assert_and_check_return_val(op && oe, -1);

    // attach zst
    zlibraw->zst.next_in = (Bytef*)ip;
    zlibraw->zst.avail_in = (uInt)(ie - ip);

    zlibraw->zst.next_out = (Bytef*)op;
    zlibraw->zst.avail_out = (uInt)(oe - op);

    // deflate 
    tb_int_t r = deflate(&zlibraw->zst, !sync? Z_NO_FLUSH : Z_SYNC_FLUSH);
    tb_assertf_and_check_return_val(r == Z_OK || r == Z_STREAM_END, -1, "sync: %ld, error: %d", sync, r);
    tb_trace_d("deflate: %u => %u, sync: %ld", ie - ip, (tb_byte_t*)zlibraw->zst.next_out - op, sync);

    // update 
    ist->p = (tb_byte_t*)zlibraw->zst.next_in;
    ost->p = (tb_byte_t*)zlibraw->zst.next_out;

    // end?
    tb_check_return_val(r != Z_STREAM_END || ost->p > op, -1);

    // ok?
    return (ost->p - op);
}
static tb_long_t tb_zip_zlibraw_spak_inflate(tb_zip_ref_t zip, tb_static_stream_ref_t ist, tb_static_stream_ref_t ost, tb_long_t sync)
{
    tb_zip_zlibraw_t* zlibraw = tb_zip_zlibraw_cast(zip);
    tb_assert_and_check_return_val(zlibraw && ist && ost, -1);

    // the input stream
    tb_byte_t* ip = ist->p;
    tb_byte_t* ie = ist->e;
    tb_check_return_val(ip && ip < ie, 0);

    // the output stream
    tb_byte_t* op = ost->p;
    tb_byte_t* oe = ost->e;
    tb_assert_and_check_return_val(op && oe, -1);

    // attach zst
    zlibraw->zst.next_in = (Bytef*)ip;
    zlibraw->zst.avail_in = (uInt)(ie - ip);

    zlibraw->zst.next_out = (Bytef*)op;
    zlibraw->zst.avail_out = (uInt)(oe - op);

    // inflate 
    tb_int_t r = inflate(&zlibraw->zst, !sync? Z_NO_FLUSH : Z_SYNC_FLUSH);
    tb_assertf_and_check_return_val(r == Z_OK || r == Z_STREAM_END, -1, "sync: %ld, error: %d", sync, r);
    tb_trace_d("inflate: %u => %u, sync: %ld", ie - ip, (tb_byte_t*)zlibraw->zst.next_out - op, sync);

    // update 
    ist->p = (tb_byte_t*)zlibraw->zst.next_in;
    ost->p = (tb_byte_t*)zlibraw->zst.next_out;

    // end?
    tb_check_return_val(r != Z_STREAM_END || ost->p > op, -1);

    // ok?
    return (ost->p - op);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_zip_ref_t tb_zip_zlibraw_init(tb_size_t action)
{   
    // make zip
    tb_zip_ref_t zip = (tb_zip_ref_t)tb_malloc0_type(tb_zip_zlibraw_t);
    tb_assert_and_check_return_val(zip, tb_null);
    
    // init zip
    zip->algo       = TB_ZIP_ALGO_ZLIBRAW;
    zip->action     = (tb_uint16_t)action;

    // open zst
    switch (action)
    {
    case TB_ZIP_ACTION_INFLATE:
        {
            zip->spak = tb_zip_zlibraw_spak_inflate;

            // no zlib header
            if (inflateInit(&((tb_zip_zlibraw_t*)zip)->zst) != Z_OK) goto fail;
        }
        break;
    case TB_ZIP_ACTION_DEFLATE:
        {
            zip->spak = tb_zip_zlibraw_spak_deflate;
            if (deflateInit(&((tb_zip_zlibraw_t*)zip)->zst, Z_DEFAULT_COMPRESSION) != Z_OK) goto fail;
        }
        break;
    default:
        break;
    }

    // ok
    return zip;

fail:
    if (zip) tb_free(zip);
    return tb_null;
}
tb_void_t tb_zip_zlibraw_exit(tb_zip_ref_t zip)
{
    tb_zip_zlibraw_t* zlibraw = tb_zip_zlibraw_cast(zip);
    if (zlibraw) 
    {
        // close zst
        if (zip->action == TB_ZIP_ACTION_INFLATE) inflateEnd(&(zlibraw->zst));
        else if (zip->action == TB_ZIP_ACTION_DEFLATE) deflateEnd(&(zlibraw->zst));

        // free it
        tb_free(zlibraw);
    }
}


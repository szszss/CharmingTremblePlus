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
 * @file        queue.h
 * @ingroup     container
 *
 */
#ifndef TB_CONTAINER_CIRCLE_QUEUE_H
#define TB_CONTAINER_CIRCLE_QUEUE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "item.h"
#include "iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the circle queue ref type
 *
 * <pre>
 * queue: |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||------|
 *       head                                                           last    tail
 *
 * queue: ||||||||||||||-----|--------------------------||||||||||||||||||||||||||
 *                   last  tail                       head                
 *
 * performance: 
 *
 * put: O(1)
 * pop: O(1)
 *
 * iterator:
 *
 * next: fast
 * prev: fast
 *
 * </pre>
 *
 * @note the index of the same item is mutable
 *
 */
typedef tb_iterator_ref_t   tb_circle_queue_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init queue
 *
 * @param maxn          the item maxn, using the default maxn if be zero
 * @param func          the item func
 *
 * @return              the queue
 */
tb_circle_queue_ref_t   tb_circle_queue_init(tb_size_t maxn, tb_item_func_t func);

/*! exit queue
 *
 * @param queue         the queue
 */
tb_void_t               tb_circle_queue_exit(tb_circle_queue_ref_t queue);

/*! the queue head item
 *
 * @param queue         the queue
 *
 * @return              the head item
 */
tb_pointer_t            tb_circle_queue_head(tb_circle_queue_ref_t queue);

/*! the queue last item
 *
 * @param queue         the queue
 *
 * @return              the last item
 */
tb_pointer_t            tb_circle_queue_last(tb_circle_queue_ref_t queue);

/*! clear the queue
 *
 * @param queue         the queue
 */
tb_void_t               tb_circle_queue_clear(tb_circle_queue_ref_t queue);

/*! put the queue item
 *
 * @param queue         the queue
 * @param data          the item data
 */
tb_void_t               tb_circle_queue_put(tb_circle_queue_ref_t queue, tb_cpointer_t data);

/*! pop the queue item
 *
 * @param queue         the queue
 */
tb_void_t               tb_circle_queue_pop(tb_circle_queue_ref_t queue);

/*! get the queue item
 *
 * @param queue         the queue
 *
 * @return              the queue item
 */
tb_pointer_t            tb_circle_queue_get(tb_circle_queue_ref_t queue);

/*! load queue from the stream
 *
 * @param queue         the queue
 * @param stream        the stream
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_circle_queue_load(tb_circle_queue_ref_t queue, tb_stream_ref_t stream);

/*! save queue to the stream
 *
 * @param queue         the queue
 * @param stream        the stream
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_circle_queue_save(tb_circle_queue_ref_t queue, tb_stream_ref_t stream);

/*! the queue size
 *
 * @param queue         the queue
 *
 * @return              the queue size
 */
tb_size_t               tb_circle_queue_size(tb_circle_queue_ref_t queue);

/*! the queue maxn
 *
 * @param queue         the queue
 *
 * @return              the queue maxn
 */
tb_size_t               tb_circle_queue_maxn(tb_circle_queue_ref_t queue);

/*! the queue full?
 *
 * @param queue         the queue
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_circle_queue_full(tb_circle_queue_ref_t queue);

/*! the queue null?
 *
 * @param queue         the queue
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_circle_queue_null(tb_circle_queue_ref_t queue);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif


//
//  queue.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/28/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef queue_h
#define queue_h

#include <stdint.h>
#include <i386/locks.h>

struct queue_entry {
    struct queue_entry	*next;		/* next element */
    struct queue_entry	*prev;		/* previous element */
    
};

typedef struct queue_entry	*queue_t;
typedef	struct queue_entry	queue_head_t;
typedef	struct queue_entry	queue_chain_t;
typedef	struct queue_entry	*queue_entry_t;

struct mpqueue_head {
    struct queue_entry	head;		/* header for queue */
        uint64_t        earliest_soft_deadline;
        uint64_t        count;
        lck_mtx_t		lock_data;
        lck_mtx_ext_t	lock_data_ext;
};

typedef struct mpqueue_head	mpqueue_head_t;

#endif /* queue_h */

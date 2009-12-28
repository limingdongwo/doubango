/*
* Copyright (C) 2009 Mamadou Diop.
*
* Contact: Mamadou Diop <diopmamadou@yahoo.fr>
*	
* This file is part of Open Source Doubango Framework.
*
* DOUBANGO is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*	
* DOUBANGO is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*	
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*
*/

/**@file tsk_condwait.h
 * @brief Pthread CondWait
 *
 * @author Mamadou Diop <diopmamadou(at)yahoo.fr>
 *
 * @date Created: Sat Nov 8 16:54:58 2009 mdiop
 */
#ifndef _TINYSAK_CONDWAIT_H_
#define _TINYSAK_CONDWAIT_H_

#include "tinySAK_config.h"
#include "tsk_mutex.h"

/**
* Pthread condwait handle.
*/
typedef void tsk_condwait_handle_t;

TINYSAK_API tsk_condwait_handle_t* tsk_condwait_create();
TINYSAK_API int tsk_condwait_wait(tsk_condwait_handle_t* handle);
TINYSAK_API int tsk_condwait_timedwait(tsk_condwait_handle_t* handle, uint64_t ms);
TINYSAK_API int tsk_condwait_signal(tsk_condwait_handle_t* handle);
TINYSAK_API int tsk_condwait_broadcast(tsk_condwait_handle_t* handle);
TINYSAK_API void tsk_condwait_destroy(tsk_condwait_handle_t** handle);

#endif /* _TINYSAK_CONDWAIT_H_ */

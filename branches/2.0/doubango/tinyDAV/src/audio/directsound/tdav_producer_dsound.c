/*
* Copyright (C) 2010-2011 Mamadou Diop.
*
* Contact: Mamadou Diop <diopmamadou(at)doubango.org>
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
* GNU General Public License for more details.
*	
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*
*/

/**@file tdav_producer_dsound.c
 * @brief Microsoft DirectSound producer.
 *
 * @author Mamadou Diop <diopmamadou(at)doubango.org>
 *

 */
#include "tinydav/audio/directsound/tdav_producer_dsound.h"

#if HAVE_DSOUND_H

#if defined(_MSC_VER)
#	pragma comment(lib, "dsound.lib")
#	pragma comment(lib, "dxguid.lib")
#endif

#include "tsk_thread.h"
#include "tsk_memory.h"
#include "tsk_debug.h"

#include <initguid.h>

extern void tdav_win32_print_error(const char* func, HRESULT hr);

static void *__playback_thread(void *param)
{
	tdav_producer_dsound_t* dsound = (tdav_producer_dsound_t*)param; 

	HRESULT hr;
	LPVOID lpvAudio1, lpvAudio2;
	DWORD dwBytesAudio1, dwBytesAudio2;

	TSK_DEBUG_INFO("__record_thread -- START");

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	while(dsound->started){
		DWORD dwEvent = WaitForMultipleObjects(TDAV_DSOUND_PRODUCER_NOTIF_POS_COUNT, dsound->notifEvents, FALSE, INFINITE);
		if(!dsound->started){
			break;
		}
		
		// lock
		if((hr = IDirectSoundCaptureBuffer_Lock(dsound->captureBuffer, (dwEvent * dsound->bytes_per_notif), dsound->bytes_per_notif, &lpvAudio1, &dwBytesAudio1, &lpvAudio2, &dwBytesAudio2, 0)) != DS_OK){
			tdav_win32_print_error("IDirectSoundCaptureBuffer_Lock", hr);
			continue;
		}

		if(TMEDIA_PRODUCER(dsound)->enc_cb.callback){
			if(lpvAudio2){
				TMEDIA_PRODUCER(dsound)->enc_cb.callback(TMEDIA_PRODUCER(dsound)->enc_cb.callback_data, lpvAudio1, dwBytesAudio1);
				TMEDIA_PRODUCER(dsound)->enc_cb.callback(TMEDIA_PRODUCER(dsound)->enc_cb.callback_data, lpvAudio2, dwBytesAudio2);
			}
			else{
				TMEDIA_PRODUCER(dsound)->enc_cb.callback(TMEDIA_PRODUCER(dsound)->enc_cb.callback_data, lpvAudio1, dwBytesAudio1);
			}
		}

		// unlock
		if((hr = IDirectSoundCaptureBuffer_Unlock(dsound->captureBuffer, lpvAudio1, dwBytesAudio1, lpvAudio2, dwBytesAudio2)) != DS_OK){
			tdav_win32_print_error("IDirectSoundCaptureBuffer_Unlock", hr);
			continue;
		}		
	}

	TSK_DEBUG_INFO("__record_thread -- STOP");
	

	return tsk_null;
}




/* ============ Media Producer Interface ================= */
static int tdav_producer_dsound_set(tmedia_producer_t* self, const tmedia_param_t* param)
{	
	tdav_producer_dsound_t* dsound = (tdav_producer_dsound_t*)self;
	if(param->plugin_type == tmedia_ppt_producer){
		if(param->value_type == tmedia_pvt_int32){
			if(tsk_striequals(param->key, "mute")){
				dsound->mute = (TSK_TO_INT32((uint8_t*)param->value) != 0);
				if(dsound->started){
					if(dsound->mute){
						IDirectSoundCaptureBuffer_Stop(dsound->captureBuffer);
					}
					else{
						IDirectSoundCaptureBuffer_Start(dsound->captureBuffer, DSBPLAY_LOOPING);
					}
				}
				return 0;
			}
		}
	}
	return tdav_producer_audio_set(TDAV_PRODUCER_AUDIO(self), param);
}
static int tdav_producer_dsound_prepare(tmedia_producer_t* self, const tmedia_codec_t* codec)
{
	HRESULT hr;

	WAVEFORMATEX wfx = {0};
	DSCBUFFERDESC dsbd = {0};

	tdav_producer_dsound_t* dsound = (tdav_producer_dsound_t*)self;

	if(!dsound || !codec){
		TSK_DEBUG_ERROR("Invalid parameter");
		return -1;
	}

	if(dsound->device || dsound->captureBuffer){
		TSK_DEBUG_ERROR("Producer already prepared");
		return -2;
	}

	TMEDIA_PRODUCER(dsound)->audio.channels = codec->plugin->audio.channels;
	TMEDIA_PRODUCER(dsound)->audio.rate = codec->plugin->rate;
	TMEDIA_PRODUCER(dsound)->audio.ptime = codec->plugin->audio.ptime;

	/* Create capture device */
	if((hr = DirectSoundCaptureCreate(NULL, &dsound->device, NULL) != DS_OK)){
		tdav_win32_print_error("DirectSoundCaptureCreate", hr);
		return -3;
	}

	/* Creates the capture buffer */
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = TMEDIA_PRODUCER(dsound)->audio.channels;
	wfx.nSamplesPerSec = TMEDIA_PRODUCER(dsound)->audio.rate;
	wfx.wBitsPerSample = TMEDIA_PRODUCER(dsound)->audio.bits_per_sample;
	wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample/8);
	wfx.nAvgBytesPerSec = (wfx.nSamplesPerSec * wfx.nBlockAlign);

	/* Average bytes (count) for each notification */
	dsound->bytes_per_notif = ((wfx.nAvgBytesPerSec * TMEDIA_PRODUCER(dsound)->audio.ptime)/1000);

	dsbd.dwSize = sizeof(DSCBUFFERDESC);
	dsbd.dwBufferBytes = (TDAV_DSOUND_PRODUCER_NOTIF_POS_COUNT * dsound->bytes_per_notif);
	dsbd.lpwfxFormat = &wfx;

	if((hr = IDirectSoundCapture_CreateCaptureBuffer(dsound->device, &dsbd, &dsound->captureBuffer, NULL)) != DS_OK){
		tdav_win32_print_error("IDirectSoundCapture_CreateCaptureBuffer", hr);
		return -4;
	}	

	return 0;
}

static int tdav_producer_dsound_start(tmedia_producer_t* self)
{
	tdav_producer_dsound_t* dsound = (tdav_producer_dsound_t*)self;

	tsk_size_t i;
	HRESULT hr;
	LPDIRECTSOUNDNOTIFY lpDSBNotify;
	DSBPOSITIONNOTIFY pPosNotify[TDAV_DSOUND_PRODUCER_NOTIF_POS_COUNT] = {0};

	if(!dsound){
		TSK_DEBUG_ERROR("Invalid parameter");
		return -1;
	}

	if(!dsound->device || !dsound->captureBuffer){
		TSK_DEBUG_ERROR("Producer not prepared");
		return -2;
	}

	if(dsound->started){
		TSK_DEBUG_WARN("Producer already started");
		return 0;
	}

	if((hr = IDirectSoundCaptureBuffer_QueryInterface(dsound->captureBuffer, &IID_IDirectSoundNotify, (LPVOID*)&lpDSBNotify)) != DS_OK){
		tdav_win32_print_error("IDirectSoundCaptureBuffer_QueryInterface", hr);
		return -3;
	}

	/* Events associated to notification points */
	for(i = 0; i<TDAV_DSOUND_PRODUCER_NOTIF_POS_COUNT; i++){
		dsound->notifEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		pPosNotify[i].dwOffset = (dsound->bytes_per_notif * i) + (dsound->bytes_per_notif >> 1);
		pPosNotify[i].hEventNotify = dsound->notifEvents[i];
	}
	if((hr = IDirectSoundNotify_SetNotificationPositions(lpDSBNotify, TDAV_DSOUND_PRODUCER_NOTIF_POS_COUNT, pPosNotify)) != DS_OK){
		IDirectSoundNotify_Release(lpDSBNotify);
		tdav_win32_print_error("IDirectSoundBuffer_QueryInterface", hr);
		return -4;
	}
	
	if((hr = IDirectSoundNotify_Release(lpDSBNotify))){
		tdav_win32_print_error("IDirectSoundNotify_Release", hr);
	}

	/* Start the buffer */
	if((hr = IDirectSoundCaptureBuffer_Start(dsound->captureBuffer, DSBPLAY_LOOPING)) != DS_OK){
		tdav_win32_print_error("IDirectSoundCaptureBuffer_Start", hr);
		return -5;
	}

	/* start the reader thread */
	dsound->started = tsk_true;
	tsk_thread_create(&dsound->tid[0], __playback_thread, dsound);

	return 0;
}

static int tdav_producer_dsound_pause(tmedia_producer_t* self)
{
	return 0;
}

static int tdav_producer_dsound_stop(tmedia_producer_t* self)
{
	tdav_producer_dsound_t* dsound = (tdav_producer_dsound_t*)self;

	HRESULT hr;

	if(!self){
		TSK_DEBUG_ERROR("Invalid parameter");
		return -1;
	}

	if(!dsound->started){
		TSK_DEBUG_WARN("Producer not started");
		return 0;
	}

	/* should be done here */
	dsound->started = tsk_false;

	/* stop thread */
	if(dsound->tid[0]){
		tsk_thread_join(&(dsound->tid[0]));
	}

	if((hr = IDirectSoundCaptureBuffer_Stop(dsound->captureBuffer)) != DS_OK){
		tdav_win32_print_error("IDirectSoundCaptureBuffer_Stop", hr);
	}

	return 0;
}


//
//	WaveAPI producer object definition
//
/* constructor */
static tsk_object_t* tdav_producer_dsound_ctor(tsk_object_t * self, va_list * app)
{
	tdav_producer_dsound_t *producer = self;
	if(producer){
		/* init base */
		tdav_producer_audio_init(TDAV_PRODUCER_AUDIO(producer));
		/* init self */
		
	}
	return self;
}
/* destructor */
static tsk_object_t* tdav_producer_dsound_dtor(tsk_object_t * self)
{ 
	tdav_producer_dsound_t *dsound = self;
	if(dsound){
		tsk_size_t i;

		/* stop */
		if(dsound->started){
			tdav_producer_dsound_stop(self);
		}

		/* deinit base */
		tdav_producer_audio_deinit(TDAV_PRODUCER_AUDIO(dsound));
		/* deinit self */
		if(dsound->captureBuffer){
			IDirectSoundCaptureBuffer_Release(dsound->captureBuffer);
		}
		if(dsound->device){
			IDirectSoundCapture_Release(dsound->device);
		}
		for(i = 0; i<sizeof(dsound->notifEvents)/sizeof(HANDLE); i++){
			if(dsound->notifEvents[i]){
				CloseHandle(dsound->notifEvents[i]);
			}
		}
	}

	return self;
}
/* object definition */
static const tsk_object_def_t tdav_producer_dsound_def_s = 
{
	sizeof(tdav_producer_dsound_t),
	tdav_producer_dsound_ctor, 
	tdav_producer_dsound_dtor,
	tdav_producer_audio_cmp, 
};
/* plugin definition*/
static const tmedia_producer_plugin_def_t tdav_producer_dsound_plugin_def_s = 
{
	&tdav_producer_dsound_def_s,
	
	tmedia_audio,
	"Microsoft DirectSound producer",
	
	tdav_producer_dsound_set,
	tdav_producer_dsound_prepare,
	tdav_producer_dsound_start,
	tdav_producer_dsound_pause,
	tdav_producer_dsound_stop
};
const tmedia_producer_plugin_def_t *tdav_producer_dsound_plugin_def_t = &tdav_producer_dsound_plugin_def_s;


#endif /* HAVE_DSOUND_H */
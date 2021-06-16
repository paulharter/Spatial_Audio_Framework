/*
 * Copyright 2020 Leo McCormack
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * @file decorrelator_internal.h
 * @brief A multi-channel decorrelator
 *
 * @author Leo McCormack
 * @date 07.07.2020
 */


#ifndef __DECORRELATOR_INTERNAL_H_INCLUDED__
#define __DECORRELATOR_INTERNAL_H_INCLUDED__

#include "decorrelator.h"  /* Include header for this example */
#include "saf.h"           /* Main include header for SAF */
#include "saf_externals.h" /* To also include SAF dependencies (cblas etc.) */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ========================================================================== */
/*                            Internal Parameters                             */
/* ========================================================================== */

#ifndef FRAME_SIZE
# define FRAME_SIZE ( 128 )                  /**< Framesize, in time-domain samples */
#endif
#define HOP_SIZE ( 128 )                     /**< STFT hop size */
#define HYBRID_BANDS ( HOP_SIZE + 5 )        /**< Number of frequency bands */
#define TIME_SLOTS ( FRAME_SIZE / HOP_SIZE ) /**< Number of STFT timeslots */
#if (FRAME_SIZE % HOP_SIZE != 0)
# error "FRAME_SIZE must be an integer multiple of HOP_SIZE"
#endif
    
/* ========================================================================== */
/*                                 Structures                                 */
/* ========================================================================== */

/**
 * Main structure for decorrelator. Contains variables for audio buffers, afSTFT,
 * rotation matrices, internal variables, flags, user parameters
 */
typedef struct _decorrelator
{
    /* audio buffers + afSTFT time-frequency transform handle */
    int fs;                         /**< host sampling rate */ 
    float** InputFrameTD;
    float** OutputFrameTD;
    float_complex*** InputFrameTF;
    float_complex*** transientFrameTF;
    float_complex*** OutputFrameTF;
    void* hSTFT;                    /**< afSTFT handle */
    int afSTFTdelay;                /**< for host delay compensation */ 
    float freqVector[HYBRID_BANDS]; /**< frequency vector for time-frequency transform, in Hz */
     
    /* our codec configuration */
    void* hDecor; 
    void* hDucker;
    CODEC_STATUS codecStatus;       /**< see #CODEC_STATUS */
    float progressBar0_1;           /**< Current (re)initialisation progress, between [0..1] */
    char* progressBarText;          /**< Current (re)initialisation step, string */
    
    /* internal variables */
    PROC_STATUS procStatus;         /**< see #PROC_STATUS */
    int new_nChannels;

    /* user parameters */
    int nChannels;
    int enableTransientDucker;
    float decorAmount;
    int compensateLevel;
    
} decorrelator_data;


/* ========================================================================== */
/*                             Internal Functions                             */
/* ========================================================================== */

/**
 * Sets codec status. 
 */
void decorrelator_setCodecStatus(void* const hDecor,
                                 CODEC_STATUS newStatus);


#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif /* __DECORRELATOR_INTERNAL_H_INCLUDED__ */

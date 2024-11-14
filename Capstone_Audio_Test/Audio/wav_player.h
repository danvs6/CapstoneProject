/*
 * wav_player.h
 *
 *  Created on: 17 September 2024
 *      Author: Phi Lu
 */

#ifndef WAV_PLAYER_H_
#define WAV_PLAYER_H_

#include <stdbool.h>
#include <stdint.h>

// Audio buffer state
typedef enum {
    BUFFER_OFFSET_NONE = 0,
    BUFFER_OFFSET_HALF,
    BUFFER_OFFSET_FULL,
} BUFFER_StateTypeDef;

typedef enum {
    PLAY_Idle = 0,
    PLAY_Pause,
    PLAY_Resume,
} PLAY_State_e;

typedef struct {
    uint32_t   ChunkID;       /* 0 */
    uint32_t   FileSize;      /* 4 */
    uint32_t   FileFormat;    /* 8 */
    uint32_t   SubChunk1ID;   /* 12 */
    uint32_t   SubChunk1Size; /* 16 */
    uint16_t   AudioFormat;   /* 20 */
    uint16_t   NbrChannels;   /* 22 */
    uint32_t   SampleRate;    /* 24 */
    uint32_t   ByteRate;      /* 28 */
    uint16_t   BlockAlign;    /* 32 */
    uint16_t   BitPerSample;  /* 34 */
    uint32_t   SubChunk2ID;   /* 36 */
    uint32_t   SubChunk2Size; /* 40 */
} WAV_HeaderTypeDef;

/**
 * @brief Select WAV file to play from USB
 * @param filePath: Path to the WAV file
 * @retval Returns true if the file is found and selected
 */
bool wavPlayer_fileSelect(const char* filePath);

/**
 * @brief WAV file play from USB
 */
void wavPlayer_play(void);

/**
 * @brief WAV stop
 */
void wavPlayer_stop(void);

/**
 * @brief Process WAV data from USB
 */
void wavPlayer_process(void);

/**
 * @brief Check if end of file reached in USB playback
 */
bool wavPlayer_isFinished(void);

/**
 * @brief Pause and resume WAV playback from USB
 */
void wavPlayer_pause(void);
void wavPlayer_resume(void);

/**
 * @brief Write audio file from USB to flash memory
 * @param filePath: Path to the audio file on the USB drive
 * @retval Returns true if the file is successfully written to flash
 */
bool writeAudioFileToFlash(const char* filePath);

/**
 * @brief Play WAV file from flash memory
 * @param samplingFreq: Sample rate of the audio file
 */
void wavPlayer_playFromFlash(uint32_t samplingFreq);

/**
 * @brief Process WAV data from flash for playback
 * @param buffer: Pointer to the buffer used for audio playback
 * @param size: Size of the buffer
 */
void wavPlayer_processFromFlash(uint8_t* buffer, uint32_t size);

#endif /* WAV_PLAYER_H_ */


///*
// * wav_player.h
// *
//*  Created on: 17 September 2024
// *      Author: Phi Lu
// */
//
//#ifndef WAV_PLAYER_H_
//#define WAV_PLAYER_H_
//
//#include <stdbool.h>
//#include <stdint.h>
//
////Audio buffer state
//typedef enum
//{
//  BUFFER_OFFSET_NONE = 0,
//  BUFFER_OFFSET_HALF,
//  BUFFER_OFFSET_FULL,
//}BUFFER_StateTypeDef;
//
//typedef enum
//{
//  PLAY_Idle=0,
//  PLAY_Pause,
//  PLAY_Resume,
//}PLAY_State_e;
//
//typedef struct
//{
//  uint32_t   ChunkID;       /* 0 */
//  uint32_t   FileSize;      /* 4 */
//  uint32_t   FileFormat;    /* 8 */
//  uint32_t   SubChunk1ID;   /* 12 */
//  uint32_t   SubChunk1Size; /* 16*/
//  uint16_t   AudioFormat;   /* 20 */
//  uint16_t   NbrChannels;   /* 22 */
//  uint32_t   SampleRate;    /* 24 */
//
//  uint32_t   ByteRate;      /* 28 */
//  uint16_t   BlockAlign;    /* 32 */
//  uint16_t   BitPerSample;  /* 34 */
//  uint32_t   SubChunk2ID;   /* 36 */
//  uint32_t   SubChunk2Size; /* 40 */
//
//}WAV_HeaderTypeDef;
//
///**
// * @brief Select WAV file to play
// * @retval returns true when file is found in USB Drive
// */
//bool wavPlayer_fileSelect(const char* filePath);
//
///**
// * @brief WAV File Play
// */
//void wavPlayer_play(void);
//
///**
// * @brief WAV stop
// */
//void wavPlayer_stop(void);
//
///**
// * @brief Process WAV
// */
//void wavPlayer_process(void);
//
///**
// * @brief isEndofFile reached
// */
//bool wavPlayer_isFinished(void);
//
///**
// * @brief WAV pause/resume
// */
//void wavPlayer_pause(void);
//void wavPlayer_resume(void);
//
//#endif /* WAV_PLAYER_H_ */

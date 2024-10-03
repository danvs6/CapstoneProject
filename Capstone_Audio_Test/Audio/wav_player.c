/*
 * wav_player.c
 *
 *  Created on: 17 Apr 2020
 *      Author: Mohamed Yaqoob
 */

#include "wav_player.h"
#include "audioI2S.h"
#include "fatfs.h"

//WAV File System variables
static FIL wavFile;

//WAV Audio Buffer
static uint32_t fileLength;
#define AUDIO_BUFFER_SIZE  4096*1
static uint8_t audioBuffer[AUDIO_BUFFER_SIZE];
static __IO uint32_t audioRemainSize = 0;

//WAV Player
static uint32_t samplingFreq;
static UINT playerReadBytes = 0;
static bool isFinished=0;
//WAV Player process states
typedef enum
{
  PLAYER_CONTROL_Idle=0,
  PLAYER_CONTROL_HalfBuffer,
  PLAYER_CONTROL_FullBuffer,
  PLAYER_CONTROL_EndOfFile,
}PLAYER_CONTROL_e;
static volatile PLAYER_CONTROL_e playerControlSM = PLAYER_CONTROL_Idle;

static void wavPlayer_reset(void)
{
  audioRemainSize = 0;
  playerReadBytes = 0;
}

/**
 * @brief Select WAV file to play
 * @retval returns true when file is found in USB Drive
 */
bool wavPlayer_fileSelect(const char* filePath)
{
  WAV_HeaderTypeDef wavHeader;
  UINT readBytes = 0;
  //Open WAV file

  // TESTING ERROR
 // FRESULT res = f_open(&wavFile, filePath, FA_READ);

  // Declare an integer for each FR_x result, initialized to 0
//  int fr_ok = 0;
//  int fr_disk_err = 0;
//  int fr_int_err = 0;
//  int fr_not_ready = 0;
//  int fr_no_file = 0;
//  int fr_no_path = 0;
//  int fr_invalid_name = 0;
//  int fr_denied = 0;
//  int fr_exist = 0;
//  int fr_invalid_object = 0;
//  int fr_write_protected = 0;
//  int fr_invalid_drive = 0;
//  int fr_not_enabled = 0;
//  int fr_no_filesystem = 0;
//  int fr_mkfs_aborted = 0;
//  int fr_timeout = 0;
//  int fr_locked = 0;
//  int fr_not_enough_core = 0;
//  int fr_too_many_open_files = 0;
//  int fr_invalid_parameter = 0;
//
//  switch(res) {
//      case FR_OK:
//          fr_ok = 1;
//          break;
//      case FR_DISK_ERR:
//          fr_disk_err = 1;
//          break;
//      case FR_INT_ERR:
//          fr_int_err = 1;
//          break;
//      case FR_NOT_READY:
//          fr_not_ready = 1;
//          break;
//      case FR_NO_FILE:
//          fr_no_file = 1;
//          break;
//      case FR_NO_PATH:
//          fr_no_path = 1;
//          break;
//      case FR_INVALID_NAME:
//          fr_invalid_name = 1;
//          break;
//      case FR_DENIED:
//          fr_denied = 1;
//          break;
//      case FR_EXIST:
//          fr_exist = 1;
//          break;
//      case FR_INVALID_OBJECT:
//          fr_invalid_object = 1;
//          break;
//      case FR_WRITE_PROTECTED:
//          fr_write_protected = 1;
//          break;
//      case FR_INVALID_DRIVE:
//          fr_invalid_drive = 1;
//          break;
//      case FR_NOT_ENABLED:
//          fr_not_enabled = 1;
//          break;
//      case FR_NO_FILESYSTEM:
//          fr_no_filesystem = 1;
//          break;
//      case FR_MKFS_ABORTED:
//          fr_mkfs_aborted = 1;
//          break;
//      case FR_TIMEOUT:
//          fr_timeout = 1;
//          break;
//      case FR_LOCKED:
//          fr_locked = 1;
//          break;
//      case FR_NOT_ENOUGH_CORE:
//          fr_not_enough_core = 1;
//          break;
//      case FR_TOO_MANY_OPEN_FILES:
//          fr_too_many_open_files = 1;
//          break;
//      case FR_INVALID_PARAMETER:
//          fr_invalid_parameter = 1;
//          break;
//      default:
//          // Handle unknown FRESULT codes if needed
//          break;
//  }


  // END OF TESTING ERROR

  if(f_open(&wavFile, filePath, FA_READ) != FR_OK)
  {
    return false;
  }
  //Read WAV file Header
  f_read(&wavFile, &wavHeader, sizeof(wavHeader), &readBytes);
  //Get audio data size
  fileLength = wavHeader.FileSize;
  //Play the WAV file with frequency specified in header
  samplingFreq = wavHeader.SampleRate;
  return true;
}

/**
 * @brief WAV File Play
 */
void wavPlayer_play(void)
{
  isFinished = false;
  //Initialise I2S Audio Sampling settings
  audioI2S_init(samplingFreq);
  //Read Audio data from USB Disk
  f_lseek(&wavFile, 0);
  f_read (&wavFile, &audioBuffer[0], AUDIO_BUFFER_SIZE, &playerReadBytes);
  audioRemainSize = fileLength - playerReadBytes;
  //Start playing the WAV
  audioI2S_play((uint16_t *)&audioBuffer[0], AUDIO_BUFFER_SIZE);
}

/**
 * @brief Process WAV
 */
void wavPlayer_process(void)
{
  switch(playerControlSM)
  {
  case PLAYER_CONTROL_Idle:
    break;

  case PLAYER_CONTROL_HalfBuffer:
    playerReadBytes = 0;
    playerControlSM = PLAYER_CONTROL_Idle;
    f_read (&wavFile, &audioBuffer[0], AUDIO_BUFFER_SIZE/2, &playerReadBytes);
    if(audioRemainSize > (AUDIO_BUFFER_SIZE / 2))
    {
      audioRemainSize -= playerReadBytes;
    }
    else
    {
      audioRemainSize = 0;
      playerControlSM = PLAYER_CONTROL_EndOfFile;
    }
    break;

  case PLAYER_CONTROL_FullBuffer:
    playerReadBytes = 0;
    playerControlSM = PLAYER_CONTROL_Idle;
    f_read (&wavFile, &audioBuffer[AUDIO_BUFFER_SIZE/2], AUDIO_BUFFER_SIZE/2, &playerReadBytes);
    if(audioRemainSize > (AUDIO_BUFFER_SIZE / 2))
    {
      audioRemainSize -= playerReadBytes;
    }
    else
    {
      audioRemainSize = 0;
      playerControlSM = PLAYER_CONTROL_EndOfFile;
    }
    break;

  case PLAYER_CONTROL_EndOfFile:
    f_close(&wavFile);
    wavPlayer_reset();
    isFinished = true;
    playerControlSM = PLAYER_CONTROL_Idle;
    break;
  }
}

/**
 * @brief WAV stop
 */
void wavPlayer_stop(void)
{
  audioI2S_stop();
  isFinished = true;
}

/**
 * @brief WAV pause/resume
 */
void wavPlayer_pause(void)
{
  audioI2S_pause();
}
void wavPlayer_resume(void)
{
  audioI2S_resume();
}

/**
 * @brief isEndofFile reached
 */
bool wavPlayer_isFinished(void)
{
  return isFinished;
}

/**
 * @brief Half/Full transfer Audio callback for buffer management
 */
void audioI2S_halfTransfer_Callback(void)
{
  playerControlSM = PLAYER_CONTROL_HalfBuffer;
}
void audioI2S_fullTransfer_Callback(void)
{
  playerControlSM = PLAYER_CONTROL_FullBuffer;
//  audioI2S_changeBuffer((uint16_t*)&audioBuffer[0], AUDIO_BUFFER_SIZE / 2);
}


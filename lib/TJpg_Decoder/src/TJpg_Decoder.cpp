/*
TJpg_Decoder.cpp

Created by Bodmer 18/10/19
Modified by Blueforcer 02/23

Latest version here:
https://github.com/Bodmer/TJpg_Decoder
*/

#include "TJpg_Decoder.h"

// Create a class instance to be used by the sketch (defined as extern in header)
TJpg_Decoder TJpgDec;

#ifdef ESP8266
// Bytes to give the decoder: up to TJPGD_WORKSPACE_SIZE, otherwise the largest free block
// minus a small reserve for the WiFi stack; 0 when that is too small to be worth trying.
static size_t jpgWorkspaceSize()
{
  size_t maxBlock = ESP.getMaxFreeBlockSize();
  size_t size = maxBlock > 256 ? maxBlock - 256 : 0;
  if (size > TJPGD_WORKSPACE_SIZE)
    size = TJPGD_WORKSPACE_SIZE;
  return size >= 1024 ? size : 0;
}

#ifdef AWTRIX_HEAP_LOG
// How much of the workspace a decode really used (printed every 50th decode)
static void logJpgPool(JRESULT result, size_t poolSize, size_t left)
{
  static uint32_t decodes = 0;
  if (result != JDR_OK || ++decodes % 50 == 1)
    Serial.printf("[jpg] result %d, workspace %u, used %u\n", result, poolSize, poolSize - left);
}
#endif
#endif

/***************************************************************************************
** Function name:           TJpg_Decoder
** Description:             Constructor
***************************************************************************************/
TJpg_Decoder::TJpg_Decoder()
{
  // Setup a pointer to this class for static functions
  thisPtr = this;
}

/***************************************************************************************
** Function name:           ~TJpg_Decoder
** Description:             Destructor
***************************************************************************************/
TJpg_Decoder::~TJpg_Decoder()
{
  // Bye
}

/***************************************************************************************
** Function name:           setJpgScale
** Description:             Set the reduction scale factor (1, 2, 4 or 8)
***************************************************************************************/
void TJpg_Decoder::setSwapBytes(bool swapBytes)
{
  _swap = swapBytes;
}

/***************************************************************************************
** Function name:           setJpgScale
** Description:             Set the reduction scale factor (1, 2, 4 or 8)
***************************************************************************************/
void TJpg_Decoder::setJpgScale(uint8_t scaleFactor)
{
  switch (scaleFactor)
  {
  case 1:
    jpgScale = 0;
    break;
  case 2:
    jpgScale = 1;
    break;
  case 4:
    jpgScale = 2;
    break;
  case 8:
    jpgScale = 3;
    break;
  default:
    jpgScale = 0;
  }
}

/***************************************************************************************
** Function name:           setCallback
** Description:             Set the sketch callback function to render decoded blocks
***************************************************************************************/
void TJpg_Decoder::setCallback(SketchCallback sketchCallback)
{
  tft_output = sketchCallback;
}

/***************************************************************************************
** Function name:           jd_input (declared static)
** Description:             Called by tjpgd.c to get more data
***************************************************************************************/
unsigned int TJpg_Decoder::jd_input(JDEC *jdec, uint8_t *buf, unsigned int len)
{
  TJpg_Decoder *thisPtr = TJpgDec.thisPtr;
  jdec = jdec; // Supress warning

  // Handle an array input
  if (thisPtr->jpg_source == TJPG_ARRAY)
  {
    // Avoid running off end of array
    if (thisPtr->array_index + len > thisPtr->array_size)
    {
      len = thisPtr->array_size - thisPtr->array_index;
    }

    // If buf is valid then copy len bytes to buffer
    if (buf)
      memcpy_P(buf, (const uint8_t *)(thisPtr->array_data + thisPtr->array_index), len);

    // Move pointer
    thisPtr->array_index += len;
  }

#ifdef TJPGD_LOAD_FFS
  // Handle SPIFFS input
  else if (thisPtr->jpg_source == TJPG_FS_FILE)
  {
    // Check how many bytes are available
    uint32_t bytesLeft = thisPtr->jpgFile.available();
    if (bytesLeft < len)
      len = bytesLeft;

    if (buf)
    {
      // Read into buffer, pointer moved as well
      thisPtr->jpgFile.read(buf, len);
    }
    else
    {
      // Buffer is null, so skip data by moving pointer
      thisPtr->jpgFile.seek(thisPtr->jpgFile.position() + len);
    }
  }
#endif

#if defined(TJPGD_LOAD_SD_LIBRARY)
  // Handle SD library input
  else if (thisPtr->jpg_source == TJPG_SD_FILE)
  {
    // Check how many bytes are available
    uint32_t bytesLeft = thisPtr->jpgSdFile.available();
    if (bytesLeft < len)
      len = bytesLeft;

    if (buf)
    {
      // Read into buffer, pointer moved as well
      thisPtr->jpgSdFile.read(buf, len);
    }
    else
    {
      // Buffer is null, so skip data by moving pointer
      thisPtr->jpgSdFile.seek(thisPtr->jpgSdFile.position() + len);
    }
  }
#endif

  return len;
}

/***************************************************************************************
** Function name:           jd_output (declared static)
** Description:             Called by tjpgd.c with an image block for rendering
***************************************************************************************/
// Pass image block back to the sketch for rendering, may be a complete or partial MCU
int TJpg_Decoder::jd_output(JDEC *jdec, void *bitmap, JRECT *jrect)
{

  TJpg_Decoder *thisPtr = TJpgDec.thisPtr;
  jdec = jdec; // Supress warning as ID is not used

  int16_t x = jrect->left + thisPtr->jpeg_x;
  int16_t y = jrect->top + thisPtr->jpeg_y;
  uint16_t w = jrect->right + 1 - jrect->left;
  uint16_t h = jrect->bottom + 1 - jrect->top;
  // Pass the image block and rendering parameters in a callback to the sketch
  return thisPtr->tft_output(x, y, w, h, (uint16_t *)bitmap);
}

/***************************************************************************************
** Function name:           drawFsJpg für AWTRIX
** Description:             Draw a jpg with opened file handle at x,y
***************************************************************************************/
JRESULT TJpg_Decoder::drawFsJpg(int32_t x, int32_t y, fs::File inFile)
{

  JDEC jdec;
  JRESULT jresult = JDR_OK;

  jpg_source = TJPG_FS_FILE;
  jpeg_x = x;
  jpeg_y = y;

  jdec.swap = _swap;

  jpgFile = inFile;

#ifdef ESP8266
  // The fixed workspace if one was set (setWorkspace()), otherwise taken from the heap
  // only while decoding: TJPGD_WORKSPACE_SIZE is a worst case, so take what is free and
  // let jd_prepare() decide whether it is enough. malloc is 4-byte aligned.
  uint8_t *workspace = fixedWorkspace;
  size_t poolSize = fixedWorkspaceSize;
  if (!workspace)
  {
    poolSize = jpgWorkspaceSize();
    if (!poolSize) return JDR_MEM1;
    workspace = (uint8_t *)malloc(poolSize);
    if (!workspace) return JDR_MEM1;
  }
#else
  const size_t poolSize = TJPGD_WORKSPACE_SIZE; // the member array "workspace"
#endif
  jresult = jd_prepare(&jdec, jd_input, workspace, poolSize, (unsigned int)0);

  // Extract image and render
  if (jresult == JDR_OK)
  {
    jresult = jd_decomp(&jdec, jd_output, jpgScale);
  }
#ifdef ESP8266
#ifdef AWTRIX_HEAP_LOG
  logJpgPool(jresult, poolSize, jdec.sz_pool);
#endif
  if (workspace != fixedWorkspace)
    free(workspace);
#endif
  inFile.seek(0);
  return jresult;
}


JRESULT TJpg_Decoder::drawJpg(int32_t x, int32_t y, const uint8_t jpeg_data[], uint32_t  data_size) {
  JDEC jdec;
  JRESULT jresult = JDR_OK;

  jpg_source = TJPG_ARRAY;
  array_index = 0;
  array_data  = jpeg_data;
  array_size  = data_size;

  jpeg_x = x;
  jpeg_y = y;

  jdec.swap = _swap;

  // Analyse input data
#ifdef ESP8266
  // The fixed workspace if one was set (setWorkspace()), otherwise taken from the heap
  // only while decoding: TJPGD_WORKSPACE_SIZE is a worst case, so take what is free and
  // let jd_prepare() decide whether it is enough. malloc is 4-byte aligned.
  uint8_t *workspace = fixedWorkspace;
  size_t poolSize = fixedWorkspaceSize;
  if (!workspace)
  {
    poolSize = jpgWorkspaceSize();
    if (!poolSize) return JDR_MEM1;
    workspace = (uint8_t *)malloc(poolSize);
    if (!workspace) return JDR_MEM1;
  }
#else
  const size_t poolSize = TJPGD_WORKSPACE_SIZE; // the member array "workspace"
#endif
  jresult = jd_prepare(&jdec, jd_input, workspace, poolSize, 0);

  // Extract image and render
  if (jresult == JDR_OK) {
    jresult = jd_decomp(&jdec, jd_output, jpgScale);
  }
#ifdef ESP8266
#ifdef AWTRIX_HEAP_LOG
  logJpgPool(jresult, poolSize, jdec.sz_pool);
#endif
  if (workspace != fixedWorkspace)
    free(workspace);
#endif

  return jresult;
}
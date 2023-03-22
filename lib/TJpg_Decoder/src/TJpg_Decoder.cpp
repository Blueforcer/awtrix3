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

  jresult = jd_prepare(&jdec, jd_input, workspace, TJPGD_WORKSPACE_SIZE, (unsigned int)0);

  // Extract image and render
  if (jresult == JDR_OK)
  {
    jresult = jd_decomp(&jdec, jd_output, jpgScale);
  }
  inFile.seek(0);
  return jresult;
}

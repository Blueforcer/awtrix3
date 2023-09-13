#ifndef GifPlayer_H
#define GifPlayer_H
#include <LittleFS.h>
class GifPlayer
{
public:
#define ERROR_NON 0
#define ERROR_FILEOPEN 1
#define ERROR_FILENOTGIF 2
#define ERROR_BADGIFFORMAT 3
#define ERROR_UNKNOWNCONTROLEXT 4
#define ERROR_FINISHED 5
#define WIDTH 32
#define HEIGHT 8
  uint32_t currentFrame;

private:
  long lastFrameTime;
  int newframeDelay;
  CRGB FrameBuffer[HEIGHT][WIDTH];
  bool lastFrameDrawn = false;
  unsigned long nextFrameTime = 0;
#define GIFHDRTAGNORM "GIF87a"
#define GIFHDRTAGNORM1 "GIF89a"
#define GIFHDRSIZE 6
  FastLED_NeoMatrix *mtx;
#define COLORTBLFLAG 0x80
#define INTERLACEFLAG 0x40
#define TRANSPARENTFLAG 0x01
#define NO_TRANSPARENT_INDEX -1
#define DISPOSAL_NONE 0
#define DISPOSAL_LEAVE 1
#define DISPOSAL_BACKGROUND 2
#define DISPOSAL_RESTORE 3

  typedef struct
  {
    byte Red;
    byte Green;
    byte Blue;
  } _RGB;
  int lsdWidth;
  int lsdHeight;
  int lsdPackedField;
  int lsdAspectRatio;
  int lsdBackgroundIndex;
  int offsetX;
  int offsetY;
  int tbiImageX;
  int tbiImageY;
  int tbiWidth;
  int tbiHeight;
  int tbiPackedBits;
  boolean tbiInterlaced;

public:
  int frameDelay;
  int transparentColorIndex;
  int prevBackgroundIndex;
  int prevDisposalMethod;
  int disposalMethod;
  int lzwCodeSize;
  boolean keyFrame;
  int rectX;
  int rectY;
  int rectWidth;
  int rectHeight;
  int colorCount;
  _RGB gifPalette[256];
  byte lzwImageData[1280];
  char tempBuffer[260];
  File file;
  byte imageData[WIDTH * HEIGHT];
  byte imageDataBU[WIDTH * HEIGHT];

  void backUpStream(int n)
  {
    file.seek(file.position() - n, SeekSet);
  }

  int readByte()
  {
    int b = file.read();
    return b;
  }

  int readWord()
  {
    int b0 = readByte();
    int b1 = readByte();
    return (b1 << 8) | b0;
  }

  int readIntoBuffer(void *buffer, int numberOfBytes)
  {
    int result = file.read(static_cast<uint8_t *>(buffer), numberOfBytes);
    return result;
  }

  void fillImageDataRect(byte colorIndex, int x, int y, int width, int height)
  {
    int yOffset;
    for (int yy = y; yy < height + y; yy++)
    {
      yOffset = yy * WIDTH;
      for (int xx = x; xx < width + x; xx++)
      {
        imageData[yOffset + xx] = colorIndex;
      }
    }
  }

  void fillImageData(byte colorIndex)
  {
    memset(imageData, colorIndex, sizeof(imageData));
  }

  void copyImageDataRect(byte *src, byte *dst, int x, int y, int width, int height)
  {

    int yOffset, offset;

    for (int yy = y; yy < height + y; yy++)
    {
      yOffset = yy * WIDTH;
      for (int xx = x; xx < width + x; xx++)
      {
        offset = yOffset + xx;
        dst[offset] = src[offset];
      }
    }
  }

  void parsePlainTextExtension()
  {
    byte len = readByte();
    readIntoBuffer(tempBuffer, len);
    len = readByte();
    while (len != 0)
    {
      readIntoBuffer(tempBuffer, len);
      len = readByte();
    }
  }

  void parseGraphicControlExtension()
  {
    readByte();
    int packedBits = readByte();
    frameDelay = readWord();
    transparentColorIndex = readByte();

    if ((packedBits & TRANSPARENTFLAG) == 0)
    {
      // Indicate no transparent index
      transparentColorIndex = NO_TRANSPARENT_INDEX;
    }
    disposalMethod = (packedBits >> 2) & 7;
    if (disposalMethod > 3)
    {
      disposalMethod = 0;
    }

    readByte(); // Toss block end
  }

  void parseApplicationExtension()
  {
    memset(tempBuffer, 0, sizeof(tempBuffer));
    byte len = readByte();
    readIntoBuffer(tempBuffer, len);
    len = readByte();
    while (len != 0)
    {
      readIntoBuffer(tempBuffer, len);
      len = readByte();
    }
  }

  void parseCommentExtension()
  {
    byte len = readByte();
    while (len != 0)
    {
      memset(tempBuffer, 0, sizeof(tempBuffer));
      readIntoBuffer(tempBuffer, len);
      len = readByte();
    }
  }

  int parseGIFFileTerminator()
  {
    byte b = readByte();
    if (b != 0x3B)
    {
      return ERROR_BADGIFFORMAT;
    }
    else
    {
      return ERROR_NON;
    }
  }

  unsigned long parseTableBasedImage()
  {
    tbiImageX = readWord();
    tbiImageY = readWord();
    tbiWidth = readWord();
    tbiHeight = readWord();
    tbiPackedBits = readByte();
    tbiInterlaced = ((tbiPackedBits & INTERLACEFLAG) != 0);
    boolean localColorTable = ((tbiPackedBits & COLORTBLFLAG) != 0);
    if (localColorTable)
    {
      int colorBits = ((tbiPackedBits & 7) + 1);
      colorCount = 1 << colorBits;
      int colorTableBytes = sizeof(_RGB) * colorCount;
      readIntoBuffer(gifPalette, colorTableBytes);
    }

    if (keyFrame)
    {
      if (transparentColorIndex == NO_TRANSPARENT_INDEX)
      {
        fillImageData(lsdBackgroundIndex);
      }
      else
      {
        fillImageData(transparentColorIndex);
      }
      keyFrame = false;

      rectX = 0;
      rectY = 0;
      rectWidth = WIDTH;
      rectHeight = HEIGHT;
    }

    if ((prevDisposalMethod != DISPOSAL_NONE) && (prevDisposalMethod != DISPOSAL_LEAVE))
    {
      memset(FrameBuffer, 0, sizeof(FrameBuffer));
    }

    if (prevDisposalMethod == DISPOSAL_BACKGROUND)
    {
      fillImageDataRect(prevBackgroundIndex, rectX, rectY, rectWidth, rectHeight);
    }
    else if (prevDisposalMethod == DISPOSAL_RESTORE)
    {
      copyImageDataRect(imageDataBU, imageData, rectX, rectY, rectWidth, rectHeight);
    }
    prevDisposalMethod = disposalMethod;
    if (disposalMethod != DISPOSAL_NONE)
    {
      rectX = tbiImageX;
      rectY = tbiImageY;
      rectWidth = tbiWidth;
      rectHeight = tbiHeight;
      if (disposalMethod == DISPOSAL_BACKGROUND)
      {
        if (transparentColorIndex != NO_TRANSPARENT_INDEX)
        {
          prevBackgroundIndex = transparentColorIndex;
        }
        else
        {
          prevBackgroundIndex = lsdBackgroundIndex;
        }
      }
      else if (disposalMethod == DISPOSAL_RESTORE)
      {
        copyImageDataRect(imageData, imageDataBU, rectX, rectY, rectWidth, rectHeight);
      }
    }
    lzwCodeSize = readByte();
    int offset = 0;
    int dataBlockSize = readByte();
    while (dataBlockSize != 0)
    {
      backUpStream(1);
      dataBlockSize++;
      if (offset + dataBlockSize <= (int)sizeof(lzwImageData))
      {
        readIntoBuffer(lzwImageData + offset, dataBlockSize);
      }
      else
      {
        int i;
        for (i = 0; i < dataBlockSize; i++)
          file.read();
      }

      offset += dataBlockSize;
      dataBlockSize = readByte();
    }
    lzw_decode_init(lzwCodeSize, lzwImageData);
    decompressAndDisplayFrame();
    redrawLastFrame();
    transparentColorIndex = NO_TRANSPARENT_INDEX;
    disposalMethod = DISPOSAL_NONE;
    if (frameDelay < 1)
    {
      frameDelay = 1;
    }
    newframeDelay = frameDelay * 10;
    return frameDelay * 10;
  }

#define LZW_MAXBITS 10
#define LZW_SIZTABLE (1 << LZW_MAXBITS)
  unsigned int mask[17] = {
      0x0000, 0x0001, 0x0003, 0x0007,
      0x000F, 0x001F, 0x003F, 0x007F,
      0x00FF, 0x01FF, 0x03FF, 0x07FF,
      0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF,
      0xFFFF};

  byte *pbuf;
  int bbits;
  int bbuf;
  int cursize; // The current code size
  int curmask;
  int codesize;
  int clear_code;
  int end_code;
  int newcodes; // First available code
  int top_slot; // Highest code for current size
  int extra_slot;
  int slot; // Last read code
  int fc, oc;
  int bs; // Current buffer size for GIF
  byte *sp;
  byte stack[LZW_SIZTABLE];
  byte suffix[LZW_SIZTABLE];
  unsigned int prefix[LZW_SIZTABLE];

  void lzw_decode_init(int csize, byte *buf)
  {
    pbuf = buf;
    bbuf = 0;
    bbits = 0;
    bs = 0;
    codesize = csize;
    cursize = codesize + 1;
    curmask = mask[cursize];
    top_slot = 1 << cursize;
    clear_code = 1 << codesize;
    end_code = clear_code + 1;
    slot = newcodes = clear_code + 2;
    oc = fc = -1;
    sp = stack;
  }

  int lzw_get_code()
  {
    while (bbits < cursize)
    {
      if (!bs)
      {
        bs = *pbuf++;
      }
      bbuf |= (*pbuf++) << bbits;
      bbits += 8;
      bs--;
    }
    int c = bbuf;
    bbuf >>= cursize;
    bbits -= cursize;
    return c & curmask;
  }

  int lzw_decode(byte *buf, int len)
  {
    int l, c, code;

    if (end_code < 0)
    {
      return 0;
    }
    l = len;

    for (;;)
    {
      while (sp > stack)
      {
        *buf++ = *(--sp);
        if ((--l) == 0)
        {
          goto the_end;
        }
      }
      c = lzw_get_code();
      if (c == end_code)
      {
        break;
      }
      else if (c == clear_code)
      {
        cursize = codesize + 1;
        curmask = mask[cursize];
        slot = newcodes;
        top_slot = 1 << cursize;
        fc = oc = -1;
      }
      else
      {

        code = c;
        if ((code == slot) && (fc >= 0))
        {
          *sp++ = fc;
          code = oc;
        }
        else if (code >= slot)
        {
          break;
        }
        while (code >= newcodes)
        {
          *sp++ = suffix[code];
          code = prefix[code];
        }
        *sp++ = code;
        if ((slot < top_slot) && (oc >= 0))
        {
          suffix[slot] = code;
          prefix[slot++] = oc;
        }
        fc = code;
        oc = c;
        if (slot >= top_slot)
        {
          if (cursize < LZW_MAXBITS)
          {
            top_slot <<= 1;
            curmask = mask[++cursize];
          }
          else
          {
          }
        }
      }
    }
    end_code = -1;
  the_end:
    return len - l;
  }

  void redrawLastFrame()
  {
    for (int y = 0; y < lsdHeight; y++)
    {
      if (y >= sizeof(FrameBuffer) / sizeof(FrameBuffer[0]))
      {
        // y is out of bounds for FrameBuffer
        break;
      }

      for (int x = 0; x < lsdWidth; x++)
      {
        if (x >= sizeof(FrameBuffer[0]) / sizeof(FrameBuffer[0][0]))
        {
          // x is out of bounds for FrameBuffer
          break;
        }
        int xDraw = x + offsetX;
        int yDraw = y + offsetY;
        mtx->drawPixel(xDraw, yDraw, FrameBuffer[y][x]);
      }
    }
  }

  void decompressAndDisplayFrame()
  {
    if (tbiInterlaced)
    {
      for (int line = tbiImageY + 0; line < tbiHeight + tbiImageY; line += 8)
      {
        lzw_decode(imageData + (line * WIDTH) + tbiImageX, tbiWidth);
      }
      for (int line = tbiImageY + 4; line < tbiHeight + tbiImageY; line += 8)
      {
        lzw_decode(imageData + (line * WIDTH) + tbiImageX, tbiWidth);
      }
      for (int line = tbiImageY + 2; line < tbiHeight + tbiImageY; line += 4)
      {
        lzw_decode(imageData + (line * WIDTH) + tbiImageX, tbiWidth);
      }
      for (int line = tbiImageY + 1; line < tbiHeight + tbiImageY; line += 2)
      {
        lzw_decode(imageData + (line * WIDTH) + tbiImageX, tbiWidth);
      }
    }
    else
    {
      for (int line = tbiImageY; line < tbiHeight + tbiImageY; line++)
      {
        lzw_decode(imageData + (line * WIDTH) + tbiImageX, tbiWidth);
      }
    }

    int pixel, yOffset;
    for (int y = tbiImageY; y < tbiHeight + tbiImageY; y++)
    {
      yOffset = y * WIDTH;
      for (int x = tbiImageX; x < tbiWidth + tbiImageX; x++)
      {
        pixel = imageData[yOffset + x];
        if (pixel != transparentColorIndex)
        {
          CRGB color;
          color.r = gifPalette[pixel].Red;
          color.g = gifPalette[pixel].Green;
          color.b = gifPalette[pixel].Blue;
          FrameBuffer[y][x] = color;
        }
        else
        {
          if (disposalMethod == DISPOSAL_BACKGROUND)
          {
            FrameBuffer[y][x] = CRGB::Black;
          }
        }
      }
    }
    ++currentFrame;
    lastFrameTime = millis();
  }

public:
  void setMatrix(FastLED_NeoMatrix *matrix)
  {
    mtx = matrix;
  }

  uint32_t getFrame()
  {
    return currentFrame;
  }

  int playGif(int x, int y, File *imageFile, uint32_t frame = 0)
  {
    offsetX = x;
    offsetY = y;

    if (imageFile->name() == file.name())
    {
      drawFrame();
      return lsdWidth;
    }
    else
    {
      currentFrame = 0;
      file = *imageFile;

      memset(FrameBuffer, 0, sizeof(FrameBuffer));
      memset(gifPalette, 0, sizeof(gifPalette));
      memset(lzwImageData, 0, sizeof(lzwImageData));
      memset(imageData, 0, sizeof(imageData));
      memset(imageDataBU, 0, sizeof(imageDataBU));
      memset(stack, 0, sizeof(stack));
      memset(suffix, 0, sizeof(suffix));
      memset(prefix, 0, sizeof(prefix));
      if (frame != 0)
      {

        parseGifHeader();
        parseLogicalScreenDescriptor();
        parseGlobalColorTable();
        do
        {
          drawFrame(true);
        } while (currentFrame < frame);
      }
      else
      {
        parseGifHeader();
        parseLogicalScreenDescriptor();
        parseGlobalColorTable();
        drawFrame();
      }
    }
    return lsdWidth;
  }

  boolean parseGifHeader()
  {
    char buffer[10];
    readIntoBuffer(buffer, GIFHDRSIZE);
    if ((strncmp(buffer, GIFHDRTAGNORM, GIFHDRSIZE) != 0) &&
        (strncmp(buffer, GIFHDRTAGNORM1, GIFHDRSIZE) != 0))
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  void parseLogicalScreenDescriptor()
  {
    lsdWidth = readWord();
    lsdHeight = readWord();
    lsdPackedField = readByte();
    lsdBackgroundIndex = readByte();
    lsdAspectRatio = readByte();
  }

  void parseGlobalColorTable()
  {
    if (lsdPackedField & COLORTBLFLAG)
    {
      colorCount = 1 << ((lsdPackedField & 7) + 1);
      int colorTableBytes = sizeof(_RGB) * colorCount;
      readIntoBuffer(gifPalette, colorTableBytes);
    }
  }

  unsigned long drawFrame(bool force = false)
  {

    if (!force)
    {
      if (millis() - lastFrameTime < newframeDelay)
      {
        redrawLastFrame();
        return 0;
      }
    }

    lastFrameDrawn = false;

    boolean done = false;
    while (!done)
    {
      byte b = readByte();
      if (b == 0x2c)
      {
        parseTableBasedImage();
        return 0;
      }
      else if (b == 0x21)
      {
        b = readByte();
        switch (b)
        {
        case 0x01:
          parsePlainTextExtension();
          break;
        case 0xf9:
          parseGraphicControlExtension();
          break;
        case 0xfe:
          parseCommentExtension();
          break;
        case 0xff:
          parseApplicationExtension();
          break;
        default:
          return ERROR_UNKNOWNCONTROLEXT;
        }
      }
      else
      {
        done = true;
        file.seek(0);
        currentFrame = 0;
        parseGifHeader();
        parseLogicalScreenDescriptor();
        parseGlobalColorTable();
        drawFrame();
        return ERROR_FINISHED;
      }
    }
    return ERROR_NON;
  }
};
#endif
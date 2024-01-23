/**
** The original 3x5 font is licensed under the 3-clause BSD license:
**
** Copyright 1999 Brian J. Swetland
** Copyright 1999 Vassilii Khachaturov
** Portions (of vt100.c/vt100.h) copyright Dan Marks
** Modifications for Awtrix for improved readability and LaMetric Style Copyright 2023 Blueforcer
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions, and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions, and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the authors may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** Modifications to TomThumb for improved readability are from Robey Pointer,
** see:
** http://robey.lag.net/2010/01/23/tiny-monospace-font.html
**
** Massive modifications in 2018-2023 for Awtrix for improved readability by Blueforcer
**
** The original author does not have any objection to relicensing of Robey
** Pointer's modifications (in this file) in a more permissive license.  See
** the discussion at the above blog, and also here:
** http://opengameart.org/forumtopic/how-to-submit-art-using-the-3-clause-bsd-license
**
** Feb 21, 2016: Conversion from Linux BDF --> Adafruit GFX font,
** with the help of this Python script:
** https://gist.github.com/skelliam/322d421f028545f16f6d
** William Skellenger (williamj@skellenger.net)
** Twitter: @skelliam
**
*/
// AwtrixFont Version 20230713

const uint8_t AwtrixBitmaps[] PROGMEM = {
    0x00,                               /*[0]    0x20 space */
    0x80, 0x80, 0x80, 0x00, 0x80,       /*[1]    0x21 exclam */
    0xA0, 0xA0,                         /*[2]    0x22 quotedbl */
    0xA0, 0xE0, 0xA0, 0xE0, 0xA0,       /*[3]    0x23 numbersign */
    0x60, 0xC0, 0x60, 0xC0, 0x40,       /*[4]    0x24 dollar */
    0xA0, 0x20, 0x40, 0x80, 0xA0,       /*[5]    0x25 percent */
    0xC0, 0xC0, 0xE0, 0xA0, 0x60,       /*[6]    0x26 ampersand */
    0x80, 0x80,                         /*[7]    0x27 quotesingle */
    0x40, 0x80, 0x80, 0x80, 0x40,       /*[8]    0x28 parenleft */
    0x80, 0x40, 0x40, 0x40, 0x80,       /*[9]    0x29 parenright */
    0xA0, 0x40, 0xA0,                   /*[10]   0x2A asterisk */
    0x40, 0xE0, 0x40,                   /*[11]   0x2B plus */
    0x40, 0x80,                         /*[12]   0x2C comma */
    0xE0,                               /*[13]   0x2D hyphen */
    0x80,                               /*[14]   0x2E period */
    0x20, 0x20, 0x40, 0x80, 0x80,       /*[15]   0x2F slash */
    0xE0, 0xA0, 0xA0, 0xA0, 0xE0,       /*[16]   0x30 zero */
    0x40, 0xC0, 0x40, 0x40, 0xE0,       /*[17]   0x31 one */
    0xE0, 0x20, 0xE0, 0x80, 0xE0,       /*[18]   0x32 two */
    0xE0, 0x20, 0xE0, 0x20, 0xE0,       /*[19]   0x33 three */
    0xA0, 0xA0, 0xE0, 0x20, 0x20,       /*[20]   0x34 four */
    0xE0, 0x80, 0xE0, 0x20, 0xE0,       /*[21]   0x35 five */
    0xE0, 0x80, 0xE0, 0xA0, 0xE0,       /*[22]   0x36 six */
    0xE0, 0x20, 0x20, 0x20, 0x20,       /*[23]   0x37 seven */
    0xE0, 0xA0, 0xE0, 0xA0, 0xE0,       /*[24]   0x38 eight */
    0xE0, 0xA0, 0xE0, 0x20, 0xE0,       /*[25]   0x39 nine */
    0x80, 0x00, 0x80,                   /*[26]   0x3A colon */
    0x40, 0x00, 0x40, 0x80,             /*[27]   0x3B semicolon */
    0x20, 0x40, 0x80, 0x40, 0x20,       /*[28]   0x3C less */
    0xE0, 0x00, 0xE0,                   /*[29]   0x3D equal */
    0x80, 0x40, 0x20, 0x40, 0x80,       /*[30]   0x3E greater */
    0xE0, 0x20, 0x40, 0x00, 0x40,       /*[31]   0x3F question */
    0x40, 0xA0, 0xE0, 0x80, 0x60,       /*[32]   0x40 at */
    0xC0, 0xA0, 0xE0, 0xA0, 0xA0,       /*[33]   0x41 A */
    0xC0, 0xA0, 0xC0, 0xA0, 0xC0,       /*[34]   0x42 B */
    0x40, 0xA0, 0x80, 0xA0, 0x40,       /*[35]   0x43 C */
    0xC0, 0xA0, 0xA0, 0xA0, 0xC0,       /*[36]   0x44 D */
    0xE0, 0x80, 0xE0, 0x80, 0xE0,       /*[37]   0x45 E */
    0xE0, 0x80, 0xE0, 0x80, 0x80,       /*[38]   0x46 F */
    0x60, 0x80, 0xA0, 0xA0, 0x60,       /*[39]   0x47 G */
    0xA0, 0xA0, 0xE0, 0xA0, 0xA0,       /*[40]   0x48 H */
    0x80, 0x80, 0x80, 0x80, 0x80,       /*[41]   0x49 I */
    0x20, 0x20, 0x20, 0xA0, 0x40,       /*[42]   0x4A J */
    0xA0, 0xA0, 0xC0, 0xA0, 0xA0,       /*[43]   0x4B K */
    0x80, 0x80, 0x80, 0x80, 0xE0,       /*[44]   0x4C L */
    0x88, 0xD8, 0xA8, 0x88, 0x88,       /*[45]   0x4D M */
    0x90, 0xD0, 0xB0, 0x90, 0x90,       /*[46]   0x4E N */
    0x40, 0xA0, 0xA0, 0xA0, 0x40,       /*[47]   0x4F O */
    0xE0, 0xA0, 0xC0, 0x80, 0x80,       /*[48]   0x50 P */
    0x40, 0xA0, 0xA0, 0xA0, 0x70,       /*[49]   0x51 Q */
    0xE0, 0xA0, 0xC0, 0xA0, 0xA0,       /*[50]   0x52 R */
    0xE0, 0x80, 0xE0, 0x20, 0xE0,       /*[51]   0x53 S */
    0xE0, 0x40, 0x40, 0x40, 0x40,       /*[52]   0x54 T */
    0xA0, 0xA0, 0xA0, 0xA0, 0xE0,       /*[53]   0x55 U */
    0xA0, 0xA0, 0xA0, 0xA0, 0x40,       /*[54]   0x56 V */
    0x88, 0x88, 0x88, 0xA8, 0x50,       /*[55]   0x57 W */
    0xA0, 0xA0, 0x40, 0xA0, 0xA0,       /*[56]   0x58 X */
    0xA0, 0xA0, 0xE0, 0x20, 0xC0,       /*[57]   0x59 Y */
    0xE0, 0x20, 0x40, 0x80, 0xE0,       /*[58]   0x5A Z */
    0xE0, 0x80, 0x80, 0x80, 0xE0,       /*[59]   0x5B bracketleft */
    0x80, 0x40, 0x20,                   /*[60]   0x5C backslash */
    0xE0, 0x20, 0x20, 0x20, 0xE0,       /*[61]   0x5D bracketright */
    0x40, 0xA0,                         /*[62]   0x5E asciicircum */
    0xE0,                               /*[63]   0x5F underscore */
    0x80, 0x40,                         /*[64]   0x60 grave */
    0xC0, 0x60, 0xA0, 0xE0,             /*[65]   0x61 a */
    0x80, 0xC0, 0xA0, 0xA0, 0xC0,       /*[66]   0x62 b */
    0x60, 0x80, 0x80, 0x60,             /*[67]   0x63 c */
    0x20, 0x60, 0xA0, 0xA0, 0x60,       /*[68]   0x64 d */
    0x60, 0xA0, 0xC0, 0x60,             /*[69]   0x65 e */
    0x20, 0x40, 0xE0, 0x40, 0x40,       /*[70]   0x66 f */
    0x60, 0xA0, 0xE0, 0x20, 0x40,       /*[71]   0x67 g */
    0x80, 0xC0, 0xA0, 0xA0, 0xA0,       /*[72]   0x68 h */
    0x80, 0x00, 0x80, 0x80, 0x80,       /*[73]   0x69 i */
    0x20, 0x00, 0x20, 0x20, 0xA0, 0x40, /*[74]   0x6A j */
    0x80, 0xA0, 0xC0, 0xC0, 0xA0,       /*[75]   0x6B k */
    0xC0, 0x40, 0x40, 0x40, 0xE0,       /*[76]   0x6C l */
    0xE0, 0xE0, 0xE0, 0xA0,             /*[77]   0x6D m */
    0xC0, 0xA0, 0xA0, 0xA0,             /*[78]   0x6E n */
    0x40, 0xA0, 0xA0, 0x40,             /*[79]   0x6F o */
    0xC0, 0xA0, 0xA0, 0xC0, 0x80,       /*[80]   0x70 p */
    0x60, 0xA0, 0xA0, 0x60, 0x20,       /*[81]   0x71 q */
    0x60, 0x80, 0x80, 0x80,             /*[82]   0x72 r */
    0x60, 0xC0, 0x60, 0xC0,             /*[83]   0x73 s */
    0x40, 0xE0, 0x40, 0x40, 0x60,       /*[84]   0x74 t */
    0xA0, 0xA0, 0xA0, 0x60,             /*[85]   0x75 u */
    0xA0, 0xA0, 0xE0, 0x40,             /*[86]   0x76 v */
    0xA0, 0xE0, 0xE0, 0xE0,             /*[87]   0x77 w */
    0xA0, 0x40, 0x40, 0xA0,             /*[88]   0x78 x */
    0xA0, 0xA0, 0x60, 0x20, 0x40,       /*[89]   0x79 y */
    0xE0, 0x60, 0xC0, 0xE0,             /*[90]   0x7A z */
    0x60, 0x40, 0x80, 0x40, 0x60,       /*[91]   0x7B braceleft */
    0x80, 0x80, 0x80, 0x80, 0x80,       /*[92]   0x7C bar */
    0xC0, 0x40, 0x20, 0x40, 0xC0,       /*[93]   0x7D braceright */
    0x60, 0xC0,                         /*[94]   0x7E asciitilde */

    0xE0, 0xA0, 0xE0, 0xA0, 0xA0,              // [95]   А
    0xE0, 0x80, 0xE0, 0xA0, 0xC0,              // [96]   Б
    0xC0, 0xA0, 0xE0, 0xA0, 0xC0,              // [97]   В
    0xE0, 0x80, 0x80, 0x80, 0x80,              // [98]   Г
    0x60, 0x50, 0x50, 0x50, 0xE8,              // [99]   Д
    0xE0, 0x80, 0xC0, 0x80, 0xE0,              // [100]  Е 
    0xA8, 0xA8, 0x70, 0xA8, 0xA8,              // [101]  Ж
    0xC0, 0x20, 0x60, 0x20, 0xE0,              // [102]  З
    0x90, 0x90, 0xB0, 0xD0, 0x90,              // [103]  И
    0x20, 0x90, 0xB0, 0xD0, 0x90,	       // [104]  Й 
    0xA0, 0xA0, 0xC0, 0xA0, 0xA0,              // [105]  К
    0x60, 0xA0, 0xA0, 0xA0, 0xA0,              // [106]  Л
    0x88, 0xD8, 0xA8, 0x88, 0x88,              // [107]  М
    0xA0, 0xA0, 0xE0, 0xA0, 0xA0,              // [108]  Н
    0xE0, 0xA0, 0xA0, 0xA0, 0xE0,              // [109]  О
    0xE0, 0xA0, 0xA0, 0xA0, 0xA0,              // [110]  П
    0xE0, 0xA0, 0xE0, 0x80, 0x80,              // [111]  Р
    0xE0, 0x80, 0x80, 0x80, 0xE0,              // [112]  С
    0xE0, 0x40, 0x40, 0x40, 0x40,              // [113]  Т
    0xA0, 0xA0, 0xE0, 0x20, 0xC0,              // [114]  У
    0xF8, 0xA8, 0xF8, 0x20, 0x20,              // [115]  Ф
    0xA0, 0xA0, 0x40, 0xA0, 0xA0,              // [116]  Х
    0xA0, 0xA0, 0xA0, 0xA0, 0xD0,              // [117]  Ц
    0xA0, 0xA0, 0x60, 0x20, 0x20,              // [118]  Ч
    0xA8, 0xA8, 0xA8, 0xA8, 0xF8,              // [119]  Ш
    0xA8, 0xA8, 0xA8, 0xA8, 0xF4,              // [120]  Щ
    0xC0, 0x40, 0x70, 0x50, 0x70,              // [121]  Ъ
    0x88, 0x88, 0xE8, 0xA8, 0xE8,              // [122]  Ы
    0x80, 0x80, 0xE0, 0xA0, 0xE0,              // [123]  Ь
    0xC0, 0x20, 0x60, 0x20, 0xC0,              // [124]  Э
    0xB8, 0xA8, 0xE8, 0xA8, 0xB8,              // [125]  Ю
    0xE0, 0xA0, 0x60, 0xA0, 0xA0,              // [126]  Я
    0xA0, 0x00, 0x40, 0x40, 0x40, 0x00, 0x00,  // [127]  Ї
    0x60, 0x80, 0xC0, 0x80, 0x60,	           // [128]  Є

    0x80, 0x00, 0x80, 0x80, 0x80,       /*[129] 0xA1 exclamdown */
    0x40, 0xE0, 0x80, 0xE0, 0x40,       /*[130] 0xA2 cent */
    0x60, 0x40, 0xE0, 0x40, 0xE0,       /*[131] 0xA3 sterling */
    0xA0, 0x40, 0xE0, 0x40, 0xA0,       /*[132] 0xA4 currency */
    0xA0, 0xA0, 0x40, 0xE0, 0x40,       /*[133] 0xA5 yen */
    0x80, 0x80, 0x00, 0x80, 0x80,       /*[134] 0xA6 brokenbar */
    0x60, 0x40, 0xA0, 0x40, 0xC0,       /*[135] 0xA7 section */
    0xA0,                               /*[136] 0xA8 dieresis */
    0x60, 0x80, 0x60,                   /*[137] 0xA9 copyright */
    0x60, 0xA0, 0xE0, 0x00, 0xE0,       /*[138] 0xAA ordfeminine */
    0x40, 0x80, 0x40,                   /*[139] 0xAB guillemotleft */
    0xE0, 0x20,                         /*[140] 0xAC logicalnot */
    0xC0,                               /*[141] 0xAD softhyphen */
    0xC0, 0xC0, 0xA0,                   /*[142] 0xAE registered */
    0xE0,                               /*[143] 0xAF macron */
    0xC0, 0xC0, 0x00,                   /*[144] 0xB0 degree */
    0x40, 0xE0, 0x40, 0x00, 0xE0,       /*[145] 0xB1 plusminus */
    0xC0, 0x40, 0x60,                   /*[146] 0xB2 twosuperior */
    0xE0, 0x60, 0xE0,                   /*[147] 0xB3 threesuperior */
    0x40, 0x80,                         /*[148] 0xB4 acute */
    0xA0, 0xA0, 0xA0, 0xC0, 0x80,       /*[149] 0xB5 mu */
    0x60, 0xA0, 0x60, 0x60, 0x60,       /*[150] 0xB6 paragraph */
    0xE0, 0xE0, 0xE0,                   /*[151] 0xB7 periodcentered */
    0x40, 0x20, 0xC0,                   /*[152] 0xB8 cedilla */
    0x80, 0x80, 0x80,                   /*[153] 0xB9 onesuperior */
    0x40, 0xA0, 0x40, 0x00, 0xE0,       /*[154] 0xBA ordmasculine */
    0x80, 0x40, 0x80,                   /*[155] 0xBB guillemotright */
    0x80, 0x80, 0x00, 0x60, 0x20,       /*[156] 0xBC onequarter */
    0x80, 0x80, 0x00, 0xC0, 0x60,       /*[157] 0xBD onehalf */
    0xC0, 0xC0, 0x00, 0x60, 0x20,       /*[158] 0xBE threequarters */
    0x40, 0x00, 0x40, 0x80, 0xE0,       /*[159] 0xBF questiondown */
    0x40, 0x20, 0x40, 0xE0, 0xA0,       /*[160] 0xC0 Agrave */
    0x40, 0x80, 0x40, 0xE0, 0xA0,       /*[161] 0xC1 Aacute */
    0xE0, 0x00, 0x40, 0xE0, 0xA0,       /*[162] 0xC2 Acircumflex */
    0x60, 0xC0, 0x40, 0xE0, 0xA0,       /*[163] 0xC3 Atilde */
    0xA0, 0x40, 0xA0, 0xE0, 0xA0,       /*[164] 0xC4 Adieresis */
    0xC0, 0xC0, 0xA0, 0xE0, 0xA0,       /*[165] 0xC5 Aring */
    0x60, 0xC0, 0xE0, 0xC0, 0xE0,       /*[166] 0xC6 AE */
    0x60, 0x80, 0x80, 0x60, 0x20, 0x40, /*[167] 0xC7 Ccedilla */
    0x40, 0x20, 0xE0, 0xC0, 0xE0,       /*[168] 0xC8 Egrave */
    0x40, 0x80, 0xE0, 0xC0, 0xE0,       /*[169] 0xC9 Eacute */
    0xE0, 0x00, 0xE0, 0xC0, 0xE0,       /*[170] 0xCA Ecircumflex */
    0xA0, 0x00, 0xE0, 0xC0, 0xE0,       /*[171] 0xCB Edieresis */
    0x40, 0x20, 0xE0, 0x40, 0xE0,       /*[172] 0xCC Igrave */
    0x40, 0x80, 0xE0, 0x40, 0xE0,       /*[173] 0xCD Iacute */
    0xE0, 0x00, 0xE0, 0x40, 0xE0,       /*[174] 0xCE Icircumflex */
    0xA0, 0x00, 0xE0, 0x40, 0xE0,       /*[175] 0xCF Idieresis */
    0xC0, 0xA0, 0xE0, 0xA0, 0xC0,       /*[176] 0xD0 Eth */
    0xC0, 0x60, 0xA0, 0xE0, 0xA0,       /*[177] 0xD1 Ntilde */
    0x40, 0x20, 0xE0, 0xA0, 0xE0,       /*[178] 0xD2 Ograve */
    0x40, 0x80, 0xE0, 0xA0, 0xE0,       /*[179] 0xD3 Oacute */
    0xE0, 0x00, 0xE0, 0xA0, 0xE0,       /*[180] 0xD4 Ocircumflex */
    0xC0, 0x60, 0xE0, 0xA0, 0xE0,       /*[181] 0xD5 Otilde */
    0xA0, 0x00, 0xE0, 0xA0, 0xE0,       /*[182] 0xD6 Odieresis */
    0xA0, 0x40, 0xA0,                   /*[183] 0xD7 multiply */
    0x60, 0xA0, 0xE0, 0xA0, 0xC0,       /*[184] 0xD8 Oslash */
    0x80, 0x40, 0xA0, 0xA0, 0xE0,       /*[185] 0xD9 Ugrave */
    0x20, 0x40, 0xA0, 0xA0, 0xE0,       /*[186] 0xDA Uacute */
    0xE0, 0x00, 0xA0, 0xA0, 0xE0,       /*[187] 0xDB Ucircumflex */
    0xA0, 0x00, 0xA0, 0xA0, 0xE0,       /*[188] 0xDC Udieresis */
    0x20, 0x40, 0xA0, 0xE0, 0x40,       /*[189] 0xDD Yacute */
    0x80, 0xE0, 0xA0, 0xE0, 0x80,       /*[190] 0xDE Thorn */
    0x60, 0xA0, 0xC0, 0xA0, 0xC0, 0x80, /*[191] 0xDF germandbls */
    0x40, 0x20, 0x60, 0xA0, 0xE0,       /*[192] 0xE0 agrave */
    0x40, 0x80, 0x60, 0xA0, 0xE0,       /*[193] 0xE1 aacute */
    0xE0, 0x00, 0x60, 0xA0, 0xE0,       /*[194] 0xE2 acircumflex */
    0x60, 0xC0, 0x60, 0xA0, 0xE0,       /*[195] 0xE3 atilde */
    0xA0, 0x00, 0x60, 0xA0, 0xE0,       /*[196] 0xE4 adieresis */
    0x60, 0x60, 0x60, 0xA0, 0xE0,       /*[197] 0xE5 aring */
    0x60, 0xE0, 0xE0, 0xC0,             /*[198] 0xE6 ae */
    0x60, 0x80, 0x60, 0x20, 0x40,       /*[199] 0xE7 copy&pasteistrash */
    0x40, 0x20, 0x60, 0xE0, 0x60,       /*[200] 0xE8 egrave */
    0x40, 0x80, 0x60, 0xE0, 0x60,       /*[201] 0xE9 eacute */
    0xE0, 0x00, 0x60, 0xE0, 0x60,       /*[202] 0xEA ecircumflex */
    0xA0, 0x00, 0x60, 0xE0, 0x60,       /*[203] 0xEB edieresis */
    0x80, 0x40, 0x80, 0x80, 0x80,       /*[204] 0xEC igrave */
    0x40, 0x80, 0x40, 0x40, 0x40,       /*[205] 0xED iacute */
    0xE0, 0x00, 0x40, 0x40, 0x40,       /*[206] 0xEE icircumflex */
    0xA0, 0x00, 0x40, 0x40, 0x40,       /*[207] 0xEF idieresis */
    0x60, 0xC0, 0x60, 0xA0, 0x60,       /*[208] 0xF0 eth */
    0xC0, 0x60, 0xC0, 0xA0, 0xA0,       /*[209] 0xF1 ntilde */
    0x40, 0x20, 0x40, 0xA0, 0x40,       /*[210] 0xF2 ograve */
    0x40, 0x80, 0x40, 0xA0, 0x40,       /*[211] 0xF3 oacute */
    0xE0, 0x00, 0x40, 0xA0, 0x40,       /*[212] 0xF4 ocircumflex */
    0xC0, 0x60, 0x40, 0xA0, 0x40,       /*[213] 0xF5 otilde */
    0xA0, 0x00, 0x40, 0xA0, 0x40,       /*[214] 0xF6 odieresis */
    0x40, 0x00, 0xE0, 0x00, 0x40,       /*[215] 0xF7 divide */
    0x60, 0xE0, 0xA0, 0xC0,             /*[216] 0xF8 oslash */
    0x80, 0x40, 0xA0, 0xA0, 0x60,       /*[217] 0xF9 ugrave */
    0x20, 0x40, 0xA0, 0xA0, 0x60,       /*[218] 0xFA uacute */
    0xE0, 0x00, 0xA0, 0xA0, 0x60,       /*[219] 0xFB ucircumflex */
    0xA0, 0x00, 0xA0, 0xA0, 0x60,       /*[220] 0xFC udieresis */
    0x20, 0x40, 0xA0, 0x60, 0x20, 0x40, /*[221] 0xFD yacute */
    0x80, 0xC0, 0xA0, 0xC0, 0x80,       /*[222] 0xFE thorn */
    0xA0, 0x00, 0xA0, 0x60, 0x20, 0x40, /*[223] 0xFF ydieresis */
    0x00,                               /*[224] 0x11D gcircumflex */
    0x60, 0xC0, 0xE0, 0xC0, 0x60,       /*[225] 0x152 OE */
    0x60, 0xE0, 0xC0, 0xE0,             /*[226] 0x153 oe */
    0xA0, 0x60, 0xC0, 0x60, 0xC0,       /*[227] 0x160 Scaron */
    0xA0, 0x60, 0xC0, 0x60, 0xC0,       /*[228] 0x161 scaron */
    0xA0, 0x00, 0xA0, 0x40, 0x40,       /*[229] 0x178 Ydieresis */
    0xA0, 0xE0, 0x60, 0xC0, 0xE0,       /*[230] 0x17D Zcaron */
    0xA0, 0xE0, 0x60, 0xC0, 0xE0,       /*[231] 0x17E zcaron */
    0x00,                               /*[232] 0xEA4 uni0EA4 */
    0x00,                               /*[233] 0x13A0 uni13A0 */
    0x80,                               /*[234] 0x2022 bullet */
    0xA0,                               /*[235] 0x2026 ellipsis */
    0x60, 0xC0, 0xE0, 0xC0, 0x60,       /*[236] 0x20AC Euro */
    0xE0, 0xA0, 0xA0, 0xA0, 0xE0,       /*[237] 0xFFFD uniFFFD */
};

/* {offset, width, height, advance cursor, x offset, y offset} */
const GFXglyph AwtrixFontGlyphs[] PROGMEM = {
    { 0 , 8 , 1 , 2 , 0 , -5 },   /*[0]    0x20 space */
    { 1 , 8 , 5 , 2 , 0 , -5 },   /*[1]    0x21 exclam */
    { 6 , 8 , 2 , 4 , 0 , -5 },   /*[2]    0x22 quotedbl */
    { 8 , 8 , 5 , 4 , 0 , -5 },   /*[3]    0x23 numbersign */
    { 13 , 8 , 5 , 4 , 0 , -5 },  /*[4]    0x24 dollar */
    { 18 , 8 , 5 , 4 , 0 , -5 },  /*[5]    0x25 percent */
    { 23 , 8 , 5 , 4 , 0 , -5 },  /*[6]    0x26 ampersand */
    { 28 , 8 , 2 , 2 , 0 , -5 },  /*[7]    0x27 quotesingle */
    { 30 , 8 , 5 , 3 , 0 , -5 },  /*[8]    0x28 parenleft */
    { 35 , 8 , 5 , 3 , 0 , -5 },  /*[9]    0x29 parenright */
    { 40 , 8 , 3 , 4 , 0 , -5 },  /*[10]   0x2A asterisk */
    { 43 , 8 , 3 , 4 , 0 , -4 },  /*[11]   0x2B plus */
    { 46 , 8 , 2 , 3 , 0 , -1 },  /*[12]   0x2C comma */
    { 48 , 8 , 1 , 4 , 0 , -3 },  /*[13]   0x2D hyphen */
    { 49 , 8 , 1 , 2 , 0 , -1 },  /*[14]   0x2E period */
    { 50 , 8 , 5 , 4 , 0 , -5 },  /*[15]   0x2F slash */
    { 55 , 8 , 5 , 4 , 0 , -5 },  /*[16]   0x30 zero */
    { 60 , 8 , 5 , 4 , 0 , -5 },  /*[17]   0x31 one */
    { 65 , 8 , 5 , 4 , 0 , -5 },  /*[18]   0x32 two */
    { 70 , 8 , 5 , 4 , 0 , -5 },  /*[19]   0x33 three */
    { 75 , 8 , 5 , 4 , 0 , -5 },  /*[20]   0x34 four */
    { 80 , 8 , 5 , 4 , 0 , -5 },  /*[21]   0x35 five */
    { 85 , 8 , 5 , 4 , 0 , -5 },  /*[22]   0x36 six */
    { 90 , 8 , 5 , 4 , 0 , -5 },  /*[23]   0x37 seven */
    { 95 , 8 , 5 , 4 , 0 , -5 },  /*[24]   0x38 eight */
    { 100 , 8 , 5 , 4 , 0 , -5 }, /*[25]   0x39 nine */
    { 105 , 8 , 3 , 2 , 0 , -4 }, /*[26]   0x3A colon */
    { 108 , 8 , 4 , 3 , 0 , -4 }, /*[27]   0x3B semicolon */
    { 112 , 8 , 5 , 4 , 0 , -5 }, /*[28]   0x3C less */
    { 117 , 8 , 3 , 4 , 0 , -4 }, /*[29]   0x3D equal */
    { 120 , 8 , 5 , 4 , 0 , -5 }, /*[30]   0x3E greater */
    { 125 , 8 , 5 , 4 , 0 , -5 }, /*[31]   0x3F question */
    { 130 , 8 , 5 , 4 , 0 , -5 }, /*[32]   0x40 at */
    { 135 , 8 , 5 , 4 , 0 , -5 }, /*[33]   0x41 A */
    { 140 , 8 , 5 , 4 , 0 , -5 }, /*[34]   0x42 B */
    { 145 , 8 , 5 , 4 , 0 , -5 }, /*[35]   0x43 C */
    { 150 , 8 , 5 , 4 , 0 , -5 }, /*[36]   0x44 D */
    { 155 , 8 , 5 , 4 , 0 , -5 }, /*[37]   0x45 E */
    { 160 , 8 , 5 , 4 , 0 , -5 }, /*[38]   0x46 F */
    { 165 , 8 , 5 , 4 , 0 , -5 }, /*[39]   0x47 G */
    { 170 , 8 , 5 , 4 , 0 , -5 }, /*[40]   0x48 H */
    { 175 , 8 , 5 , 2 , 0 , -5 }, /*[41]   0x49 I */
    { 180 , 8 , 5 , 4 , 0 , -5 }, /*[42]   0x4A J */
    { 185 , 8 , 5 , 4 , 0 , -5 }, /*[43]   0x4B K */
    { 190 , 8 , 5 , 4 , 0 , -5 }, /*[44]   0x4C L */
    { 195 , 8 , 5 , 6 , 0 , -5 }, /*[45]   0x4D M */
    { 200 , 8 , 5 , 5 , 0 , -5 }, /*[46]   0x4E N */
    { 205 , 8 , 5 , 4 , 0 , -5 }, /*[47]   0x4F O */
    { 210 , 8 , 5 , 4 , 0 , -5 }, /*[48]   0x50 P */
    { 215 , 8 , 5 , 5 , 0 , -5 }, /*[49]   0x51 Q */
    { 220 , 8 , 5 , 4 , 0 , -5 }, /*[50]   0x52 R */
    { 225 , 8 , 5 , 4 , 0 , -5 }, /*[51]   0x53 S */
    { 230 , 8 , 5 , 4 , 0 , -5 }, /*[52]   0x54 T */
    { 235 , 8 , 5 , 4 , 0 , -5 }, /*[53]   0x55 U */
    { 240 , 8 , 5 , 4 , 0 , -5 }, /*[54]   0x56 V */
    { 245 , 8 , 5 , 6 , 0 , -5 }, /*[55]   0x57 W */
    { 250 , 8 , 5 , 4 , 0 , -5 }, /*[56]   0x58 X */
    { 255 , 8 , 5 , 4 , 0 , -5 }, /*[57]   0x59 Y */
    { 260 , 8 , 5 , 4 , 0 , -5 }, /*[58]   0x5A Z */
    { 265 , 8 , 5 , 4 , 0 , -5 }, /*[59]   0x5B bracketleft */
    { 270 , 8 , 3 , 4 , 0 , -4 }, /*[60]   0x5C backslash */
    { 273 , 8 , 5 , 4 , 0 , -5 }, /*[61]   0x5D bracketright */
    { 278 , 8 , 2 , 4 , 0 , -5 }, /*[62]   0x5E asciicircum */
    { 280 , 8 , 1 , 4 , 0 , -1 }, /*[63]   0x5F underscore */
    { 281 , 8 , 2 , 3 , 0 , -5 }, /*[64]   0x60 grave */
    { 283 , 8 , 4 , 4 , 0 , -4 }, /*[65]   0x61 a */
    { 287 , 8 , 5 , 4 , 0 , -5 }, /*[66]   0x62 b */
    { 292 , 8 , 4 , 4 , 0 , -4 }, /*[67]   0x63 c */
    { 296 , 8 , 5 , 4 , 0 , -5 }, /*[68]   0x64 d */
    { 301 , 8 , 4 , 4 , 0 , -4 }, /*[69]   0x65 e */
    { 305 , 8 , 5 , 4 , 0 , -5 }, /*[70]   0x66 f */
    { 310 , 8 , 5 , 4 , 0 , -4 }, /*[71]   0x67 g */
    { 315 , 8 , 5 , 4 , 0 , -5 }, /*[72]   0x68 h */
    { 320 , 8 , 5 , 2 , 0 , -5 }, /*[73]   0x69 i */
    { 325 , 8 , 6 , 4 , 0 , -5 }, /*[74]   0x6A j */
    { 331 , 8 , 5 , 4 , 0 , -5 }, /*[75]   0x6B k */
    { 336 , 8 , 5 , 4 , 0 , -5 }, /*[76]   0x6C l */
    { 341 , 8 , 4 , 4 , 0 , -4 }, /*[77]   0x6D m */
    { 345 , 8 , 4 , 4 , 0 , -4 }, /*[78]   0x6E n */
    { 349 , 8 , 4 , 4 , 0 , -4 }, /*[79]   0x6F o */
    { 353 , 8 , 5 , 4 , 0 , -4 }, /*[80]   0x70 p */
    { 358 , 8 , 5 , 4 , 0 , -4 }, /*[81]   0x71 q */
    { 363 , 8 , 4 , 4 , 0 , -4 }, /*[82]   0x72 r */
    { 367 , 8 , 4 , 4 , 0 , -4 }, /*[83]   0x73 s */
    { 371 , 8 , 5 , 4 , 0 , -5 }, /*[84]   0x74 t */
    { 376 , 8 , 4 , 4 , 0 , -4 }, /*[85]   0x75 u */
    { 380 , 8 , 4 , 4 , 0 , -4 }, /*[86]   0x76 v */
    { 384 , 8 , 4 , 4 , 0 , -4 }, /*[87]   0x77 w */
    { 388 , 8 , 4 , 4 , 0 , -4 }, /*[88]   0x78 x */
    { 392 , 8 , 5 , 4 , 0 , -4 }, /*[89]   0x79 y */
    { 397 , 8 , 4 , 4 , 0 , -4 }, /*[90]   0x7A z */
    { 401 , 8 , 5 , 4 , 0 , -5 }, /*[91]   0x7B braceleft */
    { 406 , 8 , 5 , 2 , 0 , -5 }, /*[92]   0x7C bar */
    { 411 , 8 , 5 , 4 , 0 , -5 }, /*[93]   0x7D braceright */
    { 416 , 8 , 2 , 4 , 0 , -5 }, /*[94]   0x7E asciitilde */

    {418  , 8, 5, 4, 0, -5 },   // [95]  0x7F А
    {423  , 8, 5, 4, 0, -5 },   // [96]  0x80 Б
    {428  , 8, 5, 4, 0, -5 },   // [97]  0x81 В
    {433  , 8, 5, 4, 0, -5 },   // [98]  0x82 Г
    {438  , 8, 5, 6, 0, -5 },   // [99]  0x83 Д
    {443  , 8, 5, 4, 0, -5 },   // [100] 0x84 Е
    {448  , 8, 5, 6, 0, -5 },   // [101] 0x85 Ж
    {453  , 8, 5, 4, 0, -5 },   // [102] 0x86 З
    {458  , 8, 5, 5, 0, -5 },   // [103] 0x87 И
    {463  , 8, 5, 5, 0, -5 },   // [104] 0x88 Й
    {468  , 8, 5, 4, 0, -5 },   // [105] 0x89 К
    {473  , 8, 5, 4, 0, -5 },   // [106] 0x8A Л
    {478  , 8, 5, 6, 0, -5 },   // [107] 0x8B М
    {483  , 8, 5, 4, 0, -5 },   // [108] 0x8C Н
    {488  , 8, 5, 4, 0, -5 },   // [109] 0x8D О
    {493  , 8, 5, 4, 0, -5 },   // [110] 0x8E П
    {498  , 8, 5, 4, 0, -5 },   // [111] 0x8F Р
    {503  , 8, 5, 4, 0, -5 },   // [112] 0x90 С
    {508  , 8, 5, 4, 0, -5 },   // [113] 0x91 Т
    {513  , 8, 5, 4, 0, -5 },   // [114] 0x92 У
    {518  , 8, 5, 6, 0, -5 },   // [115] 0x93 Ф
    {523  , 8, 5, 4, 0, -5 },   // [116] 0x94 Х
    {528  , 8, 5, 5, 0, -5 },   // [117] 0x95 Ц
    {533  , 8, 5, 4, 0, -5 },   // [118] 0x96 Ч
    {538  , 8, 5, 6, 0, -5 },   // [119] 0x97 Ш
    {543  , 8, 5, 7, 0, -5 },   // [120] 0x98 Щ
    {548  , 8, 5, 5, 0, -5 },   // [121] 0x99 Ъ
    {553  , 8, 5, 6, 0, -5 },   // [122] 0x9A Ы
    {558  , 8, 5, 4, 0, -5 },   // [123] 0x9B Ь
    {563  , 8, 5, 4, 0, -5 },   // [124] 0x9C Э
    {568  , 8, 5, 6, 0, -5 },   // [125] 0x9D Ю
    {573  , 8, 5, 4, 0, -5 },   // [126] 0x9E Я
    {578  , 8, 7, 4, 0, -5 },   // [127] 0x9F Ї
    {585  , 8, 5, 4, 0, -5 },   // [128] 0xA0 Є        
    {590  , 8 , 5 , 2 , 0 , -5 } , /*[129] 0xA1 exclamdown */       
    {595  , 8 , 5 , 4 , 0 , -5 } , /*[130] 0xA2 cent */             
    {600  , 8 , 5 , 4 , 0 , -5 } , /*[131] 0xA3 sterling */         
    {605  , 8 , 5 , 4 , 0 , -5 } , /*[132] 0xA4 currency */         
    {610  , 8 , 5 , 4 , 0 , -5 } , /*[133] 0xA5 yen */              
    {615  , 8 , 5 , 2 , 0 , -5 } , /*[134] 0xA6 brokenbar */        
    {620  , 8 , 5 , 4 , 0 , -5 } , /*[135] 0xA7 section */          
    {625  , 8 , 1 , 4 , 0 , -5 } , /*[136] 0xA8 dieresis */         
    {626  , 8 , 3 , 4 , 0 , -5 } , /*[137] 0xA9 copyright */        
    {629  , 8 , 5 , 4 , 0 , -5 } , /*[138] 0xAA ordfeminine */      
    {634  , 8 , 3 , 3 , 0 , -5 } , /*[139] 0xAB guillemotleft */    
    {637  , 8 , 2 , 4 , 0 , -4 } , /*[140] 0xAC logicalnot */       
    {639  , 8 , 1 , 3 , 0 , -3 } , /*[141] 0xAD softhyphen */       
    {640  , 8 , 3 , 4 , 0 , -5 } , /*[142] 0xAE registered */       
    {643  , 8 , 1 , 4 , 0 , -5 } , /*[143] 0xAF macron */           
    {644  , 8 , 3 , 3 , 0 , -5 } , /*[144] 0xB0 degree */           
    {647  , 8 , 5 , 4 , 0 , -5 } , /*[145] 0xB1 plusminus */        
    {652  , 8 , 3 , 4 , 0 , -5 } , /*[146] 0xB2 twosuperior */      
    {655  , 8 , 3 , 4 , 0 , -5 } , /*[147] 0xB3 threesuperior */    
    {658  , 8 , 2 , 3 , 0 , -5 } , /*[148] 0xB4 acute */            
    {660  , 8 , 5 , 4 , 0 , -5 } , /*[149] 0xB5 mu */               
    {665  , 8 , 5 , 4 , 0 , -5 } , /*[150] 0xB6 paragraph */        
    {670  , 8 , 3 , 4 , 0 , -4 } , /*[151] 0xB7 periodcentered */   
    {673  , 8 , 3 , 4 , 0 , -3 } , /*[152] 0xB8 cedilla */          
    {676  , 8 , 3 , 2 , 0 , -5 } , /*[153] 0xB9 onesuperior */      
    {679  , 8 , 5 , 4 , 0 , -5 } , /*[154] 0xBA ordmasculine */     
    {684  , 8 , 3 , 3 , 0 , -5 } , /*[155] 0xBB guillemotright */   
    {687  , 8 , 5 , 4 , 0 , -5 } , /*[156] 0xBC onequarter */       
    {692  , 8 , 5 , 4 , 0 , -5 } , /*[157] 0xBD onehalf */          
    {697  , 8 , 5 , 4 , 0 , -5 } , /*[158] 0xBE threequarters */    
    {702  , 8 , 5 , 4 , 0 , -5 } , /*[159] 0xBF questiondown */     
    {707  , 8 , 5 , 4 , 0 , -5 } , /*[160] 0xC0 Agrave */           
    {712  , 8 , 5 , 4 , 0 , -5 } , /*[161] 0xC1 Aacute */           
    {717  , 8 , 5 , 4 , 0 , -5 } , /*[162] 0xC2 Acircumflex */      
    {722  , 8 , 5 , 4 , 0 , -5 } , /*[163] 0xC3 Atilde */           
    {727  , 8 , 5 , 4 , 0 , -5 } , /*[164] 0xC4 Adieresis */        
    {732  , 8 , 5 , 4 , 0 , -5 } , /*[165] 0xC5 Aring */            
    {737  , 8 , 5 , 4 , 0 , -5 } , /*[166] 0xC6 AE */               
    {742  , 8 , 6 , 4 , 0 , -5 } , /*[167] 0xC7 Ccedilla */         
    {748  , 8 , 5 , 4 , 0 , -5 } , /*[168] 0xC8 Egrave */           
    {753  , 8 , 5 , 4 , 0 , -5 } , /*[169] 0xC9 Eacute */           
    {758  , 8 , 5 , 4 , 0 , -5 } , /*[170] 0xCA Ecircumflex */      
    {763  , 8 , 5 , 4 , 0 , -5 } , /*[171] 0xCB Edieresis */        
    {768  , 8 , 5 , 4 , 0 , -5 } , /*[172] 0xCC Igrave */           
    {773  , 8 , 5 , 4 , 0 , -5 } , /*[173] 0xCD Iacute */           
    {778  , 8 , 5 , 4 , 0 , -5 } , /*[174] 0xCE Icircumflex */      
    {783  , 8 , 5 , 4 , 0 , -5 } , /*[175] 0xCF Idieresis */        
    {788  , 8 , 5 , 4 , 0 , -5 } , /*[176] 0xD0 Eth */              
    {793  , 8 , 5 , 4 , 0 , -5 } , /*[177] 0xD1 Ntilde */           
    {798  , 8 , 5 , 4 , 0 , -5 } , /*[178] 0xD2 Ograve */           
    {803  , 8 , 5 , 4 , 0 , -5 } , /*[179] 0xD3 Oacute */           
    {808  , 8 , 5 , 4 , 0 , -5 } , /*[180] 0xD4 Ocircumflex */      
    {813  , 8 , 5 , 4 , 0 , -5 } , /*[181] 0xD5 Otilde */           
    {818  , 8 , 5 , 4 , 0 , -5 } , /*[182] 0xD6 Odieresis */        
    {823  , 8 , 3 , 4 , 0 , -4 } , /*[183] 0xD7 multiply */         
    {826  , 8 , 5 , 4 , 0 , -5 } , /*[184] 0xD8 Oslash */           
    {831  , 8 , 5 , 4 , 0 , -5 } , /*[185] 0xD9 Ugrave */           
    {836  , 8 , 5 , 4 , 0 , -5 } , /*[186] 0xDA Uacute */           
    {841  , 8 , 5 , 4 , 0 , -5 } , /*[187] 0xDB Ucircumflex */      
    {846  , 8 , 5 , 4 , 0 , -5 } , /*[188] 0xDC Udieresis */        
    {851  , 8 , 5 , 4 , 0 , -5 } , /*[189] 0xDD Yacute */           
    {856  , 8 , 5 , 4 , 0 , -5 } , /*[190] 0xDE Thorn */            
    {861  , 8 , 6 , 4 , 0 , -5 } , /*[191] 0xDF germandbls */       
    {867  , 8 , 5 , 4 , 0 , -5 } , /*[192] 0xE0 agrave */           
    {872  , 8 , 5 , 4 , 0 , -5 } , /*[193] 0xE1 aacute */           
    {877  , 8 , 5 , 4 , 0 , -5 } , /*[194] 0xE2 acircumflex */      
    {882  , 8 , 5 , 4 , 0 , -5 } , /*[195] 0xE3 atilde */           
    {887  , 8 , 5 , 4 , 0 , -5 } , /*[196] 0xE4 adieresis */        
    {892  , 8 , 5 , 4 , 0 , -5 } , /*[197] 0xE5 aring */            
    {897  , 8 , 4 , 4 , 0 , -4 } , /*[198] 0xE6 ae */               
    {901  , 8 , 5 , 4 , 0 , -4 } , /*[199] 0xE7 copy&pasteistrash */
    {906  , 8 , 5 , 4 , 0 , -5 } , /*[200] 0xE8 egrave */           
    {911  , 8 , 5 , 4 , 0 , -5 } , /*[201] 0xE9 eacute */           
    {916  , 8 , 5 , 4 , 0 , -5 } , /*[202] 0xEA ecircumflex */      
    {921  , 8 , 5 , 4 , 0 , -5 } , /*[203] 0xEB edieresis */        
    {926  , 8 , 5 , 3 , 0 , -5 } , /*[204] 0xEC igrave */           
    {931  , 8 , 5 , 3 , 0 , -5 } , /*[205] 0xED iacute */           
    {936  , 8 , 5 , 4 , 0 , -5 } , /*[206] 0xEE icircumflex */      
    {941  , 8 , 5 , 4 , 0 , -5 } , /*[207] 0xEF idieresis */        
    {946  , 8 , 5 , 4 , 0 , -5 } , /*[208] 0xF0 eth */              
    {951  , 8 , 5 , 4 , 0 , -5 } , /*[209] 0xF1 ntilde */           
    {956  , 8 , 5 , 4 , 0 , -5 } , /*[210] 0xF2 ograve */           
    {961  , 8 , 5 , 4 , 0 , -5 } , /*[211] 0xF3 oacute */           
    {966  , 8 , 5 , 4 , 0 , -5 } , /*[212] 0xF4 ocircumflex */      
    {971  , 8 , 5 , 4 , 0 , -5 } , /*[213] 0xF5 otilde */           
    {976  , 8 , 5 , 4 , 0 , -5 } , /*[214] 0xF6 odieresis */        
    {981  , 8 , 5 , 4 , 0 , -5 } , /*[215] 0xF7 divide */           
    {986  , 8 , 4 , 4 , 0 , -4 } , /*[216] 0xF8 oslash */           
    {990  , 8 , 5 , 4 , 0 , -5 } , /*[217] 0xF9 ugrave */           
    {995  , 8 , 5 , 4 , 0 , -5 } , /*[218] 0xFA uacute */           
    {1000 , 8 , 5 , 4 , 0 , -5 }, /*[219] 0xFB ucircumflex */      
    {1005 , 8 , 5 , 4 , 0 , -5 }, /*[220] 0xFC udieresis */        
    {1010 , 8 , 6 , 4 , 0 , -5 }, /*[221] 0xFD yacute */           
    {1016 , 8 , 5 , 4 , 0 , -4 }, /*[222] 0xFE thorn */            
    {1021 , 8 , 6 , 4 , 0 , -5 }, /*[223] 0xFF ydieresis */        
    {1027 , 8 , 1 , 2 , 0 , -1 }, /*[224] 0x11D gcircumflex */     
    {1028 , 8 , 5 , 4 , 0 , -5 }, /*[225] 0x152 OE */              
    {1033 , 8 , 4 , 4 , 0 , -4 }, /*[226] 0x153 oe */              
    {1037 , 8 , 5 , 4 , 0 , -5 }, /*[227] 0x160 Scaron */          
    {1042 , 8 , 5 , 4 , 0 , -5 }, /*[228] 0x161 scaron */          
    {1047 , 8 , 5 , 4 , 0 , -5 }, /*[229] 0x178 Ydieresis */       
    {1052 , 8 , 5 , 4 , 0 , -5 }, /*[230] 0x17D Zcaron */          
    {1057 , 8 , 5 , 4 , 0 , -5 }, /*[231] 0x17E zcaron */          
    {1062 , 8 , 1 , 2 , 0 , -1 }, /*[232] 0xEA4 uni0EA4 */         
    {1063 , 8 , 1 , 2 , 0 , -1 }, /*[233] 0x13A0 uni13A0 */        
    {1064 , 8 , 1 , 2 , 0 , -3 }, /*[234] 0x2022 bullet */         
    {1065 , 8 , 1 , 4 , 0 , -1 }, /*[235] 0x2026 ellipsis */       
    {1066 , 8 , 5 , 4 , 0 , -5 }, /*[236] 0x20AC Euro */           
    {1071 , 8 , 5 , 4 , 0 , -5 }, /*[237] 0xFFFD uniFFFD */        
};

const GFXfont AwtrixFont PROGMEM = {
    (uint8_t *)AwtrixBitmaps,
    (GFXglyph *)AwtrixFontGlyphs,
    0x20, 0xFF, 6};

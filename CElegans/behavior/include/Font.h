/* Font
 *
 * Copyright (C) 2003-2004, Alexander Zaprjagaev <frustum@frustum.org>
 * Modified by Abdul Bezrati <abezrati@hotmail.com>
 */

#ifndef __FONT_H__
#define __FONT_H__

#include "TextureUtils.h"

class CFont
{
  public:
    CFont();
   ~CFont();

    void printProportional(float widthRatio , float heightRatio,
                           float width,       float height,
                           float r,  float g, float b,
                           const std::string &string);

    void print(float x,     float y,
               float width, float height,
               float r,     float g, float b,
               const std::string &string);

    void printSubString(float x,     float y,
                        float width, float height,
                        float r,     float g, float b,
                        int   start, int end,
                        const std::string &string);

    
    Tuple2i getStringDimensions(const std::string &string);
    bool    load(const char *fontPath);
    int     getMaxFittingLength(const std::string &string, int bounds);
    int     getHeight();
    const   int *getCharHorizontalGlyphs() const;

  private:
    Texture   fontTexture;
    int       spaces[256],
              fontHeight;
    GLint     fontBaseRange;
};

#endif /* __FONT_H__ */

#ifndef TEXTURE_UTILS_H
#define TEXTURE_UTILS_H

#include "GenUtils.h"

/*******************************************************************************************/
/*MediaInfo                                                                                */
/*                                                                                         */
/*******************************************************************************************/

template <class MediaInfoType>
class MediaInfo
{
  public:

    MediaInfo(const std::string &mediaPath_, MediaInfoType mediaArg = 0)
    {
      userCount = 1;
      mediaPath = mediaPath_;
      media     = mediaArg;
    }

    MediaInfo(const MediaInfo &copy)
    {
      this->operator=(copy);
    }

    MediaInfo &operator =(const MediaInfo &copy)
    {
      if(this != &copy)
      {
        userCount = copy.userCount;
        mediaPath = copy.mediaPath;
        media     = copy.media;
      }
      return *this;
    }

    void setMediaPath(const char *mediaPath_)
    {
      mediaPath = mediaPath_;
    }

    const std::string &getMediaPath() const
    {
      return mediaPath;
    }

    void increaseUserCount()
    {
      userCount++;
    }

    void decreaseUserCount()
    {
      userCount--;
    }

    unsigned int getUserCount()
    {
      return userCount;
    }

    MediaInfoType &getMedia()
    {
      return media;
    }

    void  setMedia(MediaInfoType &mediaArg)
    {
      media = mediaArg;
    }

    bool operator > (const MediaInfo &info)
    {
      return mediaPath > info.mediaPath;
    }

    bool operator < (const MediaInfo &info)
    {
      return  mediaPath < info.mediaPath;
    }

    bool operator == (const MediaInfo &info)
    {
      return (mediaPath == info.mediaPath) &&
             (userCount == info.userCount) &&
             (media     == info.media    );
    }

    bool operator != (const MediaInfo &info)
    {
      return !this->operator ==(info);
    }

  protected:
    unsigned int   userCount;
    MediaInfoType  media;
    std::string    mediaPath;

};

typedef MediaInfo<unsigned int> TextureInfo;

/*******************************************************************************************/
/*TexturesManager                                                                          */
/*                                                                                         */
/*******************************************************************************************/

class TexturesManager
{
  public:
    static bool        addTextureInfo(TextureInfo *textureInfo);
    static TextureInfo *getTextureInfo(const char* texturePath);
    static TextureInfo *getTextureInfo(GLuint textureID);
    static void         flushUnusedTextures();

    static void printTexturesInfo();
    static void flushAllTextures();
  private:
    static vector<TextureInfo*> textureCollection;

};

/*******************************************************************************************/
/*Image                                                                                    */
/*                                                                                         */
/*******************************************************************************************/

class Image
{
  private:
    std::string     path;
    unsigned char  *dataBuffer;
    unsigned int    internalFormat,
                    components,
                    format;

    unsigned short  height,
                    width,
                    depth;

    bool loadTGA(const char* filename);
    bool loadUncompressed8BitTGA(const char * filename);
    bool loadCompressedTrueColorTGA(const char * filename);
    bool loadUncompressedTrueColorTGA(const char * filename);

    bool loadJPG(const char* filename);
    void getJPGInfo();
    int  decodeScanJPG();
    int  decodeJPG();

  public:
    Image(const char* path = NULL);
   ~Image();

   static bool isDDSVolume(const char* filename);
   static bool isDDSCube  (const char* filename);

   void setHeight(unsigned short);
   void setWidth(unsigned short);
   void setDepth(unsigned short);

   void setFormat(unsigned int );
   void setDataBuffer(const unsigned char* );
   void flipVertically();
   void setInternalFormat(unsigned int );
   void setComponentsCount(unsigned int );

   bool load(const char*);

   const std::string    &getPath()            const;
   const unsigned int    getComponentsCount() const;
   const unsigned int    getInternalFormat()  const;
   const unsigned char*  getDataBuffer()      const;
   const unsigned int    getFormat()          const;

   const unsigned short getHeight() const;
   const unsigned short getWidth()  const;
   const unsigned short getDepth()  const;
};

/*
 * PNG loader library for OpenGL v1.45 (10/07/00)
 * by Ben Wyatt ben@wyatt100.freeserve.co.uk
 * Using LibPNG 1.0.2 and ZLib 1.1.3
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the author be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is hereby granted to use, copy, modify, and distribute this
 * source code, or portions hereof, for any purpose, without fee, subject to
 * the following restrictions:
 *
 * 1. The origin of this source code must not be misrepresented. You must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered versions must be plainly marked as such and must not be
 *    misrepresented as being the original source.
 * 3. This notice must not be removed or altered from any source distribution.
 */


#ifndef _GLPNG_H_
#define _GLPNG_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* XXX This is from Win32's <windef.h> */
#ifndef APIENTRY
	#if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
		#define APIENTRY    __stdcall
	#else
		#define APIENTRY
	#endif
#endif

/* Mipmapping parameters */
#define PNG_NOMIPMAPS      0 /* No mipmapping                        */
#define PNG_BUILDMIPMAPS  -1 /* Calls a clone of gluBuild2DMipmaps() */
#define PNG_SIMPLEMIPMAPS -2 /* Generates mipmaps without filtering  */

/* Who needs an "S" anyway? */
#define PNG_NOMIPMAP     PNG_NOMIPMAPS
#define PNG_BUILDMIPMAP  PNG_BUILDMIPMAPS
#define PNG_SIMPLEMIPMAP PNG_SIMPLEMIPMAPS

/* Transparency parameters */
#define PNG_CALLBACK  -3 /* Call the callback function to generate alpha   */
#define PNG_ALPHA     -2 /* Use alpha channel in PNG file, if there is one */
#define PNG_SOLID     -1 /* No transparency                                */
#define PNG_STENCIL    0 /* Sets alpha to 0 for r=g=b=0, 1 otherwise       */
#define PNG_BLEND1     1 /* a = r+g+b                                      */
#define PNG_BLEND2     2 /* a = (r+g+b)/2                                  */
#define PNG_BLEND3     3 /* a = (r+g+b)/3                                  */
#define PNG_BLEND4     4 /* a = r*r+g*g+b*b                                */
#define PNG_BLEND5     5 /* a = (r*r+g*g+b*b)/2                            */
#define PNG_BLEND6     6 /* a = (r*r+g*g+b*b)/3                            */
#define PNG_BLEND7     7 /* a = (r*r+g*g+b*b)/4                            */
#define PNG_BLEND8     8 /* a = sqrt(r*r+g*g+b*b)                          */

typedef struct {
	unsigned int Width;
	unsigned int Height;
	unsigned int Depth;
	unsigned int Alpha;
} pngInfo;

typedef struct {
	unsigned int Width;
	unsigned int Height;
	unsigned int Depth;
	unsigned int Alpha;

	unsigned int Components;
	unsigned char *Data;
	unsigned char *Palette;
} pngRawInfo;

extern int APIENTRY pngLoadRaw(const char *filename, pngRawInfo *rawinfo);
extern int APIENTRY pngLoadRawF(FILE *file, pngRawInfo *rawinfo);

extern int APIENTRY pngLoad(const char *filename, int mipmap, int trans, pngInfo *info);
extern int APIENTRY pngLoadF(FILE *file, int mipmap, int trans, pngInfo *info);

extern unsigned int APIENTRY pngBind(const char *filename, int mipmap, int trans, pngInfo *info, int wrapst, int minfilter, int magfilter);
extern unsigned int APIENTRY pngBindF(FILE *file, int mipmap, int trans, pngInfo *info, int wrapst, int minfilter, int magfilter);

extern void APIENTRY pngSetStencil(unsigned char red, unsigned char green, unsigned char blue);
extern void APIENTRY pngSetAlphaCallback(unsigned char (*callback)(unsigned char red, unsigned char green, unsigned char blue));
extern void APIENTRY pngSetViewingGamma(double viewingGamma);
extern void APIENTRY pngSetStandardOrientation(int standardorientation);

bool loadPNG(const char* filename, Image* image);

#ifdef __cplusplus
}
#endif

#endif

/*******************************************************************************************/
/*Texture                                                                                  */
/*                                                                                         */
/*******************************************************************************************/

class Texture : public IOXMLObject
{
  public:
    Texture(GLuint target = GL_TEXTURE_2D);
    Texture(const Texture &copy);
    Texture &operator = (const Texture &);
   ~Texture();

    virtual bool loadXMLSettings(const TiXmlElement *element);
    virtual bool exportXMLSettings(std::ofstream &xmlFile  );

    bool load2D(const char* infoEndOrPath,
                GLuint clampS    = GL_REPEAT,
                GLuint clampT    = GL_REPEAT,
                GLuint magFilter = GL_LINEAR,
                GLuint minFilter = GL_LINEAR_MIPMAP_LINEAR,
                bool   mipmap    = true);

    bool load2DImage(const Image &image,
                     GLuint clampS    = GL_REPEAT,
                     GLuint clampT    = GL_REPEAT,
                     GLuint magFilter = GL_LINEAR,
                     GLuint minFilter = GL_LINEAR_MIPMAP_LINEAR,
                     bool  mipmap    = true);

    bool loadCube(const char* infoEndOrPath,
                  GLuint clampS    = GL_CLAMP_TO_EDGE,
                  GLuint clampT    = GL_CLAMP_TO_EDGE,
                  GLuint magFilter = GL_LINEAR,
                  GLuint minFilter = GL_LINEAR_MIPMAP_LINEAR,
                  bool  mipmap    = true);

    bool create1DShell(const char* name,
                       GLuint width     = 128,
                       GLuint inFormat  = GL_RGBA8,
                       GLuint format    = GL_RGBA,
                       GLuint clampS    = GL_REPEAT,
                       GLuint clampT    = GL_REPEAT,
                       GLuint magFilter = GL_LINEAR,
                       GLuint minFilter = GL_LINEAR);

    bool create2DShell(const char* name,
                       GLuint width     = 128,
                       GLuint height    = 128,
                       GLuint inFormat  = GL_RGBA8,
                       GLuint format    = GL_RGBA,
                       GLuint clampS    = GL_REPEAT,
                       GLuint clampT    = GL_REPEAT,
                       GLuint magFilter = GL_LINEAR,
                       GLuint minFilter = GL_LINEAR);

    bool createRectShell(const char* name,
                         GLuint width     = 128,
                         GLuint height    = 128,
                         GLuint inFormat  = GL_RGBA8,
                         GLuint format    = GL_RGBA,
                         GLuint clampS    = GL_CLAMP_TO_EDGE,
                         GLuint clampT    = GL_CLAMP_TO_EDGE,
                         GLuint magFilter = GL_LINEAR,
                         GLuint minFilter = GL_LINEAR);

    bool create3DShell(const char* name,
                       GLuint width     = 64,
                       GLuint height    = 64,
                       GLuint depth     = 64,
                       GLuint inFormat  = GL_RGBA8,
                       GLuint format    = GL_RGBA,
                       GLuint clampS    = GL_REPEAT,
                       GLuint clampT    = GL_REPEAT,
                       GLuint clampR    = GL_REPEAT,
                       GLuint magFilter = GL_LINEAR,
                       GLuint minFilter = GL_LINEAR);

    bool createCubeShell(const char* name,
                         GLuint size      = 128,
                         GLuint inFormat  = GL_RGBA8,
                         GLuint format    = GL_RGBA,
                         GLuint clampS    = GL_CLAMP_TO_EDGE,
                         GLuint clampT    = GL_CLAMP_TO_EDGE,
                         GLuint magFilter = GL_LINEAR,
                         GLuint minFilter = GL_LINEAR);

    bool createNoise3D(const  char* name,
                       GLuint size = 128);

    bool createNormalizingCube(const char* name,
                               GLuint size         = 128,
                               bool   buildMipmaps = true);

    void copyCurrentBuffer(int newTarget = -1);

    const bool activate(GLuint unit = 0);
    const bool deactivate();


    void  setID(GLuint textureID);
    const GLuint  getID() const;

    void  setTarget(GLuint target);
    const GLuint  getTarget() const;

    const GLuint  getHeight() const;
    const GLuint  getWidth()  const;
    const GLuint  getDepth()  const;
    void  destroy();

  protected:
    GLuint  height,
            width,
            depth;
  private:
    GLuint  currUnit,
            target,
            id;

    bool loadTextureFace(const Image &image, GLuint target, bool   mipmap);

    bool finalizeLoading(const char* string);
    bool checkForRepeat (const char* string);

    int getMagFilteri(const std::string &value);
    int getMinFilteri(const std::string &value);
    int getWrapModei (const std::string &value);
    int getTypei     (const std::string &value);

    int getValidWrapMode (int clamp);
    int getValidMagFilter(int filter);
    int getValidMinFilter(int filter);

};
#endif

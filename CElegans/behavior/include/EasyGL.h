#ifndef EASY_GL_H
#define EASY_GL_H


#include "TextureUtils.h"
#include "GenUtils.h"
#include "XMLUtils.h"
#include "Font.h"

  typedef enum
  {
    MB_UNKNOWN_BUTTON,
    MB_BUTTON1,
    MB_BUTTON2,
    MB_BUTTON3,
  } MOUSEBUTTON;

  typedef enum
  {
    ME_CLICKED,
    ME_DOUBLE_CLICKED,
    ME_PRESSED,
    ME_DRAGGED,
    ME_RELEASED,
    ME_MOVED,
    ME_SCROLLING_UP, 
    ME_SCROLLING_DOWN,
    ME_SCROLLING_NONE
  } MOUSEEVENT;

  typedef enum
  {
    AT_CENTER,
    AT_CORNERLU,
    AT_CORNERRU,
    AT_CORNERLD,
    AT_CORNERRD
  } ANCHORTYPE;

  typedef enum
  {
    TE_BACKSPACE_DELETE,
    TE_SIMPLE_DELETE,
    TE_INSERT_CHAR,
    TE_BATCH_DELETE,
    TE_PARSE_VISIBLE
  } TEXTEVENT;

  typedef enum 
  {
    WT_SLIDER    = 1,
    WT_BUTTON,
    WT_CHECK_BOX,
    WT_CHECK_BOX_MARK,
    WT_RADIO_BUTTON,
    WT_CHECK_RB_MARK,
    WT_LABEL,
    WT_TEXT_AREA,
    WT_MATERIAL_SURFACE,
    WT_PANEL,
    WT_SEPARATOR,
    WT_TEXT_BOX,
    WT_COMBO_BOX,
    WT_TABBED_PANEL,
    WT_UNKNOWN
  } WIDGETTYPE;

  typedef enum 
  {
    PL_FREE_LAYOUT,
    PL_YAXIS_LAYOUT,
    PL_YAXIS_CEN_LAYOUT,
    PL_XAXIS_LAYOUT,
    PL_GRID_LAYOUT
  } PANELLAYOUT;


  typedef enum
  {
    OR_HORIZONTAL,
    OR_VERTICAL
  } ORIENTATION;

  typedef enum
  {
    KE_PRESSED  = 1,
    KE_RELEASED
  } KEYBOARDEVENT;

  typedef enum
  {
    KB_PAUSE = 19,
    KB_ENTER = 13,
    KB_SPACE = 32,
    KB_ESCAPE = 27,
    KB_BACK_SPACE = 8,
    KB_NUMPAD_PLUS = 107,
    KB_NUMPAD_MINUS = 109,

    KB_LEFT = 37,
    KB_UP, KB_RIGHT, KB_DOWN,

    KB_0 = 48,
    KB_1, KB_2, KB_3, KB_4, KB_5, KB_6, KB_7,
    KB_8, KB_9,

    KB_A = 65,
    KB_B, KB_C, KB_D, KB_E, KB_F, KB_G, KB_H,
    KB_I, KB_J, KB_K, KB_L, KB_M, KB_N, KB_O,
    KB_P, KB_Q, KB_R, KB_S, KB_T, KB_U, KB_V,
    KB_W, KB_X, KB_Y, KB_Z,

    KB_NUMPAD0 = 96,
    KB_NUMPAD1, KB_NUMPAD2, KB_NUMPAD3, KB_NUMPAD4,
    KB_NUMPAD5, KB_NUMPAD6, KB_NUMPAD7, KB_NUMPAD8,
    KB_NUMPAD9,

    KB_F1 = 112,
    KB_F2, KB_F3, KB_F4, KB_F5, KB_F6, KB_F7,
    KB_F8, KB_F9, KB_F10, KB_F11, KB_F12
 } KEYBOARDBUTTON;


/**********************************************************************************/
/*class KeyEvent                                                                  */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class KeyEvent
{
  public:
    KeyEvent(int id);
    KeyEvent(const KeyEvent& copy);
    KeyEvent &operator= (const KeyEvent& copy);

    bool displayable();
    char getKeyChar();
    int  getKeyID();

  private:
    int id;
};

/**********************************************************************************/
/*class MouseEvent                                                                */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class MouseEvent
{
  public:
    MouseEvent();
    MouseEvent(int id, int xMouse,
               int yMouse, int yInvMouse,
               int scrolling = ME_SCROLLING_NONE);
    MouseEvent(const MouseEvent& copy);
    MouseEvent &operator= (const MouseEvent& copy);

    int  getScrolling();
    int  getButtonID();
    int  getX();
    int  getY();
    int  getYInverse();

    void consume();
  private:
    int  scrolling,
         buttonID,
         x,
         y,
         yInverse;
};

/**********************************************************************************/
/*class GUIEvent                                                                  */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIRectangle;
class GUIEvent
{
  public:
    GUIEvent(GUIRectangle *element);
    GUIEvent(const GUIEvent& copy);
    GUIEvent &operator= (const GUIEvent& copy);

    GUIRectangle *getEventSource();
    const std::string &getCallbackString();

    void setConsumed(bool);
    bool isConsumed();

  private:
    GUIRectangle *eventSource;
    std::string   callbackString;
    bool          consumed;
};

/**********************************************************************************/
/*class GUIEventListener                                                          */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIEventListener
{
  public:
    virtual void actionPerformed(GUIEvent &evt) = 0;
};

/**********************************************************************************/
/*class GUIFont                                                                   */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIFont : public NamedObject
{
  public:
    GUIFont(const std::string &guiFontPath = "") : NamedObject(guiFontPath){}

   ~GUIFont();

    CFont *getFontObject();

    bool operator ==(const GUIFont &compare);
    bool operator !=(const GUIFont &compare);

    bool loadXMLSettings(const TiXmlElement *node);
    bool build();

  private:
    CFont  font;
};

/**********************************************************************************/
/*class GUIText                                                                   */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIText
{
  public:
    GUIText(const std::string &text = "");

    GUIText(const GUIText & text);
    GUIText &operator =(const GUIText     &text);
    GUIText &operator =(const std::string &text);

    virtual bool loadXMLSettings(const TiXmlElement *node);

    void  computeDimensions();
    void  printCenteredXY(int x, int y, int startIndex = 0, int endIndex = -1);
    void  printCenteredX (int x, int y, int startIndex = 0, int endIndex = -1);
    void  printCenteredY (int x, int y, int startIndex = 0, int endIndex = -1);
    void  print(int x, int y, int startIndex = 0, int endIndex = -1);

    const std::string &getString();

    void setString(const std::string &text);
    void setString(const char        *text);
    void clear();

    void  setSize(int x, int y);
    void  setSize(const Tuple2i& size);
    const Tuple2i& getSize();

    void  setColor(float r, float g, float b);
    void  setColor(const Tuple3f &color);
    const Tuple3f &getColor();

    int   getHeight();
    int   getWidth();

    void   setHeightScale(float hs);
    void   setWidthScale(float ws);
    void   setScales(Tuple2f scales);

    float  getHeightScale();
    float  getWidthScale();
    const  Tuple2f & getScales();

    bool  needUpdating();
    void  forceUpdate(bool update);

    void setFontIndex(int fontIndex);
    int  getFontIndex();

  protected:
    std::string text;
    Tuple2i     position,
                size;
    Tuple2f     scales;
    Tuple3f     color;
    bool        update;
    int         fontIndex;
};

/**********************************************************************************/
/*class  GUIFontManager                                                           */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIFontManager
{
  public:
    static int      addFont(const TiXmlElement *fontNodex);
    static int      findFontIndex(GUIFont *font);
    static GUIFont *getFont(size_t index);

    static void     setCurrentFont(GUIFont *currentFont);
    static void     setCurrentFont(size_t index);
    static GUIFont *getCurrentFont();

    static void     setDefaultFont(GUIFont *defaultFont);
    static void     setDefaultFont(size_t index);
    static GUIFont *getDefaultFont();
    static int      getCharacterWidth(char Char, GUIFont *font);
    static int      getCharacterWidth(char Char, int      index);
   
    static void     clear();

  private:
    static bool addFont(GUIFont *font);

    static vector<GUIFont *>  guiFontList;

    static GUIFont *currentFont,
                   *defaultFont;
};

/**********************************************************************************/
/*class GUITexCoordDescriptor                                                     */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUITexCoordDescriptor
{

  private:
    void    setType(const std::string &type);

    Tuple4f texCoords;
    int     widgetType;

  public:

    GUITexCoordDescriptor(const int type = WT_UNKNOWN);
    GUITexCoordDescriptor(const GUITexCoordDescriptor &copy);
    GUITexCoordDescriptor &operator =(const GUITexCoordDescriptor &copy);

    static void setTextureWidth(int width);
    static void setTextureHeight(int height);

    static int getTextureWidth();
    static int getTextureHeight();

    void loadXMLSettings(const TiXmlElement *element);
    void setType(int type);
    int  getType();

    void setTexCoords(float x, float y, float z, float w);
    void setTexCoords(const Tuple4f& texCoords);

    const Tuple4f &getTexCoords();
};

/**********************************************************************************/
/*class GUIRectangle                                                              */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIRectangle
{
  protected:
    GUIRectangle *parent;

    std::string   callbackString;

    Tuple4i       windowBounds;
    Tuple2f       position,
                  dimensions;

    bool          mouseOver,
                  released,
                  focused,
                  pressed,
                  clicked,
                  visible,
                  active,
                  update;

    int           lastAction,
                  widgetType,
                  anchor,
                  z;

  public:
    GUIRectangle(const std::string &callback = "");
    virtual ~GUIRectangle(){};

    void               setCallbackString(const std::string& callback);
    const std::string &getCallbackString();

    bool     loadXMLSettings(const TiXmlElement *node);

    void     setDimensions(float width, float height);
    void     setDimensions(const Tuple2f &dimensions);
    const    Tuple2f &getDimensions();

    void     setPosition(float xScaleOrPosition, float yScaleOrPosition);
    void     setPosition(const Tuple2f &scalesOrPosition);
    const    Tuple2f &getPosition();
    virtual  Tuple2i  getCenter();

    int   getWidth();
    int   getHeight();

    void  setActive(bool active);
    bool  isActive();

    void  setAnchorPoint(const std::string &anchor);
    void  setAnchorPoint(int anchor);
    int   getAnchorPoint();

    virtual void  forceUpdate(bool update);

    void  setVisible(bool active);
    bool  isVisible();

    bool  isAttached();
    int   getWidgetType();

    virtual void checkMouseEvents(MouseEvent  &evt, int extraInfo, bool reservedBits = false);
    virtual void checkKeyboardEvents(KeyEvent  evt, int extraInfo);
    virtual void render(float) = 0;

    virtual void          setParent(GUIRectangle *parent);
    virtual GUIRectangle *getParent();

    virtual  GUITexCoordDescriptor *getTexCoordsInfo(int type);
    virtual  GUIEventListener      *getEventsListener();

    virtual void  enableGUITexture();
    virtual void  disableGUITexture();

    virtual void  setZCoordinate(int z);
    virtual int   getZCoordinate();

    virtual const void     computeWindowBounds();
    virtual const Tuple4i &getWindowBounds();

    bool  eventDetected();
    bool  isMouseOver();
    bool  isReleased();
    bool  isFocused();
    bool  isDragged();
    bool  isClicked();
    bool  isPressed();
};

typedef vector<GUITexCoordDescriptor> GUITexCoordsInfo;
typedef DistanceObject<GUIRectangle*> ZWidget;
typedef vector<GUIRectangle *>        Widgets;
typedef vector<ZWidget>               ZWidgets;

/**********************************************************************************/
/*class GUIClippedRectangle                                                       */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIClippedRectangle
{
  protected:
    Texture  texture;
    Tuple2f  texCoords[7];

    Tuple4f  textureRectangle,
             bgColor;

    Tuple3f  bordersColor;
    Tuple2i  vertices[7];
    bool     drawBackground,
             drawBounds;
    int      clipSize;

  public:
    GUIClippedRectangle(int clipSize = 5);
    void  enableBGColor(bool enable);
    bool  isBGColorOn();

    void  computeClippedBounds(const Tuple4i &windowBounds);

    void  setClipSize(int clipSize);
    int   getClipSize();

    void  setVisibleBounds(bool visible);
    bool  boundsVisible();

    void  setBGColor(float x, float y, float z, float alpha);
    void  setBGColor(const Tuple4f& color);
    const Tuple4f &getBGColor();
    const Tuple2i *getVertices() const;

    void  setBordersColor(const Tuple3f& color);
    void  setBordersColor(float x, float y, float z);
    const Tuple3f &getBordersColor();
    const Tuple2f *getTexCoords() const;

    void  setTextureRectangle(float x, float y, float z, float w);
    void  setTextureRectangle(const Tuple4f &textureRectangle);
    const Tuple4f &getTextureRectangle();

    Texture &getTexture();
    void     setTexture(const Texture & texture);

    virtual void renderClippedBounds();
    virtual bool loadXMLClippedRectangleInfo(const TiXmlElement *element);
};

/**********************************************************************************/
/*class GUISeparator                                                              */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUISeparator : public GUIRectangle
{
  public:
    GUISeparator(int orientation = OR_HORIZONTAL);
    virtual bool loadXMLSettings(const TiXmlElement *element);
    virtual void checkMouseEvents(MouseEvent  &evt, int extraInfo){};
    virtual void checkKeyboardEvents(KeyEvent evt, int extraInfo){};
    virtual void render(float);

    virtual const Tuple4i &getWindowBounds();
    virtual const void     computeWindowBounds();

    void    setOrientation(int orientation);
    int     getOrientation();

    void    setRatio(float ratio);
    float   getRatio();
  private:
    int   orientation;
    float ratio;
};

/**********************************************************************************/
/*class GUIAlphaElement                                                           */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIAlphaElement : public GUIRectangle
{

  protected:
    GUIText    label;
    Tuple4f    color;
    float      alphaFadeScale,
               minAlpha;

    void       modifyCurrentAlpha(float tick);
 public:
    GUIAlphaElement(const std::string &callback = "");

    virtual void render(float clockTick) = 0;
    virtual bool loadXMLSettings(const TiXmlElement *node);

    void  setMinAlpha(float minAlpha);
    float getMinAlpha();

    void  setAlphaFadeScale(float scale);
    float getAlphaFadeScale();

    void  setAlpha(float alpha);
    float getAlpha();

    void  setLabelString(const std::string &label);
    const std::string  &getLabelString();

    void  setColor(const Tuple3f& color);
    void  setColor(float x, float y, float z);
    const Tuple4f &getColor();

    GUIText * getLabel();
};

/**********************************************************************************/
/*class GUILabel                                                                  */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUILabel : public GUIAlphaElement
{
  public:
    GUILabel(const std::string &labelString    = "Unknown Label",
             const std::string &callbackString = "");

    virtual void  render(float clockTick);
    virtual bool  loadXMLSettings(const TiXmlElement *element);
    virtual const Tuple4i &getWindowBounds();
};

/**********************************************************************************/
/*class GUITextBox                                                                */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUITextBox : public GUIAlphaElement
{
  public:
    GUITextBox(const std::string &callback  = "",
               const std::string &fieldText = "");

    virtual void    render(float clockTick);
    virtual void    checkMouseEvents(MouseEvent &evt, int extraInfo, bool reservedBits = false);
    virtual void    checkKeyboardEvents(KeyEvent evt, int extraInfo);
    virtual bool    loadXMLSettings(const TiXmlElement *node);
    virtual const   Tuple4i &getWindowBounds();
    void            setText(const std::string &text);
    const   std::string &getText() const;

    bool    textChanged();

    void  setBordersColor(const Tuple3f& color);
    void  setBordersColor(float r, float g, float b);
    const Tuple3f &getBordersColor() const;

    void  setPadding(const Tuple2i& p);
    void  setPadding(int x, int y);
    const Tuple2i &getPadding() const;

  protected:
    Tuple2i padding;
    Tuple3f bordersColor,
            textColor;
    float   blinkerTimer;
    size_t  blinkerPosition,
            textStartIndex,
            textEndIndex;
    bool    blinkerOn;

  private:
    void    setupText(int type, char info = 0);
    void    setupBlinker(size_t mouseX);
};

/**********************************************************************************/
/*class GUIButton                                                                 */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIButton : public GUIAlphaElement, public GUIClippedRectangle
{
  public:
    GUIButton(const std::string &callback = "");

    virtual void  render(float clockTick);
    virtual bool  loadXMLSettings(const TiXmlElement *node);
    virtual const void computeWindowBounds();

    void enableBounce(bool bounce);
    bool bounceEnabled();

  private:
    bool bounce;
};

/**********************************************************************************/
/*class GUICheckBox                                                               */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUICheckBox : public GUIAlphaElement
{
  public:
    GUICheckBox(const std::string &callback = "");

    virtual void render(float clockTick);
    virtual void checkMouseEvents(MouseEvent &evt, int extraInfo, bool reservedBits = false);
    virtual bool loadXMLSettings(const TiXmlElement *node);
    virtual void setChecked(bool enabled);

    void  setAlphaMark(float mark);
    float getAlphaMark();

    void  setMinAlphaMark(float mark);
    float getMinAlphaMark();

    bool  isChecked();

    void  setBGColor(const Tuple3f& color);
    void  setBGColor(float x, float y, float z);
    const Tuple3f &getBGColor();
    virtual const Tuple4i &getWindowBounds();

  protected:
    Tuple3f bgColor;
    float   markRatio,
            alphaMark,
            minAlphaMark;
    bool    checked;
    int     extra,
            markOffset,
            primaryTexDesc,
            secondaryTexDesc;
};

/**********************************************************************************/
/*class GUIRadioButton                                                            */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIRadioButton : public GUICheckBox
{
  public:
    GUIRadioButton(const std::string &callback = "");
    virtual void checkMouseEvents(MouseEvent &newEvent, int extraInfo, bool bits);
};

/**********************************************************************************/
/*class GUISlider                                                                 */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUISlider : public GUIAlphaElement
{
  public:
    GUISlider(const std::string &callback    = "",
              const int          orientation = OR_HORIZONTAL);

    virtual const Tuple4i &getWindowBounds();
    virtual void  checkMouseEvents(MouseEvent &evt, int extreaInfo, bool reservedBits = false);
    virtual bool  loadXMLSettings(const TiXmlElement *element);
    virtual void  render(float clockTick);

    float getProgress();
    void  setProgress(float zeroToOne);

    void  setDiscDimensions(const Tuple2i& dimensions);
    void  setDiscDimensions(int width, int height);
    int   getOrientation();

    const Tuple2i &getDiscDimensions();

  private:
    Tuple4i realWindowBounds;
    Tuple2i discDimensions;
    float   progress;
    int     orientation,
            offset;

    void    renderVertical();
};

/**********************************************************************************/
/*class GUIPanel                                                                  */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIPanel : public GUIRectangle,
                 public GUIClippedRectangle
{
  protected:
    Widgets  elements;
    Tuple2i  interval;
    int      layout;

  private:
    void    packFreeLayout();
    void    packYAxisLayout();
    void    packXAxisLayout();
    void    correctPosition();

  public:
    GUIPanel(const std::string &callback = "");
    virtual ~GUIPanel();
    GUIRectangle *getWidgetByCallbackString(const std::string &name);

    void  setInterval(int width, int height);
    void  setInterval(const Tuple2i &dimensions);
    const Tuple2i &getInterval();

    void  setLayout(int layout);
    int   getLayout();

    int   getTreeHeight();

    virtual bool  loadXMLSettings(const char *stackPath);
    virtual bool  loadXMLSettings(const TiXmlElement *element);

    virtual void  notify(GUIRectangle *element);
    virtual void  checkMouseEvents(MouseEvent  &evt, int extraInfo, bool bitInfo = false);
    virtual void  checkKeyboardEvents(KeyEvent evt, int extraInfo);
    virtual void  render(float tick);

    virtual void  collectZWidgets(ZWidgets &container);
    virtual void  forceUpdate(bool);
    virtual bool  addWidget(GUIRectangle *widget);
    virtual void  pack();
    virtual void  clear();

    int           getWidgetCountByType(int type);
    Widgets      &getWidgets();
};

/**********************************************************************************/
/*class GUIFrame                                                                  */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIFrame : public GUIPanel
{
  private:
    GUIEventListener   *listener;
    GUITexCoordsInfo    texCoords;
    Texture             elementsTexture;
    Tuple2i             windowSize;
    int                 updateCount;
  public:
    GUIFrame();
   ~GUIFrame();

    virtual bool  loadXMLSettings(const TiXmlElement *element);

    void     addOrReplaceTexCoordsInfo(GUITexCoordDescriptor &info);
    void     setElementsTexture(const char* texturePath);
    void     setElementsTexture(const Texture &texture);
    void     setGUIEventListener(GUIEventListener *listener);
    Texture *getElementsTexture();

    virtual GUITexCoordDescriptor *getTexCoordsInfo(int type);
    virtual GUIEventListener      *getEventsListener();

    virtual const Tuple4i &getWindowBounds();
    virtual void  forceUpdate(bool update);
    virtual void  enableGUITexture();
    virtual void  disableGUITexture();
    virtual void  render(float tick);
    virtual void  clear();
};

/**********************************************************************************/
/*class GUIComboBox                                                               */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUIComboBox : public GUIRectangle, public GUIEventListener
{
  public:
    GUIComboBox(const std::string& cbs = "");
    virtual ~GUIComboBox();

    virtual GUIEventListener  *getEventsListener();
    virtual const void         computeWindowBounds();
    virtual bool               loadXMLSettings(const TiXmlElement *node);
    virtual void               render(float clockTick);
    virtual void               checkMouseEvents(MouseEvent &evt, int extraInfo, bool reservedBits = false);
    virtual void               actionPerformed(GUIEvent &evt);

    void                    setFontScales(float w, float h);
    void                    setFontScales(const Tuple2f &scales);
    const   Tuple2f        &getFontScales();

    void                         addItem(const std::string &item);
    const   vector<std::string> &getItems() const;

    const   char*                getSelectedItem()  const;
    const   char*                getItem(size_t index) const;
    int                          getItemIndex(const std::string &item);
    bool                         setSelectedItem(const std::string &item);
    bool                         setSelectedItemIndex(size_t index);

    void    setScrollingColor(float r, float g, float b, float alpha);
    void    setScrollingColor(const Tuple4f &rgba);
    const   Tuple4f &getScrollingColor() const;

    void    enableScrollingRectangle(bool);
    bool    scrollingRectangleEnabled();

    bool    isDeployed();

  private:

    void           finalizeSize();

    vector<std::string>  items;
    GUIButton           *dropMenuButton;
    GUIPanel            *upperPanel,
                        *lowerPanel;
    GUILabel            *currentSelection;

    Tuple4i              scrollingRectangle;
    Tuple4f              scrollingColor;
    Tuple2f              fontScales;
      
    bool                 displayScrollingRectangle,
                         lockItems,
                         deployed;

    int                  selectionIndex,
                         fontIndex;
};

/**********************************************************************************/
/*class GUITabbedPanel                                                            */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/

class GUITabbedPanel : public GUIRectangle, public GUIEventListener
{
  public:
    GUITabbedPanel(const std::string &callback = "");
   ~GUITabbedPanel();

    virtual GUIEventListener  *getEventsListener();
    virtual const void         computeWindowBounds();
    virtual bool               loadXMLSettings(const TiXmlElement *node);
    virtual void               render(float clockTick);
    virtual void               checkMouseEvents(MouseEvent &evt, int extraInfo, bool reservedBits = false);
    virtual void               checkKeyboardEvents(KeyEvent evt, int extraInfo);
    virtual void               actionPerformed(GUIEvent &evt);

    void               setTabButtonsColor(float r, float g, float b);
    void               setTabButtonsColor(const Tuple3f& color);
    const Tuple3f     &getTabButtonsColor() const;

    void               setTabButtonsBordersColor(float r, float g, float b);
    void               setTabButtonsBordersColor(const Tuple3f& color);
    const Tuple3f     &getTabButtonsBordersColor() const;

    void               setFontScales(float w, float h);
    void               setFontScales(const Tuple2f &scales);
    const   Tuple2f   &getFontScales();

    bool               addPanel(GUIPanel *panel);
    const   GUIPanel*  getCurrentPanel()      const;
    const   int        getCurrentPanelIndex() const;

    GUIButton         *getTabButton(int index);
    GUIPanel          *getLowerPanel();
  private:
    GUIPanel      *upperPanel,
                  *lowerPanel,
                  *mainPanel;

    Tuple3f        tabButtonsBordersColor,
                   tabButtonsColor;
    Tuple2f        fontScales;
    bool           lockItems;
    int            fontIndex;
};

#endif

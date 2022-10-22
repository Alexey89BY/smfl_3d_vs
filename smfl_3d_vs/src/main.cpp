#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <GL/glew.h>
//#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "load_bmp.hpp"
#include "load_obj.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "text2D.hpp"
#include "sprite2d.hpp"
#include "world.hpp"
#include "physics.hpp"

//-------------------------------------------------------------------------------------------------
//static const wchar_t *g_textStartupMutex = L"3D Game running...";
static const wchar_t *g_textAppCaption = L"3D Game";
static const int SKYBOX_SIDES_COUNT = 6;

//-------------------------------------------------------------------------------------------------
static sf::RenderWindow *g_windowApp;
static sf::Vector2u g_windowSize;
static sf::Vector2i g_mouseOffset;
static bool g_isRunning;
static bool g_isPaused;
static bool g_mouseIsGrabbed;
static GLuint g_vertexArrayBox;
static GLuint g_vertexArrayBoxNumber;
static GLuint g_vertexArraySky;
static GLuint g_vertexArraySkyNumber;
static GLuint g_vertexesbox; // This will identify our vertex buffer
static GLuint g_vertexessky;
static GLuint g_indexesbox;
static GLuint g_indexessky;
static GLuint g_textureBox;
static GLuint g_texturesSky[SKYBOX_SIDES_COUNT];
static GLuint g_programID;
static GLuint g_matrixMvpId;
static GLuint g_matrixUvId;
static GLuint g_textureCrosshair;
static GLuint g_texturePaused;
static sf::Music *g_backgroundMusic;
static sf::Listener *g_listener;
static sf::SoundBuffer *g_soundPlayerJumpBuffer;
static sf::Sound *g_soundPlayerJump;
static sf::Sound *g_soundCopter;
static sf::SoundBuffer *g_soundCopterBuffer;

//-------------------------------------------------------------------------------------------------
static void processEvent(sf::Event const & event);
static void initViewer();
static void initScene();
static void drawScene();
static void drawHUD();
static void cleanScene();
static void initAudio();
static void updateAudio();
static void cleanAudio();
static void pauseGame(bool isPaused);

//-------------------------------------------------------------------------------------------------
int main()
{
//if (CreateMutex(NULL,TRUE,const_StartupMutexName)==NULL)
//  throw;

//if (GetLastError()==ERROR_ALREADY_EXISTS)
//  {
//  HWND hWnd=FindWindow(Application::WndClassName,Application::AppCaption);
//  if (IsWindowVisible(MainWnd))
//    {
//    if (IsIconic(MainWnd)) ShowWindow(MainWnd,SW_RESTORE);
//    SetForegroundWindow(MainWnd);
//    }
//  throw;
//  }

  // create the window
  g_windowApp = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), g_textAppCaption, sf::Style::None, sf::ContextSettings(24, 8, 4, 3, 1));
  g_windowApp->setVerticalSyncEnabled(false);
  g_windowApp->setKeyRepeatEnabled(false);
  g_windowApp->setActive(true);

  // load resources, initialize the OpenGL states, ...
  glewExperimental = GL_TRUE;
  glewInit();

  initAudio();
  initScene();

  g_windowSize = g_windowApp->getSize();
  initViewer();

  Physics::Begin();
  World::Begin();

  g_windowApp->requestFocus();
  pauseGame(false);

    //sf::Font font;
//if (!font.loadFromFile("arial.ttf"))
//{
//  return 0;
    // error...
//}

  g_isRunning = true;
  while (g_isRunning)
  {
    // handle events
    sf::Event event;
    while (g_windowApp->pollEvent(event))
    {
      processEvent(event);
    }

    Player::update();
    Physics::update();
    drawScene();
    drawHUD();
    updateAudio();

    // end the current frame (internally swaps the front and back buffers)
    g_windowApp->display();

    //sf::Time elapsed = clock.getElapsedTime();
    //sf::String text(std::to_string(elapsed.asSeconds()));
    //g_windowApp->setTitle(text);
  }

  // release resources...
  World::End();
  Physics::End();
  cleanScene();
  cleanAudio();

  delete g_windowApp;

  return (0);
}

//-------------------------------------------------------------------------------------------------
static void resetMousePosition()
{
  sf::Mouse::setPosition(g_mouseOffset, *g_windowApp);
}

//-------------------------------------------------------------------------------------------------
static void processEvent(sf::Event const & event)
{
  switch (event.type)
  {
  case sf::Event::MouseMoved:
    if (g_mouseIsGrabbed)
    {
      float deltaX = event.mouseMove.x - g_mouseOffset.x;
      float deltaY = event.mouseMove.y - g_mouseOffset.y;
      Player::setLookXY(deltaX, deltaY);
      resetMousePosition();
    }
    break;

  case sf::Event::MouseWheelScrolled:
    if (g_mouseIsGrabbed)
    {
      switch (event.mouseWheelScroll.wheel)
      {
      case sf::Mouse::Wheel::VerticalWheel:
        Player::setLookZ(event.mouseWheelScroll.delta);
        break;

      case sf::Mouse::Wheel::HorizontalWheel:
        Player::setLookW(event.mouseWheelScroll.delta);
        break;

      default:
        break;
      }
    }
    break;

  case sf::Event::MouseButtonPressed:
  case sf::Event::MouseButtonReleased:
    if (g_mouseIsGrabbed)
    {
      bool isPressed = (event.type == sf::Event::MouseButtonPressed);
      Player::setControls(event.mouseButton.button, isPressed);
    }
    break;

  case sf::Event::KeyPressed:
  case sf::Event::KeyReleased:
    {
      bool isPressed = (event.type == sf::Event::KeyPressed);
      switch (event.key.code)
      {
      case sf::Keyboard::W:
        g_playerIsForwardPressed = isPressed;
        break;
      case sf::Keyboard::S:
        g_playerIsBackwardPressed = isPressed;
        break;
      case sf::Keyboard::A:
        g_playerIsLeftPressed = isPressed;
        break;
      case sf::Keyboard::D:
        g_playerIsRightPressed = isPressed;
        break;
      case sf::Keyboard::Q:
        g_playerIsDownPressed = isPressed;
        break;
      case sf::Keyboard::E:
        g_playerIsUpPressed = isPressed;
        break;
      case sf::Keyboard::Escape:
        g_isRunning = false;
        break;
      default:
        break;
      }
      //Player::setControls(event.key.code, true);
      //Player::setControls(event.key.code, false);
    }
    break;

  case sf::Event::GainedFocus:
    pauseGame(false);
    break;

  case sf::Event::LostFocus:
  case sf::Event::MouseLeft:
    pauseGame(true);
    break;

  case sf::Event::Resized: // adjust the viewport when the window is resized
    g_windowSize = sf::Vector2u(event.size.width, event.size.height);
    initViewer();
    break;

  case sf::Event::Closed: // end the program
    g_isRunning = false;
    break;

  default:
    break;
  }
}

//-------------------------------------------------------------------------------------------------
static GLuint createTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  uint8_t pixel[4] = {r, g, b, a};

  GLuint textureID = 0;
  glGenTextures(1, &textureID);

  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
  glBindTexture(GL_TEXTURE_2D, 0);

  glFlush();

  return (textureID);
}

//-------------------------------------------------------------------------------------------------
static GLuint loadTexture(const char * imagepath, GLint wrapMode = GL_REPEAT, bool isMipMapped = true)
{
  GLuint textureID = 0;
  sf::Image image;
  if (image.loadFromFile(imagepath))
  {
    printf("Texture %s\n", imagepath);

    GLuint width = image.getSize().x;
    GLuint height = image.getSize().y;

    // Create one OpenGL texture
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    glFlush();
  }

  return (textureID);
}

//-------------------------------------------------------------------------------------------------
static bool createIndexedVertexBuffer(const char *objPath, GLuint &out_vertexBuffer, GLuint &out_vertexArray, GLuint &out_indexArray, GLuint &out_elemetsNumber)
{
  out_vertexBuffer = 0;

  std::vector < XYZUVN_VERTEX > vertices;
  std::vector < unsigned int > indexes;

  loadOBJ(objPath, vertices, indexes);

#ifdef DEBUG
  printf("%s\n", objPath);

  for (unsigned int i = 0; i < vertices.size(); ++i)
  {
    printf("%f %f %f %f %f %f %f %f\n", vertices[i].px, vertices[i].py, vertices[i].pz, vertices[i].tx, vertices[i].ty, vertices[i].nx, vertices[i].ny, vertices[i].nz);
  }

  for (unsigned int i = 0; i < indexes.size(); ++i)
  {
    printf("%u ", indexes[i]);
  }

  printf("\n");
#endif

  out_elemetsNumber = indexes.size();

  glGenVertexArrays(1, &out_vertexBuffer);
  glBindVertexArray(out_vertexBuffer);

  glGenBuffers(1, &out_vertexArray);
  glBindBuffer(GL_ARRAY_BUFFER, out_vertexArray);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(XYZUVN_VERTEX), vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &out_indexArray);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_indexArray);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(unsigned int), indexes.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  //glEnableVertexAttribArray(2);
  //glEnableVertexAttribArray(3);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(XYZUVN_VERTEX), (const GLvoid*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(XYZUVN_VERTEX), (const GLvoid*)12);
  //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const GLvoid*)20);
  //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const GLvoid*)32);

  glBindVertexArray(0);

  glFlush();

  return (true);
}

//-------------------------------------------------------------------------------------------------
static void initScene()
{
  createIndexedVertexBuffer("models/crate.obj", g_vertexArrayBox, g_vertexesbox, g_indexesbox, g_vertexArrayBoxNumber);
  createIndexedVertexBuffer("models/skybox.obj", g_vertexArraySky, g_vertexessky, g_indexessky, g_vertexArraySkyNumber);

  g_textureBox = loadTexture("textures/box.png");

  g_texturesSky[0] = loadTexture("textures/skybox/front.png", GL_CLAMP_TO_EDGE);
  g_texturesSky[1] = loadTexture("textures/skybox/right.png", GL_CLAMP_TO_EDGE);
  g_texturesSky[2] = loadTexture("textures/skybox/left.png", GL_CLAMP_TO_EDGE);
  g_texturesSky[3] = loadTexture("textures/skybox/back.png", GL_CLAMP_TO_EDGE);
  g_texturesSky[4] = loadTexture("textures/skybox/bottom.png", GL_CLAMP_TO_EDGE);
  g_texturesSky[5] = loadTexture("textures/skybox/top.png", GL_CLAMP_TO_EDGE);

  g_programID = LoadShaders("shaders/default.vert", "shaders/default.frag");

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClearDepth(1.0f);

  // Enable depth test
  glDepthFunc(GL_LESS);

  // Only during the initialisation
  g_matrixMvpId = glGetUniformLocation(g_programID, "matrixMVP");
  g_matrixUvId = glGetUniformLocation(g_programID, "matrixUV");

  g_textureCrosshair = loadTexture("textures/crosshair.png");
  g_texturePaused = loadTexture("textures/paused.png");
  Sprite2D::initialize();

  GLuint texture = loadTexture("textures/font/cp437-thin_8x16.png", GL_CLAMP_TO_BORDER);
  Text2D::initialize(texture, 256, 128, 32, 8);
}

//-------------------------------------------------------------------------------------------------
static void initViewer()
{
  g_mouseOffset.x = g_windowSize.x / 2;
  g_mouseOffset.y = g_windowSize.y / 2;

  float viewAspect = static_cast<float>(g_windowSize.x) / static_cast<float>(g_windowSize.y);
  Player::setViewAspect(viewAspect);
}

//-------------------------------------------------------------------------------------------------
static void drawScene()
{
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  // Camera matrix
  glm::mat4 const & matrixProjection = Player::getProjectionMatrix();

  glm::mat4 const & matrixHead = Player::getViewHeadMatrix();
  glm::mat4 const & matrixView = Player::getViewBodyMatrix();

  glm::mat4 matrixProjectionHead = matrixProjection * matrixHead;
  glm::mat4 matrixProjectionView = matrixProjection * matrixView;

  //g_matrixView = glm::inverse(matrixView);

  // Use our shader
  glUseProgram(g_programID);

  {
    glm::mat4 matrix = glm::mat4(1.0f);
    glUniformMatrix4fv(g_matrixUvId, 1, GL_FALSE, &matrix[0][0]);
  }

  // Draw background
  glUniformMatrix4fv(g_matrixMvpId, 1, GL_FALSE, &matrixProjectionHead[0][0]);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray(g_vertexArraySky);

  for (int index = 0; index < SKYBOX_SIDES_COUNT; ++index)
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texturesSky[index]);

    intptr_t offset = index * 6 * sizeof(unsigned int);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const GLvoid*)offset);
  }

  // Draw foreground
  glClear(GL_DEPTH_BUFFER_BIT);

  for (int index = 0; index < World::staticWorld->BoxCount; ++index)
  {
    glm::mat4 const &matrixModel = World::staticWorld->Boxes[index].Matrix;

    glm::mat4 matrixMvp = matrixProjectionView * matrixModel;
    glUniformMatrix4fv(g_matrixMvpId, 1, GL_FALSE, &matrixMvp[0][0]);

    glBindVertexArray(g_vertexArrayBox);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_textureBox);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, texture2);

    glDrawElements(GL_TRIANGLES, g_vertexArrayBoxNumber, GL_UNSIGNED_INT, 0);
  }

  for (int index = 0; index < World::dynamicWorld->BoxCount; ++index)
  {
    glm::mat4 const &matrixModel = World::dynamicWorld->Boxes[index].Matrix;

    glm::mat4 matrixMvp = matrixProjectionView * matrixModel;
    glUniformMatrix4fv(g_matrixMvpId, 1, GL_FALSE, &matrixMvp[0][0]);

    glBindVertexArray(g_vertexArrayBox);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_textureBox);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, texture2);

    glDrawElements(GL_TRIANGLES, g_vertexArrayBoxNumber, GL_UNSIGNED_INT, 0);
  }

  glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------
static void drawHUD()
{
  Sprite2D::begin();

  Sprite2D::setTexture(g_textureCrosshair);
  Sprite2D::setColor(1.0f, 1.0f, 1.0f, 1.0f);
  Sprite2D::setRect(0.0f, 0.0f, 1.0f, 1.0f);
  Sprite2D::draw(0.0f, 0.0f, 64.0f / g_windowSize.x, 64.0f / g_windowSize.y);

  if (g_isPaused)
  {
    Sprite2D::setTexture(g_texturePaused);
    Sprite2D::setColor(1.0f, 1.0f, 1.0f, 1.0f);
    Sprite2D::setRect(0.0f, 0.0f, 1.0f, 1.0f);
    Sprite2D::draw(0.0f, 0.0f, 0.5f, 0.25f);
  }

  Sprite2D::end();

  Text2D::begin();

	Text2D::setColor(1.0f, 1.0f, 1.0f, 1.0f);

  //snprintf(Text2D::buffer(), Text2D::limit(), "Time: %f", g_physicsTimeTotal);
  //Text2D::draw(-0.95f, 0.95f, 0.025f, 0.1f);

  //snprintf(Text2D::buffer(), Text2D::limit(), "%f %f %f", g_playerAngles.x, g_playerAngles.y, g_playerAngles.z);
  //Text2D::draw(-0.95f, 0.85f, 0.025f, 0.1f);

  glm::vec3 upVector = Player::getUpVector();
  glm::vec3 direction = Player::getDirection();
  glm::vec3 position = Player::getPosition();

  snprintf(Text2D::buffer(), Text2D::limit(), "upVector %f %f %f", upVector.x, upVector.y, upVector.z);
  Text2D::draw(-0.95f, 0.85f, 0.025f, 0.1f);

  snprintf(Text2D::buffer(), Text2D::limit(), "direction %f %f %f", direction.x, direction.y, direction.z);
  Text2D::draw(-0.95f, 0.75f, 0.025f, 0.1f);

  snprintf(Text2D::buffer(), Text2D::limit(), "position %f %f %f", position.x, position.y, position.z);
  Text2D::draw(-0.95f, 0.65f, 0.025f, 0.1f);

  Text2D::end();
}
//-------------------------------------------------------------------------------------------------
static void cleanScene()
{
  Text2D::cleanup();
  Sprite2D::cleanup();
  /*
  	// Delete buffers
	glDeleteBuffers(1, &Text2DVertexBufferID);
	glDeleteBuffers(1, &Text2DUVBufferID);

	// Delete texture
	glDeleteTextures(1, &Text2DTextureID);

	// Delete shader
	glDeleteProgram(Text2DShaderID);
	*/
}

//-------------------------------------------------------------------------------------------------
static void initAudio()
{
  g_listener = new sf::Listener();
  g_listener->setGlobalVolume(0.0f);

  g_backgroundMusic = new sf::Music();
  g_backgroundMusic->openFromFile("sound/Peace.ogg");
  g_backgroundMusic->setLoop(true);
  g_backgroundMusic->play();

  g_soundPlayerJumpBuffer = new sf::SoundBuffer();
  g_soundPlayerJumpBuffer->loadFromFile("sound/Player/Jump.wav");

  g_soundPlayerJump = new sf::Sound();
  g_soundPlayerJump->setBuffer(*g_soundPlayerJumpBuffer);
  g_soundPlayerJump->setRelativeToListener(true);

  g_soundCopterBuffer = new sf::SoundBuffer();
  g_soundCopterBuffer->loadFromFile("sound/Copter.wav");

  g_soundCopter = new sf::Sound();
  g_soundCopter->setBuffer(*g_soundCopterBuffer);
  g_soundCopter->setLoop(true);
  g_soundCopter->play();
}

//-------------------------------------------------------------------------------------------------
static void updateAudio()
{
  glm::vec3 upVector = Player::getUpVector();
  glm::vec3 direction = Player::getDirection();
  glm::vec3 position = Player::getPosition();

  g_listener->setPosition(position.x, position.y, position.z);
  g_listener->setDirection(direction.x, direction.y, direction.z);
  g_listener->setUpVector(upVector.x, upVector.y, upVector.z);
}

//-------------------------------------------------------------------------------------------------
static void cleanAudio()
{
  delete g_soundPlayerJump;
  delete g_soundPlayerJumpBuffer;

  delete g_soundCopter;
  delete g_soundCopterBuffer;

  delete g_backgroundMusic;
  delete g_listener;
}

//-------------------------------------------------------------------------------------------------
static void pauseGame(bool isPaused)
{
  if (isPaused)
  {
    g_windowApp->setMouseCursorGrabbed(false);
    g_windowApp->setMouseCursorVisible(true);
    g_mouseIsGrabbed = false;

    g_listener->setGlobalVolume(0.0f);
  }
  else
  {
    g_windowApp->setMouseCursorGrabbed(true);
    g_windowApp->setMouseCursorVisible(false);
    g_mouseIsGrabbed = true;

    Player::resetView();
    resetMousePosition();

    g_listener->setGlobalVolume(100.0f);
  }

  Physics::enable(isPaused);
}

//-------------------------------------------------------------------------------------------------
/*
#define BACKGROUND_SIDES_COUNT 6

class CGraphicConsole
  {
  private:
    struct CConsoleString
      {
      size_t length;
      wchar_t data[80];
      } strings[100];

  public:

    void Printf(const wchar_t* message);
    void Draw(void);
  };

//--------------------------------------------------------------------------------------------------

static bool NotifyEvent;
static unsigned int AdapterId;

static D3DPRESENT_PARAMETERS PresentationParameters;
static IDirect3D9 *Object;
static IDirect3DDevice9 *Device;
static ID3DXSprite *Sprite;
static IDirect3DTexture9 *TextureMap;
static IDirect3DSwapChain9 *SwapChain;
static D3DCAPS9 Caps;

static ID3DXMesh *Airplane;

static ID3DXMesh *BoxBody;
static IDirect3DTexture9 *BoxTextures[BACKGROUND_SIDES_COUNT];
static IDirect3DTexture9 *TextureAirplaneA;
static IDirect3DTexture9 *TextureAirplaneB;

static long FPSActual;
static volatile long FPSCounter;

VECTOR3 vnz;
VECTOR3 vnx;
VECTOR3 vny;

void TransformByMatrix(VECTOR3* Out, const VECTOR3* In, const VECTOR3 Basis[3])
{
Out->x = In->dot(Basis[0]);
Out->x = In->dot(Basis[1]);
Out->x = In->dot(Basis[2]);
}


void SetWorldTransform(D3DXMATRIX *InM)
{
static D3DXMATRIX DCM;
static D3DXMATRIX *pTransformMatrix = NULL;

if (pTransformMatrix == NULL)
  {
  VECTOR3 x_acl(1.0, 0.0, 0.0);
  VECTOR3 y_acl(0.0, 1.0, 0.0);
  VECTOR3 z_acl(0.0, 0.0, 1.0);

  VECTOR3 x_d3d(1.0, 0.0, 0.0);
  VECTOR3 y_d3d(0.0, 1.0, 0.0);
  VECTOR3 z_d3d(0.0, 0.0, 1.0);

  D3DXMATRIX DCM_d3d_acl( TO_FLOAT(x_acl.dot(x_d3d)), TO_FLOAT(y_acl.dot(x_d3d)), TO_FLOAT(z_acl.dot(x_d3d)), 0.0f,
                          TO_FLOAT(x_acl.dot(y_d3d)), TO_FLOAT(y_acl.dot(y_d3d)), TO_FLOAT(z_acl.dot(y_d3d)), 0.0f,
                          TO_FLOAT(x_acl.dot(z_d3d)), TO_FLOAT(y_acl.dot(z_d3d)), TO_FLOAT(z_acl.dot(z_d3d)), 0.0f,
                          0.0f,             0.0f,             0.0f,             1.0f );

  VECTOR3 x_veh(1.0, 0.0, 0.0);
  VECTOR3 z_veh(0.0, 0.0, 1.0);
  VECTOR3 y_veh = z_veh.cross(x_veh);

  D3DXMATRIX DCM_acl_veh( TO_FLOAT(x_veh.dot(x_acl)), TO_FLOAT(y_veh.dot(x_acl)), TO_FLOAT(z_veh.dot(x_acl)), 0.0f,
                          TO_FLOAT(x_veh.dot(y_acl)), TO_FLOAT(y_veh.dot(y_acl)), TO_FLOAT(z_veh.dot(y_acl)), 0.0f,
                          TO_FLOAT(x_veh.dot(z_acl)), TO_FLOAT(y_veh.dot(z_acl)), TO_FLOAT(z_veh.dot(z_acl)), 0.0f,
                          0.0f,             0.0f,             0.0f,             1.0f );

  D3DXMatrixMultiply(&DCM, &DCM_acl_veh, &DCM_d3d_acl);

  pTransformMatrix = &DCM;
  }

D3DXMATRIX result;
D3DXMatrixMultiply(&result, InM, pTransformMatrix);

Device->SetTransform(D3DTS_WORLD, &result);
}

//--------------------------------------------------------------------------------------------------
ID3DXMesh *mmm;
ID3DXBuffer *m_buf;
LPDIRECT3DTEXTURE9 *texs;
ID3DXEffect *Effect;

unsigned int Passes;

IDirect3DTexture9 *TextureDefault, *TextureHUD;

ID3DXFont *FontDefault;
ID3DXLine *Line;


D3DXHANDLE h_m_w, h_m_v, h_m_p, h_light;
static D3DXQUATERNION q_a,q_b,q_c;
static D3DXMATRIX m_a, m_b, m_c;
static ID3DXMesh *Mesh;
static unsigned long HUDCrossColor = 0x7FFFFFFF;
static D3DXMATRIX mHUD;


// Создаем эффект (шейдер) из файла
D3DXCreateTextureFromResource(Device,Application::GetInstance(),MAKEINTRESOURCE(IDB_BITMAP1),&TextureDefault);
D3DXCreateTextureFromResourceEx(Device,Application::GetInstance(),MAKEINTRESOURCE(IDB_BITMAP2),D3DX_DEFAULT,D3DX_DEFAULT,1,0,D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,D3DX_FILTER_NONE,D3DX_FILTER_NONE,0xFF00FF,NULL,NULL,&TextureHUD);

//D3DXIMAGE_INFO image_info = {0};

//D3DXCreateTextureFromFileEx(Device,L"h:\\Programs\\ASP\\3DGame\\Game\\Textures\\Screenshot_1.png",D3DX_DEFAULT,D3DX_DEFAULT,1,0,D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,&image_info,NULL,&TextureMap);

D3DXCreateMeshFVF(12,14,D3DXMESH_WRITEONLY,D3DFVF_XYZ|D3DFVF_TEX1,Device,&Mesh);

IDirect3DVertexBuffer9 *VB;
Mesh->GetVertexBuffer(&VB);
void *pVertices;
VB->Lock(0,14*sizeof(VERTEX),&pVertices,0);

memcpy(pVertices,Vertexes,14*sizeof(VERTEX));
VB->Unlock();

IDirect3DIndexBuffer9 *IB;
Mesh->GetIndexBuffer(&IB);
IB->Lock(0,36*sizeof(unsigned short int),&pVertices,0);
memcpy(pVertices,Indexes,36* sizeof(unsigned short int));
VB->Unlock();

D3DXATTRIBUTERANGE ar;
ar.AttribId = 0;
ar.FaceCount = 12;
ar.FaceStart = 0;
ar.VertexStart = 0;
ar.VertexStart = 14;

Mesh->SetAttributeTable(&ar,1);

D3DXCreateLine(Device, &Line);

D3DLIGHT9 light;
ZeroMemory( &light, sizeof(D3DLIGHT9) );
light.Type       = D3DLIGHT_POINT;
light.Diffuse.r  = 1.0f;
light.Diffuse.g  = 1.0f;
light.Diffuse.b  = 1.0f;

// Tell the device about the light and turn it on
light.Attenuation0 = 0.1f;
light.Range=20.0f;


Device->SetRenderState(D3DRS_LIGHTING,TRUE);

Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
Device->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_PHONG);

//Device->SetRenderState(D3DRS_FOGENABLE,TRUE);
//Device->SetRenderState(D3DRS_FOGCOLOR,World::ColorBackground);
Device->SetRenderState(D3DRS_FOGTABLEMODE,D3DFOG_EXP2 );
Device->SetRenderState(D3DRS_FOGDENSITY,F2DW(0.01f) );

Device->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
Device->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
Device->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_ANISOTROPIC);

Device->SetRenderState(D3DRS_ZENABLE,D3DZB_USEW);

D3DXLoadMeshFromX(L"H:\\Programs\\ASP\\3DGame\\Game\\Models\\airplane 2.x", 0, Device, NULL, NULL, NULL, NULL, &Airplane);
D3DXCreateTextureFromFile(Device, L"H:\\Programs\\ASP\\3DGame\\Game\\Models\\wings.bmp", &TextureAirplaneA);
D3DXCreateTextureFromFile(Device, L"H:\\Programs\\ASP\\3DGame\\Game\\Models\\bihull.bmp", &TextureAirplaneB);

D3DXMatrixTranslation(&mHUD,(PresentationParameters.BackBufferWidth-62)*0.5f,(PresentationParameters.BackBufferHeight-62)*0.5f,0);

Device->SetSamplerState(0,D3DSAMP_MAXANISOTROPY,16);

Device->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);

  {
  D3DXMATRIX m;
  FLOAT Aspect = (PresentationParameters.BackBufferWidth) / (FLOAT)PresentationParameters.BackBufferHeight;
  FLOAT FOV = D3DXToRadian(67.5f);

  //D3DXMatrixOrthoRH(&m, 2.0f, 2.0f, const_Zero, 1.0f/const_Zero);
  D3DXMatrixPerspectiveFovRH(&m, FOV, Aspect, 0.01f, 500.0f);

  Device->SetTransform(D3DTS_PROJECTION,&m);
  }
//D3DXVECTOR4 v4;
//D3DXVECTOR3 v3(1.0f,0.0f,1.0f);
//D3DXMatrixInverse(&m_a,&x,&m);
//D3DXVec3Transform(&v4,&v3,&m_a);

D3DXCreateSprite(Device,&Sprite);

D3DXCreateFont(Device, -15, 0, FW_NORMAL, 0, 0, 0, 0, 0, FIXED_PITCH | FF_DONTCARE, NULL, &FontDefault);

D3DMATERIAL9 mat = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,1,1,1},1.0f};
Device->SetMaterial(&mat);
Device->SetTexture(0,TextureDefault);

Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
Device->SetRenderState(D3DRS_LIGHTING, FALSE);

Device->GetSwapChain(0,&SwapChain);

struct BACKGROUND_VERTEX {float x, y, z, u, v;};

D3DXCreateMeshFVF(12,24,0,D3DFVF_XYZ|D3DFVF_TEX1,Device,&BoxBody);

}

//--------------------------------------------------------------------------------------------------
void GetECEFMatrix(D3DMATRIX* OutM, VECTOR3* ECEFPos)
{

D3DXVECTOR3 ecefOffset(TO_FLOAT(ECEFPos->x), TO_FLOAT(ECEFPos->y), TO_FLOAT(ECEFPos->z));

if (D3DXVec3LengthSq(&ecefOffset) > 0.0)
  {

D3DXVECTOR3 ecefY(0.0f, 0.0f, 0.0f);

D3DXVECTOR3 ecefZ(0.0f, 0.0f, 6356752.0f);


D3DXPLANE p, pn;

D3DXPlaneFromPoints(&p, &ecefY, &ecefZ, &ecefOffset);

D3DXPlaneNormalize(&pn, &p);

D3DXVECTOR3 ecefX(pn.a, pn.b, pn.c);

D3DXVec3Scale(&ecefZ, &ecefOffset, -1.0f);

D3DXVec3Normalize(&ecefY, &ecefZ);

D3DXVec3Cross(&ecefZ, &ecefX, &ecefY);

D3DXMATRIX m;

D3DXMatrixIdentity(&m);

*(D3DXVECTOR3*)(&(m._11)) = ecefX;
*(D3DXVECTOR3*)(&(m._21)) = ecefY;
*(D3DXVECTOR3*)(&(m._31)) = ecefZ;

if (D3DXMatrixInverse((D3DXMATRIX*)OutM, NULL, &m))
  return;
  }


D3DXMatrixIdentity((D3DXMATRIX*)OutM);
}

GRAPHIC_LINE::GRAPHIC_LINE(unsigned int length, unsigned long color, float scale)
  {
  UNREFERENCED_PARAMETER(scale);

  //Scale = scale;
  Color = color;
  PointsCount = length * 2 * MEMS_SAMLES_PER_SEC;
  CurrentPoint = PointsCount;

  Data = new VERTEX_LINE[PointsCount];
  memset(Data, 0, PointsCount*sizeof(*Data));
  }

void GRAPHIC_LINE::Draw() const
  {
  Device->DrawPrimitiveUP(D3DPT_LINELIST, PointsCount/2, Data, sizeof(VERTEX_LINE));
  }

void GRAPHIC_LINE::SavePoint(VECTOR3 const &Point)
  {
  CurrentPoint = LOOP_INC(CurrentPoint, PointsCount);

  PointOld.toFLOAT(&(Data[CurrentPoint].x));

  Data[CurrentPoint].color = Color;

  CurrentPoint = LOOP_INC(CurrentPoint, PointsCount);

  VECTOR3 PointNew = Point;//Scale;
  PointNew.toFLOAT(&(Data[CurrentPoint].x));
  PointOld = PointNew;

  Data[CurrentPoint].color = Color;
  }

//------------------------------------------------------------------------------------------------
void C_MODEL::Draw()
{
Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
Device->SetTexture(0, NULL);

//va.x += (Output->VectorA.x - va.x) / 100;
//va.y += (Output->VectorA.y - va.y) / 100;
//va.z += (Output->VectorA.z - va.z) / 100;

//VECTOR3 vnx(-0.98973723069031572f, 0.067059359699961088f, 0.11863421263393473f);
//VECTOR3 vny(-0.069137644837044404f, -0.99658892350362305f, -0.013465643744522256f);
//VECTOR3 vnz(0.11754390465615354f, -0.021569424757829379f, 0.99283341522825530f);


static VERTEX_LINE *track;

if (track == NULL)
  {
  track = new VERTEX_LINE[points_count];

  for (unsigned int idx = 0; idx<points_count; idx++)
    {
    VERTEX_LINE* vertex = track+idx;
    MODEL_INPUT* point = points+idx;

    vertex->x = (point->ECEFPos.x - 3156066)*0.25;
    vertex->y = (point->ECEFPos.y - 1840498)*0.25;
    vertex->z = (point->ECEFPos.z - 5210752)*0.25;

    vertex->color = D3DCOLOR_XRGB(255, 255, 255);
    }
  }

//D3DXMatrixAffineTransformation(

//D3DXVECTOR4 ecefp;
//D3DXVECTOR4 ecefv;

  {
//  D3DXMATRIX mmm;

//  GetECEFMatrix(&mmm, &(Output->VectorECEFPos));

//  D3DXVECTOR3 v(Output->VectorECEFVel.x, Output->VectorECEFVel.y, Output->VectorECEFVel.z);
//  D3DXVECTOR3 p(-Output->VectorECEFPos.x, -Output->VectorECEFPos.y, -Output->VectorECEFPos.z);
//  D3DXVec3Transform(&ecefv, &v, &mmm);
//  D3DXVec3Transform(&ecefp, &p, &mmm);
  }

//D3DXVECTOR3 vpv(Output->VectorPolarVel.x * sinf(Output->VectorPolarVel.y), 0.0f , Output->VectorPolarVel.x * cosf(Output->VectorPolarVel.y));


Device->DrawPrimitiveUP(D3DPT_LINESTRIP, points_count-1, track, sizeof(VERTEX_LINE));


VERTEX_LINE line[] = {

  {0, 0, 0, D3DCOLOR_XRGB(0xAf,0xAf,0xAf)}, {TO_FLOAT(VectorAccelPure.x), TO_FLOAT(VectorAccelPure.y), TO_FLOAT(VectorAccelPure.z), D3DCOLOR_XRGB(0xAf,0xAf,0xAf)},
  {0, 0, 0, D3DCOLOR_XRGB(0,0,0)}, {TO_FLOAT(VectorAccelOffset.x), TO_FLOAT(VectorAccelOffset.y), TO_FLOAT(VectorAccelOffset.z), D3DCOLOR_XRGB(0xff,0xff,0)},
  {0, 0, 0, D3DCOLOR_XRGB(0x7f,0x7f,0x7f)}, {TO_FLOAT(VectorAccel.x), TO_FLOAT(VectorAccel.y), TO_FLOAT(VectorAccel.z), D3DCOLOR_XRGB(0,0,0)},

  {0, 0, 0, 0xFFFF00FF}, {TO_FLOAT(VectorVelocity.x), TO_FLOAT(VectorVelocity.y), TO_FLOAT(VectorVelocity.z), 0xFFFF00FF},
  {0, 0, 0, 0xFF00FFFF}, {TO_FLOAT(VectorTranslation.x), TO_FLOAT(VectorTranslation.y), TO_FLOAT(VectorTranslation.z), 0xFF00FFFF},

 //{0, 0, 0, D3DCOLOR_XRGB(0,0,0xff)}, {Output->vnz.x*100.0f, Output->vnz.y*100.0f, Output->vnz.z*100.0f, D3DCOLOR_XRGB(0xff,0xff,0xff)},
 //{0, 0, 0, D3DCOLOR_XRGB(0xff,0,0)}, {Output->vnx.x*100.0f, Output->vnx.y*100.0f, Output->vnx.z*100.0f, D3DCOLOR_XRGB(0xff,0xff,0xff)},
 //{0, 0, 0, D3DCOLOR_XRGB(0,0xff,0)}, {Output->vny.x*100.0f, Output->vny.y*100.0f, Output->vny.z*100.0f, D3DCOLOR_XRGB(0xff,0xff,0xff)},

  //{0, 0, 0, 0xFF7F7F7F}, {ecefv.x, ecefv.y, ecefv.z, 0xFF7F7F7F},
  //{0, 0, 0, 0xFF777777}, {ecefp.x, ecefp.y, ecefp.z, 0xFF777777},

  //{0, 0, 0, 0xFFFFFFFF}, {vpv.x, vpv.y, vpv.z, 0xFFFFFFFF},

  //{0, 0, 0, 0xFFFFFFFF}, {Output->VectorGSlope.x, Output->VectorGSlope.y, Output->VectorGSlope.z, 0xFF0000FF},
  //{0, 0, 0, 0xFFFFFFFF}, {Output->VectorECEFPos.x, Output->VectorECEFPos.y, Output->VectorECEFPos.z, 0xFFFF0000},
  //{0, 0, 0, 0xFFFFFFFF}, {Output->VectorECEFVel.x, Output->VectorECEFVel.y, Output->VectorECEFVel.z, 0xFF00FF00},
  //{0, 0, 0, 0xFFFFFFFF}, {Output->VectorECEFZ.x, Output->VectorECEFZ.y, Output->VectorECEFZ.z, 0xFF0000FF},

 //{0, 0, 0, 0xFFFFFFFF}, {-0.05176, -0.19592, +0.97919, 0xFF7F7F7F},
 //{0, 0, 0, 0xFF000000}, {Output->vec_accel.x, Output->vec_accel.y, Output->vec_accel.z, 0xFF7F7F7F},
 //{0, 0, 0, 0xFF000000}, {-0.03485, 0.98029, 0.19427, 0xFF00007F},
  };

Device->DrawPrimitiveUP(D3DPT_LINELIST, ARRAYSIZE(line)/2, &line, sizeof(VERTEX_LINE));

LineAccel->Draw();
LineAccelOffset->Draw();
LineVelocity->Draw();
LineTranslation->Draw();
return;

//if (Output == &FloatModelOutput)
//  return;

D3DMATRIX mmm, out, in, mm1;

mmm._11=Output->VectorECEFX.x; mmm._12=Output->VectorECEFX.y; mmm._13=Output->VectorECEFX.z; mmm._14=0.0f;
mmm._21=Output->VectorECEFY.x; mmm._22=Output->VectorECEFY.y; mmm._23=Output->VectorECEFY.z; mmm._24=0.0f;
mmm._31=Output->VectorECEFZ.x; mmm._32=Output->VectorECEFZ.y; mmm._33=Output->VectorECEFZ.z; mmm._34=0.0f;
mmm._41=0.0f; mmm._42=0.0f; mmm._43=0.0f; mmm._44=1.0f;



D3DXMatrixInverse((D3DXMATRIX*)&mm1, NULL, (D3DXMATRIX*)&mmm);

D3DXMatrixMultiply((D3DXMATRIX*)&out, (D3DXMATRIX*)&in, (D3DXMATRIX*)&mm1);

//D3DXMatrixRotationQuaternion((D3DXMATRIX*)&out, &(FloatModelOutput.QuartW));

Device->GetTransform(D3DTS_WORLD, &mmm);

D3DXMatrixMultiply((D3DXMATRIX*)&in, (D3DXMATRIX*)&out, (D3DXMATRIX*)&mmm);

Device->SetTexture(0, TextureDefault);

//float x = 0.001f*GetTickCount();

//D3DMATRIX m;

//D3DXMatrixRotationYawPitchRoll((D3DXMATRIX*)&in, x, x, 0);


D3DXMatrixScaling((D3DXMATRIX*)&in, 0.75f, 0.125f, 0.25f);

Device->SetTransform(D3DTS_WORLD, &in);
Mesh->DrawSubset(0);

//Device->SetTransform(D3DTS_WORLD, &in);
//Mesh->DrawSubset(0);
}
//--------------------------------------------------------------------------------------------------
static inline void Graphic::DrawForeground(void)
{
Device->Clear(0,NULL,D3DCLEAR_ZBUFFER,0,1.0f,0);

Device->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)Player::GetViewMatrix(&m_a));

D3DXMATRIX m_world;
//D3DXMatrixRotationYawPitchRoll(&m_world, D3DX_PI * 0.5f, -D3DX_PI * 0.5f, 0.0f);
//SetWorldTransform(&m_world);

D3DXMatrixIdentity(&m_world);
Device->SetTransform(D3DTS_WORLD, &m_world);

Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
Device->SetTexture(0, NULL);
Device->DrawPrimitiveUP(D3DPT_LINELIST, ARRAYSIZE(xyzline) / 2, &xyzline, sizeof(VERTEX_LINE));

FloatModel.Draw();


  if (0) {
  D3DXMATRIX m_a;
  D3DXMATRIX m_b;
  D3DXMATRIX m_c;

  D3DXMatrixScaling(&m_a, 0.25f, 0.25f, 0.25f);
  D3DXMatrixRotationYawPitchRoll(&m_b, -D3DX_PI * 0.5f, 0.0f, D3DX_PI * 0.5f);
  D3DXMatrixMultiply(&m_c, &m_a, &m_b);

  D3DXQUATERNION quat;
  FloatModel.QuaternionGyro.toFLOAT(&quat.x);
  D3DXMatrixRotationQuaternion(&m_a, &quat);
  D3DXMatrixMultiply(&m_b, &m_c, &m_a);

  D3DXVECTOR3 vec;
  FloatModel.VectorTranslation.toFLOAT(&vec.x);
  D3DXMatrixTranslation(&m_a, vec.x, vec.y, vec.z);
  D3DXMatrixMultiply(&m_c, &m_b, &m_a);

  Device->SetTransform(D3DTS_WORLD, &m_c);

  Device->SetTexture(0, TextureAirplaneB);
  Airplane->DrawSubset(0);
  Device->SetTexture(0, TextureAirplaneB);
  Airplane->DrawSubset(1);
  Device->SetTexture(0, TextureAirplaneA);
  Airplane->DrawSubset(2);
  Device->SetTexture(0, TextureAirplaneB);
  Airplane->DrawSubset(3);
  Device->SetTexture(0, TextureAirplaneB);
  Airplane->DrawSubset(4);
  Device->SetTexture(0, TextureAirplaneB);
  Airplane->DrawSubset(5);
  Device->SetTexture(0, TextureAirplaneB);
  Airplane->DrawSubset(6);
  Device->SetTexture(0, TextureAirplaneB);
  Airplane->DrawSubset(7);
  }

//D3DXMatrixTranslation(&m_a, 5.0f, 0.0f, 0.0f);
//SetWorldTransform(&m_a);

//Device->DrawPrimitiveUP(D3DPT_LINELIST, 3, &xyzline, sizeof(VERTEX_LINE));

//DrawModelOutput(&IntegerModel);
}
//--------------------------------------------------------------------------------------------------
char buf[200];

static double FFT_scalex;
static double FFT_scaley = 200;
static bool FFT_Ready = false;
static double FFT_Re[FFT_N];
static double FFT_Im[FFT_N];
static D3DXVECTOR2 FFT_Lines[FFT_N];

void FFT_Draw(const double Data[FFT_N], unsigned long Color)
{
//Line->Draw(FFT_Lines, FFT_N/2, Color);

if (!FFT_Ready)
  {
  FFT_scalex = PresentationParameters.BackBufferWidth / ((float)FFT_N);

  for (unsigned int i=0; i<FFT_N; i++)
    FFT_Lines[i].x = TO_FLOAT(i*FFT_scalex);

  FFT_Ready = true;
  }

float A = (FFT_N-1)/2;

for (unsigned int i=0; i<FFT_N; i++)
  {
  float k = (i-A)/(0.3f*A);

  FFT_Re[i] = Data[i]*exp(-0.5f*k*k);
  FFT_Im[i] = 0;
  }

FFT_Transform(FFT_Re, FFT_Im, FFT_SHIFT, false); // вычисляем прямое БПФ


for (unsigned int i=0; i<(FFT_N/2); i++)
  {
  double amplitude = _hypot(FFT_Im[i], FFT_Re[i]);

  FFT_Lines[i].y = TO_FLOAT(PresentationParameters.BackBufferHeight - FFT_scaley * amplitude);
  }

double offset = PresentationParameters.BackBufferHeight/2;

for (unsigned int i=0; i<FFT_N; i++)
  {
  double amplitude = Data[i];

  FFT_Lines[i].y = TO_FLOAT(offset - FFT_scaley * amplitude);
  }

Line->Draw(FFT_Lines, FFT_N, Color);

}

double TrackScale(double x)
{
return x*(111111.1e-1f);
}

void TrackTransformToLine(D3DXVECTOR2 &line_point, VECTOR3 &track_point)
{
UNREFERENCED_PARAMETER(track_point);

//line_point.y = TO_FLOAT(TrackScale(track_point.x - points[points_point_idx].PositionBLH.x));
//line_point.x = TO_FLOAT(TrackScale(track_point.y - points[points_point_idx].PositionBLH.y));

line_point.x = (PresentationParameters.BackBufferWidth / 2) + line_point.x;
line_point.y = (PresentationParameters.BackBufferHeight / 2) - line_point.y;
}

void DrawModelOutputHUD(C_MODEL *Output, RECT *r)
{
Line->Begin();


//FFT_Draw(Output->FFT_x, D3DCOLOR_XRGB(255, 0, 0));
//FFT_Draw(Output->FFT_y, D3DCOLOR_XRGB(0, 255, 0));
//FFT_Draw(Output->FFT_z, D3DCOLOR_XRGB(0, 0, 255));
//FFT_Draw(Output->FFT_w, D3DCOLOR_XRGB(255, 255, 255));


for (unsigned int i= 0; i<Output->LineA->GetLength(); i++)
  {
  FFT_Lines[i].x = i/2;
  FFT_Lines[i].y = 500 - 50 * Output->LineA->GetData()[i].x;
  }

Line->Draw(FFT_Lines, Output->LineA->GetLength(), D3DCOLOR_XRGB(0, 0, 255));

for (unsigned int i= 0; i<Output->LineV->GetLength(); i++)
  {
  FFT_Lines[i].x = i/2;
  FFT_Lines[i].y = 500 - 50 * Output->LineV->GetData()[i].x;
  }

Line->Draw(FFT_Lines, Output->LineV->GetLength(), D3DCOLOR_XRGB(255, 0, 0));

//static D3DXVECTOR2 *pnts;
//const unsigned int step = 5*MEMS_SAMLES_PER_SEC;

//unsigned int num = points_count / step;

if (pnts == NULL)
  { pnts = new D3DXVECTOR2[num]; }

for (unsigned int idx = 0; idx < num; idx++)
  {
  TrackTransformToLine(pnts[idx], points[idx*step].PolarPos);
  }

Line->Draw(pnts, num, D3DCOLOR_XRGB(255, 255, 255));

TrackTransformToLine(pnts[0], points[points_point_idx].PolarPos);

pnts[1].x = pnts[0].x;
pnts[1].y = pnts[0].y+5;

pnts[2].x = pnts[0].x+5;
pnts[2].y = pnts[0].y;

pnts[3].x = pnts[0].x;
pnts[3].y = pnts[0].y-5;

pnts[4].x = pnts[0].x-5;
pnts[4].y = pnts[0].y;

pnts[5].x = pnts[0].x;
pnts[5].y = pnts[0].y+5;

pnts[6].x = pnts[0].x;
pnts[6].y = pnts[0].y;

pnts[7].x = pnts[0].x+40.0f*sinf(points[points_point_idx].PolarVel.y);
pnts[7].y = pnts[0].y-40.0f*cosf(points[points_point_idx].PolarVel.y);

Line->Draw(pnts, 8, D3DCOLOR_XRGB(255, 255, 0));

Line->End();

Sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

D3DXMATRIX ddd;
D3DXMatrixIdentity(&ddd);
Sprite->SetTransform(&ddd);

unsigned long n = 0;

//r->top += 15;
//FontDefault->DrawTextA(Output->Title,-1,r,DT_NOCLIP|DT_NOPREFIX,0xFFFFFFFF);

r->top += 15;
r->bottom = 100;
r->right = 100;
n = _snprintf(buf, sizeof(buf), "Acc. out: %+14.6lf %+14.6lf %+14.6lf", Output->VectorAccelPure.x, Output->VectorAccelPure.y, Output->VectorAccelPure.z);
FontDefault->DrawTextA(NULL, buf,n,r,DT_NOCLIP,0xFFFFFFFF);

r->top += 15;
n = _snprintf(buf, sizeof(buf), "Vector G: %+14.6lf %+14.6lf %+14.6lf = %+14.6lf", Output->VectorAccelOffset.x, Output->VectorAccelOffset.y, Output->VectorAccelOffset.z, Output->VectorAccelOffsetLen);
FontDefault->DrawTextA(NULL, buf,n,r,DT_NOCLIP,0xFFFFFFFF);

r->top += 15;
n = _snprintf(buf, sizeof(buf), "Acceler.: %+14.6lf %+14.6lf %+14.6lf = %+14.6lf", Output->VectorAccel.x, Output->VectorAccel.y, Output->VectorAccel.z, Output->VectorAccelLen);
FontDefault->DrawTextA(Sprite, buf,n,r,DT_NOCLIP,0xFFFFFFFF);

r->top += 15;
n = _snprintf(buf, sizeof(buf), "Gyr.pure: %+14.6lf %+14.6lf %+14.6lf", Output->VectorGyroPure.x, Output->VectorGyroPure.y, Output->VectorGyroPure.z);
FontDefault->DrawTextA(Sprite, buf,n,r,DT_NOCLIP,0xFFFFFFFF);

r->top += 15;
n = _snprintf(buf, sizeof(buf), "Gyr.off.: %+14.6lf %+14.6lf %+14.6lf", Output->VectorGyroOffset.x, Output->VectorGyroOffset.y, Output->VectorGyroOffset.z);
FontDefault->DrawTextA(Sprite, buf,n,r,DT_NOCLIP,0xFFFFFFFF);

r->top += 15;
n = _snprintf(buf, sizeof(buf), "Gyro.   : %+14.6lf %+14.6lf %+14.6lf", Output->VectorGyro.x, Output->VectorGyro.y, Output->VectorGyro.z);
FontDefault->DrawTextA(Sprite, buf,n,r,DT_NOCLIP,0xFFFFFFFF);

r->top += 15;
n = _snprintf(buf, sizeof(buf), "Gyro. Q : %+14.6lf %+14.6lf %+14.6lf %+14.6lf", Output->QuaternionGyro.v.x, Output->QuaternionGyro.v.y, Output->QuaternionGyro.v.z, Output->QuaternionGyro.s);
FontDefault->DrawTextA(Sprite,buf,n,r,DT_NOCLIP,0xFFFFFFFF);


r->top += 15;
n = _snprintf(buf, sizeof(buf), "Velocity: %+14.6lf %+14.6lf %+14.6lf = %+14.6lf", Output->VectorVelocity.x, Output->VectorVelocity.y, Output->VectorVelocity.z, Output->VectorVelocityLen);
FontDefault->DrawTextA(Sprite,buf,n,r,DT_NOCLIP,0xFFFFFFFF);

r->top += 15;
n = _snprintf(buf, sizeof(buf), "Position: %+14.6lf %+14.6lf %+14.6lf = %+14.6lf", Output->VectorTranslation.x, Output->VectorTranslation.y, Output->VectorTranslation.z, Output->VectorTranslationLen);
FontDefault->DrawTextA(Sprite,buf,n,r,DT_NOCLIP,0xFFFFFFFF);

//r->top += 15;
//n = sprintf_s(buf, sizeof(buf), "Sloped G: %+14.6lf %+14.6lf %+14.6lf; Angle = %+14.6lf", Output->VectorGSlope.x, Output->VectorGSlope.y, Output->VectorGSlope.z, Output->AngleSlope);
//FontDefault->DrawTextA(buf,n,r,DT_NOCLIP|DT_NOPREFIX,0xFFFFFFFF);

//r->top += 15;
//n = sprintf_s(buf, sizeof(buf), "Flags: Low = % 6u; High = % 6u; Move = % 4u; Pitch = % 4u", Output->CounterALow, Output->CounterAHigh, Output->CounterMove, Output->CounterPitch);
//FontDefault->DrawTextA(buf,n,r,DT_NOCLIP|DT_NOPREFIX,0xFFFFFFFF);

// Retard
//r->top += 15;
//n = sprintf_s(buf, sizeof(buf), "Boost : Accel = %+14.6lf; Polar = %+14.6lf; ECEF = %+14.6lf", Output->BoostAccel, Output->BoostPolar, Output->BoostECEF);
//FontDefault->DrawTextA(buf,n,r,DT_NOCLIP|DT_NOPREFIX,0xFFFFFFFF);

// Boost
//r->top += 15;
//n = sprintf_s(buf, sizeof(buf), "Retard: Accel = %+14.6lf; Polar = %+14.6lf; ECEF = %+14.6lf", Output->RetardAccel, Output->RetardPolar, Output->RetardECEF);
//FontDefault->DrawTextA(buf,n,r,DT_NOCLIP|DT_NOPREFIX,0xFFFFFFFF);

// Drift
//r->top += 15;
//n = sprintf_s(buf, sizeof(buf), "Drift : Accel = %+14.6lf; Polar = %+14.6lf", Output->DriftAccel, (Output->DriftPolar));
//FontDefault->DrawTextA(buf,n,r,DT_NOCLIP|DT_NOPREFIX,0xFFFFFFFF);

r->top += 15;
n = sprintf_s(buf, sizeof(buf), "Player pos : x = %+14.6lf; y = %+14.6lf; z = %+14.6lf", Player::BodyPosition.Output->DriftAccel, (Output->DriftPolar));
FontDefault->DrawTextA(buf,n,r,DT_NOCLIP|DT_NOPREFIX,0xFFFFFFFF);

r->top += 15;
n = sprintf_s(buf, sizeof(buf), "player look : Accel = %+14.6lf; Polar = %+14.6lf", Output->DriftAccel, (Output->DriftPolar));
FontDefault->DrawTextA(buf,n,r,DT_NOCLIP|DT_NOPREFIX,0xFFFFFFFF);

Sprite->End();
Sprite->Flush();
}
//--------------------------------------------------------------------------------------------------
static inline void Graphic::DrawHUD(void)
{
//FontDefault->Begin();

RECT r={0};
r.top = 10;
r.left = 5;
DrawModelOutputHUD(&FloatModel, &r);

//r.top = 10;
//r.left = (PresentationParameters.BackBufferWidth / 2)+5;
//DrawModelOutputHUD(&IntegerModel, &r);

Sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
D3DXMATRIX ddd;
D3DXMatrixIdentity(&ddd);
Sprite->SetTransform(&ddd);

unsigned long n = 0;

r.top = 5;
r.left = 5;
n = _snprintf(buf, sizeof(buf), "Time : % 9.3lf (%3u)        Point % 9.3lf : % 8u/ % 8u", Physics::GetTime(), FPSActual, FloatModel.Time, Physics::GetCurrentSimPointIdx(), InputPoints.GetPointsNumber());
FontDefault->DrawTextA(Sprite, buf,n,&r,DT_NOCLIP|DT_SINGLELINE,0xFFFFFFFF);

//FontDefault->End();

VECTOR3 pnt1(55.17f, 30.12f, 0.0f);
D3DXVECTOR2 pnt2;

TrackTransformToLine(pnt2, pnt1);

D3DXMATRIX mmmm;

float scalex = TrackScale(0.00007f);
float scaley = TrackScale(0.000043f);

D3DXMatrixScaling(&mmmm, scalex, scaley, 1.0f);

mmmm._41 = pnt2.x;
mmmm._42 = pnt2.y;

Sprite->DrawTransform(TextureMap,NULL,&mmmm,0x7FFFFFFF);

Sprite->SetTransform(&mHUD);
Sprite->Draw(TextureHUD,NULL,NULL,NULL,HUDCrossColor);
Sprite->End();
}
*/

#include "player.hpp"
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>


static const float g_playerMouseSensetive = -0.005f;

static float g_viewAspect;
static glm::vec3 g_playerAngles;
static glm::vec3 g_playerPosition = glm::vec3( 0, 0, -5 );
static glm::mat4 ViewHeadMatrix;
static glm::mat4 ViewBodyMatrix;
static glm::mat4 ProjectionMatrix;
static float g_playerSpeed = 2.0f;

bool g_playerIsForwardPressed;
bool g_playerIsBackwardPressed;
bool g_playerIsUpPressed;
bool g_playerIsDownPressed;
bool g_playerIsRightPressed;
bool g_playerIsLeftPressed;

//PLAYER_CONTROL g_playerControls[g_playerControlsNumber];


Player::Player()
{
  //ctor
}

Player::~Player()
{
  //dtor
}


void Player::resetView()
{
  g_playerAngles = glm::vec3(0.0f, 0.0f, 100.0f);
}


glm::mat4 const & Player::getViewHeadMatrix()
{
	return ViewHeadMatrix;
}


glm::mat4 const & Player::getViewBodyMatrix()
{
	return ViewBodyMatrix;
}


glm::mat4 const & Player::getProjectionMatrix()
{
	return ProjectionMatrix;
}


glm::vec3 Player::getPosition()
{
  return glm::vec3(ViewBodyMatrix[3][0], ViewBodyMatrix[3][1], ViewBodyMatrix[3][2]);
}


glm::vec3 Player::getDirection()
{
  return glm::vec3(ViewBodyMatrix[2][0], ViewBodyMatrix[2][1], ViewBodyMatrix[2][2]);
}


glm::vec3 Player::getUpVector()
{
  return glm::vec3(ViewBodyMatrix[1][0], ViewBodyMatrix[1][1], ViewBodyMatrix[1][2]);
}


void Player::setLookXY(float deltaX, float deltaY)
{
  g_playerAngles.y += deltaY * g_playerMouseSensetive;
  g_playerAngles.x += deltaX * g_playerMouseSensetive;
}


void Player::setLookZ(float deltaZ)
{
  g_playerSpeed += deltaZ;
}


void Player::setLookW(float deltaW)
{
  g_playerAngles.z += deltaW;
}


void Player::setControls(int keyCode, bool isPressed)
{
//  for (int index = 0; index < g_playerControlsNumber; ++index)
//  {
//    if (g_playerControls[index].button == keyCode)
//    {
//      g_playerControls[index].state = isPressed;
//      break;
//    }
//  }
}


void Player::setViewAspect(float viewAspect)
{
  g_viewAspect = viewAspect;
}


void Player::update()
{
  if (g_playerAngles.x > glm::pi<glm::float_t>())
  {
    g_playerAngles.x -= glm::two_pi<glm::float_t>();
  }

  if (g_playerAngles.x < -glm::pi<glm::float_t>())
  {
    g_playerAngles.x += glm::two_pi<glm::float_t>();
  }

  if (g_playerAngles.y > glm::half_pi<glm::float_t>())
  {
    g_playerAngles.y = glm::half_pi<glm::float_t>();
  }

  if (g_playerAngles.y < -glm::half_pi<glm::float_t>())
  {
    g_playerAngles.y = -glm::half_pi<glm::float_t>();
  }

  if (g_playerAngles.z < 45.0f)
  {
    g_playerAngles.z = 45.0f;
  }
  else
  if (g_playerAngles.z > 150.0f)
  {
    g_playerAngles.z = 150.0f;
  }

  if (g_playerSpeed < 0.1f)
  {
    g_playerSpeed = 0.1f;
  }
  else
  if (g_playerSpeed > 100.0f)
  {
    g_playerSpeed = 100.0f;
  }
}


void Player::computeMatricesFromInputs(float deltaTime)
{
	// Compute new orientation
	float horizontalAngle = g_playerAngles.x;
	float verticalAngle = g_playerAngles.y;

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - glm::half_pi<glm::float_t>()),
		0,
		cos(horizontalAngle - glm::half_pi<glm::float_t>())
	);

	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	float deltaPosition = deltaTime * g_playerSpeed;

	// Move forward
	if (g_playerIsForwardPressed)
  {
		g_playerPosition += direction * deltaPosition;
	}
	// Move backward
	if (g_playerIsBackwardPressed)
	{
		g_playerPosition -= direction * deltaPosition;
	}
	// Strafe right
	if (g_playerIsRightPressed)
	{
		g_playerPosition += right * deltaPosition;
	}
	// Strafe left
	if (g_playerIsLeftPressed)
	{
		g_playerPosition -= right * deltaPosition;
	}

	if (g_playerIsUpPressed)
	{
		g_playerPosition += up * deltaPosition;
	}

	if (g_playerIsDownPressed)
	{
		g_playerPosition -= up * deltaPosition;
	}

//    glm::mat4 mX = glm::rotate(g_playerAngles.y, glm::vec3(1.0f, 0.0f, 0.0f));
//    glm::mat4 mY = glm::rotate(g_playerAngles.x, glm::vec3(0.0f, 1.0f, 0.0f));
//    glm::mat4 mZ = glm::translate(glm::vec3(0.0f, -1.68f, 0.0f));
//    glm::mat4 mW = glm::translate(glm::vec3(-5.0f, -5.0f, -5.0f));

	float FoV = g_playerAngles.z;

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), g_viewAspect, 0.0625f, 1024.0f);
	// Camera matrix
	ViewHeadMatrix = glm::lookAt(glm::vec3(0, 0, 0), direction, up);

  glm::mat4 pos = glm::translate(-g_playerPosition);
  ViewBodyMatrix = ViewHeadMatrix * pos;
}

/*
if (KeyPressed(DIK_PAUSE)) PostMessage(wnd_hMain,WM_QUIT,0,0);
if (KeyPressed(DIK_Y)) Box[0].Angular.Force.z+=0.001f;
if (KeyPressed(DIK_U)) Box[0].Angular.Force.x+=0.001f;
if (KeyPressed(DIK_I)) Box[0].Angular.Force.y+=0.001f;
if (KeyPressed(DIK_H)) Box[0].Linear.Force.x+=0.001f;
if (KeyPressed(DIK_J)) Box[0].Linear.Force.z+=0.001f;
if (KeyPressed(DIK_K)) Box[0].Linear.Force.y+=0.001f;

//==============================================================================
D3DXMATRIX plr_ViewMatrix,plr_WorldMatrix;
const float RYG_FACTOR=5.11f;
RECT * pif_prCharA, * pif_prCharB, * pif_prCharC;
RECT pif_rCross, pif_rHeartHolder, pif_rHealthPanel, pif_rShieldPanel, pif_rPulseBkg;
RECT pif_rFirstBigDigit, pif_rBigDigits[10], pif_rSmallDigits[10];
RECT pif_rShieldPanelFrames[3], * pif_prShieldPanelFrame, pif_rPulseBkgFrames[16], * pif_prPulseBkgFrameA, * pif_prPulseBkgFrameB;
D3DXMATRIX pif_mCross, pif_mHeartHolder, pif_mHealthPanel, pif_mShieldPanel, pif_mPulseBkg;
D3DXMATRIX pif_mHCharC, pif_mHCharB, pif_mHCharA, pif_mSCharC, pif_mSCharB, pif_mSCharA, pif_mPCharC, pif_mPCharB, pif_mPCharA;
D3DXMATRIX pif_mShieldPanelFrames[3], * pif_pmShieldPanelFrame, pif_mPulseBkgFrames[16], * pif_pmPulseBkgFrameA,  * pif_pmPulseBkgFrameB;
IDirect3DTexture9 * pif_tInterface;
unsigned int pif_HColor, pif_Alpha, pif_AlphaDelay, pif_cPulseBkg, pif_PColor;
float pif_vHealth, pif_vShield;
//==============================================================================
class CPlayer
{
public:
D3DVECTOR Location, Orientation, Speed, NewLocation, SizeBox;
bool IsLeft,IsJump, IsSitDown, IsRun, IsMove, BeInStDown;
float AbsSpeed, fDelay,Bobbing,Eye, NewEye, NewHeight, Health, Shield, Pulse;
unsigned int Delay;
//--------------------------------------
CPlayer();
void Damage(float HealthOut, float ShieldOut);
void Refresh();
void HeartBit();
void LookAtMouse(long x, long y);
void Live();
void RunForward();
void RunBackward();
void RunLeft();
void RunRigth();
void Jump();
void SitDown(bool Yes);
void WriteHealth();
void DrawInterface();
} Player;
//------------------------------------------------------------------------------
CPlayer::CPlayer()
{
int i;
pif_HColor=0xff00ff00;
pif_rCross.top=0;
pif_rCross.bottom=64;
pif_rCross.left=0;
pif_rCross.right=64;
pif_rHeartHolder.top=75;
pif_rHeartHolder.bottom=163;
pif_rHeartHolder.left=1;
pif_rHeartHolder.right=99;
pif_rHealthPanel.top=113;
pif_rHealthPanel.bottom=163;
pif_rHealthPanel.left=99;
pif_rHealthPanel.right=193;

pif_rShieldPanel.top=66;
pif_rShieldPanel.bottom=112;
pif_rShieldPanel.left=107;
pif_rShieldPanel.right=185;

pif_rPulseBkg.top=3;
pif_rPulseBkg.bottom=31;
pif_rPulseBkg.left=66;
pif_rPulseBkg.right=130;

pif_rFirstBigDigit.top=224;
pif_rFirstBigDigit.bottom=254;
pif_rFirstBigDigit.left=2;
pif_rFirstBigDigit.right=20;
for(i=0;i<10;i++)
{
pif_rBigDigits[i].top=224;
pif_rBigDigits[i].bottom=254;
pif_rBigDigits[i].left=22+i*20;
pif_rBigDigits[i].right=pif_rBigDigits[i].left+18;
}

for(i=0;i<10;i++)
{
pif_rSmallDigits[i].top=214;
pif_rSmallDigits[i].bottom=221;
pif_rSmallDigits[i].left=1+i*5;
pif_rSmallDigits[i].right=pif_rSmallDigits[i].left+4;
}

for(i=0;i<3;i++)
{
pif_rShieldPanelFrames[i].top=66;
pif_rShieldPanelFrames[i].bottom=72+20*i;
pif_rShieldPanelFrames[i].left=107;
pif_rShieldPanelFrames[i].right=185;
}

for(i=0;i<16;i++)
{
pif_rPulseBkgFrames[i].top=3;
pif_rPulseBkgFrames[i].bottom=31;
pif_rPulseBkgFrames[i].left=66+(i<<2);
pif_rPulseBkgFrames[i].right=pif_rPulseBkgFrames[i].left+4;
}

pif_prShieldPanelFrame=&pif_rShieldPanelFrames[0];
pif_pmShieldPanelFrame=&pif_mShieldPanelFrames[0];
pif_prPulseBkgFrameA=&pif_rPulseBkgFrames[1];
pif_pmPulseBkgFrameA=&pif_mPulseBkgFrames[1];
pif_prPulseBkgFrameB=&pif_rPulseBkgFrames[0];
pif_pmPulseBkgFrameB=&pif_mPulseBkgFrames[0];

AbsSpeed=0.005f;
SizeBox.x=0.4f;
NewHeight=1.76f;
SizeBox.z=0.4f;
NewEye=0.62f;
Speed.y=0.0f;
Location.y=1.0f;
Location.z=20.0f;
NewLocation=Location;
Health=120.0f;
Shield=50.0f;
Pulse=0.0f;
}
#define LCD_CHARS_COUNT 21

//------------------------------------------------------------------------------
struct SLCDDisplay {
IDirect3DTexture9 * tBitMap;
RECT * pFirstRect, * pRects[LCD_CHARS_COUNT-1], Rect[LCD_CHARS_COUNT];
bool Constructor();
} gam_LCDDisplay;
//------------------------------------------------------------------------------
//void PrintF(float Value, D3DMATRIX * Cells);
//void PrintUI(unsigned int Va);
//void Destructor()
//{
//delete[] rChars;
//}
//==============================================================================
void CPlayer::Refresh()
{
//--------------------------------------
D3DXMatrixRotationY(&a,Orientation.x);
D3DXMatrixRotationX(&b,Orientation.y);
D3DXMatrixMultiply(&c,&a,&b);
D3DXMatrixRotationZ(&a,Orientation.z);
D3DXMatrixMultiply(&plr_ViewMatrix,&c,&a);
//--------------------------------------
D3DXMatrixTranslation(&b,-Location.x,-Location.y-Eye,-Location.z);
D3DXMatrixMultiply(&plr_WorldMatrix,&b,&plr_ViewMatrix);
//--------------------------------------
gam_ListenerParams.vPosition=Location;
gam_ListenerParams.vOrientFront.x=plr_ViewMatrix._13;
gam_ListenerParams.vOrientFront.y=plr_ViewMatrix._23;
gam_ListenerParams.vOrientFront.z=plr_ViewMatrix._33;
gam_ListenerParams.vOrientTop.x=plr_ViewMatrix._12;
gam_ListenerParams.vOrientTop.y=plr_ViewMatrix._22;
gam_ListenerParams.vOrientTop.z=plr_ViewMatrix._32;
gam_dsListener->SetAllParameters(&gam_ListenerParams,DS3D_IMMEDIATE);
}
//------------------------------------------------------------------------------
void CPlayer::Damage(float HealthOut, float ShieldOut)
{
Shield-=ShieldOut;
Health-=HealthOut;
if (Shield<0) {Health+=Shield; HealthOut-=Shield; Shield=0;}
if (Health<=0) {Health=0; gam_IsLive=false; return;}
//if (HealthOut<50.0f)
//{
//	if (HealthOut<25.0f)
//	{
//	if (HealthOut<6.25f) Plr_SColor=0x1fff0000; else {Plr_SColor=0x3fff0000; WoundWeak->Play(0,0,0);}
//	} else {Plr_SColor=0x7fff0000; WoundMedium->Play(0,0,0);}
//} else {Plr_SColor=0xffff0000; WoundStrong->Play(0,0,0);}
}
//------------------------------------------------------------------------------
void CPlayer::SitDown(bool Yes)
{
if (Yes)
{
	SizeBox.y=1.0f;
    NewEye=0.24f;
	AbsSpeed=0.0025f;
	if (!IsSitDown) {IsSitDown=true; NewLocation.y+=0.39f;}
}
else
{

	if (NewEye<0.62f)
	{
		NewEye+=0.001f;
		NewHeight+=0.002f;
		NewLocation.y+=0.004f;
	} else
	{
      NewEye=0.62f;
	  NewHeight=1.76f;
	  AbsSpeed=0.005f;
	}
	IsSitDown=false;
}
}
//------------------------------------------------------------------------------
void CPlayer::Jump()
{
if (IsJump) return;
Speed.y=5.0f;
snd_PlayerJump->Play(0,0,0);
IsJump=true;
}
//------------------------------------------------------------------------------
void CPlayer::Live()
{
pif_cPulseBkg+=0x00001010;
pif_prPulseBkgFrameA++;
pif_pmPulseBkgFrameA++;
if (pif_prPulseBkgFrameA>&pif_rPulseBkgFrames[15])
{
	pif_prPulseBkgFrameA=&pif_rPulseBkgFrames[0];
	pif_pmPulseBkgFrameA=&pif_mPulseBkgFrames[0];
}
if ((pif_prPulseBkgFrameA==&pif_rPulseBkgFrames[10])||(pif_prPulseBkgFrameA==&pif_rPulseBkgFrames[4])) pif_cPulseBkg=0x3fff0000;
pif_prPulseBkgFrameB++;
pif_pmPulseBkgFrameB++;
if (pif_prPulseBkgFrameB>&pif_rPulseBkgFrames[15])
{
	pif_prPulseBkgFrameB=&pif_rPulseBkgFrames[0];
	pif_pmPulseBkgFrameB=&pif_mPulseBkgFrames[0];
}
if ((pif_Alpha&0xff000000)>0x5f000000)
{
		if (pif_AlphaDelay) pif_AlphaDelay--;
		else
		{
		pif_Alpha-=0x01000000;
		pif_AlphaDelay=2;
		}
}
if (pif_vShield>=1.0f)
{
	if (pif_pmShieldPanelFrame!=&pif_mShieldPanelFrames[2])
	{
		pif_Alpha=0xff000000;
		pif_pmShieldPanelFrame++;
		pif_prShieldPanelFrame++;
	} else
	{
		if (pif_vShield!=Shield) pif_Alpha=0xff000000;
		if (fabs(pif_vShield-Shield)>=1.0f)
		{
			if (pif_vShield<Shield) pif_vShield+=0.5f; else pif_vShield-=0.5f;
		} else pif_vShield=Shield;
	}

} else
	if (pif_pmShieldPanelFrame!=&pif_mShieldPanelFrames[0])
	{
		pif_Alpha=0xff000000;
		pif_pmShieldPanelFrame--;
		pif_prShieldPanelFrame--;
	} else
	{
		if (Shield>=1.0f) pif_vShield=1.0f;
	}
if (pif_vHealth!=Health) pif_Alpha=0xff000000;
if (fabs(pif_vHealth-Health)>=1.0f)
{
	if (pif_vHealth<Health) pif_vHealth+=1.0f; else pif_vHealth-=1.0f;
} else pif_vHealth=Health;

if (Health>100.0f) Health-=0.02f;
Pulse+=1.0f;
if (Pulse>511.0f) Pulse=0.0f;
if (Health<0) IsLive=false;
if (Health<100.0f)
{
Health+=0.001f;
float k=0.2f*cosf(gam_uiTime*0.025f)+1.0f;
D3DXMatrixScaling(&a,k,k,1);
D3DXMatrixTranslation(&b,32-16*k,(ScreenHeight-32)-16*k,0);
D3DXMatrixMultiply(&Plr_Health,&a,&b);
}
else
{
Health=100.0f;
D3DXMatrixTranslation(&Plr_Health,16,ScreenHeight-48,0);
}
if (Plr_SColor & 0xff000000) if (!Delay--) {Delay=10;Plr_SColor-=0x01000000;}
if ((fabs(Speed.y)>0.5f)||IsJump) {IsMove=false; fDelay=1.0f;}
if (IsMove)
{
	if (fDelay>0.0f) {fDelay-=AbsSpeed; Orientation.z+=Bobbing;}
else
{
	fDelay=1.5f;
	if (IsLeft)
	{
		SoundLeft->Play(0,0,0);
		Bobbing=0.00005f;
		IsLeft=false;
	}
	else
	{
		SoundRight->Play(0,0,0);
		Bobbing=-0.00005f;
		IsLeft=true;
	}
}
IsMove=false;
} else {Orientation.z=0; fDelay=0.75f;}
Speed.y-=0.00981f;
NewLocation.y+=Speed.y*0.001f;
//--------------------
bool oX,oY,oZ,nX,nY,nZ;
for(int i=0;i<BoxesCount;i++)
{
oX=(SizeBox.x+Box[i].Size.x)>(2*fabs(Location.x-Box[i].Location.x));
oY=(SizeBox.y+Box[i].Size.y)>(2*fabs(Location.y-Box[i].Location.y));
oZ=(SizeBox.z+Box[i].Size.z)>(2*fabs(Location.z-Box[i].Location.z));
nX=(SizeBox.x+Box[i].Size.x)>(2*fabs(NewLocation.x-Box[i].Location.x));
nY=(SizeBox.y+Box[i].Size.y)>(2*fabs(NewLocation.y-Box[i].Location.y));
nZ=(SizeBox.z+Box[i].Size.z)>(2*fabs(NewLocation.z-Box[i].Location.z));
if (nX&&nY&&nZ)
{
if (!oY)
{
    NewHeight=SizeBox.y;
	NewEye=Eye;
	NewLocation.y=Location.y;
	if (Speed.y<-1.0f) {IsJump=false; SoundLand->Play(0,0,0);}
	float l=((float)fabs(Speed.y)-7.672f);
	if (l>0) Damage((float)l*l*1.112f,1.0f);
	Speed.y=0.01f;
}
if (!oZ) NewLocation.z=Location.z;
if (!oX) NewLocation.x=Location.x;
}
}
//--------------------
SizeBox.y=NewHeight;
Eye=NewEye;
Location=NewLocation;
}
//------------------------------------------------------------------------------
struct SCamera:SMarker
{
float FOV, Aspect;
};

//------------------------------------------------------------------------------
struct SPlayer:BODY
{
SMarker LeftHand, RightHand;
SCamera Eye;
};
union UMatrix
{
D3DMATRIX Matrix;
struct {D3DVECTOR VectorX; float Reserved1; D3DVECTOR VectorY; float Reserved2; D3DVECTOR VectorZ; float Reserved3; D3DVECTOR VectorW; float Reserved4;};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Player
{
static D3DXVECTOR3 LookAngles;

static D3DXVECTOR3 LookAxisSensitive;

float StandUpHeight;

bool PlayerUpdated;

CRITICAL_SECTION CriticalSectionUserInput;

D3DXMATRIX BodyMatrix, CameraMatrix;
D3DXQUATERNION BodyRotation, CameraRotation;
D3DXVECTOR3 BodyPosition, CameraPosition;

unsigned long Begin()
{


D3DXQuaternionIdentity(&BodyRotation);
D3DXQuaternionIdentity(&CameraRotation);

CameraPosition.y = 0;//1.68f;

D3DXMatrixIdentity(&BodyMatrix);
D3DXMatrixIdentity(&CameraMatrix);

BodyPosition.x = 0;
BodyPosition.y = 0;
BodyPosition.z = 1.0;

InitializeCriticalSection(&CriticalSectionUserInput);
//Size.x = 0.5f;
//Size.y = 1.8f;
//Size.z = 0.5f;

//D3DXCreateCylinder(Graphic::Device,Size.x,Size.y,Size.z,16,1,&Mesh,NULL);
return ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------------------------
void *GetViewMatrix(void* Matrix, bool Background)
{
D3DXMATRIX m;

D3DXMatrixMultiply(&m,&CameraMatrix,&BodyMatrix);

if (Background)
  {
  m._41 = 0.0f;
  m._42 = 0.0f;
  m._43 = 0.0f;
  }

D3DXMATRIX *pm = D3DXMatrixInverse((D3DXMATRIX*)Matrix, NULL, &m);

if (pm == NULL)
  {
  D3DXMatrixIdentity((D3DXMATRIX*)Matrix);
  }

return (Matrix);
}

//--------------------------------------------------------------------------------------------------
void Refresh(double TimeStep)
{
D3DXMatrixRotationY(&Player::BodyMatrix, Player::LookAngles.x);

  {
  D3DXVECTOR4 vect;
  D3DXVec3Transform(&vect,&v,&Player::BodyMatrix);

  Player::BodyPosition += (*(D3DXVECTOR3 *)&vect)*TO_FLOAT(TimeStep);
  }
  *D3DXMatrixWVector(&Player::BodyMatrix) = Player::BodyPosition;

D3DXMatrixAffineTransformation(&Player::CameraMatrix, 1.0, NULL, &Player::CameraRotation, &Player::CameraPosition);
}
//--------------------------------------------------------------------------------------------------
}
*/

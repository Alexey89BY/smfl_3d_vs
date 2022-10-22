#include "load_bmp.hpp"
#include <cstdio>

//------------------------------------------------------------------------------
bool loadBMP(
    const char * path,
    unsigned int & width,
    unsigned int & height,
    std::vector < uint8_t > & bitmap
)
{
  // Data read from the header of the BMP file
  unsigned char header[54]; // Each BMP file begins by a 54-bytes header
  unsigned int dataPos;     // Position in the file where the actual data begins
  unsigned int imageSize;   // = width*height*3

  // Open the file
  FILE * file = nullptr;
  fopen_s(&file, path,"rb");
  //if (!file){return 0;}

  if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
    std::fill(&header[0], &header[54], 0);
  }

  if ( header[0]!='B' || header[1]!='M' ){

  }

  // Read ints from the byte array
  dataPos    = *(int*)&(header[0x0A]);
  imageSize  = *(int*)&(header[0x22]);
  width      = *(int*)&(header[0x12]);
  height     = *(int*)&(header[0x16]);

  // Some BMP files are misformatted, guess missing information
  imageSize=width*height*4; // 3 : one byte for each Red, Green and Blue component
  dataPos=54; // The BMP header is done that way

    // Create a buffer
  bitmap.resize(imageSize);

  fseek(file, dataPos, SEEK_SET);
  fread(bitmap.data(), 1, imageSize, file);
  fclose(file);

  return (true);
}

/*
//------------------------------------------------------------------------------
bool My3DCreateTextureFromFile(IDirect3DDevice9 ** Device, IDirect3DTexture9 ** Texture, char * FileName, bool UseAlpha)
{
int h=_lopen(FileName,0);
if (h==-1) return false;
unsigned char * Buffer=(unsigned char*)malloc(6);
if ((Buffer)&&(_lread(h,(void *)Buffer,6)==6)&&(Buffer[0]=='T')&&(Buffer[1]=='E')&&(Buffer[2]=='X'))
{

D3DFORMAT PixelFormat;
switch (Buffer[3])
{
case 32: if (UseAlpha) {PixelFormat=D3DFMT_A8R8G8B8; break;}
case 24: PixelFormat=D3DFMT_X8R8G8B8; break;
default: goto _ErrorExit;
}

unsigned int W=(1<<(Buffer[4])), H=(1<<(Buffer[5]));
if (FAILED((*Device)->CreateTexture(W,H,1,0,PixelFormat,D3DPOOL_MANAGED,Texture,NULL))) goto _ErrorExit;
unsigned int TextureSize=(W*H*Buffer[3])>>3;

Buffer=(unsigned char *)realloc((void*)Buffer,TextureSize);
if(!Buffer) goto _ErrorExit;
unsigned int i;
for(i=0;i<TextureSize;i++) Buffer[i]=0;
_lread(h,(void *)Buffer,TextureSize);
_lclose(h);
D3DLOCKED_RECT Rect;
(*Texture)->LockRect(0,&Rect,NULL,0);
unsigned int * j=(unsigned int *)Rect.pBits;
unsigned char b,g,r,a;
for(i=0;i<TextureSize;j++)
{
r=Buffer[i++];
g=Buffer[i++];
b=Buffer[i++];
a=Buffer[i++];
*j=(((unsigned int)a)<<24)+(((unsigned int)r)<<16)+(((unsigned int)g)<<8)+b;
}
(*Texture)->UnlockRect(0);
return true;
}
_ErrorExit: _lclose(h);
if (*Texture) (*Texture)->Release();
if (Buffer) free((void *)Buffer);
return false;
}
//------------------------------------------------------------------------------
bool _LoadBinTexture32A1L(IDirect3DDevice9 ** Device, IDirect3DTexture9 ** Texture, unsigned int Width, unsigned int Height, char * FileName)
{
unsigned int TexSize=Width*Height;
if (SUCCEEDED((*Device)->CreateTexture(Width,Height,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,Texture,NULL)))
{
	D3DLOCKED_RECT Rect;
	(*Texture)->LockRect(0,&Rect,NULL,0);
	_asm mov edx, Rect.pBits;
	_asm mov ecx, TexSize;
	_asm mov eax, 7f7f7f7fh
_LOOP:_asm mov [edx], eax
	_asm add edx, 4
	_asm loop _LOOP
	int h=-1;
	if ((h=_lopen(FileName,0))!=-1)
	{
	_lread(h,Rect.pBits,(TexSize<<2));
	_lclose(h);
	}
	(*Texture)->UnlockRect(0);
	return true;
} else return false;
}
*/

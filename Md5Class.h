// Md5Class.h: interface for the CMd5Class class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MD5CLASS_H__46D41E80_4C5D_499C_A548_FB80EDEDF193__INCLUDED_)
#define AFX_MD5CLASS_H__46D41E80_4C5D_499C_A548_FB80EDEDF193__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef unsigned       int uint4;
typedef unsigned short int uint2;
typedef unsigned      char uchar;

char* PrintMD5(uchar md5Digest[16]);
char* MD5String(char* szString);
char* MD5File(char* szFilename);

class md5
{
	// Methods
public:
	md5() { Init(); }
	void	Init();
	void	Update(uchar* chInput, uint4 nInputLen);
	void	Finalize();
	uchar*	Digest() { return m_Digest; }
	
private:
	
	void	Transform(uchar* block);
	void	Encode(uchar* dest, uint4* src, uint4 nLength);
	void	Decode(uint4* dest, uchar* src, uint4 nLength);
	
	
	inline	uint4	rotate_left(uint4 x, uint4 n)
	{ return ((x << n) | (x >> (32-n))); }
	
	inline	uint4	F(uint4 x, uint4 y, uint4 z)
	{ return ((x & y) | (~x & z)); }
	
	inline  uint4	G(uint4 x, uint4 y, uint4 z)
	{ return ((x & z) | (y & ~z)); }
	
	inline  uint4	H(uint4 x, uint4 y, uint4 z)
	{ return (x ^ y ^ z); }
	
	inline  uint4	I(uint4 x, uint4 y, uint4 z)
	{ return (y ^ (x | ~z)); }
	
	inline	void	FF(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac)
	{ a += F(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }
	
	inline	void	GG(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac)
	{ a += G(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }
	
	inline	void	HH(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac)
	{ a += H(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }
	
	inline	void	II(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac)
	{ a += I(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }
	
	// Data
private:
	uint4		m_State[4];
	uint4		m_Count[2];
	uchar		m_Buffer[64];
	uchar		m_Digest[16];
	uchar		m_Finalized;
	
};

#endif // !defined(AFX_MD5CLASS_H__46D41E80_4C5D_499C_A548_FB80EDEDF193__INCLUDED_)

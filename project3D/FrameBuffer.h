#pragma once
#include "typedefs.h"
class FrameBuffer
{
public:
	void GenBuffer();
	FrameBuffer() {
		m_w = m_h = 512;  m_buf = 0; m_tex = 0; m_dep = 0; }
	FrameBuffer(uint w, uint h);
	~FrameBuffer();


	int getTex() { return m_tex; }
	int getDep() { return m_dep; }
	int getBuf() { return m_buf; }
	int getW() { return m_w; }
	int getH() { return m_h; }
private:
	uint m_buf;
	uint m_tex;
	uint m_dep;

	uint m_w, m_h;
};


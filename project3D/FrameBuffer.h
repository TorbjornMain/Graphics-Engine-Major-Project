#pragma once
class FrameBuffer
{
public:
	void GenBuffer();
	FrameBuffer() {
		m_w = m_h = 512;  m_buf = 0; m_tex = 0; m_dep = 0; }
	FrameBuffer(unsigned int w, unsigned int h);
	~FrameBuffer();


	int getTex() { return m_tex; }
	int getDep() { return m_dep; }
	int getBuf() { return m_buf; }
	int getW() { return m_w; }
	int getH() { return m_h; }
private:
	unsigned int m_buf;
	unsigned int m_tex;
	unsigned int m_dep;

	unsigned int m_w, m_h;
};


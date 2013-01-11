///////////////////////////////////////////////////////////////////////////
//  Interpolation Filter
//  By Brian Jack (gau_veldt@hotmail.com)
///////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <string.h>
#include <math.h>

#include "resample.hpp"
/*
std::ostream& operator << (std::ostream &os, RGBA &rgba) {
os << "(" << std::setbase(16) << std::setfill('0')
<< std::setw(2) << (unsigned int)rgba.red
<< std::setw(2) << (unsigned int)rgba.green
<< std::setw(2) << (unsigned int)rgba.blue
<< std::setw(2) << (unsigned int)rgba.alpha
<< std::setfill(' ') << std::setbase(10) << ")";
return os;
}
*/

#if 0
class StringError
{
protected:
	char *message;

public:
	StringError(const char *str)
	{
		int len=strlen(str);
		message=new char[len+1];
		if (message)
		{
			strcpy(message,str);
		}
	}

	StringError(const StringError &src)
	{
		message=strdup(src.message);
	}

	StringError()
	{
		message=new char[10];
		if (message)
		{
			message[9]='\0';
			strcpy(message,"Undefined");
		}
	}

	operator const char* ()
	{
		return message;
	}

	~StringError()
	{
		delete [] message;
	}
};
#endif

class SmartRGBA
{
public:
	double red;
	double green;
	double blue;
	double alpha;

	SmartRGBA()
	{
		red = 0.0;
		green = 0.0;
		blue = 0.0;
		alpha = 0.0;
	}

	SmartRGBA(const RGBA &src)
	{
		red = (double)src.red;
		green = (double)src.green;
		blue = (double)src.blue;
		alpha = (double)src.alpha;
	}

	operator RGBA() const
	{
		static RGBA rc;
		rc.red = (unsigned char)red;
		rc.green = (unsigned char)green;
		rc.blue = (unsigned char)blue;
		rc.alpha = (unsigned char)alpha;

		return rc;
	}

	SmartRGBA& operator = (const RGBA &src)
	{
		red = (double)src.red;
		green = (double)src.green;
		blue = (double)src.blue;
		alpha = (double)src.alpha;

		return *this;
	}

	SmartRGBA& operator = (double val)
	{
		red = val;
		green = val;
		blue = val;
		alpha = val;

		return *this;
	}

	SmartRGBA operator* (double val)
	{
		static SmartRGBA temp;
		temp.red = red * val;
		temp.green = green * val;
		temp.blue = blue * val;
		temp.alpha = alpha * val;

		return temp;
	}

	SmartRGBA operator / (double val)
	{
		static SmartRGBA temp;
		temp.red = red / val;
		temp.green = green / val;
		temp.blue = blue / val;
		temp.alpha = alpha / val;

		return temp;
	}

	SmartRGBA& operator += (const RGBA &src)
	{
		red += (double)src.red;
		green += (double)src.green;
		blue += (double)src.blue;
		alpha += (double)src.alpha;

		return *this;
	}

	SmartRGBA& operator += (const SmartRGBA &src)
	{
		red += src.red;
		green += src.green;
		blue += src.blue;
		alpha += src.alpha;

		return *this;
	}
};

/*
std::ostream& operator << (std::ostream& os, SmartRGBA& srgba) {
os << "["
<< srgba.red << ", "
<< srgba.green << ", "
<< srgba.blue << ", "
<< srgba.alpha
<< "]";
return os;
}
*/

template <class X>
class ClumpBuffer
{
private:
	X *data;
	int head;
	int tail;
	int width;
	unsigned int size;
	unsigned int total;

	ClumpBuffer(const ClumpBuffer &)
	{}

public:
	ClumpBuffer(unsigned int sz, unsigned int w=1)
	{
#if 0
		if (sz < 1) throw StringError("Attempt to make ClumpBuffer with 0 slots");
		if (w < 1) throw StringError("Attempt to make ClumpBuffer with 0 width");
#endif
		width = 0;
		head = 0;
		tail = 0;
		size = 0;
		total = 0;

		if (w >= 1 && sz >= 1)
		{
			data=new X[sz*w];
			if (data)
			{
				width=w;
				total=sz;
			}
		}
	}

	bool IsValid()
	{
		return data != NULL;
	}

	X& operator[] (unsigned int idx)
	{
#if 0
		if (idx >= size) throw StringError("Index into ClumpBuffer out of range");
#endif
		idx += head;
		if (idx >= total) idx -= total;

		return data[idx*width];
	}

	X* getLine(unsigned int idx)
	{
#if 0
		if (idx >= size) throw StringError("Index into ClumpBuffer out of range");
#endif
		idx = idx + head;
		if (idx >= total) idx -= total;

		return &(data[idx * width]);
	}

	void flush()
	{
		size = 0;
		head = 0;
		tail = 0;
	}

	void apply(ClumpBuffer<double>& vals, X* dest)
	{
		static double den;
		static int i, j;
		static SmartRGBA sum;
		den = 0.0;
#if 0
		if (vals.length() != size) throw StringError("Size of pixel/proportion ClumpBuffers do not match");
#endif
		for (i = 0; i < (int)size; ++i)
			den += vals[i];
#if 0
		if (den != 1.0) throw StringError("Interpolation factors should add to 1");
#endif
		if (width < 2)
		{
			sum = 0.0;
			for (i = 0; i < (int)size; ++i)
			{
				sum += ((SmartRGBA)(data[(head+i)%total]) * (vals[i]));
			}
			*dest = (RGBA)sum;
		}
		else
		{
			for (j = 0; j < width; ++j)
			{
				sum = 0.0;
				for (i = 0; i < (int)size; ++i)
				{
					sum += ((SmartRGBA)(data[((head+i)%total)*width+j]) * (vals[i]));
				}
				dest[j] = (RGBA)sum;
			}
		}
	}

	void apply_avg(ClumpBuffer<double>& vals, X* dest)
	{
		static double den;
		static SmartRGBA sum;
		static unsigned int i, j;
		den = 0.0;
#if 0
		if (vals.length() != size) throw StringError("Size of pixel/proportion ClumpBuffers do not match");
#endif
		for (i = 0; i < size; ++i)
			den += vals[i];
#if 0
		if (den == 1.0) throw StringError("Identical axes should copy");
		if (den <= 1.0) throw StringError("Enlargement should interpolate");
		if (den == 0.0) throw StringError("Averaging has a denominator of zero");
#endif
		if (width < 2)
		{
			sum = 0.0;
			for (i = 0; i < size; ++i)
				sum += ((SmartRGBA)(data[(head + i) % total]) * (vals[i]));
			*dest = (RGBA)(sum / den);
		}
		else
		{
			for (j = 0; j < (unsigned int) width; ++j)
			{
				sum = 0.0;
				for (i = 0; i < size; ++i)
				{
					sum += ((SmartRGBA)(data[((head+i)%total)*width+j]) * (vals[i]));
				}
				dest[j] = (RGBA)(sum / den);
			}
		}
	}

	~ClumpBuffer()
	{
		delete[] data;
	}

	X& top_slot() const
	{
#if 0
		if (size == 0) throw StringError("ClumpBuffer is empty");
#endif
		return data[head * width];
	}

	X* top_line() const
	{
#if 0
		if (size == 0) throw StringError("ClumpBuffer is empty");
#endif
		return &(data[head * width]);
	}

	X& btm_slot() const
	{
#if 0
		if (size == 0) throw StringError("ClumpBuffer is empty");
#endif
		return data[tail * width];
	}

	X* btm_line() const
	{
#if 0
		if (size == 0) throw StringError("ClumpBuffer is empty");
#endif
		return &(data[tail * width]);
	}

	unsigned int slotCount()
	{
		return total;
	}

	unsigned int length()
	{
		return size;
	}

	X& pull_slot()
	{
#if 0
		if (size == 0) throw StringError("ClumpBuffer is empty");
#endif
		X& temp = data[head * width];
		++head;
		if (head == total) head = 0;
		--size;

		return temp;
	}

	X* pull_line()
	{
#if 0
		if (size == 0) throw StringError("ClumpBuffer is empty");
#endif
		X* temp = &(data[head * width]);
		++head;
		if (head == total) head = 0;
		--size;

		return temp;
	}

	X& next_slot()
	{
#if 0
		if (size == total) throw StringError("ClumpBuffer is full");
#endif
		X& temp = data[tail * width];
		++tail;
		if (tail == total) tail = 0;
		++size;

		return temp;
	}

	X* next_line()
	{
#if 0
		if (size == total) throw StringError("ClumpBuffer is full");
#endif
		X* temp = &(data[tail*width]);
		++tail;
		if (tail == total) tail=0;
		++size;

		return temp;
	}

	std::ostream& dumpLine(std::ostream& os, unsigned int idx)
	{
#if 0
		if (idx >= size) throw StringError("Index into ClumpBuffer out of range");
#endif
		idx = idx + head;
		if (idx >= total) idx -= total;
		os << "[" << idx << "]: ";
		for (int w_idx = 0; w_idx < width; ++w_idx)
		{
			os << data[idx * width + w_idx];
			if ((w_idx + 1) < width) os << ", ";
		}
		os << std::endl;

		return os;
	}

	std::ostream& dump(std::ostream &os)
	{
		os << "("
			<< "data=" << data << ", "
			<< "hp=" << head << ", "
			<< "tp=" << tail << ", "
			<< "w=" << width << ", "
			<< "s=" << size << ", "
			<< "tot=" << total
			<< ")";

		return os;
	}
};

template <class X>
std::ostream& operator << (std::ostream &os, ClumpBuffer<X> &cb)
{
	return cb.dump(os);
}

typedef enum 
{
	rs_interpolate, 
	rs_average, 
	rs_copy
} 
RESAMPLE_MODE;

RGBA *resample(const RGBA *src, int src_w, int src_h, int dest_w, int dest_h)
{
	if (src_w <= 0 || src_h <= 0 || dest_w <= 0 || dest_h <= 0)
		return NULL;

	RGBA *dest = new RGBA[dest_h * dest_w];
	if (dest == NULL)
		return NULL;

	// determine clump sizes
	double clump_x, clump_y;
	if ((src_w < dest_w) && (src_w > 1))
		clump_x = (double)(src_w - 1) / (double)(dest_w - 1);
	else
		clump_x = (double)src_w / (double)dest_w;

	if ((src_h < dest_h) && (src_h > 1))
		clump_y = (double)(src_h - 1) / (double)(dest_h - 1);
	else
		clump_y=(double)src_h / (double)dest_h;

	// establish clump buffers and indices
	int clump_x_size = (int)(1 + ceil(clump_x));
	int clump_y_size = (int)(1 + ceil(clump_y));
	ClumpBuffer<RGBA> clump_xbuf(clump_x_size);
	ClumpBuffer<RGBA> clump_ybuf(clump_y_size, dest_w);
	ClumpBuffer<double> clump_xfracbuf(clump_x_size);
	ClumpBuffer<double> clump_yfracbuf(clump_y_size);

	if (!clump_xbuf.IsValid() || !clump_ybuf.IsValid() || !clump_xfracbuf.IsValid() || !clump_yfracbuf.IsValid())
	{
		delete[] dest;
		return NULL;
	}

	// starting pixel positions in source bitmap
	double src_x_pos = 0.0;
	double src_y_pos = 0.0;

	// starting pixel positions in destination bitmap
	int dest_x = 0;
	int dest_y = 0;

	// determine resample modes
	RESAMPLE_MODE mode_x, mode_y;

	// determine resample mode for X
	if (clump_x == 1) mode_x = rs_copy;
	if (clump_x < 1) mode_x = rs_interpolate;
	if (clump_x > 1) mode_x = rs_average;

	// determine resample mode for Y
	if (clump_y == 1) mode_y = rs_copy;
	if (clump_y < 1) mode_y = rs_interpolate;
	if (clump_y > 1) mode_y = rs_average;

	int minpos_x, maxpos_x;
	int minpos_y, maxpos_y;
	int curpos_x,

		curpos_y,
		deltapos_x,
		deltapos_y,
		nextpos_x,
		nextpos_y;

	int clump_head_x = 0;
	int clump_head_y = 0;
	int clump_tail_x = 0;
	int clump_tail_y = 0;

	curpos_y = -clump_y_size;
	for (dest_y = 0; dest_y < dest_h; ++dest_y)
	{
		// shift new lines the clump buffer for Y
		minpos_y = (int)(floor(src_y_pos));
		maxpos_y = (int)(minpos_y + ceil(clump_y));
		deltapos_y = minpos_y - curpos_y;
		nextpos_y = 1 + maxpos_y - deltapos_y;

		while (deltapos_y > 0)
		{
			if (curpos_y >= 0)
			{
				// pull stale line
				clump_ybuf.pull_slot();
				clump_yfracbuf.pull_slot();
			}

			if (nextpos_y < src_h)
			{
				// generate next line (into Y clump buffer)
				RGBA* next_line = clump_ybuf.next_line();
				clump_yfracbuf.next_slot() = 0.0;
				src_x_pos = 0.0;
				curpos_x = -clump_x_size;
				for (dest_x = 0; dest_x < dest_w; ++dest_x)
				{
					// shift new source pixels into the clump buffer for X
					minpos_x = (int)(floor(src_x_pos));
					maxpos_x = (int)(minpos_x + ceil(clump_x));
					deltapos_x = minpos_x - curpos_x;
					nextpos_x = 1 + maxpos_x - deltapos_x;

					while (deltapos_x > 0)
					{
						if (curpos_x >= 0)
						{
							// pull stale pixel
							clump_xbuf.pull_slot();
							clump_xfracbuf.pull_slot();
						}

						// read source pixel(s)
						RGBA& slot_c = clump_xbuf.next_slot();
						slot_c = *src;
						double& slot_d = clump_xfracbuf.next_slot();
						slot_d = 0.0;
						if ((nextpos_x + 1) < src_w)
							++src;

						++nextpos_x;
						--deltapos_x;
						++curpos_x;
					}

					// apply appropriate transformation
					switch (mode_x)
					{
					case rs_interpolate:
						{
							double& a = clump_xfracbuf[0];
							double& b = clump_xfracbuf[1];
							b = src_x_pos - floor(src_x_pos);
							a = 1.0 - b;
							clump_xbuf.apply(clump_xfracbuf, &(next_line[dest_x]));
						}
						break;
					case rs_average:
						{
							double amt = clump_x;
							double next = 1.0 - (amt - floor(amt));
							clump_xfracbuf[0] = next;
							amt -= next;
							for (int k = 1; k < clump_x_size; ++k)
							{
								next = (amt < 1.0) ? amt : 1.0;
								clump_xfracbuf[k] = next;
								amt = (amt < 1.0) ? 0.0 : (amt - 1.0);
							}
							clump_xbuf.apply_avg(clump_xfracbuf, &(next_line[dest_x]));
						}
						break;
					case rs_copy:
						next_line[dest_x] = clump_xbuf.btm_slot();
						break;
					}

					// go to next x clump
					src_x_pos += clump_x;
				}

				// code above stays at last source position in row
				// so this is needed to go to the first position for
				// the next row
				++src;

				// flush buffers for next row
				clump_xbuf.flush();
				clump_xfracbuf.flush();
			}
			else
			{
				// beyond end so repeat last line
				RGBA* last_line = clump_ybuf.top_line();
				RGBA* next_line = clump_ybuf.next_line();
				clump_yfracbuf.next_slot() = 0.0;

				for (dest_x = 0; dest_x < dest_w; ++dest_x)
				{
					*next_line = *last_line;
					++next_line;
					++last_line;
				}
			}

			++nextpos_y;
			--deltapos_y;
			++curpos_y;
		}

		// apply appropriate transformation
		switch (mode_y)
		{
		case rs_interpolate:
			{
				double& a = clump_yfracbuf[0];
				double& b = clump_yfracbuf[1];
				b = src_y_pos - floor(src_y_pos);
				a = 1.0 - b;
				clump_ybuf.apply(clump_yfracbuf, &(dest[dest_y * dest_w]));
			}
			break;
		case rs_average:
			{
				double amt = clump_y;
				double next = 1.0 - (amt - floor(amt));
				clump_yfracbuf[0] = next;
				amt -= next;

				for (int k = 1; k < clump_y_size; ++k)
				{
					next = (amt < 1.0) ? amt : 1.0;
					clump_yfracbuf[k] = next;
					amt = (amt<1.0) ? 0.0 : (amt - 1.0);
				}
				clump_ybuf.apply_avg(clump_yfracbuf, &(dest[dest_y * dest_w]));
			}
			break;
		case rs_copy:
			{
				for (int i = 0; i < dest_w; ++i)
					dest[dest_y * dest_w + i] = *(clump_ybuf.btm_line() + i);
			}
			break;
		}

		// go to next y clump
		src_y_pos += clump_y;
	}

	// flush line buffers
	clump_ybuf.flush();
	clump_yfracbuf.flush();

	return dest;
}

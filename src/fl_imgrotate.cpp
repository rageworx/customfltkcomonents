#include "fl_imgrotate.h"

#include <math.h>
#ifndef M_PI
#define M_PI 3.141592654
#endif

#pragma pack(push)
#pragma pack(1)
typedef struct ssRGB
{	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} sRGB;

typedef sRGB *pRGB;
#pragma pack(pop)

float min4(float a, float b, float c, float d)
{
	if (a < b)
	{
		if (c < a)
		{
			if (d < c)
				return d;
			else
				return c;
		}
		else
		{
			if (d < a)
				return d;
			else
				return a;
		}
	}
	else
	{
		if (c < b)
		{
			if (d < c)
				return d;
			else
				return c;
		}
		else
		{
			if (d < b)
				return d;
			else
				return b;
		}
	}
}

float max4(float a, float b, float c, float d)
{
	if (a > b)
	{
		if (c > a)
		{
			if (d > c)
				return d;
			else
				return c;
		}
		else
		{
			if (d > a)
				return d;
			else
				return a;
		}
	}
	else
	{
		if (c > b)
		{
			if (d > c)
				return d;
			else
				return c;
		}
		else
		{
			if (d > b)
				return d;
			else
				return b;
		}
	}
}

bool getRotatedImage( Fl_RGB_Image* src, float angle, Fl_RGB_Image* &out )
{
	float CtX = ( (float) src->w() ) / 2;
	float CtY = ( (float) src->h() ) / 2;

	float cA = (float) cos(angle);
	float sA = (float) sin(angle);

	float x1 = CtX + (-CtX) * cA - (-CtY) * sA;
	float x2 = CtX + ( src->w() - CtX) * cA - (-CtY) * sA;
	float x3 = CtX + ( src->w() - CtX) * cA - ( src->h() - CtY) * sA;
	float x4 = CtX + (-CtX) * cA - ( src->h() - CtY) * sA;

	float y1 = CtY + (-CtY) * cA + (-CtX) * sA;
	float y2 = CtY + ( src->h() - CtY) * cA + (-CtX) * sA;
	float y3 = CtY + ( src->h() - CtY) * cA + ( src->w() - CtX) * sA;
	float y4 = CtY + (-CtY) * cA + ( src->w() - CtX) * sA;

	int OfX = ((int) floor(min4(x1, x2, x3, x4)));
	int OfY = ((int) floor(min4(y1, y2, y3, y4)));

	int dstW = ((int) ceil(max4(x1, x2, x3, x4))) - OfX;
	int dstH = ((int) ceil(max4(y1, y2, y3, y4))) - OfY;

    uchar* out_buff = new uchar[ dstW * dstH * 4 ];
    if ( out_buff == NULL )
        return false;

    memset( out_buff, 0, dstW * dstH * 4 );

    const uchar* psrcimg = (const uchar*)src->array;

	for (int stepY = 0; stepY < dstH; stepY++)
	{
		for (int stepX = 0; stepX < dstW; stepX++)
		{
            float CtX2= ((float) src->w() ) / 2 - OfX;
            float CtY2= ((float) src->h() ) / 2 - OfY;

            float orgX= ( cA*(stepX-CtX2) + sA*(stepY-CtY2)) + CtX;
            float orgY= (-sA*(stepX-CtX2) + cA*(stepY-CtY2)) + CtY;

			int iorgX = (int) orgX;
			int iorgY = (int) orgY;

            if ((orgX >= 0) && (orgY >= 0) && (orgX < src->w()-1) && (orgY < src->h()-1))
            {
                sRGB* pdst= (sRGB*)(&out_buff[ ( stepY*dstW + dstW - stepX - 1 ) * 4 ]);
                sRGB* psrc= (sRGB*)(&psrcimg[ ( iorgX + iorgY * src->w() ) * 4 ]);

                float r,g,b,a;

                r =  (psrc)->r            * (1-(orgX-iorgX)) * (1-(orgY-iorgY))
                    +(psrc+1)->r          * (   orgX-iorgX)  * (1-(orgY-iorgY))
                    +(psrc+src->w())->r   * (1-(orgX-iorgX)) * (   orgY-iorgY)
                    +(psrc+src->w()+1)->r * (   orgX-iorgX)  * (   orgY-iorgY);

                g =  (psrc)->g            * (1-(orgX-iorgX)) * (1-(orgY-iorgY))
                    +(psrc+1)->g          * (   orgX-iorgX)  * (1-(orgY-iorgY))
                    +(psrc+src->w())->g   * (1-(orgX-iorgX)) * (   orgY-iorgY)
                    +(psrc+src->w()+1)->g * (   orgX-iorgX)  * (   orgY-iorgY);

                b =  (psrc)->b            * (1-(orgX-iorgX)) * (1-(orgY-iorgY))
                    +(psrc+1)->b          * (   orgX-iorgX)  * (1-(orgY-iorgY))
                    +(psrc+src->w())->b   * (1-(orgX-iorgX)) * (   orgY-iorgY)
                    +(psrc+src->w()+1)->b * (   orgX-iorgX)  * (   orgY-iorgY);

                a =  (psrc)->a            * (1-(orgX-iorgX)) * (1-(orgY-iorgY))
                    +(psrc+1)->a          * (   orgX-iorgX)  * (1-(orgY-iorgY))
                    +(psrc+src->w())->a   * (1-(orgX-iorgX)) * (   orgY-iorgY)
                    +(psrc+src->w()+1)->a * (   orgX-iorgX)  * (   orgY-iorgY);

                pdst->r = (uchar)(r+0.5);
                pdst->g = (uchar)(g+0.5);
                pdst->b = (uchar)(b+0.5);
                pdst->a = (uchar)(a+0.5);
            }
		}
	}

    out = new Fl_RGB_Image( out_buff, dstW, dstH, 4 );

    if ( out == NULL )
    {
        delete[] out_buff;
        return false;
    }

    out->uncache();

    return true;
}

void removeRotatedImage( Fl_RGB_Image* img )
{
    if ( img != NULL )
    {
        if ( ( img->alloc_array == false ) && ( img->array != NULL ) )
        {
            delete[] img->array;
        }

        delete img;
        img = NULL;
    }
}

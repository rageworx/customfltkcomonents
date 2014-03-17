#include <FL/fl_draw.H>
#include "Fl_ImageDisplayBox.h"

void bilinear_resize( const Fl_Image* srcimg, Fl_RGB_Image* &outimg, int w, int h );

Fl_ImageDisplayBox::Fl_ImageDisplayBox(int x,int y,int w,int h)
: Fl_Box(x, y, w, h) ,
  _traceline(true),
  dispimg(NULL),
  palletimg(NULL),
  resizedimg(NULL),
  transparentbg(true),
  userovrdraw(NULL)
{
    box(FL_NO_BOX);
}

Fl_ImageDisplayBox::~Fl_ImageDisplayBox()
{
    dispimg = NULL;

    if ( resizedimg != NULL )
    {
        delete resizedimg;

        resizedimg = NULL;
    }
}

void Fl_ImageDisplayBox::transparent(bool enabled)
{
    transparentbg = enabled;

    if ( transparentbg == true )
    {
        box(FL_NO_BOX);
    }
    else
    {
        box(FL_FLAT_BOX);
    }
}

void Fl_ImageDisplayBox::image(Fl_Image* aimg)
{
    dispimg = aimg;

    if ( resizedimg != NULL )
    {
        delete resizedimg;
        resizedimg = NULL;
    }
}

void Fl_ImageDisplayBox::palletimage(Fl_Image* aimg)
{
    palletimg = aimg;
}

void Fl_ImageDisplayBox::unloadimage()
{
    if ( dispimg != NULL )
    {
        dispimg = NULL;
    }

    if ( resizedimg != NULL )
    {
        delete resizedimg;
        resizedimg = NULL;
    }

    if ( palletimg != NULL )
    {
        palletimg = NULL;
    }
}

void Fl_ImageDisplayBox::draw()
{
    fl_push_clip( x(), y(), w(), h() );

    Fl_Box::draw();

    if( transparentbg == false )
    {
        // draw check boxes for transparency PNG images...
        drawbg();
    }

    if ( palletimg != NULL )
    {
        drawimg( palletimg );
    }

    if ( dispimg != NULL )
    {
        drawimg( dispimg, resizedimg, true );
    }

    if ( ( dispimg != NULL ) && ( _traceline == true ) )
    {
        fl_color(FL_RED);

        fl_xyline( 0, y() + putY - 1, x() + w() );
        fl_xyline( 0, y() + putY + putH , x() + w() );

        fl_yxline( x() + putX - 1, 0 , y() + h() );
        fl_yxline( x() + putX + putW  , 0 , y() + h() );
    }

    if ( userovrdraw != NULL )
    {
        userovrdraw->drawuserimage(w(),h());
    }

    fl_pop_clip();
}

void Fl_ImageDisplayBox::drawbg()
{
    fl_color( fl_rgb_color(50,50,50) );

    fl_rectf(x(), y(), w(), h());

    fl_color( fl_rgb_color(100,100,100) );

    int iDrew = 0;

    for (int cy=y(); cy<y()+h(); cy+=10)
    {
        iDrew =  cy / 2;

        for (int cx=x(); cx<x()+w(); cx+=10)
        {
            if ( iDrew%2 == 0 )
            {
                fl_rectf(cx,cy,10,10);
            }

            iDrew++;
        }
    }
}

#ifdef USING_BILINEAR_RESIZE
void Fl_ImageDisplayBox::drawimg( Fl_Image* img, Fl_RGB_Image* rszimg, bool userszimg )
#else
void Fl_ImageDisplayBox::drawimg( Fl_Image* img, Fl_Image* rszimg, bool userszimg )
#endif
{
    putX = 0;
    putY = 0;
    putW = 0;
    putH = 0;

    // Ratio calculation .
    if ( ( ( img->w() > w() ) || ( img->h() > h() ) ) && ( userszimg == true ) )
    {
        float ratio_l = 0.0f;

        if ( img->w() > w() )
        {
            ratio_l = float( w() ) / float( img->w() );
        }
        else
        {
            ratio_l = float( h() ) / float( img->h() );
        }

        putW = abs( float(img->w()) * ratio_l );
        putH = abs( float(img->h()) * ratio_l );

        if ( putW+1 == w() )
        {
            putW = w();
            putH++;
        }

        putX = ( w() - putW ) / 2;
        putY = ( h() - putH ) / 2;

        if ( rszimg != NULL )
        {
            delete rszimg;

            rszimg = NULL;
        }

        if ( rszimg == NULL )
        {
            rszimg = img->copy(putW, putH);

            if ( rszimg != NULL )
            {
                rszimg->draw(x() + putX, y() + putY, putW, putH);
            }
        }

        fl_color( FL_RED );

    }
    else
    {
        putX = ( w() - img->w() ) / 2;
        putY = ( h() - img->h() ) / 2;
        putW = img->w();
        putH = img->h();

        img->draw(x() + putX, y() + putY, putW, putH);
    }
}

#include "Fl_TransBox.h"

#define DEFAULT_ALPHA		0x7F

Fl_TransBox::Fl_TransBox(int x, int y, int w, int h, char* l)
 : Fl_Box(x, y, w, h) ,
   alpha(DEFAULT_ALPHA),
   dragEnabled(false),
   dispimg(NULL),
   inLabel(l),
   buffer(NULL)
 {
    box(FL_NO_BOX);

    buffer = new unsigned char[4*w*h];
    img = new Fl_RGB_Image(buffer, w, h, 4);
    image(img);

    color(FL_BLACK);
}

Fl_TransBox::~Fl_TransBox()
{
    if ( buffer != NULL )
    {
        delete[] buffer;
    }
}

void Fl_TransBox::color(Fl_Color c)
{
    r = unsigned char (c >> 24);
    g = unsigned char (c >> 16);
    b = unsigned char (c >> 8);

    fill_buffer();
    img->uncache();
}

void Fl_TransBox::set_alpha(unsigned char a)
{
    alpha = a;
    fill_buffer();
    img->uncache();
}

int Fl_TransBox::handle(int e)
{
    // allows to drag the TransparentBox

    static int xn;
    static int yn;

    switch( e )
    {
        case FL_PUSH:
            xn = Fl::event_x() - x();
            yn = Fl::event_y() - y();
            return 1;

        case FL_DRAG:
            if ( dragEnabled == true )
            {
                position(Fl::event_x() - xn, Fl::event_y() - yn);
                window()->redraw();
            }
            return 1;

        case FL_RELEASE:
            return 1;

        default:
            return 0;
    }
}

void Fl_TransBox::fill_buffer()
{
    unsigned char *p = buffer;

    for (int cnt=0; cnt<( 4*w()*h() ); cnt+=4)
    {
        *p++ = r;
        *p++ = g;
        *p++ = b;
        *p++ = alpha;
    }
}

static bool isInResize = false;

void Fl_TransBox::resize(int x, int y, int w, int h)
{
    Fl_Box::resize(x,y,w,h);

    if ( isInResize == true )
        return;

    if ( image() != NULL )
    {
        isInResize = true;

        image( NULL );

        if ( buffer != NULL )
        {
            delete[] buffer;
            buffer = NULL;
        }

        buffer = new unsigned char[4*w*h];

        if ( img != NULL )
        {
            delete img;
            img = NULL;
        }

        img    = new Fl_RGB_Image(buffer, w, h, 4);

        fill_buffer();
        img->uncache();

        image( img );

        isInResize = false;
    }
}

void Fl_TransBox::draw()
{
    if ( image() == NULL )
        return;

    fl_push_clip( x(), y(), w(), h() );

    Fl_Box::draw();

    if ( dispimg != NULL )
    {
        int putX = ( w() - dispimg->w() ) / 2;
        int putY = ( h() - dispimg->h() ) / 2;

        dispimg->draw(x() + putX, y() + putY);
    }
    else
    if ( inLabel != NULL )
    {
        fl_color(FL_WHITE);
        fl_draw(inLabel, 0, 0);
    }

    fl_pop_clip();
}

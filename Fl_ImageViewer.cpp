#include <cmath>
#include <FL/fl_draw.H>
#include "Fl_ImageViewer.h"

////////////////////////////////////////////////////////////////////////////////

class Fl_IMGDBox : public Fl_Box
{
    public:
        Fl_IMGDBox(int x,int y,int w,int h, char* l = NULL );
        virtual ~Fl_IMGDBox();

    public:
        Fl_ImageViewer* imageviewer;

    private:
        int handle( int event );

    private:
        bool ignorehandle;
};

Fl_IMGDBox::Fl_IMGDBox(int x,int y,int w,int h, char* l )
 : Fl_Box( x, y, w, h, l ),
   imageviewer(NULL),
   ignorehandle(false)
{
    box( FL_NO_BOX );

}

Fl_IMGDBox::~Fl_IMGDBox()
{

}

int Fl_IMGDBox::handle( int event )
{
    int ret = Fl_Box::handle( event );

    if ( ignorehandle == true )
        return 0;

    switch( event )
    {
        case FL_DND_ENTER:
        case FL_DND_DRAG:
        case FL_DND_RELEASE:
        case FL_DND_LEAVE:
        case FL_PASTE:
            {
                ignorehandle = true;
                ret = imageviewer->prochandle( event );
                ignorehandle = false;
            }
            break;
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////

Fl_ImageViewer::Fl_ImageViewer(int x,int y,int w,int h)
 : Fl_Scroll( x, y, w, h ),
   multiplier( 1.0f ),
   dispbox( NULL ),
   imgsrc( NULL ),
   imgcached( NULL ),
   _notifier( NULL ),
   isdrawclickpointer( false ),
   colorclicked( FL_RED ),
   colortrace( FL_GRAY )
{
    box( FL_NO_BOX );
    type( 0 );

    this->begin();

        dispbox = new Fl_IMGDBox( x, y, x + w, y + 100
#ifdef DEBUG
                              , "-- DEBUG BUILD --"
#endif
        );
        if ( dispbox != NULL )
        {
            dispbox->imageviewer = this;
            dispbox->box( FL_NO_BOX );
            dispbox->align( 0 );
#ifdef DEBUG
            dispbox->labelcolor( FL_WHITE );
            dispbox->show();
#else
            dispbox->hide();
#endif
        }

    this->end();

    // Fl_Scroll THIN_DOWN_BOX margin is 1
    // Fl_Box has internal margin as 3 !
    margin_x = 2+1;
    margin_y = 2+1;
}

Fl_ImageViewer::~Fl_ImageViewer()
{
    unloadimage();

    if ( dispbox != NULL )
    {
        delete dispbox;
        dispbox = NULL;
    }
}

void Fl_ImageViewer::image(Fl_Image* aimg)
{
    if( aimg == NULL )
        return;

#ifdef DEBUG
    if ( dispbox->label() != NULL )
    {
        dispbox->label( NULL );
    }
#endif

    unloadimage();

    imgsrc = aimg->copy( aimg->w(), aimg->h() );

    if ( imgsrc != NULL )
    {
        dispbox->resize( x(), y(), x()+1, y()+1 );
        dispbox->image( imgsrc );
        dispbox->resize( x(), y(), x() + imgsrc->w(), y() + imgsrc->h() );
        dispbox->show();
        scroll_to(0, 0);
        redraw();
    }
}

void Fl_ImageViewer::unloadimage()
{
    if ( dispbox != NULL )
    {
        dispbox->image(NULL);
        dispbox->hide();
    }

    if( imgcached != NULL )
    {
        delete imgcached;
        imgcached = NULL;
    }

    if ( imgsrc != NULL )
    {
        delete imgsrc;
        imgsrc = NULL;
    }
}

void Fl_ImageViewer::multiplyratio( float rf )
{
    if ( dispbox == NULL )
        return;

    if ( imgsrc == NULL )
        return;

    if ( imgcached != NULL )
    {
        if ( dispbox->image() == imgcached )
        {
            dispbox->image( NULL );
            dispbox->hide();
        }

        delete imgcached;
        imgcached = NULL;
    }

    multiplier   = rf;
    float f_w    = imgsrc->w() * multiplier;
    float f_h    = imgsrc->h() * multiplier;

    imgcached = imgsrc->copy( f_w, f_h );

    dispbox->resize( x(),
                     y(),
                     x() + int(f_w),
                     y() + int(f_h) );
    dispbox->image( imgcached );
    dispbox->show();

    if ( isdrawclickpointer == true )
    {
        isdrawclickpointer =false;
    }

    if ( isdrawruler == true )
    {
        isdrawruler = false;
    }

    this->redraw();

    if ( _notifier != NULL )
    {
        _notifier->OnResized( multiplier );
    }
}

void Fl_ImageViewer::fitwidth()
{
    if ( imgsrc == NULL )
        return;

    float f_w = float( w() ) / float( imgsrc->w() );

    multiplyratio( f_w );
}

void Fl_ImageViewer::fitheight()
{
    if ( imgsrc == NULL )
        return;

    float f_h = float( h() ) / float( imgsrc->h() );

    multiplyratio( f_h );

}

int Fl_ImageViewer::imgw()
{
    if ( dispbox != NULL )
    {
        return dispbox->w();
    }

    return 0;
}

int Fl_ImageViewer::imgh()
{
    if ( dispbox != NULL )
    {
        return dispbox->h();
    }

    return 0;
}

int Fl_ImageViewer::prochandle( int event )
{
    return handle( event );
}

int Fl_ImageViewer::handle( int event )
{
    int ret = Fl_Scroll::handle( event );

    static int  mouse_btn;
    static int  shift_key;
    static bool pushed_drag = false;
    static int  check_x;
    static int  check_y;
    static int  pushed_drag_point_x;
    static int  pushed_drag_point_y;

    switch( event )
    {
        case FL_DND_ENTER:
        case FL_DND_DRAG:
        case FL_DND_RELEASE:
        case FL_DND_LEAVE:
            if ( _notifier != NULL )
            {
                ret = 1;
            }
            else
            {
                ret = 0;
            }
            break;

        case FL_PASTE:
            if ( _notifier != NULL )
            {
                static const char* retC = Fl::event_text();
                _notifier->OnDropFile( retC );
                fl_cursor( FL_CURSOR_CROSS );

                ret = 1;
            }
            else
            {
                ret = 0;
            }
            break;

        case FL_PUSH:
            shift_key = Fl::event_shift();
            mouse_btn = Fl::event_button();
            if ( mouse_btn == FL_LEFT_MOUSE )
            {
                if ( isdrawclickpointer == true )
                {
                    isdrawclickpointer = false;
                    this->redraw();
                }

                check_x = Fl::event_x() + margin_x;
                check_y = Fl::event_y() + margin_y;

                if ( ( shift_key > 0 ) && ( dispbox->image() != NULL ) )
                {
                    coordclicked_x = check_x;
                    coordclicked_y = check_y;
                    isdrawruler = true;
                }
                else
                {
                    pushed_drag_point_x = check_x + hscrollbar.value() - x();
                    pushed_drag_point_y = check_y + scrollbar.value() - y();
                    pushed_drag = true;
                }
            }
            return 1;

        case FL_MOVE:
            check_x = Fl::event_x();
            check_y = Fl::event_y();

            if ( _notifier != NULL )
            {
                _notifier->OnMove( check_x, check_y );
            }
            return 1;


        case FL_DRAG:
            isdrawclickpointer = false;
            if ( isdrawruler == true )
            {
                coordmoving_x = Fl::event_x();
                coordmoving_y = Fl::event_y();

                this->redraw();
            }
            else
            if ( pushed_drag == true )
            {
                int scl_x = hscrollbar.value();
                int scl_y = scrollbar.value();
                int cur_x = pushed_drag_point_x - Fl::event_x();
                int cur_y = pushed_drag_point_y - Fl::event_y();

                if ( cur_x < 0 )
                {
                    cur_x = 0;
                }

                if ( cur_y < 0 )
                {
                    cur_y = 0;
                }

                if ( dispbox != NULL )
                {
                    if ( dispbox->image() != NULL )
                    {
                        if ( dispbox->w() > this->w() )
                        {
                            if ( cur_x > ( dispbox->w() - this->w() ) )
                            {
                                cur_x = dispbox->w() - this->w();
                            }
                        }
                        else
                        {
                            cur_x = 0;
                        }

                        if ( dispbox->h() > this->h() )
                        {
                            if ( cur_y > ( dispbox->h() - this->h() ) )
                            {
                                cur_y = dispbox->h() - this->h();
                            }
                        }
                        else
                        {
                            cur_y = 0;
                        }

                        this->scroll_to( cur_x, cur_y );

                        if ( _notifier != NULL )
                        {
                            _notifier->OnDraged( cur_x, cur_y );
                        }
                    }
                }
            }
            return 1;

        case FL_ENTER:
            fl_cursor( FL_CURSOR_CROSS );
            return 1;

        case FL_LEAVE:
            fl_cursor( FL_CURSOR_DEFAULT );
            return 1;

        case FL_RELEASE:
            shift_key = Fl::event_shift();
            mouse_btn = Fl::event_button();
            if ( mouse_btn == FL_LEFT_MOUSE )
            {
                if ( isdrawruler == true )
                {
                    check_x = Fl::event_x();
                    check_y = Fl::event_y();

                    int last_x = check_x + margin_x;
                    int last_y = check_y + margin_y;
                    isdrawruler = false;

                    if ( _notifier != NULL )
                    {
                        _notifier->OnRuler( coordclicked_x,
                                            coordclicked_y,
                                            last_x,
                                            last_y );
                    }
                }
                else
                if ( pushed_drag == true )
                {
                    pushed_drag = false;
                }
            }
            else
            if ( mouse_btn == FL_RIGHT_MOUSE )
            {
                if ( imgsrc != NULL )
                {
                    //check_x = Fl::event_x() - x() + margin_x;
                    check_x = Fl::event_x() - x() - margin_x;
                    //check_y = Fl::event_y() - y() + margin_y;
                    check_y = Fl::event_y() - y() - margin_y;

                    if ( check_x < 0 )
                    {
                        check_x = 0;
                    }

                    if ( check_y < 0 )
                    {
                        check_y = 0;
                    }

                    double f_x = double( hscrollbar.value() + check_x ) / multiplier;
                    double f_y = double( scrollbar.value() + check_y ) / multiplier;

                    check_x = roundl( f_x );
                    check_y = roundl( f_y );

                    if ( check_x < 0 )
                    {
                        check_x = 0;
                    }
                    else
                    if ( check_x >= imgsrc->w() )
                    {
                        check_x = imgsrc->w();
                    }

                    if ( check_y < 0 )
                    {
                        check_y = 0;
                    }
                    else
                    if ( check_y >= imgsrc->h() )
                    {
                        check_y = imgsrc->h();
                    }

                    // I don't know why it is going wrong coordination,
                    // But it need to be doing minus 2 again for matching drawing line.
                    coordclicked_x = Fl::event_x() + x() - margin_x - 2;
                    coordclicked_y = Fl::event_y() + y() - margin_y - 2;
                    isdrawclickpointer = true;
                    this->redraw();

                    if ( _notifier != NULL )
                    {
                        _notifier->OnRightClick( check_x, check_y);
                    }
                }
            }

            return 1;
    }

    return ret;
}

void Fl_ImageViewer::draw()
{
    fl_push_clip( x(), y(), w(), h() );
    Fl_Scroll::draw();

    if ( isdrawruler == true )
    {
        fl_color( FL_BLUE );
        fl_line_style( 0, 3, NULL );

        fl_line( coordclicked_x, coordclicked_y, coordmoving_x, coordmoving_y );
    }
    else
    if ( isdrawclickpointer == true )
    {
        fl_color( colorclicked );
        fl_line_style( 0, 1, NULL );

        fl_line( coordclicked_x , y() , coordclicked_x, coordclicked_y - 1 );
        fl_line( coordclicked_x, coordclicked_y + 1 , coordclicked_x, y() + h() );
        fl_line( x(), coordclicked_y, coordclicked_x - 1, coordclicked_y );
        fl_line( coordclicked_x + 1, coordclicked_y, x() + w(), coordclicked_y );
    }

    fl_pop_clip();
}

void Fl_ImageViewer::box(Fl_Boxtype new_box)
{
    // It always ignore user custom box type.
    //Fl_Scroll::box( box() );
    Fl_Scroll::box( FL_THIN_DOWN_FRAME );
}

void Fl_ImageViewer::type(uchar t)
{
    // It always ignore user type = NO scroll bars.
    // Fl_Scroll::type( type() );
    Fl_Scroll::type( 0 );
}


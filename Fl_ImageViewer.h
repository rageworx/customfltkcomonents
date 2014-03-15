#ifndef __FL_IMAGEVIEWER_H__
#define __FL_IMAGEVIEWER_H__

#include <FL/Fl.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Image.H>

class Fl_ImageViewerNotifier
{
    public:
        virtual void OnRightClick( int x, int y) = 0;
        virtual void OnResized( float r ) = 0;
        virtual void OnDraged( int x, int y ) = 0;
        virtual void OnMove( int x, int y ) = 0;
        virtual void OnRuler( int startx, int starty, int endx, int endy ) = 0;
        virtual void OnDropFile( const char* path ) = 0;
};

class Fl_IMGDBox;

class Fl_ImageViewer : public Fl_Scroll
{
    public:
        Fl_ImageViewer(int x,int y,int w,int h);
        virtual ~Fl_ImageViewer();

    public:
        void image(Fl_Image* aimg);
        void unloadimage();
        void multiplyratio( float rf );
        void fitwidth();
        void fitheight();
        void box(Fl_Boxtype new_box);
        void type(uchar t);

    public:
        Fl_Image* image()                               { return imgsrc; }
        float     multiplyratio()                       { return multiplier; }
        void      coordlinecolor( Fl_Color c )          { colorclicked = c; }
        Fl_Color  coordlinecolor()                      { return colorclicked; }

    public:
        void      notifier( Fl_ImageViewerNotifier* p ) { _notifier = p; }
        Fl_ImageViewerNotifier* notifier()              { return _notifier; }

    public:
        int imgw();
        int imgh();
        int prochandle( int event );

    private:
        int handle( int event );    /// FLTK inherited handler()
        void draw();                /// FLTK inherited draw()

    private:
        float       multiplier;

    private:
        Fl_IMGDBox* dispbox;
        Fl_Image*   imgsrc;
        Fl_Image*   imgcached;

    private:
        bool        isdrawclickpointer;
        bool        isdrawruler;
        int         coordclicked_x;
        int         coordclicked_y;
        int         coordmoving_x;
        int         coordmoving_y;
        Fl_Color    colorclicked;
        Fl_Color    colortrace;
        int         margin_x;
        int         margin_y;

    private:
        Fl_ImageViewerNotifier* _notifier;
};


#endif /// of __FL_IMAGEVIEWER_H__

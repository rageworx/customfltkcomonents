#ifndef __FL_IMAGEDISPLAYBOX_H__
#define __FL_IMAGEDISPLAYBOX_H__

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Image.H>

class Fl_ImageDisplayBoxUserOverride
{
    public:
        virtual void drawuserimage(int w, int h) = 0;
};

class Fl_ImageDisplayBox : public Fl_Box
{
    public:
        Fl_ImageDisplayBox(int x,int y,int w,int h);
        virtual ~Fl_ImageDisplayBox();

    public:
        void transparent(bool enabled);
        void traceline(bool enabled) { _traceline = enabled; }
        void color(Fl_Color c);
        void image(Fl_Image* aimg);
        void palletimage(Fl_Image* aimg);
        void unloadimage();

    public:
        Fl_Image* image() { return dispimg; }
        Fl_Image* palletimage() { return palletimg; }

    public:
        void userdrawoverride( Fl_ImageDisplayBoxUserOverride* ovrride ){ userovrdraw = ovrride; }
        Fl_ImageDisplayBoxUserOverride* userdrawoverride() { return userovrdraw; }

    protected:
        void draw();
        void drawbg();
        void drawimg( Fl_Image* img, Fl_Image* rszimg = NULL, bool userszimg = false );

    protected:
        int putX;
        int putY;
        int putW;
        int putH;

    private:
        bool          transparentbg;
        bool          _traceline;
        Fl_Image*     dispimg;
        Fl_Image*     palletimg;

    private:
        Fl_Image*     resizedimg;

    protected:
        Fl_ImageDisplayBoxUserOverride* userovrdraw;
};

#endif /// __FL_IMAGEDISPLAYBOX_H__

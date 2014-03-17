#ifndef __FL_HISTOGRAMDRAWER_H__
#define __FL_HISTOGRAMDRAWER_H__

#include <FL/Fl.H>
#include <FL/Fl_Box.H>

#include <vector>

class Fl_HistogramDrawer : public Fl_Box
{
    public:
        Fl_HistogramDrawer(int x,int y,int w,int h);
        virtual ~Fl_HistogramDrawer();

    public:
        void data( std::vector<int> &refdata );
        void emphase_value(int vmin, int vmax );

    public:
        void emphase( bool toggle )     { flag_emphase = toggle; draw(); }
        bool emphase()                  { return flag_emphase; }
        void limit( int l )             { val_limit = l; }
        int  limit()                    { return val_limit; }

    protected:
        void draw();
        void resize(int,int,int,int);

    protected:
        void findmax();

    private:
        int                 val_limit;
        int                 val_max;
        int                 val_emphase_min;
        int                 val_emphase_max;
        bool                flag_emphase;
        bool                flag_drawing;
        std::vector<int>    histo;
};

#endif /// of __FL_HISTOGRAMDRAWER_H__

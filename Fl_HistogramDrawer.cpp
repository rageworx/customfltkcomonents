#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include "Fl_HistogramDrawer.h"

using namespace std;

Fl_HistogramDrawer::Fl_HistogramDrawer(int x,int y,int w,int h)
 : Fl_Box(x, y, w, h),
   val_emphase_min( 0 ),
   val_emphase_max( 0 ),
   val_limit( 0 ),
   flag_emphase( false ),
   flag_drawing( false )
{
    box( FL_NO_BOX );
    histo.clear();
}

Fl_HistogramDrawer::~Fl_HistogramDrawer()
{
    histo.clear();
}

void Fl_HistogramDrawer::data( vector<int> &refdata )
{
    if ( flag_drawing == true )
        return;

    histo.clear();

    for ( int cnt=0; cnt<refdata.size(); cnt++ )
    {
        histo.push_back( refdata[cnt] );
    }

    findmax();
}

void Fl_HistogramDrawer::emphase_value(int vmin, int vmax )
{
    val_emphase_min = vmin;
    val_emphase_max = vmax;
}

void Fl_HistogramDrawer::findmax()
{
    val_max = 0;

    int hmax = histo.size();

    for( int cnt=0; cnt<hmax; cnt++ )
    {
        int curVal = histo[cnt];
        if ( curVal > val_max )
            val_max = curVal;
    }
}

void Fl_HistogramDrawer::resize(int XX,int YY,int WW,int HH)
{
    Fl_Box::resize(XX,YY,WW,HH);
    //draw();
}

void Fl_HistogramDrawer::draw()
{
    if ( flag_drawing == true )
        return;

    flag_drawing = true;

    fl_push_clip( x(), y(), w(), h() );
    Fl_Box::draw();

    // Erase bg.
    fl_color( FL_BLACK );
    fl_rectf( x(), y(), w(), h() );

    int histo_size = histo.size();

    if ( val_limit > 0 )
    {
        histo_size = val_limit;
    }

    if ( histo_size > 0 )
    {
        fl_line_style( FL_SOLID );

        // Draw Histogram.
        float refW    = float( w() );
        float refH    = float( h() );
        float wscalef = float( histo_size ) / float( refW );
        float dscalef = float( refH ) / float( val_max );

        for(int cntH=0; cntH<w(); cntH++)
        {
            int refIdx = float( cntH ) * wscalef;
            int depth  = histo[refIdx] * dscalef;

            fl_color( FL_YELLOW );

            if ( flag_emphase == true )
            {
                if ( ( val_emphase_min <= refIdx ) &&
                     ( val_emphase_max >= refIdx ) )
                {
                    fl_color( FL_DARK_YELLOW );
                    fl_line( x() + cntH,
                             y() ,
                             x() + cntH,
                             y() + h() );

                    fl_color( FL_RED );
                }
            }

            fl_line( x() + cntH,
                     y() + ( h() - depth ),
                     x() + cntH,
                     y() + h() );
        }
    }

    fl_pop_clip();
    flag_drawing = false;
}

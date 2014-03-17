#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_RGB_Image.H>
#include "fl_smimg.h"

Fl_Window*      window  = NULL;
Fl_Box*         box_bg  = NULL;
Fl_Box*         box_im1 = NULL;
Fl_Box*         box_im2 = NULL;

int main (int argc, char ** argv)
{
    window = new Fl_Window (960, 600);

    if ( window != NULL )
    {
        window->label("ResizeEngine (From FreeImage3) for FLTK test.");
        window->begin();

        box_bg = new Fl_Box( 0, 0, 960, 600 );
        if ( box_bg != NULL )
        {
            box_bg->box( FL_NO_BOX );

            Fl_Image* img_loader = new Fl_JPEG_Image( "img/bg.jpg" );
            if ( img_loader != NULL )
            {
                box_bg->image( img_loader );
            }
        }

        Fl_Image* imgsrc = new Fl_PNG_Image( "img/ship.png" );
        if ( imgsrc != NULL )
        {
            GenericFilter* afilter = new Lanczos3Filter();
            ResizeEngine resizer( afilter );

            Fl_Image* imgscaled = resizer.scale( (Fl_RGB_Image*)imgsrc, 450, 400 );

            delete afilter;

            box_im1 = new Fl_Box( 10, 10, 450, 400, "Resize Engine used.\n* Lanczos3 * " );
            if ( box_im1 != NULL )
            {
                box_im1->labelcolor( FL_WHITE );
                box_im1->box( FL_NO_BOX );
                box_im1->image( imgscaled );
            }

            Fl_RGB_Image* imgcopy = (Fl_RGB_Image*)imgsrc->copy(450,400);
            box_im2 = new Fl_Box( 500, 10, 450, 400, "FLTK internal resize." );
            if ( box_im2 != NULL )
            {
                box_im2->labelcolor( FL_WHITE );
                box_im2->box( FL_NO_BOX );
                box_im2->image( imgcopy );
            }

        }

        window->end();
        window->show();
    }

    int retI = Fl::run();

    return retI;
}

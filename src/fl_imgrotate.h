#ifndef __FL_IMGROTATE_H__
#define __FL_IMGROTATE_H__

#include <FL/Fl.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_RGB_Image.H>

bool getRotatedImage( Fl_RGB_Image* src, float angle, Fl_RGB_Image* &out );
void removeRotatedImage( Fl_RGB_Image* img );

#endif /// of __FL_IMGROTATE_H__

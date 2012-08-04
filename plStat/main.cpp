#include <QApplication>
#include <QtGui>
//#include "OpenFileExDlg.h"
//#include "fitsio.h"
//#include <iostream.h>
#include <QtCore>
#include <QDataStream>

//#include "conio.h"
#include "math.h"
#include "./../libs/astro.h"
#include "./../libs/platesStat.h"
#include "./../libs/fitsdata.h"
/*
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
*/
QTextStream out_stream;

/*
  РѕРџРќР¦РџР®Р›Р›Р® РЇРќРЇР Р®Р‘РљРЄР•Р  РЇРћРҐРЇРќР™ РћРљР®РЇР РҐРњРќР™, РЇРќР”Р•РџР¤Р®Р«РҐР
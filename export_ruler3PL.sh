#!/bin/bash

if [[ $# -lt 1 ]]
then exit
fi

svn export --force ruler3PL $1/ruler3PL
svn export --force wcsFinder $1/wcsFinder
svn export --force plateWCS $1/plateWCS
svn export --force mb $1/mb
svn export --force astro $1/astro

svn export --force libs/alglib $1/libs/alglib
#svn export --force libs/unix $1/libs/unix
#svn export --force libs/win32 $1/libs/win32

svn export --force libs/fitsdata.cpp $1/libs/fitsdata.cpp
svn export --force libs/comfunc.cpp $1/libs/comfunc.cpp
svn export --force libs/multidim.cpp $1/libs/multidim.cpp
svn export --force libs/ccdmeasurements.cpp $1/libs/ccdmeasurements.cpp
svn export --force libs/redStat.cpp $1/libs/redStat.cpp
svn export --force libs/observatory.cpp $1/libs/observatory.cpp
svn export --force libs/identify.cpp $1/libs/identify.cpp
svn export --force libs/cmnk.cpp $1/libs/cmnk.cpp
svn export --force libs/sscat.cpp $1/libs/sscat.cpp
svn export --force libs/fBuffer.cpp $1/libs/fBuffer.cpp
svn export --force libs/vectGrid3D.cpp $1/libs/vectGrid3D.cpp
svn export --force libs/fitstools.cpp $1/libs/fitstools.cpp
svn export --force libs/sysCorr.cpp $1/libs/sysCorr.cpp

svn export --force libs/fitsdata.h $1/libs/fitsdata.h
svn export --force libs/comfunc.h $1/libs/comfunc.h
svn export --force libs/multidim.h $1/libs/multidim.h
svn export --force libs/ccdmeasurements.h $1/libs/ccdmeasurements.h
svn export --force libs/redStat.h $1/libs/redStat.h
svn export --force libs/observatory.h $1/libs/observatory.h
svn export --force libs/identify.h $1/libs/identify.h
svn export --force libs/cmnk.h $1/libs/cmnk.h
svn export --force libs/sscat.h $1/libs/sscat.h
svn export --force libs/fBuffer.h $1/libs/fBuffer.h
svn export --force libs/vectGrid3D.h $1/libs/vectGrid3D.h
svn export --force libs/fitstools.h $1/libs/fitstools.h
svn export --force libs/sysCorr.h $1/libs/sysCorr.h


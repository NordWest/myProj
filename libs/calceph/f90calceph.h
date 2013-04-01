!/*-----------------------------------------------------------------*/
!/*! 
!  \file f90calceph.h
!  \brief fortran 77/90/95 binding interface
!         requires a fortran compiler with the standard fortran 77.
!
!  \author  M. Gastineau 
!           Astronomie et Systemes Dynamiques, IMCCE, CNRS, Observatoire de Paris. 
!
!   Copyright, 2008, 2009, 2010, 2011, CNRS
!   email of the author : gastineau@imcce.fr
! 
! History :
!    \note M. Gastineau 12/10/10 : add custom error handler     
!    \note M. Gastineau 21/05/11 : support asteroid in INPOP file format 2.0
!    \note M. Gastineau 10/06/11 : support SPICE kernel file
!    \note M. Gastineau 22/08/11 : add calceph_compute_unit
!
!*/
!/*-----------------------------------------------------------------*/

!/*-----------------------------------------------------------------*/
!/* License  of this file :
!  This file is "triple-licensed", you have to choose one  of the three licenses 
!  below to apply on this file.
!  
!     CeCILL-C
!     	The CeCILL-C license is close to the GNU LGPL.
!     	( http://www.cecill.info/licences/Licence_CeCILL-C_V1-en.html )
!   
!  or CeCILL-B
!        The CeCILL-B license is close to the BSD.
!        (http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.txt)
!  
!  or CeCILL v2.0
!       The CeCILL license is compatible with the GNU GPL.
!       ( http://www.cecill.info/licences/Licence_CeCILL_V2-en.html )
!  
! 
! This library is governed by the CeCILL-C, CeCILL-B or the CeCILL license under 
! French law and abiding by the rules of distribution of free software.  
! You can  use, modify and/ or redistribute the software under the terms 
! of the CeCILL-C,CeCILL-B or CeCILL license as circulated by CEA, CNRS and INRIA  
! at the following URL "http://www.cecill.info". 
!
! As a counterpart to the access to the source code and  rights to copy,
! modify and redistribute granted by the license, users are provided only
! with a limited warranty  and the software's author,  the holder of the
! economic rights,  and the successive licensors  have only  limited
! liability. 
!
! In this respect, the user's attention is drawn to the risks associated
! with loading,  using,  modifying and/or developing or reproducing the
! software by the user in light of its specific status of free software,
! that may mean  that it is complicated to manipulate,  and  that  also
! therefore means  that it is reserved for developers  and  experienced
! professionals having in-depth computer knowledge. Users are therefore
! encouraged to load and test the software's suitability as regards their
! requirements in conditions enabling the security of their systems and/or 
! data to be ensured and,  more generally, to use and operate it in the 
! same conditions as regards security. 
!
! The fact that you are presently reading this means that you have had
! knowledge of the CeCILL-C,CeCILL-B or CeCILL license and that you accept its terms.
! */
! /*-----------------------------------------------------------------*/

       external f90calceph_seterrorhandler

       integer f90calceph_sopen
       external f90calceph_sopen
       integer f90calceph_scompute
       external f90calceph_scompute       
       integer f90calceph_sgetconstant
       external f90calceph_sgetconstant    
       integer f90calceph_sgetconstantcount
       external f90calceph_sgetconstantcount    
       integer f90calceph_sgetconstantindex
       external f90calceph_sgetconstantindex    
       external f90calceph_sclose
       
       integer f90calceph_open
       external f90calceph_open
       integer f90calceph_open_array
       external f90calceph_open_array
       integer f90calceph_compute
       external f90calceph_compute       
       integer f90calceph_compute_unit
       external f90calceph_compute_unit       
       integer f90calceph_getconstant
       external f90calceph_getconstant    
       integer f90calceph_getconstantcount
       external f90calceph_getconstantcount    
       integer f90calceph_getconstantindex
       external f90calceph_getconstantindex    
       external f90calceph_close
       integer CALCEPH_MAX_CONSTANTNAME
       PARAMETER (CALCEPH_MAX_CONSTANTNAME=33)
       integer CALCEPH_ASTEROID
       PARAMETER (CALCEPH_ASTEROID=2000000)
       integer CALCEPH_UNIT_AU
       PARAMETER (CALCEPH_UNIT_AU=1)
       integer CALCEPH_UNIT_KM
       PARAMETER (CALCEPH_UNIT_KM=2)
       integer CALCEPH_UNIT_DAY
       PARAMETER (CALCEPH_UNIT_DAY=4)
       integer CALCEPH_UNIT_SEC
       PARAMETER (CALCEPH_UNIT_SEC=8)
       integer CALCEPH_UNIT_RAD
       PARAMETER (CALCEPH_UNIT_RAD=16)

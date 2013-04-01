/*-----------------------------------------------------------------*/
/*! 
  \file calcephspice.h 
  \brief private API for calceph library
         access and interpolate SPICE Ephemeris data.

  \author  M. Gastineau 
           Astronomie et Systemes Dynamiques, IMCCE, CNRS, Observatoire de Paris. 

   Copyright, 2011, 2012, CNRS
   email of the author : gastineau@imcce.fr

  History:                                                                
*/
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
/* License  of this file :
 This file is "triple-licensed", you have to choose one  of the three licenses 
 below to apply on this file.
 
    CeCILL-C
    	The CeCILL-C license is close to the GNU LGPL.
    	( http://www.cecill.info/licences/Licence_CeCILL-C_V1-en.html )
   
 or CeCILL-B
        The CeCILL-B license is close to the BSD.
        (http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.txt)
  
 or CeCILL v2.0
      The CeCILL license is compatible with the GNU GPL.
      ( http://www.cecill.info/licences/Licence_CeCILL_V2-en.html )
 

This library is governed by the CeCILL-C, CeCILL-B or the CeCILL license under 
French law and abiding by the rules of distribution of free software.  
You can  use, modify and/ or redistribute the software under the terms 
of the CeCILL-C,CeCILL-B or CeCILL license as circulated by CEA, CNRS and INRIA  
at the following URL "http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-C,CeCILL-B or CeCILL license and that you accept its terms.
*/
/*-----------------------------------------------------------------*/

#ifndef __CALCEPH_SPICE__
#define __CALCEPH_SPICE__

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif                          

#include "calcephstatetype.h"

/*-----------------------------------------------------------------*/
/* private API */
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
/* constants */
/*-----------------------------------------------------------------*/
/*! number of bytes in a DAF record  */
#define DAF_RECORD_LEN 1024

/*! number of characters in ID WORD of SPK */
#define IDWORD_LEN 8

/*! number of characters in IFNAME of SPK */
#define IFNAME_LEN 60

/*! number of characters in BFF of SPK */
#define BFF_LEN 8

/*! number of padding characters after BFF of SPK */
#define PADBFF_LEN 603

/*! number of characters in FTP of SPK */
#define FTP_LEN 28

/*! number of padding characters after FTP of SPK */
#define PADFTP_LEN 297

/*! number of segment per record of SPK */
#define NSEGMENT_PERRECORD  25

/*! number of characters in SGEMENT ID of SPK */
#define SEGMENTID_LEN 40

/*! number of characters in FRAME of SPK */
#define FRAME_LEN 5

/*! number of word in the summary of SPK */
#define NS_SPK 5

/*! list of SPICE supported file */
enum SPICEfiletype
{
    DAF_SPK = 1,                /*!< binary SPK file */
    DAF_PCK,                    /*!< binary PCK file */
    TXT_PCK,                    /*!< text PCK file */
    TXT_FK                      /*!< text FK file */
};

/*! list of SPK data type */
enum SPKdatatype
{
    SPK_SEGTYPE2 = 2,           /*!< Chebyshev polynomials for position. fixed length time intervals. */
    SPK_SEGTYPE3 = 3            /*!< Chebyshev polynomials for position and velocity. fixed length time intervals */
};

/*-----------------------------------------------------------------*/
/* structures */
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
/*! binary SPK header */
/*-----------------------------------------------------------------*/
struct SPKHeader
{
    char idword[IDWORD_LEN];    /*!< Indicates file architecture and type */
    int nd, ni;                 /*!< Number of d.p. and integer descriptor components */
    char ifname[IFNAME_LEN];    /*!< Internal file name */
    int fwd, bwd;               /*!< Forward and backward linked list pointers */
    int free;                   /*!< First free DAF address */
    char bff[BFF_LEN];          /*!<  Binary file format ID */
    char padbff[PADBFF_LEN];    /*!< pad character after BFF */
    unsigned char ftp[FTP_LEN]; /*!<  FTP corruption test string */
    char padftp[PADFTP_LEN];    /*!< pad character after FTP */
};

/*-----------------------------------------------------------------*/
/*! Segment Header of type 2 or 3  */
/*-----------------------------------------------------------------*/
struct SPKSegmentHeader_2
{
    double T_begin;             /*!< initial epoch of first record */
    double T_len;               /*!< length of interval covered by each record */
    int count_dataperrecord;    /*!< number of data elements in each record */
    int count_record;           /*!< number of records in segment */
};

/*-----------------------------------------------------------------*/
/*! common Segment Header  for all SPK data type  */
/*-----------------------------------------------------------------*/
struct SPKSegmentHeader
{
    char id[SEGMENTID_LEN];     /*!< segment id */
    char descriptor[NS_SPK * 8];    /*!< segment descriptor */
    double T_begin;             /*!< begin of the segment in seconds */
    double T_end;               /*!< end of the segment in seconds */
    int body;                   /*!< body */
    int center;                 /*!< center */
    int refframe;               /*!< reference frame number */
    enum SPKdatatype datatype;  /*!< type of segment */
    int rec_begin;              /*!< first data record  of the segment */
    int rec_end;                /*!< last data record  of the segment */
    union
    {
        struct SPKSegmentHeader_2 data2;    /*!< valid if datatype= SPK_SEGTYPE2 */
        struct SPKSegmentHeader_2 data3;    /*!< valid if datatype= SPK_SEGTYPE3 */
    } seginfo;                  /*!< specific dat for the segment */
};

/*-----------------------------------------------------------------*/
/*! array of NSEGMENT_PERRECORD Segment Header for  SPK  */
/*-----------------------------------------------------------------*/
struct SPKSegmentHeaderArray
{
    int count;                  /*!< number of valid segment */
    struct SPKSegmentHeader array[NSEGMENT_PERRECORD];  /*!< segments */
};

/*-----------------------------------------------------------------*/
/*! double-linked list of SPKSegmentHeaderArray */
/*-----------------------------------------------------------------*/
struct SPKSegmentList
{
    struct SPKSegmentList *prev;    /*!< previous element */
    struct SPKSegmentList *next;    /*!< next element */
    int recordnumber;           /*!< number of its record in the file */
    struct SPKSegmentHeaderArray array_seg; /*!< array of segments */
};

/*-----------------------------------------------------------------*/
/*! state structure for a SPK ephemeris  */
/*-----------------------------------------------------------------*/
struct SPKfile
{
    FILE *file;                 /*!< I/O descriptor */
    struct SPKHeader header;    /*!< header of the SPK file */
    struct SPKSegmentList *list_seg;    /*!< list of array of segments */
};

/*-----------------------------------------------------------------*/
/*! list of value of a constant in a PCK ephemeris  */
/*-----------------------------------------------------------------*/
struct TXTPCKvalue
{
    struct TXTPCKvalue *next;   /*!< next value */
    char *buffer;               /*!< content of the constant */
    off_t locfirst;             /*!< location of the first character in the buffer */
    off_t loclast;              /*!< location of the last character in the buffer */
};

/*-----------------------------------------------------------------*/
/*! list of constant in a PCK ephemeris  */
/*-----------------------------------------------------------------*/
struct TXTPCKconstant
{
    struct TXTPCKconstant *next;    /*!< next constant */
    char *name;                 /*!< name */
    struct TXTPCKvalue *value;  /*!< value */
};

/*-----------------------------------------------------------------*/
/*! state structure for a text PCK ephemeris  */
/*-----------------------------------------------------------------*/
struct TXTPCKfile
{
    FILE *file;                 /*!< I/O descriptor */
    char *buffer;               /*!< content of the constant */
    struct TXTPCKconstant *listconstant;    /*!< list of constants */
};

/*-----------------------------------------------------------------*/
/*! list of frame in a FK ephemeris  */
/*-----------------------------------------------------------------*/
struct TXTFKframe
{
    struct TXTFKframe *next;    /*!< next frame */
    char *name;                 /*!< name */
    struct TXTPCKvalue *frame_name; /*!< frame name */
    int frame_id;               /*!< frame id */
    int class_id;               /*!< class id */
    int classtype;              /*!< class  */
    int center;                 /*!< center */
    /* TK frame */
    struct TXTPCKvalue *tkframe_relative;   /*!< tk relative */
    struct TXTPCKvalue *tkframe_spec;   /*!< tk spec */
    struct TXTPCKvalue *tkframe_matrix; /*!< tk matrix */
    struct TXTPCKvalue *tkframe_angles; /*!< tk euler angles  */
    struct TXTPCKvalue *tkframe_axes;   /*!< tk euler axes  */
    struct TXTPCKvalue *tkframe_unit;   /*!< tk euler angles  */
    struct TXTPCKvalue *tkframe_quaternion; /*!< tk quaternion */
};

/*-----------------------------------------------------------------*/
/*! state structure for a text FK ephemeris  */
/*-----------------------------------------------------------------*/
struct TXTFKfile
{
    struct TXTPCKfile txtpckfile;   /*!< list of constants */
    struct TXTFKframe *listframe;   /*!< list of frame */
};

/*-----------------------------------------------------------------*/
/*! state structure for a spice ephemeris file  */
/*-----------------------------------------------------------------*/
struct SPICEkernel
{
    enum SPICEfiletype filetype;    /*!< type of the ephemeris  */
    union
    {
        struct SPKfile spk;     /*!< SPK file, valid only if filetype = DAF_SPK */
        struct TXTPCKfile txtpck;   /*!< text PCK file, valid only if filetype = TXT_PCK */
        struct TXTFKfile txtfk; /*!< text FK file, valid only if filetype = TXT_FK */
    } filedata;                 /*!< content of the ephemeris */
    struct SPICEkernel *next;   /*!< next spice ephemeris file */
};

/*-----------------------------------------------------------------*/
/*! state structure for a list of spice ephemeris files */
/*-----------------------------------------------------------------*/
struct calcephbin_spice
{
    struct SPICEkernel *list;   /*!<  list of the ephemeris file */
    /* predefined constant */
    double AU;                  /*!< Astronomical Unit */
    double EMRAT;               /*!< Earth-Moon mass ratio */
};

/*-----------------------------------------------------------------*/
/*! array of ephemeris state structure */
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
/*! function */
/*-----------------------------------------------------------------*/
/*! open for reading a SPK file */
int calceph_spk_open(FILE * file, const char *filename, struct SPICEkernel *res);

/*! close for reading a SPK file */
void calceph_spk_close(struct SPKfile *eph);

/*! evaluate position/velocity of a body from a SPK file */
int calceph_spk_interpol_PV(struct calcephbin_spice *eph, struct SPKfile *pephbin, treal TimeJD0, treal Timediff,
                            int Target, int Center, stateType * Planet);
/*! evaluate position/velocity of a body in a segment of type 2 from a SPK file */
int calceph_spk_interpol_PV_segment_2(struct SPKfile *pspk, struct SPKSegmentHeader *seg,
                                      treal TimeJD0, treal Timediff, stateType * Planet);

/*! open for reading a binary PCK file */
int calceph_binpck_open(FILE * file, const char *filename, struct SPICEkernel *res);

/*! close for reading a binary PCK file */
void calceph_binpck_close(struct SPKfile *eph);

/*! evaluate position/velocity of a body from a SPK file */
int calceph_binpck_interpol_PV(struct calcephbin_spice *eph, struct SPKfile *pephbin, treal TimeJD0, treal Timediff,
                               int Target, int Center, stateType * Planet);

/*! open for reading a text PCK file */
int calceph_txtpck_open(FILE * file, const char *filename, struct SPICEkernel *res);

/*! close for reading a text PCK file */
void calceph_txtpck_close(struct TXTPCKfile *eph);

/*! open for reading a text FK file */
int calceph_txtfk_open(FILE * file, const char *filename, struct SPICEkernel *res);

/*! close for reading a text FK file */
void calceph_txtfk_close(struct TXTFKfile *eph);

/*! open for reading a text MK file */
int calceph_txtmk_open(FILE * file, const char *filename, struct calcephbin_spice *res);

/*! close for reading any SPICE file */
void calceph_spice_close(struct calcephbin_spice *eph);

/*! evaluate position/velocity of a body from a SPICE file */
int calceph_spice_interpol_PV(struct calcephbin_spice *pephbin, treal TimeJD0, treal Timediff,
                              int Target, int Center, int Unit, stateType * Planet);
/*! add a file to the list of kernel */
struct SPICEkernel *calceph_spice_addfile(struct calcephbin_spice *eph);

/*! convert id from calceph to spice */
int calceph_spice_convertid(struct calcephbin_spice *eph, int target);

/*-----------------------------------------------------------------*/
/*! internal functions to read spk/binary pck files */
/*-----------------------------------------------------------------*/
#if DEBUG
void calceph_spk_debugheader(const struct SPKHeader *header);
void calceph_txtpck_debug(const struct TXTPCKfile *header);
#endif
int calceph_spk_ftp(const unsigned char ftpbuf[FTP_LEN]);
int calceph_spk_readword(FILE * file, const char *filename, int rec_begin, int rec_end, double *record);

/*-----------------------------------------------------------------*/
/*! internal functions to read text kernel files */
/*-----------------------------------------------------------------*/
int calceph_txtpck_load(FILE * file, const char *filename, const char *header, struct TXTPCKfile *res);
int calceph_txtpck_getconstant_int(struct TXTPCKfile *eph, const char *name, int *p_pival);
int calceph_txtpck_getconstant_txtpckvalue(struct TXTPCKfile *eph, const char *name,
                                           struct TXTPCKvalue **p_ptxtpckvalue);
struct TXTPCKconstant *calceph_txtpck_getptrconstant(struct TXTPCKfile *eph, const char *name);
int calceph_txtpck_cmpvalue(struct TXTPCKvalue *value1, struct TXTPCKvalue *value2);

/*-----------------------------------------------------------------*/
/*! internal functions for frame kernel files */
/*-----------------------------------------------------------------*/
struct TXTFKframe *calceph_txtfk_findframe(struct TXTFKfile *eph, struct TXTPCKconstant *cst);

/*----------------------------------------------------------------------------*/
/* get constants */
/*----------------------------------------------------------------------------*/
double calceph_spice_getEMRAT(struct calcephbin_spice *p_pbinfile);
double calceph_spice_getAU(struct calcephbin_spice *p_pbinfile);
int calceph_spice_getconstant(struct calcephbin_spice *p_pbinfile, const char *name, double *p_pdval);
int calceph_spice_getconstantcount(struct calcephbin_spice *eph);
int calceph_spice_getconstantindex(struct calcephbin_spice *eph, int index, char name[CALCEPH_MAX_CONSTANTNAME],
                                   double *value);

int calceph_txtpck_getconstant(struct TXTPCKfile *eph, const char *name, double *p_pdval);
int calceph_txtpck_getconstantcount(struct TXTPCKfile *eph);
int calceph_txtpck_getconstantindex(struct TXTPCKfile *eph, int *index, char name[CALCEPH_MAX_CONSTANTNAME],
                                    double *value);

#endif /*__CALCEPH_SPICE__*/

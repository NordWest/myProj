//------------------------------------------------------------------------------
// Project:  AccuTime SDK
// Module:   AccuTime.h - C header for linking to the AccuTime library
// Comments: A typical library usage is quite simple:
//             1) link the AccuTime shared library/DLL;
//             2) call atInit to initialize session;
//             3) call atGetUT to obtain the current Universal Time (GMT) in
//                the COM/Delphi monotonous time format (double-precision
//                floating point value, giving the fractional number of days
//                since December 30 1899 00:00); add the atDateDelta constant
//                to convert this value to MJD
//               or
//                call atGetUnpackedUT to obtain the current UT in the
//                (Y,M,D,H,M,S) format;
//             4) call atGetLocalTime or atGetUnpackedLocalTime to obtain the
//                current local time;
//             5) call atShutdown to terminate the session.
//           Other functions are intended for informational and configuration
//           purposes and are rarely used in common applications.
// Author:   Vladimir Kouprianov, Pulkovo Observatory (v.k@bk.ru)
//------------------------------------------------------------------------------



#ifdef __cplusplus
extern "C" {
#endif


// Constant to convert COM/Delphi monotonous time to MJD
// Example:
//   double MJD = atGetUT() + atDateDelta;
#define atDateDelta 2415018.5


// Datalogging callback function prototype
//
// If your program supports some kind of datalogging facility, you may define
// such a function and pass it to atInit() (see below). AccuTime will call this
// function upon some of its important events, passing the following to your
// application:
//
//   source - name of the module from which the message originates (will always
//            be "AccuTime");
//   level  - message severity level; upon convention,
//              0 - critical error (further operation is completely impossible)
//              1 - non-fatal error (a serious failure occurred, but some
//                  recovery measures were taken, and operation continues)
//              2 - warning (possibly nothing serious, but something deserves a
//                  particular operator attention)
//              3 - informational message, marking some stage of the normal
//                   AccuTime operation
//              4 - additional info message (less important)
//              5 and above - debugging message.
//            Application may use the message level e.g. to ignore any messages
//            with levels above 2 and report only important events.
//   msg    - message text.
//
// An example of such a function could be like
//
//   void LogFunction(char *source, int level, char *msg) {
//     if (level <= 2) {
//       printf("%s [%d]: %s\n", source, level, msg);
//     }
//   }
//     ...
//   int main(...
//     atInit(&LogFunction);
//
// If datalogging is not supported or not required, simply pass null to
// atInit()
typedef void(*LOGCALLBACK)(char *source, int level, char* msg);


// atInit: Initialize the library
// Call this function once before invoking any other AccuTime functions.
//
//   logCallback - pointer to the datalogging callback function described
//                 above, or null, if logging is not needed.
void atInit(LOGCALLBACK logFunction);

// atShutdown: Finalize the library
// Call this function once after the last library usage.
void atShutdown(void);

// atConfigureLibrary: Invoke the global library configuration dialog
void atConfigureLibrary(void);

// atCountAvailableDrivers: Return the number of available timer drivers
int atCountAvailableDrivers(void);

// atGetDriverName: Return the name (ID string) of a driver with the
// specified index (0 to atCountAvailableDrivers() - 1)
char* atGetDriverName(int driverIndex);

// atGetDriverLibraryName: Return the filename (w/o path) of the driver
// implementation module (shared library/DLL) for driver with the specified
// index (0 to atCountAvailableDrivers() - 1)
char* atGetDriverLibraryName(int driverIndex);

// atGetActiveDriver: Return the index of driver which is currently active
int atGetActiveDriver(void);

// atSetActiveDriver: Change the current active driver to the specified driver
// (driverIndex should be in (0 to atCountAvailableDrivers() - 1).
// Note that the change is permanent, that is, saved across sessions and will
// remain after the system restart. On WinNT/2000/XP/2003 only users with
// administrative privileges can do this.
void atSetActiveDriver(int driverIndex);

// atConfigureDriver: Show the particular driver configuration dialog
void atConfigureDriver(int driverIndex);

// atGetLocalTime/atGetUT: Return the current date/time in the COM/Delphi
// monotonous time format (see comments at the top of this file)
double atGetLocalTime(void);
double atGetUT(void);

// atGetUnpackedLocalTime/atGetUnpackedUT: Return the current date/time in the
// YYYY-MM-DD HH:MM:SS.SSS format
void atGetUnpackedLocalTime(unsigned short *Year,
                            unsigned short *Month,
                            unsigned short *Day,
                            unsigned short *Hour,
                            unsigned short *Min,
                            double *Sec);
void atGetUnpackedUT(unsigned short *Year,
                     unsigned short *Month,
                     unsigned short *Day,
                     unsigned short *Hour,
                     unsigned short *Min,
                     double *Sec);

// atBeginCriticalSection/atEndCriticalSection: Mark a time-critical section
// Give driver a hint that it should avoid heavy CPU consumption for a while.
// Somewhat obsolete and generally not needed, thus no further comments given.
void atBeginCriticalSection(void);
void atEndCriticalSection(void);


#ifdef __cplusplus
}
#endif


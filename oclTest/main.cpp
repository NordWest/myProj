#include <QtCore>
#include <CL/cl.h>
#include "./../libs/fitsdata.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString fileName("test.fits");



    fitsdata *fitsd = new fitsdata;
    if(fitsd->openFile(fileName, 0))
    {
        qDebug() << QString("File %1 is not opened\n").arg(fileName);
        return 1;
    }

/////////////////////////////
    //fitsd->initInst(telescopeFile, insNum);
    insSettings instr("./telescopes.ini");
    instr.getNumIns(0);
    fitsd->setInstrSettings(instr);
/////////////////////////////

    //fitsd->findHstars(aper, (identNum+maxNum)/2);

    size_t szGlobalWorkSize;        // 1D var for Total # of work items
    size_t szLocalWorkSize;		    // 1D var for # of work items in the work group

    // set and log Global and Local work size dimensions
        szLocalWorkSize = 256;
        szGlobalWorkSize = shrRoundUp((int)szLocalWorkSize, iNumElements);  // rounded up to the nearest multiple of the LocalWorkSize
        shrLog("Global Work Size \t\t= %u\nLocal Work Size \t\t= %u\n# of Work Groups \t\t= %u\n\n",
               szGlobalWorkSize, szLocalWorkSize, (szGlobalWorkSize % szLocalWorkSize + szGlobalWorkSize/szLocalWorkSize));

    cl_int ciErr1, ciErr2;
    cl_platform_id cpPlatform;
    cl_device_id cdDevice;
    cl_context cxGPUContext;
    cl_command_queue cqCommandQueue;

    clGetPlatformIDs(1, &cpPlatform, NULL);
    clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &cdDevice, NULL);
    //Create the context
    cxGPUContext = clCreateContext(0, 1, &cdDevice, NULL, NULL, &ciErr1);
    // Create a command-queue
    cqCommandQueue = clCreateCommandQueue(cxGPUContext, cdDevice, 0, &ciErr1);

    //qDebug() << QString("cpPlatform: %1\n").arg(cpPlatform)



    return 0;//a.exec();
}

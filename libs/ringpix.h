#ifndef RINGPIX_H
#define RINGPIX_H

void pix2ang_ring( long nside, long ipix, double *theta, double *phi);
void ang2pix_ring( const long nside, double theta, double phi, long *ipix);

#endif // RINGPIX_H

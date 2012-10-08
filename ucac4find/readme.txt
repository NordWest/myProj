software to read data from the ucac4 
command line must be seen like this:

ucac4find.exe 23:11:45.34 +61:45:32.1 b=10x10 e:/catalogs/ucac4/u4data >out.dat

data will be selected within box 10x10 arcmins centered at RA = 23:11:45.34, Dec = +61:45:32.1
the box edges are specified according to tangential coordinates.

out.dat is the name of output ASCII-file formatted according info from  ucac4 readme file

if r=10x10, data will be selected within radius r=10 arcmins
if g=10x10, data will be selected within RA and Dec box 

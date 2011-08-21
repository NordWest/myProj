//#include "stdafx.h"
#include "identify.h"
#include "math.h"
#include "cmnk.h"

void test(double v1)
{

}
void sort(double* mv,
				   double* v1,
				   double* v2,
				   double* v3,
				   double* v4,
				   double* v5,
				   double* v6,
				   double* v7,
				   double* v8,
				   double* v9,
				   int nstr)
{
	int  ii,jj;
	double x;
	//me:byte;
	bool hy;
	int i=nstr;
	//struct _timeb timebuffer2,timebuffer; 
	//		_ftime( &timebuffer );	
		

     for( ii=2;ii<=i;ii++)
     {
          for( jj=i;jj>=ii;jj--)
          {

               hy=mv[jj-1-1]>mv[jj-1];
               if (hy)
			   {
                     x=mv[jj-1-1];
                     mv[jj-1-1]=mv[jj-1];
                     mv[jj-1]=x;
					 
					 x=v1[jj-1-1];
                     v1[jj-1-1]=v1[jj-1];
                     v1[jj-1]=x;
					 
					 x=v2[jj-1-1];
                     v2[jj-1-1]=v2[jj-1];
                     v2[jj-1]=x;
					 
					 x=v3[jj-1-1];
                     v3[jj-1-1]=v3[jj-1];
                     v3[jj-1]=x;
					 
					 x=v4[jj-1-1];
                     v4[jj-1-1]=v4[jj-1];
                     v4[jj-1]=x;
					 
					 x=v5[jj-1-1];
                     v5[jj-1-1]=v5[jj-1];
                     v5[jj-1]=x;
					 
					 x=v6[jj-1-1];
                     v6[jj-1-1]=v6[jj-1];
                     v6[jj-1]=x;
					 
					 x=v7[jj-1-1];
                     v7[jj-1-1]=v7[jj-1];
                     v7[jj-1]=x;
					 
					 x=v8[jj-1-1];
                     v8[jj-1-1]=v8[jj-1];
                     v8[jj-1]=x;
					 
					 x=v9[jj-1-1];
                     v9[jj-1-1]=v9[jj-1];
                     v9[jj-1]=x;
					 
					 
               }
          }
	 }
	 
};
int sort(double* mv,
				   int * v1,
				   int * v2,
				   double* v3,
				   int nstr)
{	
	
	int  ii,jj;
	double x;
	int xi;

	bool hy;
	int i=nstr;
	int hj=0;
	int hj2=0;
	int oc=(i*(i-1))/2;
	

     for( ii=2;ii<=i;ii++)
     {
         
		 
		 hj2=hj2+i-ii;
		 hj=hj+i-ii;
		 
		 
		 for( jj=i;jj>=ii;jj--)
          {
			 

               hy=mv[jj-1-1]>mv[jj-1];
               if (hy)
			   {
                     x=mv[jj-1-1];
                     mv[jj-1-1]=mv[jj-1];
                     mv[jj-1]=x;
					 
					 xi=v1[jj-1-1];
                     v1[jj-1-1]=v1[jj-1];
                     v1[jj-1]=xi;
					 
					 xi=v2[jj-1-1];
                     v2[jj-1-1]=v2[jj-1];
                     v2[jj-1]=xi;
					 
					 x=v3[jj-1-1];
                     v3[jj-1-1]=v3[jj-1];
                     v3[jj-1]=x;
				 
               }
			   
          }
		  
	 }
	 
	 return 1;
	

};

int otogdestv(double xscale,double yscale,int t100, int t10s,
			  bool newinstrument,
			  double* ksi, double* eta, double* xn10, double* yn10,int* sootv)
{
	double porog=1;
	int mnsfi=4;
	int i;
	int (*z1cat),(*z2cat),(*reftab);
	
	double* rscat;
	
	int *** tabras;
	int ** cdtocat;
	
	double* pzug;
	//int *xn,(*yn);
	double(*maxn10);

	double t10h,t10w;
	double pi;
	int kols,kols0;
	double t5deg,aa2,bb;
	int bt10s;
	double mrac,mdlc,mra[4],mdc[4];
	int mflotogdstv;
	mflotogdstv=0;
	bt10s=t10s;

	
	//xn=NULL;
	t5deg=5;
	aa2=0;
	bb=180;
	kols=(int)((bb-aa2)/(t5deg/2))+2;
	kols0=kols;
	kols=kols*2;


	
	tabras=new int**[kols];
	for(int i0=0;i0<kols;i0++)
	{
		tabras[i0]=new int*[t10s];
		for(int i=0;i<t10s;i++)
		{
			tabras[i0][i]=new int[t100];
		}
	}
	//porog=pr->m_porog;
	//mnsfi=pr->m_mnsfi;

	
	maxn10=new double [t10s+100];
	cdtocat=new int *[kols];
	for(int i0=0;i0<kols;i0++)
	{
		cdtocat[i0]=new int[t10s];
	}
	pi=3.14159265358979324;
	
	rscat=new double [t100*t100/2];
	pzug=new double [t100*t100/2];
	z1cat=new int [t100*t100/2];
	z2cat=new int [t100*t100/2];
	reftab=new int [t100*t100/2];
	int kt100=t100*t100/2;
	int crs=0;
	double dx;
	double dy;
	
	
	for (int i=0;i<t100;i++)
	{
		

		for(int j=(i+1);j<t100;j++)
		{
			dx=ksi[i]-ksi[j];
			dy=eta[i]-eta[j];
			rscat[crs]=dx*dx+dy*dy;
			if (fabs(dy)>1e-8)
			{
				
				pzug[crs]=atan((dx/dy));
				
			} else
			{
				pzug[crs]=pi/2;
			}
			pzug[crs]=(pi/2+pzug[crs])*180.0/pi;

		
			z1cat[crs]=i;
			z2cat[crs]=j;
			crs++;
		}
	}
	int er=sort(rscat,z1cat,z2cat,pzug,crs);
	
	bool fl2pr=false;
	int niu=0;
	
	
	

	double h=rscat[crs-1]/kt100;
	int trf=0;
	for(int i=0;i<kt100;i++)
	{
		double trs=i*h;
		while (rscat[trf]<trs)
		{
			trf++;
		} 
		reftab[i]=trf;
	}
	for (int i0=0;i0<kols;i0++)
	{
		for (int i=0;i<t10s;i++)
		{
			for(int j=0;j<t100;j++)
			{
				tabras[i0][i][j]=0;
			}
		}
	}
	double hmb=0.01;
	long double* xfx;
	long double* sgmx;
	long double* xfy;
	long double* sgmy;
	double a1,a2,b1,b2,c1,c2,p,q;
	p=0; q=0;
	double maxnev=0;
	int imb=0;
	int imb2=0;
	cmnk omnkx;
	omnkx.initmnk(t10s,4);
	cmnk omnky;
	omnky.initmnk(t10s,4);
	bool* bops= new bool[t10s];
	int otp;
	
	int i0ok=107;
	int tw100;
	if (newinstrument)
	{
		tw100=100;
	} else
	{
		tw100=1;
	};
	int twk100=0;
	int twinm2=0;
	
	int inm2=0;
	int inm=0;

	 
	do //for( int imb=0;imb<10;imb++)
	{ //var mashtab
	do //for( int imb2=0;imb2<10;imb2++)
	{ //var distorsija
		
		int inm=0;
		inm2=0;
		
		
		do //for (int inm=0;inm<2;inm++)
		{
		do //for (int inm2=0;inm2<2;inm2++)
		{
			
			double kmb=1.0+hmb*imb;
			if (inm==0)
			{
				kmb=1.0-hmb*imb;
			}
			double mnmb=0.99;
			double mxmb=1.01;
			//mnmb=0.9;
			//mxmb=1.1;
			mnmb=mnmb*mnmb;
			mxmb=mxmb*mxmb;

			int mxtrf=0;
			int mntrf=0;
			double srtrf=0;
			int sf=0;
			int hj=0;
			
			
			
			fl2pr=true;
				int oc=(t10s*(t10s-1))/2;

				

			for(int i=0;i<t10s;i++)
			{
				for(int j=i+1;j<t10s;j++)
				{
				    hj++;
					
					double xn10i=xn10[i];
					double xn10j=xn10[j];
					double yn10i=yn10[i];
					double yn10j=yn10[j];
					
					dx=(xn10i-xn10j)*xscale*kmb;
					dy=(yn10i-yn10j)*yscale*kmb;
					
					double strs=dx*dx+dy*dy;
					/*if (strs>1000)
					{
						dx=1;
						dy=1;
					}*/
					double stpu=0;
					double stpu2=0;
					if (fabs(dy)>1e-8)
					{
						//stpu=atan(fabs(dx/dy));
						stpu=atan((dx/dy));
						stpu2=atan((-dx/dy));
					} else
					{
						stpu=pi/2;
						stpu2=pi/2;
					};
					stpu=(pi/2+stpu)*180.0/pi;
					stpu2=(pi/2+stpu2)*180.0/pi;

					trf=(int)(strs/h);
					if (trf>=kt100) trf=kt100-1;
					trf=reftab[trf];
					int trf1=trf;
					if (trf1<0) trf1=0;
					
					bool ist=true;
					
					
					
					
					
					do
					{
						double rpu=(stpu-pzug[trf1]);
						if (rpu<0) rpu=rpu+180;
						if (rpu<0) rpu=0;
						if (rpu>180) rpu=180;

						double rpu2=(stpu2-pzug[trf1]);
						if (rpu2<0) rpu2=rpu2+180;
						if (rpu2<0) rpu2=0;
						if (rpu2>180) rpu2=180;
						
						
						//if (ist)
						{
						

							int tf=1;//tabpu[(int)rpu];
							int tf2=(int)rpu;
							int z1=z1cat[trf1];
							int z2=z2cat[trf1];
							
							tf=1;
							double d2=t5deg/2;
							int i0a=(int)(rpu-aa2+d2)/(d2);
							int i0b=i0a+1;
						
							tabras[i0a][i][z1]=tabras[i0a][i][z1]+tf;
							tabras[i0a][i][z2]=tabras[i0a][i][z2]+tf;
							tabras[i0a][j][z1]=tabras[i0a][j][z1]+tf;
							tabras[i0a][j][z2]=tabras[i0a][j][z2]+tf;

							tabras[i0b][i][z1]=tabras[i0b][i][z1]+tf;
							tabras[i0b][i][z2]=tabras[i0b][i][z2]+tf;
							tabras[i0b][j][z1]=tabras[i0b][j][z1]+tf;
							tabras[i0b][j][z2]=tabras[i0b][j][z2]+tf;
						
							i0a=kols0+(int)(rpu2-aa2+d2)/(d2);
							i0b=i0a+1;
						
							tabras[i0a][i][z1]=tabras[i0a][i][z1]+tf;
							tabras[i0a][i][z2]=tabras[i0a][i][z2]+tf;
							tabras[i0a][j][z1]=tabras[i0a][j][z1]+tf;
							tabras[i0a][j][z2]=tabras[i0a][j][z2]+tf;

							tabras[i0b][i][z1]=tabras[i0b][i][z1]+tf;
							tabras[i0b][i][z2]=tabras[i0b][i][z2]+tf;
							tabras[i0b][j][z1]=tabras[i0b][j][z1]+tf;
							tabras[i0b][j][z2]=tabras[i0b][j][z2]+tf;
							
						}
						trf1--;
					} while ((rscat[trf1]/strs)>mnmb&& trf1>0);
					trf1=trf-trf1;
					if (mntrf<trf1) mntrf=trf1;
					srtrf=srtrf+trf1;
					sf++;

					int trf2=trf+1;
					if (trf2>=crs) trf2=crs-1;


					do
					{
						double rpu=(stpu-pzug[trf2]);
						if (rpu<0) rpu=rpu+180;
						if (rpu<0) rpu=0;
						if (rpu>180) rpu=180;
						
						double rpu2=(stpu2-pzug[trf2]);
						if (rpu2<0) rpu2=rpu2+180;
						if (rpu2<0) rpu2=0;
						if (rpu2>180) rpu2=180;
						
						int tf=1;//tabpu[(int)rpu];
						int z1=z1cat[trf2];
						int z2=z2cat[trf2];
						int tf2=(int)rpu;
						
						{
						
							tf=1;
							double d2=t5deg/2;
							int i0a=(int)(rpu-aa2+d2)/(d2);
							int i0b=i0a+1;
							if (i0a==75)
							{
								int ui=0;
							}
						
							tabras[i0a][i][z1]=tabras[i0a][i][z1]+tf;
							tabras[i0a][i][z2]=tabras[i0a][i][z2]+tf;
							tabras[i0a][j][z1]=tabras[i0a][j][z1]+tf;
							tabras[i0a][j][z2]=tabras[i0a][j][z2]+tf;

							tabras[i0b][i][z1]=tabras[i0b][i][z1]+tf;
							tabras[i0b][i][z2]=tabras[i0b][i][z2]+tf;
							tabras[i0b][j][z1]=tabras[i0b][j][z1]+tf;
							tabras[i0b][j][z2]=tabras[i0b][j][z2]+tf;
						
						
						
							i0a=kols0+(int)(rpu2-aa2+d2)/(d2);
							i0b=i0a+1;
							if (i0a==75)
							{
								int ui=0;
							}
						
							tabras[i0a][i][z1]=tabras[i0a][i][z1]+tf;
							tabras[i0a][i][z2]=tabras[i0a][i][z2]+tf;
							tabras[i0a][j][z1]=tabras[i0a][j][z1]+tf;
							tabras[i0a][j][z2]=tabras[i0a][j][z2]+tf;

							tabras[i0b][i][z1]=tabras[i0b][i][z1]+tf;
							tabras[i0b][i][z2]=tabras[i0b][i][z2]+tf;
							tabras[i0b][j][z1]=tabras[i0b][j][z1]+tf;
							tabras[i0b][j][z2]=tabras[i0b][j][z2]+tf;
						}
						
						trf2++;
					} while ((rscat[trf2]/strs)<mxmb&& (trf2<crs-1));
					trf2=trf2-trf;
					if (mxtrf<trf2) mxtrf=trf2;
					srtrf=srtrf+trf2;
					sf++;

					
				}
			}
			bool** otzv=new bool*[kols];
			for(int i0=0;i0<kols;i0++)
			{
				otzv[i0]=new bool[t100];
			}
			for(int i0=0;i0<kols;i0++)
			{
				for(int j=0;j<t100;j++)
				{
					otzv[i0][j]=false;
				}
			}
			for(int i0=0;i0<kols; i0++)
			{
				for (i=0;i<t10s;i++)
				{
					int max=-1;
					int maxj;
					for(int j=0;j<t100;j++)
					{
						
						if (tabras[i0][i][j]>max)
						{
							max=tabras[i0][i][j];
							maxj=j;
						}
						if (tabras[i0][i][j]>6)
						{
							int y=0;

						}
						
						tabras[i0][i][j]=0;
					}
					cdtocat[i0][i]=maxj;
					otzv[i0][maxj]=true;
				}
			}
			{
			
				hj=0;
				
				niu++;


				for(i=0;i<t10s;i++)
				{
					for(int j=i+1;j<t10s;j++)
					{
						hj++;
						
						double xn10i=xn10[i];
						double xn10j=xn10[j];
						double yn10i=yn10[i];
						double yn10j=yn10[j];
						
						dx=(xn10i-xn10j)*xscale*kmb;
						dy=(yn10i-yn10j)*yscale*kmb;
						
						double strs=dx*dx+dy*dy;
						
						double stpu=0;
						double stpu2=0;
						if (fabs(dy)>1e-8)
						{
						
							stpu=atan((dx/dy));
							stpu2=atan((-dx/dy));
						} else
						{
							stpu=pi/2;
							stpu2=pi/2;
						};
						stpu=(pi/2+stpu)*180.0/pi;
						stpu2=(pi/2+stpu2)*180.0/pi;
					
						trf=(int)(strs/h);
						if (trf>=kt100) trf=kt100-1;
						trf=reftab[trf];
						int trf1=trf;
						if (trf1<0) trf1=0;
						do
						{
							double rpu=(stpu-pzug[trf1]);
							if (rpu<0) rpu=rpu+180;
							if (rpu<0) rpu=0;
							if (rpu>180) rpu=180;
							
							double rpu2=(stpu2-pzug[trf1]);
							if (rpu2<0) rpu2=rpu2+180;
							if (rpu2<0) rpu2=0;
							if (rpu2>180) rpu2=180;
							
							int z1=z1cat[trf1];
							int z2=z2cat[trf1];
							
							double d2=t5deg/2;
							int i0a=(int)(rpu-aa2+d2)/(d2);
							int i0b=i0a+1;
						
							if (otzv[i0a][z1] && otzv[i0a][z2]&&
								((z1==cdtocat[i0a][i] && z2==cdtocat[i0a][j]) ||
								(z2==cdtocat[i0a][i] && z1==cdtocat[i0a][j])) 
								)
							{
								tabras[i0a][i][z1]=tabras[i0a][i][z1]+1;
								tabras[i0a][i][z2]=tabras[i0a][i][z2]+1;
								tabras[i0a][j][z1]=tabras[i0a][j][z1]+1;
								tabras[i0a][j][z2]=tabras[i0a][j][z2]+1;
							}
							if (otzv[i0b][z1] && otzv[i0b][z2]&&
								((z1==cdtocat[i0b][i] && z2==cdtocat[i0b][j]) ||
								(z2==cdtocat[i0b][i] && z1==cdtocat[i0b][j])) 
								)
							{
								tabras[i0b][i][z1]=tabras[i0b][i][z1]+1;
								tabras[i0b][i][z2]=tabras[i0b][i][z2]+1;
								tabras[i0b][j][z1]=tabras[i0b][j][z1]+1;
								tabras[i0b][j][z2]=tabras[i0b][j][z2]+1;
							}
							
							i0a=kols0+(int)(rpu2-aa2+d2)/(d2);
							i0b=i0a+1;
							
							if (otzv[i0a][z1] && otzv[i0a][z2]&&
								((z1==cdtocat[i0a][i] && z2==cdtocat[i0a][j]) ||
								(z2==cdtocat[i0a][i] && z1==cdtocat[i0a][j])) 
								)
							{
								tabras[i0a][i][z1]=tabras[i0a][i][z1]+1;
								tabras[i0a][i][z2]=tabras[i0a][i][z2]+1;
								tabras[i0a][j][z1]=tabras[i0a][j][z1]+1;
								tabras[i0a][j][z2]=tabras[i0a][j][z2]+1;
							}
							if (otzv[i0b][z1] && otzv[i0b][z2]&&
								((z1==cdtocat[i0b][i] && z2==cdtocat[i0b][j]) ||
								(z2==cdtocat[i0b][i] && z1==cdtocat[i0b][j])) 
								)
							{
								tabras[i0b][i][z1]=tabras[i0b][i][z1]+1;
								tabras[i0b][i][z2]=tabras[i0b][i][z2]+1;
								tabras[i0b][j][z1]=tabras[i0b][j][z1]+1;
								tabras[i0b][j][z2]=tabras[i0b][j][z2]+1;
							}
							
							trf1--;
						} while ((rscat[trf1]/strs)>mnmb&& trf1>0);

						int trf2=trf+1;
						if (trf2>=crs) trf2=crs-1;
						do
						{
							double rpu=(stpu-pzug[trf2]);
							if (rpu<0) rpu=rpu+180;
							if (rpu<0) rpu=0;
							if (rpu>180) rpu=180;
							
							double rpu2=(stpu2-pzug[trf2]);
							if (rpu2<0) rpu2=rpu2+180;
							if (rpu2<0) rpu2=0;
							if (rpu2>180) rpu2=180;
							
							int z1=z1cat[trf2];
							int z2=z2cat[trf2];


							double d2=t5deg/2;
							int i0a=(int)(rpu-aa2+d2)/(d2);
							int i0b=i0a+1;
						
							if (otzv[i0a][z1] && otzv[i0a][z2]&&
								((z1==cdtocat[i0a][i] && z2==cdtocat[i0a][j]) ||
								(z2==cdtocat[i0a][i] && z1==cdtocat[i0a][j])) 
								)
							{
								tabras[i0a][i][z1]=tabras[i0a][i][z1]+1;
								tabras[i0a][i][z2]=tabras[i0a][i][z2]+1;
								tabras[i0a][j][z1]=tabras[i0a][j][z1]+1;
								tabras[i0a][j][z2]=tabras[i0a][j][z2]+1;
							}
							if (otzv[i0b][z1] && otzv[i0b][z2]&&
								((z1==cdtocat[i0b][i] && z2==cdtocat[i0b][j]) ||
								(z2==cdtocat[i0b][i] && z1==cdtocat[i0b][j])) 
								)
							{
								tabras[i0b][i][z1]=tabras[i0b][i][z1]+1;
								tabras[i0b][i][z2]=tabras[i0b][i][z2]+1;
								tabras[i0b][j][z1]=tabras[i0b][j][z1]+1;
								tabras[i0b][j][z2]=tabras[i0b][j][z2]+1;
							}
							
							i0a=kols0+(int)(rpu2-aa2+d2)/(d2);
							i0b=i0a+1;
							
							if (otzv[i0a][z1] && otzv[i0a][z2]&&
								((z1==cdtocat[i0a][i] && z2==cdtocat[i0a][j]) ||
								(z2==cdtocat[i0a][i] && z1==cdtocat[i0a][j])) 
								)
							{
								tabras[i0a][i][z1]=tabras[i0a][i][z1]+1;
								tabras[i0a][i][z2]=tabras[i0a][i][z2]+1;
								tabras[i0a][j][z1]=tabras[i0a][j][z1]+1;
								tabras[i0a][j][z2]=tabras[i0a][j][z2]+1;
							}
							if (otzv[i0b][z1] && otzv[i0b][z2]&&
								((z1==cdtocat[i0b][i] && z2==cdtocat[i0b][j]) ||
								(z2==cdtocat[i0b][i] && z1==cdtocat[i0b][j])) 
								)
							{
								tabras[i0b][i][z1]=tabras[i0b][i][z1]+1;
								tabras[i0b][i][z2]=tabras[i0b][i][z2]+1;
								tabras[i0b][j][z1]=tabras[i0b][j][z1]+1;
								tabras[i0b][j][z2]=tabras[i0b][j][z2]+1;
							}
							
							trf2++;
						} while ((rscat[trf2]/strs)<mxmb&& (trf2<crs-1));
					
					}
				}
			}
			for(int i0=0;i0<kols;i0++)
			{
				delete otzv[i0];
			}
			delete otzv;
			otp=0;
			int maxotp=0;
			int mincd=2000000;
			int maxcd=-2000000;

			for (int i0=0;i0<kols;i0++)
			{
				int shotp=0;
				for (i=0;i<t10s;i++)
				{
					int max=tabras[i0][i][cdtocat[i0][i]];
					
					if (max<3)
					{
						cdtocat[i0][i]=-1;
					} else
					{
						otp++;
						shotp++;
					}
					
				
				
				}
				if (shotp>maxotp)
				{
					maxotp=shotp;
					i0ok=i0;
				}
			}
			
			otp=0;
			

			for (i=0;i<t10s;i++)
			{
				int max=0;
				if (cdtocat[i0ok][i]>=0) max=tabras[i0ok][i][cdtocat[i0ok][i]];
			
				if (max<3)
				{
					cdtocat[i0ok][i]=-1;
					sootv[i]=-1;
				} else
				{
					otp++;
					sootv[i]=cdtocat[i0ok][i];
				}
				
			}
			
			int iskl=0;
			for (i=0;i<t10s;i++)
			{
				if (cdtocat[i0ok][i]<0)
				{
					bops[i]=false;
				} else
				{
					bops[i]=true;
				}
			}
			maxnev=0;
			int maxi=0;
			
			int km;
			
			do
			{
				km=0;
				if (iskl==5)
				{
					int u=0;
				}
				for (i=0;i<t10s;i++)
				{
					if (bops[i])
					{
						double xn10i=xn10[i];
						double yn10i=yn10[i];
						
					
						omnkx.a[km][0]=ksi[cdtocat[i0ok][i]];//bb
						omnkx.a[km][1]=eta[cdtocat[i0ok][i]];//bb
						omnkx.a[km][2]=1;//aa
						omnkx.a[km][3]=xn10i;//bb
						
						
						omnky.a[km][0]=ksi[cdtocat[i0ok][i]];//bb
						omnky.a[km][1]=eta[cdtocat[i0ok][i]];//bb
						omnky.a[km][2]=1;//aa
						omnky.a[km][3]=yn10i;//bb
						km++;
					}
				}
				xfx=omnkx.ptlst(km,3);
				sgmx=omnkx.sgmnk(km,3);
				xfy=omnky.ptlst(km,3);
				sgmy=omnky.sgmnk(km,3);
				maxnev=0;
				maxi=0;
		
				for (i=0;i<t10s;i++)
				{
					if (bops[i])
					{
						double xn10i=xn10[i];
						double yn10i=yn10[i];
						
						
						int cf=cdtocat[i0ok][i];
						double xc=xfx[2]+xfx[0]*ksi[cdtocat[i0ok][i]]
									    +xfx[1]*eta[cdtocat[i0ok][i]];
						double yc=xfy[2]+xfy[0]*ksi[cdtocat[i0ok][i]]
									    +xfy[1]*eta[cdtocat[i0ok][i]];
						double nev=fabs(xc-xn10i);
						double nevx=nev;
						if (nev>maxnev)
						{
							maxnev=nev;
							maxi=i;
						}
						nev=fabs(yc-yn10i);
						if (nev>maxnev)
						{
							maxnev=nev;
							maxi=i;
						}
						
						
			
						
					}
				}
				if (maxnev>porog)
				{
					bops[maxi]=false;
					iskl++;
				}
				
			} while (maxnev>porog&& (km)>3); 
             
			long double xg[3],yg[3];
			for (int iko=0;iko<3;iko++)
			{
				xg[iko]=xfx[iko];
				yg[iko]=xfy[iko];
			};
				
				int km2=0;
				for (i=0;i<t10s;i++)
				{
					if (bops[i])
					{
						
						omnkx.a[km2][0]=xn10[i];//bb
						omnkx.a[km2][1]=yn10[i];//bb
						omnkx.a[km2][2]=1;//aa
						omnkx.a[km2][3]=ksi[cdtocat[i0ok][i]];//bb
						
						
						omnky.a[km2][0]=xn10[i];//bb
						omnky.a[km2][1]=yn10[i];//bb
						omnky.a[km2][2]=1;//aa
						omnky.a[km2][3]=eta[cdtocat[i0ok][i]];//bb
						km2++;
						
						
					}
				}
				long double* xfx2=omnkx.ptlst(km2,3);
				long double* sgmx2=omnkx.sgmnk(km2,3);
				long double* xfy2=omnky.ptlst(km2,3);
				long double* sgmy2=omnky.sgmnk(km2,3);

			double mbx=sqrt(xfx2[0]*xfx2[0]+xfy2[0]*xfy2[0]);
			double mby=sqrt(xfx2[1]*xfx2[1]+xfy2[1]*xfy2[1]);
			
			double otmas=fabs(mbx/mby-1.0);
			
			if (km<=(mnsfi-1)) maxnev=porog+1;
			if (otmas>1.1) maxnev=porog+1;
			if (otmas>1e39 || otmas<-1e39) maxnev=porog+1;
			
			
				double te=(xfx2[0]*xfx2[1]+xfy2[0]*xfy2[1]);
				double tau=(xfx2[0]*xfy2[1]-xfy2[0]*xfx2[1]);
				double g=-te/tau;
				g=180.0*atan(g)/pi;
			if (fabs(g)>5) maxnev=porog+1;
			for (int iko=0;iko<3;iko++)
			{
				xfx[iko]=xg[iko];
				xfy[iko]=yg[iko];
			};
			
			
			inm2++;
		} while (inm2<twinm2 && maxnev>porog);	//distorsija
			inm++;
		} while (inm<2 && maxnev>porog);

		
		
		imb2++;
	} while (imb2<twk100 && maxnev>porog);
		imb++;
	} while (imb<tw100 && maxnev>porog);
	
			if (maxnev<porog)
			{
				int hj=0;
			} else
			{
				for (i=0;i<t10s;i++)
				{
					sootv[i]=-1;
				}
			}
	delete bops;
	delete rscat;
	delete pzug;
	delete z1cat;
	delete z2cat;
	delete reftab;
	delete maxn10;
	for(int i0=0;i0<kols;i0++)
	{
			delete cdtocat[i0];
	}
	delete cdtocat;

	for(int i0=0;i0<kols;i0++)
	{
		
		for(int i=0;i<bt10s;i++)
		{
			delete tabras[i0][i];
		}
		delete tabras[i0];
	}
	delete tabras;
	
	
	return 1;
}

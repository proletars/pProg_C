#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <gsl/gsl_integration.h>
#include "Integrator.h"
#include "Monte_Carlo.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include "QR.h"
#include "QR_ls.h"
#define RND (double)rand()/RAND_MAX
#define FMT "%7.6f" //format of print "7 width, 3 digits after comma" 

double funs2(int i, double x){
   switch(i){
   case 0: return 1/sqrt(x); break;
   //case 1: return 1; break;
   default: {fprintf(stderr,"funs: wrong i:%d",i); return NAN;}
   }
}

int main(void) {
	// part A 
	printf("A. Plain Monte Carlo integration\n\n");
	printf("Integrating dr dphi dtheta r^2 from 0 to 1, 0 to 2pi and 0 to pi\n");
	
	int dim=3, N=1e6; double a[3]={0,0,0}, b[3]={1,2*M_PI,M_PI},result, error;
	printf("Number of points: %i\n",N);
	double f1(double* x){
		return x[0]*x[0];
		
	}

	plainmc(dim,a,b,f1,N,&result,&error);

	printf("Result: %g, correct answer is: %g\n",result,2*M_PI*M_PI/3);
	printf("Error: %g \n\n",error);

	printf("Integrating dx dy dz dq x*y^2*z^3*q^4 from 0 to 1, 1 to 2 and 2 to 3 and 3 to 4\n");
	
	dim=4; N=1e7; double a2[4]={0,1,2,3}, b2[4]={1,2,3,4};
	printf("Number of points: %i\n",N);
	double f2(double* x){
		return x[0]*x[1]*x[1]*x[2]*x[2]*x[2]*x[3]*x[3]*x[3]*x[3];
	}

	plainmc(dim,a2,b2,f2,N,&result,&error);

	printf("Result: %g, correct answer is: %g\n",result,71071.0/24.0);
	printf("Error: %g\n\n",error);

	printf("Integrating dx dy dz (1-cos(x)cos(y)cos(z))^-1/pi^3 from 0 to pi, 0 to pi and 0 to pi\n");
	
	dim=3; N=1e7; double a3[3]={0,0,0}, b3[3]={M_PI,M_PI,M_PI};
	printf("Number of points: %i\n",N);
	double f3(double* x){
		return 1/(1-cos(x[0])*cos(x[1])*cos(x[2]))/(M_PI*M_PI*M_PI);
	}

	plainmc(dim,a3,b3,f3,N,&result,&error);

	printf("Result: %g, correct answer is: %7.6f \n",result,1.3932039296856768591842462603255);
	printf("Error: %g\n\n",error);


	// part B
	int max_N=500, delta_N=5, start_N=10, i=0; dim=3;
	int n=(max_N-start_N)/delta_N+1, m=1;
	gsl_vector* x = gsl_vector_alloc(n);
	gsl_vector* y = gsl_vector_alloc(n);
	gsl_vector* dy = gsl_vector_alloc(n);
	gsl_vector* c = gsl_vector_alloc(m);
	gsl_matrix* COV = gsl_matrix_alloc(m, m);

	
	FILE* errordata =fopen("error_data.txt","w+");
	for(N=start_N;N<=max_N;N+=delta_N){
		plainmc(dim,a,b,f1,N,&result,&error);
		fprintf(errordata,"%i\t%g\n",N,error);
		gsl_vector_set(x,i,N);
		gsl_vector_set(y,i,error);
		gsl_vector_set(dy,i,1.0);
		i++;
	}

	fclose(errordata);

	

	least_squares(x,y,dy,m,funs2,c,COV);

	printf("B. Check that error behaves as O(1/√N)\n\n");	
	printf("Testing on the first function from previous ex.\n");
	printf("Fit coefficient found:%g\n",gsl_vector_get(c,0));
	printf("As one can see on the plot.svg the error follows very nicely\n"
		"the 1/sqrt(N) prediction. There is of course more uncertainty from that\n"
		"prediction for low N because here the randomness component of choosing the\n"
		"points is more important\n\n");
	FILE* errorfit = fopen("errorfit_data.txt","w+");
	delta_N=2;
	for(N=1;N<max_N;N+=delta_N){
		fprintf(errorfit,"%i\t%g\n",N,gsl_vector_get(c,0)*1/sqrt(N));
	}
	fclose(errorfit);
	




	// part C
	printf("C. 2D adaptive integrator\n\n");

	double c_lim(double x){
		return x;
	}
	double d_lim(double x){
		return x*x*x;
	}
	double a4=1.0, b4=2.0;
	// integrals and solutions found on http://tutorial.math.lamar.edu/Classes/CalcIII/DIGeneralRegion.aspx#MultInt_GenReg_Ex1a
	double two_D_function(double x,double y){
		return exp(y/x);
	}
	double two_D_function2(double x,double y){
		return 4*x*y-y*y*y;
	}
	double c_lim2(double x){
		return x*x*x;
	}
	double d_lim2(double x){
		return sqrt(x);
	}
	double acc=1e-6, eps=1e-6; 
	result = adapt_2D(two_D_function,c_lim,d_lim,a4,b4,acc,eps);
	double theo = 1.0/2.0*exp(4.0)-2.0*M_E;
	printf("Integrating dx dy exp(y/x) from 0 to 1 and for x <=y<=x^3\n");
	printf("Absolute accuracy: %g, relative accuracy: %g\n",acc,eps);
	printf("Theoretical result: %7.10f, calculated:%7.10f\n\n",theo,result);

	a4=0; b4=1;
	acc=1e-6; eps=1e-6; 
	result = adapt_2D(two_D_function2,c_lim2,d_lim2,a4,b4,acc,eps);
	theo = 55.0/156.0;
	printf("Integrating dx dy 4xy-y^3 from 0 to 1 and for x^3 <=y<=sqrt(x)\n");
	printf("Absolute accuracy: %g, relative accuracy: %g\n",acc,eps);
	printf("Theoretical result: %7.10f, calculated:%7.10f\n\n",theo,result);

	// D

	printf("D. 3D adaptive integrator\n\n");
	// Integral found at http://tutorial.math.lamar.edu/Classes/CalcIII/TripleIntegrals.aspx
	double three_D_function(double x,double y,double z){
		return 8*x*y*z;
	}
	double ax=1, bx=2, ay=2, by=3, az=0, bz=1;
	acc=1e-6; eps=1e-6; 
	result= adapt_3D(three_D_function,ax,bx,ay,by,az,bz,acc,eps);
	theo = 15.0;
	printf("Integrating dx dy dz dz 8*x*y*z from 0 to 1, 1 to 2, 2 to 3\n");
	printf("Absolute accuracy: %g, relative accuracy: %g\n",acc,eps);
	printf("Theoretical result: %7.10f, calculated:%7.10f\n\n",theo,result);


	ax=0, bx=1, ay=0, by=2*M_PI, az=0, bz=M_PI;
	acc=1e-6; eps=1e-6; 
	double f1_3D(double r,double phi, double theta){
		return r*r;
		
	}
	result= adapt_3D(f1_3D,ax,bx,ay,by,az,bz,acc,eps);
	theo = 2*M_PI*M_PI/3;
	printf("Integrating dr dphi dtheta r^2 from 0 to 1, 0 to 2pi and 0 to pi as in function 1\n");
	printf("Absolute accuracy: %g, relative accuracy: %g\n",acc,eps);
	printf("Theoretical result: %7.10f, calculated:%7.10f\n\n",theo,result);

	ax=0, bx=M_PI, ay=0, by=M_PI, az=0, bz=M_PI;
	acc=1e-3; eps=1e-4; 
	double f3_3D(double x, double y, double z){
		return 1/(1-cos(x)*cos(y)*cos(z))/(M_PI*M_PI*M_PI);
	}
	result= adapt_3D(f3_3D,ax,bx,ay,by,az,bz,acc,eps);
	theo = 1.3932039296856768591842462603255;
	printf("Integrating dx dy dz (1-cos(x)cos(y)cos(z))^-1/pi^3 from 0 to pi, 0 to pi and 0 to pi as in function 3\n");
	printf("Absolute accuracy: %g, relative accuracy: %g\n",acc,eps);
	printf("Theoretical result: %7.10f, calculated:%7.10f\n\n",theo,result);

	gsl_vector_free(x);
	gsl_vector_free(y);
	gsl_vector_free(dy);
	gsl_matrix_free(COV);
	gsl_vector_free(c);
	return EXIT_SUCCESS;
}
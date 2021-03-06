#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

/*binarysearch: works by checking wether the number is bigger
or smaller than the middle number, and repeating the same in
the list of smaller or bigger numbers until one finds the right one
it therefore takes O(log(n)) time if n is size of list.
*/
int search1(double *x, double z) {
	int i=0,j=sizeof(x)-1;
	//binarysearch:
	while(j-i>1){int m=(i+j)/2;if(x[m]) i=m;else j=m;}
	return i;
}

/* Taken from Dmitri Fedorovs Interpolation chapter */

double linterp(int n, double* x, double* y, double z){
	// first check that n and z have the legal values
	assert(n>1 && z>=x[0] && z<=x[n-1]);
	int i =0, j=n-1; 

	//binary explained above
	while (j-i > 1){ 
		int m=(i+j)/2; 

		if ( z>x[m]) i=m; else j=m;}
	//Here is the evaluation at point z. just used (1.5) from the lecture notes
	double result = y[i]+(y[i+1]-y[i])/(x[i+1]-x[i])*(z-x[i]);
	
	return result;
}

double linterp_integ(int n, double *x, double *y, double z){
	assert(n>1 && z>=x[0] && z<=x[n-1]);
	
	double integral=0.0;

	int i_z =0, j_z=n-1; 

	/*binarysearch: */
	while (j_z-i_z > 1){ 
		int m=(i_z+j_z)/2; 

		if ( z>x[m]) i_z=m; else j_z=m;}
	
	/* Integral from 0 to x[i] before the z*/
	
	for(int i=0; i<i_z; i++) {
		int j=i+1;

		integral += y[i]*(x[j]-x[i])+(y[j]-y[i])/(x[j]-x[i])*\
			1/2*(x[j]-x[i])*(x[j]-x[i]); 
	}

	/* Integral from x[i] before z until z*/
	
	integral += y[i_z]*(z-x[i_z])+(y[j_z]-y[i_z])/(x[j_z]-x[i_z])*\
			1/2*(z-x[i_z])*(z-x[i_z]); 
	return integral;
}

double linterp_der(int n, double *x, double *y, double z) {
	assert(n>1 && z>=x[0] && z<=x[n-1]);
	int i =0, j=n-1; 

	/*binarysearch: */
	while (j-i > 1){ 
		int m=(i+j)/2; 

		if ( z>x[m]) i=m; else j=m;}

	double result = (y[i+1]-y[i])/(x[i+1]-x[i]);
	
	return result;	
}

/* function used is cosine from 0 to 2pi using 11 points */
int main1() {

	const int n = 11;  
	double x[11] = {0.0, 2*M_PI/20*2, 2*M_PI/20*4, \
		2*M_PI/20*6, 2*M_PI/20*8, \
		2*M_PI/20*10, 2*M_PI/20*12,  \
		2*M_PI/20*14, 2*M_PI/20*16, \
		2*M_PI/20*18, 2*M_PI/20*20};
	double y[11] = {1.0, 0.80901,  0.30901, \
		 -0.30901,   -0.80901,   -1.0, \
		   -0.80901,    -0.30901,  \
		 0.30901, 0.80901,  1.0}; 

	FILE* out=fopen("out1.txt","w+");
	
	fprintf(out,"\n");
	fprintf(out,"Testing if linear spline works. \n");
	fprintf(out,"Testfunction is cos(x) from 0 to 2pi. \n");
	fprintf(out,"Data for plotting the linearspline is found in lineardata.txt \n");
	
	FILE *lineardata = fopen("lineardata.txt", "w+");

	double delta_x = 2*M_PI/100, x_start = 0.0, x_slut=2*M_PI, x_it;
	double spline_result;
	for(x_it = x_start; x_it <x_slut; x_it += delta_x) {
		spline_result = linterp(n, x, y, x_it);
		fprintf(lineardata, "%g \t %g \n", x_it, spline_result);
	}

	fclose(lineardata);

	FILE *data_points  =fopen("data_points.txt", "w+");
	for(int i=0; i<n; i++) {
		fprintf(data_points, "%g \t %g\n", x[i], y[i]);
	}
	fclose(data_points);

	fprintf(out,"Integral of cos(x) from 0 to pi/2 is 1 analytically \n");

	double integ_linear  = linterp_integ(n, x, y, M_PI/2);
	fprintf(out,"Using linear splines I get %g\n", integ_linear);

	fprintf(out,"The theoretical derivative at point pi/2 is -1\n");

	double der_linear  = linterp_der(n, x, y, M_PI/2);
	fprintf(out,"Using linear splines I get %g \n", der_linear); 
	fprintf(out,"\n");
	fclose(out);
	return EXIT_SUCCESS;
}

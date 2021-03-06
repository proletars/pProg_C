#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "Integrator.h"
#include <assert.h>

/* These methods are taken from Dmitri Fedorovs lecture notes 
 It works by taking in the function, integration limits a and b, the accuracies, 
 and the previous calculated values of the function at x2 and x3. It outputs
 the value of the integral evaluated on the limits within the tolerances, and if 
 it is not within toleranced it recures until it has found the answer. 
 */
double adapt24(double f(double),double old_f(double),double a,double b,
	double acc,double eps,double f2,double f3,int nrec){
	// upper limit on how many recursions are possible until if will just accept the error
	assert(nrec<1000000);
	//Rescaling to the original interval point x1 and x4 and finding the function value
	double f1=f(a+(b-a)/6),f4=f(a+5*(b-a)/6);
	// calculate integral according to Q, the trapez method and q the rectangular method
	// according to (8.49) and (8.50). The weights have also been rescaled by (b-a).
	double Q=(2*f1+f2+f3+2*f4)/6*(b-a),q=(f1+f4+f2+f3)/4*(b-a);
	// calculate the tolerance and error according to (8.46), (8.47)
	double tolerance=acc+eps*fabs(Q),error=fabs(Q-q);
	// See if the error is acceptable then can return the estimation of the integral
	// of the subinterval
	if(error<tolerance)return Q;
	// if it isn't acceptable, subdivide the interval into to equally big intervals
	// update the absolute tolerance and try again on each new subinterval. This is the recursion
	// part
	else{
		double Q1=adapt24(f,old_f,a,(a+b)/2,acc/sqrt(2.),eps,f1,f2,nrec+1);
		double Q2=adapt24(f,old_f,(a+b)/2,b,acc/sqrt(2.),eps,f3,f4,nrec+1);
	// return the sum of the integral of both subintervals
	return Q1+Q2;}
}

// The first step of calculating the function values at f2 and f3. This has been changed to take infinite values
// by using variable transformation
double adapt(
	double old_f(double),double a_old,double b_old,double acc,double eps){
	// Rescaling the points x2 and x3 and finding the function values
	int nrec=0;
	double f2, f3, x, result, a,b;

	if(isinf(a_old)!=0){
		if(isinf(b_old)!=0){
			double f_inf_inf(double t) {
				x=t/(1-t*t);
				result=old_f(x)*(1+t*t)/((1-t*t)*(1-t*t));
				return result;
			}
			a=-1; b=1;
			f2=f_inf_inf(a+2*(b-a)/6);
			f3=f_inf_inf(a+4*(b-a)/6);
			return adapt24(f_inf_inf,old_f,a,b,acc,eps,f2,f3,nrec);
		}
		else{

			double f_inf_b(double t) {
				x=t/(b_old-(1-t)/t);
				result=old_f(x)/(t*t);
				return result;
			}
			a=0; b=1;
			f2=f_inf_b(a+2*(b-a)/6);
			f3=f_inf_b(a+4*(b-a)/6);
			return adapt24(f_inf_b,old_f,a,b,acc,eps,f2,f3,nrec);

		}
	}

	else if(isinf(b_old)!=0){
		double f_a_inf(double t) {
				x=a_old+(1-t)/t;
				result=old_f(x)/(t*t);
				return result;
			}
			a=0; b=1;
			f2=f_a_inf(a+2*(b-a)/6);
			f3=f_a_inf(a+4*(b-a)/6);
			return adapt24(f_a_inf,old_f,a,b,acc,eps,f2,f3,nrec);
	}
	else{
		a=a_old; b=b_old;
		f2=old_f(a+2*(b-a)/6);
		f3=old_f(a+4*(b-a)/6);
		return adapt24(old_f,old_f,a,b,acc,eps,f2,f3,nrec);}
	
}

double clenshaw_curtis(double old_f(double),double a_old,double b_old,double acc,double eps){
	int nrec=0;
	double f2, f3,a,b;
	double f_clenshaw_curtis(double t) {
				// Here the terms are more complicated because of rescaling from
				// -1 to 1 to 0 to 1.
				double x=(a_old+b_old)/2+(a_old-b_old)/2*cos(t);
				double result=old_f(x)*sin(t)*(b_old-a_old)/2;
				return result;
			}
	a=0; b=M_PI;
	f2=f_clenshaw_curtis(a+2*(b-a)/6);
	f3=f_clenshaw_curtis(a+4*(b-a)/6);
	double res=adapt24(f_clenshaw_curtis,old_f,a,b,acc,eps,f2,f3,nrec);
	return res;
}

/***** Solves: Unsteady, compressible convection-diffusion problems.

****** Description:
****** This program solves unsteady convection-diffusion problems	
****** using the transient simple algorithm described in ch. 8.7.1 in "Computational 
****** Fluid Dynamics" by H.K. Versteeg and W. Malalasekera. Symbols and
****** variables follow exactly the notations in this reference, and all 
****** equations cited are from this reference unless mentioned otherwise.

****** References: 1. Computational Fluid Dynamics, H.K. Versteeg and W. 
******			    Malalasekera, Longman Group Ltd, 1995
******/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "variables.h"
#include "constants.h"
#include "functions.h"

//bool bluffbody (int I, int J)
//{
//
//	return (I>A && I<B && J>C && J<D);
//}


/* ################################################################# */
int main(int argc, char *argv[])
/* ################################################################# */
{
	int    iter_u, iter_v, iter_pc, iter_T, iter_eps, iter_k;
	double du, dv, time, TOTAL_TIME = 5;
	
	init();
	bound(); /* apply boundary conditions */
//	triang();
	
	for (time = Dt; time <= TOTAL_TIME; time += Dt) {
		iter = 0; 
		while (iter < MAX_ITER && SMAX > SMAXneeded && SAVG > SAVGneeded) { /* outer iteration loop */
			
			
			derivatives();
			ucoeff(aE, aW, aN, aS, aP, b);
			for (iter_u = 0; iter_u < U_ITER; iter_u++)
				solve(u, b, aE, aW, aN, aS, aP);

			vcoeff(aE, aW, aN, aS, aP, b);
			for (iter_v = 0; iter_v < V_ITER; iter_v++)
				solve(v, b, aE, aW, aN, aS, aP);

			bound();
			pccoeff(aE, aW, aN, aS, aP, b);
			for (iter_pc = 0; iter_pc < PC_ITER; iter_pc++)
				solve(pc, b, aE, aW, aN, aS, aP);

			velcorr(); /* Correct pressure and velocity */

			kcoeff(aE, aW, aN, aS, aP, b);
			for (iter_k = 0; iter_k < K_ITER; iter_k++)
				solve(k, b, aE, aW, aN, aS, aP);

			epscoeff(aE, aW, aN, aS, aP, b);
			for (iter_eps = 0; iter_eps < EPS_ITER; iter_eps++)
				solve(eps, b, aE, aW, aN, aS, aP);

			Tcoeff(aE, aW, aN, aS, aP, b);
			for (iter_T = 0; iter_T < T_ITER; iter_T++)
				solve(T, b, aE, aW, aN, aS, aP);

			viscosity();
			thermal_diffusivity();

			bound();
			storeresults(); /* Store data at current time level in arrays for "old" data*/

			iter++;
		} /* for outer iteration loop */

		printConv(time,iter); /* print convergence to the screen */

		/* reset SMAX and SAVG */
		SMAX = LARGE;
		SAVG = LARGE;

	} /* for Dt */
	output();

	return 0;

} /* main */

/* ################################################################# */
//void triang(void)
///* ################################################################# */
//{
////	grid();
//	
//	int    I, J, i, j, N, a;
//	double Dx, Dy, TRX[21], TRY[21], tresh;
//	double Triangle_x[21] = {4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,3.6,3.2,2.8,2.4,2.0,2.4,2.8,3.2,3.6,4.0};
//	double Triangle_y[21]  = {0.0, 0.004, 0.008, 0.012, 0.016, 0.020, 0.024, 0.028, 0.032, 0.036, 0.040, 0.036, 0.032, 0.028, 0.024, 0.020, 0.016, 0.012, 0.008, 0.004, 0.0};
//	
//	for (N = 0; N <= 20; N++){
//		tresh = Triangle_x[N];
//		a=0;
//		for(I = 0; I <= NPI+1; I++){
//		
//			if (x[I]>=tresh && a <=1){
//				TRX[N] = I;
//				printf("TRX is: %e\n",TRX[N]);
//				break;	
//			}				
//		}		
//	} /* for I */
//	
//	for (N = 0; N <= 20; N++){
//		tresh = Triangle_y[N]+YMAX/2;
//		a=0;
//		for(J = 0; J <= NPJ+1; J++){
//		
//			if (y[J]>=tresh && a <=1){
//				TRY[N]=J;
//				printf("TRY is: %e\n",TRY[N]);
//				break;	
//			}				
//		}		
//	} /* for J */
//
//} /* triang */

/* ################################################################# */
void grid(void)
/* ################################################################# */
{
/***** Purpose: Defining the geometrical variables ******/
/*****          See fig. 6.2-6.4 in ref. 1 ******/
	int    I, J, i, j;
	double Dx, Dy, TRX, TRY;

	/* Length of volume element */

	Dx = XMAX/NPI;
	Dy = YMAX/NPJ;


	/* Length variable for the scalar points in the x direction */

	x[0] = 0.;
	x[1] = 0.5*Dx;

	for (I = 2; I <= NPI; I++)
		x[I] = x[I-1] + Dx;

	x[NPI+1] = x[NPI] + 0.5*Dx;

	/* Length variable for the scalar points fi[i][j] in the y direction */

	y[0] = 0.;
	y[1] = 0.5*Dy;

	for (J = 2; J <= NPJ; J++)
		y[J] = y[J-1] + Dy;

	y[NPJ+1] = y[NPJ] + 0.5*Dy;

	/* Length variable for the velocity components u[i][j] in the x direction */

	x_u[0] = 0.;
	x_u[1] = 0.;

	for (i = 2; i <= NPI + 1; i++)
		x_u[i] = x_u[i-1] + Dx;

	/* Length variable for the velocity components v[i][j] in the y direction */

	y_v[0] = 0.;
	y_v[1] = 0.;
	for (j = 2; j <= NPJ + 1; j++)
		y_v[j] = y_v[j-1] + Dy;

} /* grid */

/* ################################################################# */
void init(void)
/* ################################################################# */
{
/***** Purpose: To initilise all parameters. ******/
	int    I, J, i, j;

	memalloc();
	grid();

	/* Initialising all variables  */

	omega = 1.0; /* Over-relaxation factor for SOR solver */

	/* Initialize convergence parameters at large values */

	SMAX = LARGE;
	SAVG = LARGE;

	m_in  = 1.;
	m_out = 1.;
	Dt    = 1.E-1;

	for (I = 0; I <= NPI + 1; I++) {
		i = I;
		
		for(J = 0; J<= (NPJ/2); J++){
			T[I][J] = 1.;		
			u      [i][J] = U_IN*pow(y[J]/(YMAX/2),.143);     /* Velocity in x-direction */
			}
		for(J>(NPJ/2);J<=NPJ;J++){	
			u 		[i][J] = U_IN*pow(2-y[J]/(YMAX/2),.143);
			T[I][J] = 0;		
		}
		for (J = 0; J <= NPJ + 1; J++) {
			j = J;
			v      [I][j] = 0.;       /* Velocity in y-direction */
			p      [I][J] = 0.;       /* Relative pressure */
//			T      [I][J] = 0.;     /* Temperature */
			k      [I][J] = 1e-3;     /* k */
			eps    [I][J] = 1e-4;     /* epsilon */
			uplus  [I][J] = 1.;                                            /* uplus */
			yplus1 [I][J] = sqrt(rho[I][J] * u[I][J] / mu[I][J]) * (y[1] - y[0]);   /* yplus1 */
			yplus2 [I][J] = sqrt(rho[I][J] * u[I][J] / mu[I][J]) * (y[NPJ+1] - y[NPJ]);   /* yplus2 */
			yplus  [I][J] = 1.;                                            /* yplus*/
			tw     [I][J] = 5.;                                                /* tw */
			vplus  [I][J] = 1.;                                            /* vplus */
			xplus1 [I][J] = sqrt(rho[I][J] * v[I][J] / mu[I][J]) * (x[1] - x[0]);   /* xplus1 */
			xplus2 [I][J] = sqrt(rho[I][J] * v[I][J] / mu[I][J]) * (x[NPI+1] - x[NPI]);   /* xplus2 */
			xplus  [I][J] = 1.;                                            /* xplus*/
			twx    [I][J] = 5.;                                                /* twx */
			rho    [I][J] = 1.0;      /* Density */
			mu     [I][J] = 2.E-5;    /* Viscosity */
			Cp     [I][J] = 1013.;     /* J/(K*kg) Heat capacity - assumed constant for this problem */
			Gamma  [I][J] = 0.025/Cp[I][J]; /* Thermal conductivity divided by heat capacity */

			u_old  [i][J] = u[i][J];  /* Velocity in x-direction old timestep */
			v_old  [I][j] = v[I][j];  /* Velocity in y-direction old timestep */
			pc_old [I][J] = pc[I][J]; /* Pressure correction old timestep */
			T_old  [I][J] = T[I][J];  /* Temperature old timestep */
			eps_old[I][J] = eps[I][J];  /* epsilon old timestep*/
			k_old  [I][J] = k[I][J];    /* k old timestep*/
		} /* for J */
	} /* for I */

	/* Setting the relaxation parameters */

	relax_u   = 0.3;             /* See eq. 6.36 */
	relax_v   = relax_u;       /* See eq. 6.37 */
	relax_pc  = 0.2;//1.1 - relax_u; /* See eq. 6.33 */
	relax_T   = 0.3;  /* Relaxation factor for temperature */
	relax_eps   = 0.1;  /* Relaxation factor for epsilon */
	relax_k   = 0.1; /* Relaxation factor for k */


} /* init */

/* ################################################################# */
void bound(void)
/* ################################################################# */
{
/***** Purpose: Specify boundary conditions for a calculation ******/
	int    I, J, j;

	/* Fixed temperature at the upper and lower wall */

		for(J = 0; J<= (NPJ/2); J++){	
			T[1][J]	= 1.;
			u [1][J] = U_IN*pow(y[J]/(YMAX/2),.143);     /* Velocity in x-direction */
			}
		for(J>(NPJ/2);J<=NPJ;J++){	
			T[1][J]=0.;
			u [1][J] = U_IN*pow(2-y[J]/(YMAX/2),.143);
		}

		
//	for (I = 0; I <= NPI + 1; I++) {
//		/* Temperature at the walls in Kelvin */
//		T[I][0] = 273.; /* bottom wall */
//		T[I][NPJ+1] = 273.; /* top wall */
//	} /* for J */
	

	globcont();

	/* Velocity and temperature gradient at outlet = zero: */

	for (J = 1; J <= NPJ; J++) {
		j = J;
		/* Correction factor m_in/m_out is used to satisfy global continuity */
		u[NPI+1][J] = u[NPI][J]*m_in/m_out;
		v[NPI+1][j] = v[NPI][j];
		k[NPI+1][J] = k[NPI][J];
		eps[NPI+1][J] = eps[NPI][J];
	} /* for J */

	for (J = 0; J <= NPJ+1; J++) {
		T[NPI+1][J] = T[NPI][J];
	} /* for J */
	
//	for (I = 0; I <= NPI + 1; I++) {
//		for(J=0; J<=NPJ; J++){
//			if(I>=A && I <= B && J >= C && J <= D){
//			T[I][J]     = 700.; /* Temperature in Kelvin */
//			}
//		}
//	} /* for I */

	for (J = 0; J <= NPJ + 1; J++) {
		k[1][J] = 2./3.*sqr(U_IN*Ti); /* inlet */
		eps[1][J] = pow(Cmu,0.75)*pow(k[1][J],1.5)/(0.07*YMAX*0.5); /* inlet */
	} /* for J */

} /* bound */

/* ################################################################# */
void globcont(void)
/* ################################################################# */
{
/***** Purpose: Calculate mass in and out of the calculation domain to ******/
/*****          correct for the continuity at outlet. ******/
	int    J, j;
	double AREAw;

	conv();

	m_in = 0.;
	m_out = 0.;

	for (J = 1; J <= NPJ; J++) {
		j = J;
		AREAw = y_v[j+1] - y_v[j]; /* See fig. 6.3 */
		m_in  += F_u[1][J]*AREAw;
		m_out += F_u[NPI][J]*AREAw;
	} /* for J */

} /* globcont */

/* ################################################################# */
void derivatives(void)
/* ################################################################# */
{
/***** Purpose: To calculate derivatives ******/
	int    I, J, i, j;
	
	for (I = 1; I <= NPI; I++) {
		i = I;
		for (J = 1; J <= NPJ; J++) {
			j = J;
			dudx[I][J] = (u[i+1][J] - u[i][J])   / (x_u[i+1] - x_u[i]);
			dudy[i][j] = (u[i][J]   - u[i][J-1]) / (y[J]     - y[J-1]);
			dvdx[i][j] = (v[I][j]   - v[I-1][j]) / (x[I]     - x[I-1]);
			dvdy[I][J] = (v[I][j+1] - v[I][j])   / (y_v[j+1] - y_v[j]);
			E [i][j] = sqr(dudy[i][j]) + sqr(dvdx[i][j]) + 2 * dudy[i][j] * dvdx[i][j];
         } /* for J */
   }  /* for I */                 

	for (I = 1; I <= NPI; I++) {
		i = I;
		for (J = 1; J <= NPJ; J++) {
			j = J;
			E2[I][J] = sqr(dudx[I][J]) + sqr(dvdy[I][J]) + 0.25*(E[i][j] + E[i+1][j] + E[i][j+1] + E[i+1][j+1]);
         } /* for J */
   }  /* for I */                 

} /* derivatives */

/* ################################################################# */
void solve(double **fi, double **b, double **aE, double **aW, double **aN, double **aS, double **aP)
/* ################################################################# */
{
/***** Purpose: To solve the algebraic equation 7.7. ******/
	int    I, J, space;
	double *Ari, *Cmri, Cri;

/* TDMA along a horizontal row from west to east. equation to solve: */

	/* - aW*fiW + aP*fiP - aE*fiE = aS*fiS + aN*fiN + b */

	/* equivalences with variables in eq. 7.1-7.6: */
	/* BETA = aW[I][J] Def. in eq. 7.2 */
	/* D    = aP[I][J] Def. in eq. 7.2 */
	/* ALFA = aE[I][J] Def. in eq. 7.2 */
	/* A    = Ari[I]	 Def. in eq. 7.6b */
	/* C    = Cri	 The right side assumed temporarily known (see eq. 7.8) */
	/* C�   = Cmri[I]  Def. in eq. 7.6c */
	/* b    = b[I][J]	 Def. in eq. 7.7 */

	space = max2((Iend - Istart + 3),(Jend - Jstart + 3));
	Ari   = double_1D_array(space);
	Cmri  = double_1D_array(space);

	/* Solving the (e-w) lines from the south */

	for (J = Jstart; J <= Jend; J++) {
		/* At the inlet boundary: */
		Ari [Istart-1] = 0;
		Cmri[Istart-1] = fi[Istart-1][J];

		for (I = Istart; I <= Iend; I++) { /* Forward substitution */
			Ari[I]  = aE[I][J]/(aP[I][J] - aW[I][J]*Ari[I-1]); /* eq. 7.6b */
			Cri     = aN[I][J]*fi[I][J+1] + aS[I][J]*fi[I][J-1] + b[I][J];
			Cmri[I] = (aW[I][J]*Cmri[I-1] + Cri)/(aP[I][J] - aW[I][J]*Ari[I-1]); /* eq. 7.6c */
		}

		for (I = Iend; I >= Istart; I--)  /* Back substitution */
			fi[I][J] = Ari[I]*fi[I+1][J] + Cmri[I]; /* eq. 7.6a */
	} /* for J from south */

	/* Solving the (e-w) lines from the north */

	for (J = Jend-1; J >= Jstart; J--) {
		/* At the inlet boundary: */
		Ari [Istart-1] = 0;
		Cmri[Istart-1] = fi[Istart-1][J];

		for (I = Istart; I <= Iend; I++) { /* Forward substitution */
			Ari[I]  = aE[I][J]/(aP[I][J] - aW[I][J]*Ari[I-1]); /* eq. 7.6b */
			Cri     = aN[I][J]*fi[I][J+1] + aS[I][J]*fi[I][J-1] + b[I][J];
			Cmri[I] = (aW[I][J]*Cmri[I-1] + Cri)/(aP[I][J] - aW[I][J]*Ari[I-1]);  /* eq. 7.6c */
		}

		for (I = Iend; I >= Istart; I--)  /* Back substitution */
			fi[I][J] = Ari[I]*fi[I+1][J] + Cmri[I]; /* eq. 7.6a */
	} /* for J from north */

/* TDMA along a vertical column from south to north. equation to solve: */

	/* - aS*fiW + aP*fiP - aN*fiE = aW*fiS + aE*fiN + b (eq. 7.8) */

	/* equivalences with variables in eq. 7.1-7.6: */
	/* BETA = aS[I][J] Def. in eq. 7.2 */
	/* D    = aP[I][J] Def. in eq. 7.2 */
	/* ALFA = aN[I][J] Def. in eq. 7.2 */
	/* A    = Ari[I]	 Def. in eq. 7.6b */
	/* C    = Cri      The right side assumed temporarily known (see eq. 7.8) */
	/* C�   = Cmri[I]  Def. in eq. 7.6c */
	/* b    = b[I][J]	 Def. in eq. 7.7 */

	/* Solving (n-s) lines from the west */

	for (I = Istart; I <= Iend; I++) {
		/* At the bottom boundary: */
		Ari[Jstart-1] = 0;
		Cmri[Jstart-1] = fi[I][Jstart-1];

		for (J = Jstart; J <= Jend; J++) { /* Forward substitution */
			Ari[J]  = aN[I][J]/(aP[I][J] - aS[I][J]*Ari[J-1]); /* eq. 7.6b */
			Cri     = aE[I][J]*fi[I+1][J] + aW[I][J]*fi[I-1][J] + b[I][J];
			Cmri[J] = (aS[I][J]*Cmri[J-1] + Cri)/(aP[I][J] - aS[I][J]*Ari[J-1]); /* eq. 7.6c */
		}

		for (J = Jend; J >= Jstart; J--) /* Back substitution */
			fi[I][J] = Ari[J]*fi[I][J+1] + Cmri[J]; /* eq. 7.6a */
	} /* for I from west */

	/* Solving (n-s) lines from the east */

	for (I = Iend - 1; I >= Istart; I--) {
		/* At the bottom boundary: */
		Ari[Jstart-1] = 0;
		Cmri[Jstart-1] = fi[I][Jstart-1];

		for (J = Jstart; J <= Jend; J++) { /* Forward substitution */
			Ari[J]  = aN[I][J]/(aP[I][J] - aS[I][J]*Ari[J-1]); /* eq. 7.6b */
			Cri     = aE[I][J]*fi[I+1][J] + aW[I][J]*fi[I-1][J] + b[I][J];
			Cmri[J] = (aS[I][J]*Cmri[J-1] + Cri)/(aP[I][J] - aS[I][J]*Ari[J-1]); /* eq. 7.6c */
		}

		for (J = Jend; J >= Jstart; J--) /* Back substitution */
			fi[I][J] = Ari[J]*fi[I][J+1] + Cmri[J]; /* eq. 7.6a */
	} /* for I from east */
	free(Ari);
	free(Cmri);

}

/* ################################################################# */
void solveGS(double **fi, double **b, double **aE, double **aW, double **aN, double **aS, double **aP)
/* ################################################################# */
{
/***** Purpose: To solve the algebraic equation 7.7. using Gauss-Seidel ******/
	int    I, J;

	for (I = Istart; I <= Iend; I++)
		for (J = Jstart; J <= Jend; J++)
			fi[I][J] = ( aE[I][J]*fi[I+1][J  ] + aW[I][J]*fi[I-1][J  ]
			           + aN[I][J]*fi[I  ][J+1] + aS[I][J]*fi[I  ][J-1] + b[I][J])
			           /aP[I][J];
} /* solveGS */

/* ################################################################# */
void solveSOR(double **fi, double **b, double **aE, double **aW, double **aN, double **aS, double **aP)
/* ################################################################# */
{
/***** Purpose: To solve the algebraic equation 7.7. using Successive over-relaxation ******/
	int    I, J;

	for (I = Istart; I <= Iend; I++)
		for (J = Jstart; J <= Jend; J++)
			fi[I][J] = ( aE[I][J]*fi[I+1][J  ] + aW[I][J]*fi[I-1][J  ]
			           + aN[I][J]*fi[I  ][J+1] + aS[I][J]*fi[I  ][J-1] + b[I][J])
			           /aP[I][J] * omega - (omega - 1)*fi[I][J];
} /* solveSOR */

/* ################################################################# */
void conv(void)
/* ################################################################# */
{
/***** Purpose: To calculate the convective mass flux component pr. unit ******/
/*****          area defined in eq. 5.7 ******/
	int    I, J, i, j;

	for (I = 1; I <= NPI + 1; I++) {
		i = I;
		for (J = 1; J <= NPJ + 1; J++) {
			j = J;
			F_u[i][J] = (rho[I-1][J  ]*(x[I] - x_u[i]) + rho[I][J]*(x_u[i] - x[I-1]))*u[i][J]/(x[I] - x[I-1]); /* = F(i, J) */
			F_v[I][j] = (rho[I  ][J-1]*(y[J] - y_v[j]) + rho[I][J]*(y_v[j] - y[J-1]))*v[I][j]/(y[J] - y[J-1]); /* = F(I, j) */
		} /* for J */
	} /* for I */

} /* conv */

/* ################################################################# */
void ucoeff(double **aE, double **aW, double **aN, double **aS, double **aP, double **b)
/* ################################################################# */
{
/***** Purpose: To calculate the coefficients for the u equation. ******/
	int    i, j, I, J;
	double Fw, Fe, Fs, Fn, 
	       Dw, De, Ds, Dn, 
	       AREAw, AREAe, AREAs, AREAn,
	       aPold, mun, mus;
	double Triangle_x[21] = {4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,3.6,3.2,2.8,2.4,2.0,2.4,2.8,3.2,3.6,4.0};
	double Triangle_y[21]  = {0.0, 0.4, 0.8, 1.2, 1.6, 2.0, 2.4, 2.8, 3.2, 3.6, 4.0, 3.6, 3.2, 2.8, 2.4, 2.0, 1.6, 1.2, 0.8, 0.4, 0.0};
	double Pr, Pee;
	

	Istart = 2;
	Iend   = NPI;
	Jstart = 1;
	Jend   = NPJ;

	conv();

	for (I = Istart; I <= Iend; I++) {
		i = I;
		for (J = Jstart; J <= Jend; J++) {
			j = J;
			
			/* Geometrical parameters */
			/* Areas of the cell faces */

			AREAw = y_v[j+1] - y_v[j]; /* See fig. 6.3 */
			AREAe = AREAw;
			AREAs = x[I] - x[I-1];
			AREAn = AREAs;

			/* eq. 6.9a-6.9d - the convective mass flux defined in eq. 5.8a  */
			/* note:  F = rho*u but Fw = (rho*u)w = rho*u*AREAw per definition. */

			Fw = 0.5*(F_u[i  ][J  ] + F_u[i-1][J  ])*AREAw;
			Fe = 0.5*(F_u[i+1][J  ] + F_u[i  ][J  ])*AREAe;
			Fs = 0.5*(F_v[I  ][j  ] + F_v[I-1][j  ])*AREAs;
			Fn = 0.5*(F_v[I  ][j+1] + F_v[I-1][j+1])*AREAn;

			/* eq. 6.9e-6.9h - the transport by diffusion defined in eq. 5.8b  */
			/* note: D = mu/Dx but Dw = (mu/Dx)*AREAw per definition */

			Dw = mueff[I-1][J]/(x_u[i  ] - x_u[i-1])*AREAw;
			De = mueff[I  ][J]/(x_u[i+1] - x_u[i  ])*AREAe;
			Ds = 0.25*(mueff[I-1][J  ] + mueff[I][J  ] + mueff[I-1][J-1] + mueff[I][J-1])/(y[J  ] - y[J-1])*AREAs;
			Dn = 0.25*(mueff[I-1][J+1] + mueff[I][J+1] + mueff[I-1][J  ] + mueff[I][J  ])/(y[J+1] - y[J  ])*AREAn;

			/* The source terms */

			mus = 0.25*(mueff[I][J] + mueff[I-1][J] + mueff[I][J-1] + mueff[I-1][J-1]);
			mun = 0.25*(mueff[I][J] + mueff[I-1][J] + mueff[I][J+1] + mueff[I-1][J+1]);

			if(J == 1 || J==NPJ)
				if(yplus[I][J] < 11.63)
					SP[i][J]= -mu[I][J]*AREAs/(0.5*AREAw);
				else
					SP[i][J]=-rho[I][J] * pow(Cmu, 0.25) * sqrt(k[I][J]) / uplus[I][J] * AREAs;
			else
				SP[i][J] = 0.;
			
            
			Su[i][J] = (mueff[I][J]*dudx[I][J] - mueff[I-1][J]*dudx[I-1][J]) / (x[I] - x[I-1]) + 
			           (mun        *dvdx[i][j+1] - mus        *dvdx[i][j]) / (y_v[j+1] - y_v[j]) -
                       2./3. * (rho[I][J]*k[I][J] - rho[I-1][J]*k[I-1][J])/(x[I] - x[I-1]);
			Su[I][j] *= AREAw*AREAs;
			
			/* The coefficients (hybrid differencing sheme) */

			aW[i][J] = max3( Fw, Dw + 0.5*Fw, 0.);
			aE[i][J] = max3(-Fe, De - 0.5*Fe, 0.);

			if (J==1) aS[i][J]=0.;
			else      aS[i][J] = max3( Fs, Ds + 0.5*Fs, 0.);
            
			if (J==NPJ) aN[i][J] =0.;
			else        aN[i][J] = max3(-Fn, Dn - 0.5*Fn, 0.);
            
            			/*bluff body*/	
			if((I>A && I<B && J<D && J>C)||(I>AA && I<BB && J<DD && J>CC)||(I>A2 && I<B2 && J<D2 && J>C2)||(I>AA2 && I<BB2 && J<DD2 && J>CC2)){
				SP[i][J]= -LARGE;
//				aS[i][j]= 0;
//				aN[i][j] = 0;
			}

//			if(I == B && J<D && J>C)
//				SP[i][J]= -LARGE;
			if((I >= A && I<=B && J==C)||(I >= AA && I<=BB && J==CC)||(I >= A2 && I<=B2 && J==C2)||(I >= AA2 && I<=BB2 && J==CC2)){
				aN[I][j] = 0;
				if(yplus[I][J] < 11.63)
					SP[i][J]= -mu[I][J]*AREAs/(0.5*AREAw);
				else
					SP[i][J]= - rho[I][J] * pow(Cmu, 0.25) * sqrt(k[I][J]) / uplus[I][J] * AREAs;
//				SP[I][j]=-rho[I][J] * pow(Cmu, 0.25) * sqrt(k[I][J]) / uplus[I][J] * AREAs;
			}								
			if((I >= A && I<=B && J==D)||(I >= AA && I<=BB && J==DD)||(I >= A2 && I<=B2 && J==D2)||(I >= AA2 && I<=BB2 && J==DD2)){
				aS[I][j] = 0;
				if(yplus[I][J] < 11.63)
					SP[i][J]= -mu[I][J]*AREAs/(0.5*AREAw);
				else
					SP[i][J]= - rho[I][J] * pow(Cmu, 0.25) * sqrt(k[I][J]) / uplus[I][J] * AREAs;
//				SP[I][j] = -rho[I][J] * pow(Cmu, 0.25) * sqrt(k[I][J]) / uplus[I][J] * AREAs;
			}

            /* bluff body */
            
			aPold    = 0.5*(rho[I-1][J] + rho[I][J])*AREAe*AREAn/Dt;

			/* eq. 8.31 without time dependent terms (see also eq. 5.14): */

			aP[i][J] = aW[i][J] + aE[i][J] + aS[i][J] + aN[i][J] + Fe - Fw + Fn - Fs - SP[I][J] + aPold;

			/* Calculation of d[i][J] = d_u[i][J] defined in eq. 6.23 for use in the  */
			/* equation for pression correction (eq. 6.32). See subroutine pccoeff. */

			d_u[i][J] = AREAw*relax_u/aP[i][J];

			/* Putting the integrated pressure gradient into the source term b[i][J] */
			/* The reason is to get an equation on the generalised form  */
			/* (eq. 7.7 ) to be solved by the TDMA algorithm.  */
			/* note: In reality b = a0p*fiP + Su = 0.  */

			b[i][J] = (p[I-1][J] - p[I][J])*AREAw + Su[I][J] + aPold*u_old[i][J];

			/* Introducing relaxation by eq. 6.36 . and putting also the last  */
			/* term on the right side into the source term b[i][J] */

			aP[i][J] /= relax_u;
			b [i][J] += (1 - relax_u)*aP[i][J]*u[i][J];

			/* now we have implemented eq. 6.36 in the form of eq. 7.7 */
			/* and the TDMA algorithm can be called to solve it. This is done  */
			/* in the next step of the main program. */

			} /* for j */
		} /* for i */

} /* ucoeff */

/* ################################################################# */
void vcoeff(double **aE, double **aW, double **aN, double **aS, double **aP, double **b)
/* ################################################################# */
{
/***** Purpose: To calculate the coefficients for the v equation. ******/
	int    i, j, I, J;
	double Fw, Fe, Fs, Fn, 
	       Dw, De, Ds, Dn, 
	       AREAw, AREAe, AREAs, AREAn,
	       aPold, mue, muw;

	Istart = 1;
	Iend   = NPI;
	Jstart = 2;
	Jend   = NPJ;

	conv();

	for (I = Istart; I <= Iend; I++) {
		i = I;
		for (J = Jstart; J <= Jend; J++) {
			j = J;

			/* Geometrical parameters */
			/* Areas of the cell faces */

			AREAw = y[J] - y[J-1]; /* See fig. 6.4 */
			AREAe = AREAw;
			AREAs = x_u[i+1] - x_u[i];
			AREAn = AREAs;

			/* eq. 6.11a-6.11d - the convective mass flux defined in eq. 5.8a  */
			/* note:  F = rho*u but Fw = (rho*u)w = rho*u*AREAw per definition. */

			Fw = 0.5*(F_u[i  ][J] + F_u[i  ][J-1])*AREAw;
			Fe = 0.5*(F_u[i+1][J] + F_u[i+1][J-1])*AREAe;
			Fs = 0.5*(F_v[I  ][j] + F_v[I  ][j-1])*AREAs;
			Fn = 0.5*(F_v[I  ][j] + F_v[I  ][j+1])*AREAn;

			/* eq. 6.11e-6.11h - the transport by diffusion defined in eq. 5.8b */
			/* note: D = mu/Dx but Dw = (mu/Dx)*AREAw per definition */

			Dw = 0.25*(mueff[I-1][J-1] + mueff[I  ][J-1] + mueff[I-1][J] + mueff[I  ][J])/(x[I  ] - x[I-1])*AREAw;
			De = 0.25*(mueff[I  ][J-1] + mueff[I+1][J-1] + mueff[I  ][J] + mueff[I+1][J])/(x[I+1] - x[I  ])*AREAe;
			Ds = mueff[I][J-1]/(y_v[j  ] - y_v[j-1])*AREAs;
			Dn = mueff[I][J  ]/(y_v[j+1] - y_v[j  ])*AREAn;

			/* The source terms */

			muw = 0.25*(mueff[I][J] + mueff[I-1][J] + mueff[I][J-1] + mueff[I-1][J-1]);
			mue = 0.25*(mueff[I][J] + mueff[I+1][J] + mueff[I][J-1] + mueff[I+1][J-1]);
			SP[I][j] = 0.;
			Su[I][j] = (mueff[I][J]*dvdy[I][J] - mueff[I][J-1]*dvdy[I][J-1])/(y[J] - y[J-1]) + 
			           (mue*dudy[i+1][j] - muw*dudy[i][j])/(x_u[i+1] - x_u[i]) - 
                       2./3. * (rho[I][J]*k[I][J] - rho[I][J-1]*k[I][J-1])/(y[J] - y[J-1]); 

			Su[I][j] *= AREAw*AREAs;

			/* The coefficients (hybrid differencing scheme) */

			aW[I][j] = max3( Fw, Dw + 0.5*Fw, 0.);
			aE[I][j] = max3(-Fe, De - 0.5*Fe, 0.);
			aS[I][j] = max3( Fs, Ds + 0.5*Fs, 0.);
			aN[I][j] = max3(-Fn, Dn - 0.5*Fn, 0.);
			aPold    = 0.5*(rho[I][J-1] + rho[I][J])*AREAe*AREAn/Dt;

			/*bluff body*/	
			if((I == A && J<=D && J>=C)||(I == AA && J<=DD && J>=CC)||(I == A2 && J<=D2 && J>=C2)||(I == AA2 && J<=DD2 && J>=CC2)){
				aE[I][j] = 0;
				if(xplus[I][J] < 11.63)
					SP[I][j]= -mu[I][J]*AREAw/(0.5*AREAs);
				else
					SP[I][j]=-rho[I][J] * pow(Cmu, 0.25) * sqrt(k[I][J]) / vplus[I][J] * AREAw;
			}
			if((I == B && J<=D && J>=C)||(I == BB && J<=DD && J>=CC)||(I == B2 && J<=D2 && J>=C2)||(I == BB2 && J<=DD2 && J>=CC2)){
				aW[I][j] = 0;
				if(xplus[I][J]<11.63)
					SP[I][j]= -mu[I][J]*AREAw/(0.5*AREAs);
				else
					SP[I][j]=-rho[I][J] * pow(Cmu, 0.25) * sqrt(k[I][J]) / vplus[I][J] * AREAw;
			}
//			if(I > A && I<B && J==C)
//				SP[I][j]= -LARGE;
//			if(I > A && I<B && J==D)								
//				SP[I][j] = - LARGE;
			if((I>A && I<B && J<=D && J>C)||(I>AA && I<BB && J<=DD && J>CC)||(I>A2 && I<B2 && J<=D2 && J>C2)||(I>AA2 && I<BB2 && J<=DD2 && J>CC2)){
				SP[i][J]= -LARGE;
//				aE[i][j]= 0;
//				aW[i][j] = 0;
			}
			/* bluff body */
				
			/* eq. 8.31 without time dependent terms (see also eq. 5.14): */

			aP[I][j] = aW[I][j] + aE[I][j] + aS[I][j] + aN[I][j] + Fe - Fw + Fn - Fs - SP[I][J] + aPold;

			/* Calculation of d[I][j] = d_v[I][j] defined in eq. 6.23 for use in the */
			/* equation for pression correction (eq. 6.32) (see subroutine pccoeff). */

			d_v[I][j] = AREAs*relax_v/aP[I][j];

			/* Putting the integrated pressure gradient into the source term b[I][j] */
			/* The reason is to get an equation on the generalised form */
			/* (eq. 7.7 ) to be solved by the TDMA algorithm. */
			/* note: In reality b = a0p*fiP + Su = 0. */

			b[I][j] = (p[I][J-1] - p[I][J])*AREAs + Su[I][j] + aPold*v_old[I][j];

			/* Introducing relaxation by eq. 6.37 . and putting also the last */
			/* term on the right side into the source term b[i][J] */

			aP[I][j] /= relax_v;
			b [I][j] += (1 - relax_v)*aP[I][j]*v[I][j];

			/* now we have implemented eq. 6.37 in the form of eq. 7.7 */
			/* and the TDMA algorithm can be called to solve it. This is done */
			/* in the next step of the main program. */

			} /* for j */
		} /* for i */

} /* vcoeff */

/* ################################################################# */
void pccoeff(double **aE, double **aW, double **aN, double **aS, double **aP, double **b)
/* ################################################################# */
{
/***** Purpose: To calculate the coefficients for the pc equation. ******/
	int    i, j, I, J;
	double AREAw, AREAe, AREAs, AREAn;
	double SSUM;

	SMAX = 0.;
	SSUM = 0.;
	SAVG = 0.;

	Istart = 1;
	Iend   = NPI;
	Jstart = 1;
	Jend   = NPJ;

	conv();

	for (I = Istart; I <= Iend; I++) {
		i = I;
		for (J = Jstart; J <= Jend; J++) {
			j = J;

			/* Geometrical parameters */
			/* Areas of the cell faces */

			AREAw = y_v[j+1] - y_v[j]; /* = A[i][J] See fig. 6.2 or fig. 6.5 */
			AREAe = AREAw;
			AREAs = x_u[i+1] - x_u[i]; /* = A[I][j] */
			AREAn = AREAs;

			/* The constant b� in eq. 6.32 */

			b[I][J] = F_u[i][J]*AREAw - F_u[i+1][J]*AREAe + F_v[I][j]*AREAs - F_v[I][j+1]*AREAn;

			SP[I][J] = 0.;
			Su[I][J] = 0.;
			
			b[I][J] += Su[I][J];

			SMAX     = max2(SMAX,fabs(b[I][J]));
			SSUM    += fabs(b[I][J]);
			
			/* The coefficients */

			aE[I][J] = 0.5*(rho[I  ][J  ] + rho[I+1][J  ])*d_u[i+1][J  ]*AREAe;
			aW[I][J] = 0.5*(rho[I-1][J  ] + rho[I  ][J  ])*d_u[i  ][J  ]*AREAw;
			aN[I][J] = 0.5*(rho[I  ][J  ] + rho[I  ][J+1])*d_v[I  ][j+1]*AREAn;
			aS[I][J] = 0.5*(rho[I  ][J-1] + rho[I  ][J  ])*d_v[I  ][j  ]*AREAs;

			aP[I][J] = aE[I][J] + aW[I][J] + aN[I][J] + aS[I][J] - SP[I][J];

			pc[I][J] = 0.;

			/* note: At the points nearest the boundaries, some coefficients are */
			/* necessarily zero. For instance at I = 1 and J = 1, the coefficients */
			/* aS and aW are zero since they are on the outside of the calculation */
			/* domain. This is automatically satisfied through the initialisation */
			/* where d_u[i][J] and d_v[I][j] are set to zero at these points. */

			} /* for J */
		} /* for I */

		/* Average error in mass balance is summed error devided by */
		/* number of internal grid points */
	      SAVG = SSUM/((Iend - Istart)*(Jend - Jstart));

} /* pccoeff */



/* ################################################################# */
void storeresults(void)
/* ################################################################# */
{
/***** To newly calculated variables are stored in the arrays ******/
/***** for old variables, which can be used in the next timestep ******/
	int I, J, i, j;

	for(i = 2; i <= NPI; i++)
		for(J = 1; J <= NPJ; J++)
			u_old[i][J] = u[i][J];

	for(I = 1; I <= NPI; I++)
		for(j = 2; j <= NPJ; j++)
			v_old[I][j] = v[I][j];

	for(I = 1; I <= NPI; I++)
		for(J = 1; J <= NPJ; J++) {
			pc_old[I][J]  = pc[I][J];
			T_old[I][J]   = T[I][J];
			eps_old[I][J] = eps[I][J];
			k_old[I][J]   = k[I][J];				
      }

} /* storeresults */


/* ################################################################# */
void velcorr(void)
/* ################################################################# */
{
/***** To correct the pressure and the velocities by eq. 6.24, 6.25 ******/
/*****  and a modified version of eq. 6.33. ******/
	int I, J, i, j;

	for (I = 1; I <= NPI; I++) {
		i = I;
		for (J = 1; J <= NPJ; J++) {
			j = J;

			p[I][J] += relax_pc*pc[I][J]; /* equation 6.33 */

			/* Velocity correction */
			/* Note: the relaxation factors for u and v are included  */
			/* in the d_u and d_v terms (see page 146) */

			if (i != 1)
				u[i][J] += d_u[i][J]*(pc[I-1][J  ] - pc[I][J]); /* eq. 6.24 */

			if (j != 1)
				v[I][j] += d_v[I][j]*(pc[I  ][J-1] - pc[I][J]); /* eq. 6.25 */

		} /* for J */
	} /* for I */

} /* velcorr */

/* ################################################################# */
void Tcoeff(double **aE, double **aW, double **aN, double **aS, double **aP, double **b)
/* ################################################################# */
{
/***** Purpose: To calculate the coefficients for the T equation. ******/
	int    i, j, I, J;
	double Fw, Fe, Fs, Fn, 
	       Dw, De, Ds, Dn, 
	       AREAw, AREAe, AREAs, AREAn,
	       aPold;

	Istart = 1;
	Iend   = NPI;
	Jstart = 1;
	Jend   = NPJ;
//	double Pr, Pee;

	conv();

    calculateuplus();

	for (I = Istart; I <= Iend; I++) {
		i = I;
		for (J = Jstart; J <= Jend; J++) {
			j = J;
			/* Geometrical parameters */
			/* Areas of the cell faces */

			AREAw = y_v[j+1] - y_v[j]; /* = A[i][J] See fig. 6.2 or fig. 6.5 */
			AREAe = AREAw;
			AREAs = x_u[i+1] - x_u[i]; /* = A[I][j] */
			AREAn = AREAs;
			
			/* The convective mass flux defined in eq. 5.8a */
			/* note:  F = rho*u but Fw = (rho*u)w = rho*u*AREAw per definition. */

			Fw = F_u[i  ][J  ]*AREAw;
			Fe = F_u[i+1][J  ]*AREAe;
			Fs = F_v[I  ][j  ]*AREAs;
			Fn = F_v[I  ][j+1]*AREAn;

			/* The transport by diffusion defined in eq. 5.8b */
			/* note: D = mu/Dx but Dw = (mu/Dx)*AREAw per definition */

			/* The conductivity, Gamma, at the interface is calculated */
			/* with the use of a harmonic mean. */

			Dw = Gamma[I-1][J  ]*Gamma[I  ][J  ]/(Gamma[I-1][J  ]*(x[I  ] - x_u[i  ]) + Gamma[I  ][J  ]*(x_u[i  ] - x[I-1]))*AREAw;
			De = Gamma[I  ][J  ]*Gamma[I+1][J  ]/(Gamma[I  ][J  ]*(x[I+1] - x_u[i+1]) + Gamma[I+1][J  ]*(x_u[i+1] - x[I  ]))*AREAe;
			Ds = Gamma[I  ][J-1]*Gamma[I  ][J  ]/(Gamma[I  ][J-1]*(y[J  ] - y_v[j  ]) + Gamma[I  ][J  ]*(y_v[j  ] - y[J-1]))*AREAs;
			Dn = Gamma[I  ][J  ]*Gamma[I  ][J+1]/(Gamma[I  ][J  ]*(y[J+1] - y_v[j+1]) + Gamma[I  ][J+1]*(y_v[j+1] - y[J  ]))*AREAn;

			/* The source terms */

			SP[I][J] = 0.;
			Su[I][J] = 0.;
			

			/* The coefficients (hybrid differencing scheme) */

			aW[I][J] = max3( Fw, Dw + 0.5*Fw, 0.);
			aE[I][J] = max3(-Fe, De - 0.5*Fe, 0.);
			aS[I][J] = max3( Fs, Ds + 0.5*Fs, 0.);
			aN[I][J] = max3(-Fn, Dn - 0.5*Fn, 0.);
			aPold    = rho[I][J]*AREAe*AREAn/Dt;

//			if (I > 11*NPI/200 && I < 18*NPI/200 && J > 2*NPJ/5 && J < 3*NPJ/5){
//				SP[I][J] = -LARGE;
//				Su[I][J] = LARGE*373.;
//			}
	
				/*bluff body*/	
//			if(I == A && J<D && J>C){
//				aE[I][J] = 0;
//				SP[I][j] = -rho[I][J] * pow(Cmu,0.25) * sqrt(k[I][J]) * Cp[I][J]/(Tplus[I][J]) * AREAe;
//				Su[I][J] =  rho[I][J]*pow(Cmu,0.25)*sqrt(k[I][J])*Cp[I][J]*T[I][J]/(Tplus[I][J])*AREAe;
//			}
//			if(I == B && J<D && J>C){
//				aW[I][J] = 0;
//				SP[I][j] = -rho[I][J] * pow(Cmu,0.25) * sqrt(k[I][J]) * Cp[I][J]/(Tplus[I][J]) * AREAe ;
//				Su[I][J] =  rho[I][J]*pow(Cmu,0.25)*sqrt(k[I][J])*Cp[I][J]*T[I][J]/(Tplus[I][J])*AREAe;
//			}
//			if(I > A && I<B && J==C){
//				aN[I][J] = 0;
//				SP[I][j] = -rho[I][J] * pow(Cmu,0.25) * sqrt(k[I][J]) * Cp[I][J]/(Tplus[I][J]) * AREAe;
//				Su[I][J] =  rho[I][J]*pow(Cmu,0.25)*sqrt(k[I][J])*Cp[I][J]*T[I][J]/(Tplus[I][J])*AREAe;
//			}
//			if(I > A && I<B && J==D){
//				aS[I][J] = 0;
//				SP[I][j] = -rho[I][J] * pow(Cmu,0.25) * sqrt(k[I][J]) * Cp[I][J]/(Tplus[I][J]) * AREAe;
//				Su[I][J] =  rho[I][J]*pow(Cmu,0.25)*sqrt(k[I][J])*Cp[I][J]*T[I][J]/(Tplus[I][J])*AREAe;
//			}
			
			if((I>A && I<B && J<D && J>C)||(I>AA && I<BB && J<DD && J>CC)||(I>A2 && I<B2 && J<D2 && J>C2)||(I>AA2 && I<BB2 && J<DD2 && J>CC2)){
				aN[i][J]= 0;
				aW[i][J]= 0;
				aS[i][J]= 0;
				aE[i][J]= 0;

			}
			/* bluff body */
			
			
			/* eq. 8.31 with time dependent terms (see also eq. 5.14): */

			aP[I][J] = aW[I][J] + aE[I][J] + aS[I][J] + aN[I][J] + Fe - Fw + Fn - Fs - SP[I][J] + aPold;

			/* Setting the source term equal to b */

			b[I][J] = Su[I][J] + aPold*T_old[I][J];

			/* Introducing relaxation by eq. 6.37 . and putting also the last */
			/* term on the right side into the source term b[i][J] */

			aP[I][J] /= relax_T;
			b [I][J] += (1 - relax_T)*aP[I][J]*T[I][J];

			/* now the TDMA algorithm can be called to solve the equation. */
			/* This is done in the next step of the main program. */

			} /* for J */
		} /* for I */

} /* Tcoeff */

/* ################################################################# */
void epscoeff(double **aE, double **aW, double **aN, double **aS, double **aP, double **b)
/* ################################################################# */
{
/***** Purpose: Calculate the epsilon *****/
/*****           *****/
	int    i, j, I, J;
	double Fw, Fe, Fs, Fn, 
	       Dw, De, Ds, Dn, 
	       AREAw, AREAe, AREAs, AREAn,
	       aPold;

	Istart = 1;
	Iend   = NPI;
	Jstart = 1;
	Jend   = NPJ;

	conv();
    viscosity();
    
	for (I = Istart; I <= Iend; I++) {
		i = I;
		for (J = Jstart; J <= Jend; J++) {
			j = J;
			/* Geometrical parameters */
			/* Areas of the cell faces */

			AREAw = y_v[j+1] - y_v[j]; /* = A[i][J] See fig. 6.2 or fig. 6.5 */
			AREAe = AREAw;
			AREAs = x_u[i+1] - x_u[i]; /* = A[I][j] */
			AREAn = AREAs;

			/* The convective mass flux defined in eq. 5.8a */
			/* note:  F = rho*u but Fw = (rho*u)w = rho*u*AREAw per definition. */

			Fw = F_u[i  ][J  ]*AREAw;
			Fe = F_u[i+1][J  ]*AREAe;
			Fs = F_v[I  ][j  ]*AREAs;
			Fn = F_v[I  ][j+1]*AREAn;

			/* The transport by diffusion defined in eq. 5.8b */
			/* note: D = mu/Dx but Dw = (mu/Dx)*AREAw per definition */

			/* The conductivity, Gamma, at the interface is calculated */
			/* with the use of a harmonic mean. */

			Dw = mut[I-1][J  ]*mut[I  ][J  ]/sigmaeps/(mut[I-1][J  ]*(x[I  ] - x_u[i  ]) + mut[I  ][J  ]*(x_u[i  ] - x[I-1]))*AREAw;
			De = mut[I  ][J  ]*mut[I+1][J  ]/sigmaeps/(mut[I  ][J  ]*(x[I+1] - x_u[i+1]) + mut[I+1][J  ]*(x_u[i+1] - x[I  ]))*AREAe;
			Ds = mut[I  ][J-1]*mut[I  ][J  ]/sigmaeps/(mut[I  ][J-1]*(y[J  ] - y_v[j  ]) + mut[I  ][J  ]*(y_v[j  ] - y[J-1]))*AREAs;
			Dn = mut[I  ][J  ]*mut[I  ][J+1]/sigmaeps/(mut[I  ][J  ]*(y[J+1] - y_v[j+1]) + mut[I  ][J+1]*(y_v[j+1] - y[J  ]))*AREAn;

		 /* The source terms */

			if (J==1 || J==NPJ) {
				SP[I][J] = -LARGE;
				Su[I][J] = pow(Cmu,0.75)*pow(k[I][J],1.5)/(kappa*0.5*AREAw)*LARGE;
			}
			else {
				Su[I][J] = C1eps * eps[I][J] / k[I][J] * 2. * mut[I][J] * E2[I][J];
				SP[I][J] = -C2eps * rho[I][J] * eps[I][J] / (k[I][J] + SMALL);
			}
                                  
			Su[I][J] *= AREAw*AREAs;
			SP[I][J] *= AREAw*AREAs;

			/* The coefficients (hybrid differencing sheme) */

			aW[I][J] = max3( Fw, Dw + 0.5*Fw, 0.);
			aE[I][J] = max3(-Fe, De - 0.5*Fe, 0.);
			aS[I][J] = max3( Fs, Ds + 0.5*Fs, 0.);
			aN[I][J] = max3(-Fn, Dn - 0.5*Fn, 0.);
			aPold    = rho[I][J]*AREAe*AREAn/Dt;
			
			/*bluff body*/	
			if((I == A && J<D && J>C)||(I == AA && J<DD && J>CC)||(I == A2 && J<D2 && J>C2)||(I == AA2 && J<DD2 && J>CC2)){
				SP[I][J] = -LARGE;
				Su[I][J] = pow(Cmu,0.75)*pow(k[I][J],1.5)/(kappa*0.5*AREAs)*LARGE;
			}
			if((I == B && J<D && J>C)||(I == BB && J<DD && J>CC)||(I == B2 && J<D2 && J>C2)||(I == BB2 && J<DD2 && J>CC2)){
				SP[I][J] = -LARGE;
				Su[I][J] = pow(Cmu,0.75)*pow(k[I][J],1.5)/(kappa*0.5*AREAs)*LARGE;
			}
			if((I > A && I<B && J==C)||(I > AA && I<BB && J==CC)||(I > A2 && I<B2 && J==C2)||(I > AA2 && I<BB2 && J==CC2)){
				SP[I][J] = -LARGE;
				Su[I][J] = pow(Cmu,0.75)*pow(k[I][J],1.5)/(kappa*0.5*AREAw)*LARGE;
			}
			if((I > A && I<B && J==D)||(I > AA && I<BB && J==DD)||(I > A2 && I<B2 && J==D2)||(I > AA2 && I<BB2 && J==DD2)){
				SP[I][J] = -LARGE;
				Su[I][J] = pow(Cmu,0.75)*pow(k[I][J],1.5)/(kappa*0.5*AREAw)*LARGE;
			}
			
			if((I>A && I<B && J<D && J>C)||(I>AA && I<BB && J<DD && J>CC)||(I>A2 && I<B2 && J<D2 && J>C2)||(I>AA2 && I<BB2 && J<DD2 && J>CC2)){
				SP[i][J]= -LARGE;
			}
			/* bluff body */

			/* eq. 8.31 with time dependent terms (see also eq. 5.14): */

			aP[I][J] = aW[I][J] + aE[I][J] + aS[I][J] + aN[I][J] + Fe - Fw + Fn - Fs - SP[I][J] + aPold;

			/* Setting the source term equal to b */

			b[I][J] = Su[I][J] + aPold*eps_old[I][J];

			/* Introducing relaxation by eq. 6.37 . and putting also the last */
			/* term on the right side into the source term b[i][J] */

			aP[I][J] /= relax_eps;
			b [I][J] += (1 - relax_eps)*aP[I][J]*eps[I][J];

			/* now the TDMA algorithm can be called to solve the equation. */
			/* This is done in the next step of the main program. */

			} /* for J */
		} /* for I */
	
} /*epscoef*/

/* ################################################################# */
void kcoeff(double **aE, double **aW, double **aN, double **aS, double **aP, double **b)
/* ################################################################# */
{
/***** Purpose: Calculate the epsilon *****/
/*****           *****/
	int    i, j, I, J;
	double Fw, Fe, Fs, Fn, 
	       Dw, De, Ds, Dn, 
	       AREAw, AREAe, AREAs, AREAn,
	       aPold;

	Istart = 1;
	Iend   = NPI;
	Jstart = 1;
	Jend   = NPJ;

	conv();
    viscosity();
    calculateuplus();
    
	for (I = Istart; I <= Iend; I++) {
		i = I;
		for (J = Jstart; J <= Jend; J++) {
			j = J;
			/* Geometrical parameters */
			/* Areas of the cell faces */

			AREAw = y_v[j+1] - y_v[j]; /* = A[i][J] See fig. 6.2 or fig. 6.5 */
			AREAe = AREAw;
			AREAs = x_u[i+1] - x_u[i]; /* = A[I][j] */
			AREAn = AREAs;

			/* The convective mass flux defined in eq. 5.8a */
			/* note:  F = rho*u but Fw = (rho*u)w = rho*u*AREAw per definition. */

			Fw = F_u[i  ][J  ]*AREAw;
			Fe = F_u[i+1][J  ]*AREAe;
			Fs = F_v[I  ][j  ]*AREAs;
			Fn = F_v[I  ][j+1]*AREAn;

			/* The transport by diffusion defined in eq. 5.8b */
			/* note: D = mu/Dx but Dw = (muw/Dx)*AREAw per definition */

			/* The conductivity, Gamma, at the interface is calculated */
			/* with the use of a harmonic mean. */

			Dw = mut[I-1][J  ]*mut[I  ][J  ]/sigmak/(mut[I-1][J  ]*(x[I  ] - x_u[i  ]) + mut[I  ][J  ]*(x_u[i  ] - x[I-1]))*AREAw;
			De = mut[I  ][J  ]*mut[I+1][J  ]/sigmak/(mut[I  ][J  ]*(x[I+1] - x_u[i+1]) + mut[I+1][J  ]*(x_u[i+1] - x[I  ]))*AREAe;
			Ds = mut[I  ][J-1]*mut[I  ][J  ]/sigmak/(mut[I  ][J-1]*(y[J  ] - y_v[j  ]) + mut[I  ][J  ]*(y_v[j  ] - y[J-1]))*AREAs;
			Dn = mut[I  ][J  ]*mut[I  ][J+1]/sigmak/(mut[I  ][J  ]*(y[J+1] - y_v[j+1]) + mut[I  ][J+1]*(y_v[j+1] - y[J  ]))*AREAn;

            /* The source terms */
            if (J == 1 || J == NPJ) {
				SP[I][J] = -rho[I][J] * pow(Cmu,0.75) * sqrt(k[I][J]) * uplus[I][J]/(0.5*AREAw) * AREAs * AREAw;
				Su[I][J] = tw[I][J] * 0.5 * (u[i][J] + u[i+1][J])/(0.5*AREAw) * AREAs * AREAw;
			}                 
			else {
				Su[I][J]  = 2. * mut[I][J] * E2[I][J];
				SP[I][J]  = -rho[I][J] * eps[I][J] / k[I][J];
			}
                                     
			Su[I][j] *= AREAw*AREAs;
			SP[I][j] *= AREAw*AREAs;

			/* The coefficients (hybrid differencing sheme) */

			aW[I][J] = max3( Fw, Dw + 0.5*Fw, 0.);
			aE[I][J] = max3(-Fe, De - 0.5*Fe, 0.);
            
            if (J == 1) aS[i][J] = 0;
			else        aS[i][J] = max3( Fs, Ds + 0.5*Fs, 0.);
            
			if (J == NPJ) aN[i][J] = 0;
			else          aN[i][J] = max3(-Fn, Dn - 0.5*Fn, 0.);
			
//						/*bluff body*/	
			if((I == A && J<D && J>C)||(I == AA && J<DD && J>CC)||(I == A2 && J<D2 && J>C2)||(I == AA2 && J<DD2 && J>CC2)){
				aE[I][j] = 0;
				SP[I][j]=-rho[I][J] * pow(Cmu,0.75) * sqrt(k[I][J]) * vplus[I][J]/(0.5*AREAs) * AREAw * AREAs;
				Su[I][J] = twx[I][J] * 0.5 * (v[I][j] + v[I][j+1])/(0.5*AREAs) * AREAw * AREAs;
			}
			if((I == B && J<D && J>C)||(I == BB && J<DD && J>CC)||(I == B2 && J<D2 && J>C2)||(I == BB2 && J<DD2 && J>CC2)){
				aW[I][j] = 0;
				SP[I][j]=-rho[I][J] * pow(Cmu,0.75) * sqrt(k[I][J]) * vplus[I][J]/(0.5*AREAs) * AREAw * AREAs;
				Su[I][J] = twx[I][J] * 0.5 * (v[I][j] + v[I][j+1])/(0.5*AREAs) * AREAw * AREAs;
			}
			if((I > A && I<B && J==C)||(I > AA && I<BB && J==CC)||(I > A2 && I<B2 && J==C2)||(I > AA2 && I<BB2 && J==CC2)){
				aN[i][J] = 0;
				SP[I][j]=-rho[I][J] * pow(Cmu,0.75) * sqrt(k[I][J]) * uplus[I][J]/(0.5*AREAw) * AREAs * AREAw;
				Su[I][J] = tw[I][J] * 0.5 * (u[i][J] + u[i+1][J])/(0.5*AREAw) * AREAs * AREAw;
			}
			if((I > A && I<B && J==D)||(I > AA && I<BB && J==DD)||(I > A2 && I<B2 && J==D2)||(I > AA2 && I<BB2 && J==DD2)){
				aS[i][J] = 0;
				SP[I][J]=-rho[I][J] * pow(Cmu,0.75) * sqrt(k[I][J]) * uplus[I][J]/(0.5*AREAw) * AREAs * AREAw;
				Su[I][J] = tw[I][J] * 0.5 * (u[i][J] + u[i+1][J])/(0.5*AREAw) * AREAs * AREAw;
			}
			if((I>A && I<B && J<D && J>C)||(I>AA && I<BB && J<DD && J>CC)||(I>A2 && I<B2 && J<D2 && J>C2)||(I>AA2 && I<BB2 && J<DD2 && J>CC2)){
				SP[i][J]= -LARGE;
			}
			/* bluff body */
            
            aPold    = rho[I][J]*AREAe*AREAn/Dt;

			/* eq. 8.31 with time dependent terms (see also eq. 5.14): */

			aP[I][J] = aW[I][J] + aE[I][J] + aS[I][J] + aN[I][J] + Fe - Fw + Fn - Fs - SP[I][J] + aPold;

			/* Setting the source term equal to b */

			b[I][J] = Su[I][J] + aPold*k_old[I][J];

			/* Introducing relaxation by eq. 6.37 . and putting also the last */
			/* term on the right side into the source term b[i][J] */

			aP[I][J] /= relax_k;
			b [I][J] += (1 - relax_k)*aP[I][J]*k[I][J];

			/* now the TDMA algorithm can be called to solve the equation. */
			/* This is done in the next step of the main program. */

		} /* for J */
	} /* for I */
} /*kcoeff*/

/* ################################################################# */
void calculateuplus(void)
/* ################################################################# */
{
/***** Purpose: Calculate uplus, yplus and tw  ******/

	int    i,j,I, J;
//	double Pr, Pee;

	viscosity();

//	for (I = 0; I <= NPI; I++){
//	    i=I;
//			if (yplus1[I][1] < 11.63) {
//                  tw[I][1]      = mu[I][1]*0.5*(u[i][1]+u[i+1][1])/(y[1] -y[0]);
//                  yplus1[I][1]  = sqrt(rho[I][1] * fabs(tw[I][1])) * (y[1] - y[0]) / mu[I][1];
//                  yplus[I][1]   = yplus1[I][1];
//                  uplus[I][1]   = yplus[I][1];
//            }/* if */
//            else {
//                  tw[I][1]      = rho[I][1]*pow(Cmu,0.25)*sqrt(k[I][1])*0.5*(u[i][1]+u[i+1][1])/uplus[I][1];
//                  yplus1[I][1]  = sqrt(rho[I][1] * fabs(tw[I][1])) * (y[1] - y[0]) / mu[I][1];
//                  yplus [I][1]  = yplus1[I][1];
//                  uplus [I][1]  = log(ERough*yplus[I][1])/kappa;
//            }/* else */
//            
//            if (yplus2[I][NPJ] < 11.63) {
//                  tw[I][NPJ]      = mu[I][NPJ]*0.5*(u[i][NPJ]+u[i+1][NPJ])/(y[NPJ+1] - y[NPJ]);
//                  yplus2[I][NPJ]  = sqrt(rho[I][NPJ] * fabs(tw[I][NPJ])) * (y[NPJ+1] - y[NPJ]) / mu[I][NPJ];
//                  yplus[I][NPJ]   = yplus2[I][NPJ];
//                  uplus[I][NPJ]   = yplus[I][NPJ];
//            }/* if */
//            else {
//                  tw[I][NPJ]      = rho[I][NPJ]*pow(Cmu,0.25)*sqrt(k[I][NPJ])*0.5*(u[i][NPJ]+u[i+1][NPJ])/uplus[I][NPJ];
//                  yplus2[I][NPJ]  = sqrt(rho[I][NPJ] * fabs(tw[I][NPJ])) * (y[NPJ+1] - y[NPJ]) / mu[I][NPJ];
//                  yplus [I][NPJ]  = yplus2[I][NPJ];
//                  uplus [I][NPJ]  = log(ERough*yplus[I][NPJ])/kappa;
//            }/* else */
//                  
//           } /* for */

////////////

	for (I = 0; I <= NPI; I++){
	    i=I;
		for (J = 1; J <= NPJ + 1; J++) {
			j=J;
			if((I>=A && I<=B && J<=D && J>=C)||(I>=AA && I<=BB && J<=DD && J>=CC)||(I>=A2 && I<=B2 && J<=D2 && J>=C2)||(I>=AA2 && I<=BB2 && J<=DD2 && J>=CC2)){
				if (yplus1[I][J] < 11.63) {
                  	tw[I][J]      = mu[I][J]*0.5*(u[i][J]+u[i+1][j])/(y[J] -y_v[j]);
                 	yplus1[I][J]  = sqrt(rho[I][J] * fabs(tw[I][J])) * (y[J] - y_v[J]) / mu[I][J];
                  	yplus[I][J]   = yplus1[I][J];
                 	uplus[I][J]   = yplus[I][J];
                 				/* Prandtl number laminar flow */
					Pr[I][J] = mu[I][J]*Cp[I][J]/0.025;		//Prandtl number laminar flow
					Pee[I][J] = 9.24*(pow((Pr[I][J]/sigma_t),0.75)-1)*(1+0.28*exp(-0.007*(Pr[I][J]/sigma_t)));
//                 	Tplus[I][J]	  = sigma_t*(uplus[I][J]+Pee[I][J]);
					 
//					 (rho[I][J]*pow(Cmu,0.25)*sqrt(k[I][J])*Cp[I][J])
            	}/* if */
            	else {
                  	tw[I][J]      = rho[I][J]*pow(Cmu,0.25)*sqrt(k[I][J])*0.5*(u[i][J]+u[i+1][J])/uplus[I][J];
                  	yplus1[I][J]  = sqrt(rho[I][J] * fabs(tw[I][J])) * (y[J] - y_v[j]) / mu[I][J];
                  	yplus [I][J]  = yplus1[I][J];
                  	uplus [I][J]  = log(ERough*yplus[I][J])/kappa;
                  	
        			Pr[I][J] = mu[I][J]*Cp[I][J]/0.025;		//Prandtl number laminar flow
					Pee[I][J] = 9.24*(pow((Pr[I][J]/sigma_t),0.75)-1)*(1+0.28*exp(-0.007*(Pr[I][J]/sigma_t)));
                 	Tplus[I][J]	  = sigma_t*(uplus[I][J]+Pee[I][J]);

            	}/* else */
            
				if (xplus1[I][J] < 11.63) {
                  	twx[I][J]      = mu[I][J]*0.5*(v[I][j]+v[I][j+1])/(x[I] -x_u[i]);
                  	xplus1[I][J]  = sqrt(rho[I][J] * fabs(twx[I][J])) * (x[I] - x_u[i]) / mu[I][J];
                  	xplus[I][J]   = xplus1[I][J];
                  	vplus[I][J]   = xplus[I][J];
                  	
                  	Pr[I][J] = mu[I][J]*Cp[I][J]/0.025;		//Prandtl number laminar flow
					Pee[I][J] = 9.24*(pow((Pr[I][J]/sigma_t),0.75)-1)*(1+0.28*exp(-0.007*(Pr[I][J]/sigma_t)));

            	}/* if */
            	else {
            		//////// nog zelf doen :-)
                  	twx[I][J]      = rho[I][J]*pow(Cmu,0.25)*sqrt(k[I][J])*0.5*(v[I][j]+v[I][j+1])/vplus[I][J];
                  	xplus1[I][J]  = sqrt(rho[I][J] * fabs(twx[I][J])) * (x[I] - x_u[i]) / mu[I][J];
                  	xplus [I][J]  = xplus1[I][J];
                  	vplus [I][J]  = log(ERough*xplus[I][J])/kappa;
                  	
					Pr[I][J] = mu[I][J]*Cp[I][J]/0.025;		//Prandtl number laminar flow
					Pee[I][J] = 9.24*(pow((Pr[I][J]/sigma_t),0.75)-1)*(1+0.28*exp(-0.007*(Pr[I][J]/sigma_t)));
                 	Tplus[I][J]	  = sigma_t*(vplus[I][J]+Pee[I][J]);

            	}/* else */
            }
                  
        } /* for */

	} /* cuplus */
}
/* ################################################################# */
void viscosity(void)
/* ################################################################# */
{
/***** Purpose: Calculate the viscosity in the fluid as a function of temperature *****/
	int   I, J;

	for (I = 0; I <= NPI; I++)
		for (J = 1; J <= NPJ + 1; J++) {
            mut[I][J] = rho[I][J]*Cmu*sqr(k[I][J])/(eps[I][J]+SMALL);
			mueff[I][J] = mu[I][J] + mut[I][J];
      } /* for */


} /* viscosity */

/* ################################################################# */
void thermal_diffusivity(void)
/* ################################################################# */
{
/***** Purpose: Calculate the viscosity in the fluid as a function of temperature *****/
	int   I, J;

	for (I = 0; I <= NPI; I++)
		for (J = 1; J <= NPJ + 1; J++)
            Gamma[I][J] = 0.025/Cp[I][J] + mut[I][J];

} /* thermal_diffusivity */

/* ################################################################# */
void printConv(double time, int iter)
/* ################################################################# */
{
/***** Purpose: Creating result table ******/
	if (time == Dt)
		printf (" Time\t u\t v\t SMAX\t SAVG\n");

	printf (" %4d %10.3e\t%10.2e\t%10.2e\t%10.2e\t%10.2e\t%10.2e\n", 
              iter, time, u[3*NPI/10][2*NPJ/5], v[3*NPI/10][2*NPJ/5], T[3*NPI/10][2*NPJ/5], SMAX, SAVG);

} /* printConv */

/* ################################################################# */
void output(void)
/* ################################################################# */
{
/***** Purpose: Creating result table ******/
	int    I, J, i, j;
	double ugrid, vgrid,stream,vorticity;
	FILE   *fp, *str, *velu, *velv, *vort, *triang;

/* Plot all results in output.dat */

	fp = fopen("output.dat", "w");

	for (I = 0; I <= NPI; I++) {
		i = I;
		for (J = 1; J <= NPJ; J++) {
			j = J;
			ugrid = 0.5*(u[i][J]+u[i+1][J  ]);
			vgrid = 0.5*(v[I][j]+v[I  ][j+1]);
			fprintf(fp, "%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\t%11.5e\n",
			             x[I], y[J], ugrid, vgrid, p[I][J], T[I][J], rho[I][J], mu[I][J], Gamma[I][J], k[I][J], eps[I][J], uplus[I][J], yplus[I][J], yplus1[I][J], yplus2[I][J], tw[I][J], twx[I][J], mueff[I][J]);
//			             1     2     3      4      5        6        7          8         9            10       11         12           13           14            15				16		17			18
		} /* for J */
		fprintf(fp, "\n");
	} /* for I */

	fclose(fp);

/* Plot vorticity in vort.dat */

	vort = fopen("vort.dat", "w");

	for (I = 1; I <= NPI; I++) {
		i = I;
		for (J = 1; J <= NPJ; J++) {
			j = J;
			vorticity = (u[i][J] - u[i][J-1]) / (y[J] - y[J-1]) - (v[I][j] - v[I-1][j]) / (x[I] - x[I-1]);
			fprintf(vort, "%11.5e\t%11.5e\t%11.5e\n",
			             x[I], y[J], vorticity);
		} /* for J */
		fprintf(vort, "\n");
	} /* for I */

	fclose(vort);

/* Plot streamlines in str.dat */

	str = fopen("str.dat", "w");

	for (I = 0; I <= NPI; I++) {
		i = I;
		for (J = 0; J <= NPJ; J++) {
			j = J;
			stream = -0.5*(v[I+1][j]+v[I][j])*(x[I+1]-x[I])+0.5*(u[i][J+1]+u[i][J])*(y[J+1]-y[J]);
			fprintf(str, "%11.5e\t%11.5e\t%11.5e\n",
			             x[I], y[J], stream);
		} /* for J */
		fprintf(str,  "\n");
	} /* for I */

	fclose(str);

/* Plot horizontal velocity components in velu.dat */

	velu = fopen("velu.dat", "w");

	for (I = 1; I <= NPI+1; I++) {
		i = I;
		for (J = 0; J <= NPJ+1; J++) {
			fprintf(velu, "%11.5e\t%11.5e\t%11.5e\n",
			             x_u[i], y[J], u[i][J]);
		} /* for J */
		fprintf(velu, "\n");
	} /* for I */

	fclose(velu);

/* Plot vertical velocity components in velv.dat */

	velv = fopen("velv.dat", "w");

	for (I = 0; I <= NPI+1; I++) {
		for (J = 1; J <= NPJ+1; J++) {
			j = J;
			fprintf(velv, "%11.5e\t%11.5e\t%11.5e\n",
			             x[I], y_v[j], v[I][j]);
		} /* for J */
		fprintf(velv, "\n");
	} /* for I */

	fclose(velv);

} /* output */

/* ################################################################# */
int *int_1D_array(int np)
/* ################################################################# */
{
/* create an 1D array with size [np] of type int */
	int *a;

	a = (int *) calloc(np, sizeof(int));

	return a;

} /* int_1D_array */

/* ################################################################# */
double *double_1D_array(int np)
/* ################################################################# */
{
/* create an 1D array with size [np] of type double */
	double *a;

	a = (double *) calloc(np, sizeof(double));

	return a;

} /* double_1D_array */

/* ################################################################# */
double **double_2D_matrix (int nm, int np)
/* ################################################################# */
{
/* create an 2D matrix with size [nm, np] of type double */
	int i;
	double **m;

	m = (double **) calloc(nm, sizeof(double *));
	for ( i = 0; i < nm; i++)
		m[i] = (double *) calloc(np, sizeof(double));

	return m;

} /* double_2D_matrix */

/* ################################################################# */
void memalloc(void)
/* ################################################################# */
{
	x    = double_1D_array(NPI + 2);
	x_u  = double_1D_array(NPI + 2);
	y    = double_1D_array(NPJ + 2);
	y_v  = double_1D_array(NPJ + 2);
	Triangle_x = double_1D_array(21);
	Triangle_y = double_1D_array(21);
	TRX = int_1D_array(21);
	TRY = int_1D_array(21);
	Pr  = double_2D_matrix(NPI + 2, NPJ + 2);
	Pee  = double_2D_matrix(NPI + 2, NPJ + 2);
	Tplus = double_2D_matrix(NPI + 2, NPJ + 2);

	u      = double_2D_matrix(NPI + 2, NPJ + 2);
	v      = double_2D_matrix(NPI + 2, NPJ + 2);
	pc     = double_2D_matrix(NPI + 2, NPJ + 2);
	p      = double_2D_matrix(NPI + 2, NPJ + 2);
	T      = double_2D_matrix(NPI + 2, NPJ + 2);
	rho    = double_2D_matrix(NPI + 2, NPJ + 2);
	mu     = double_2D_matrix(NPI + 2, NPJ + 2);
	mut    = double_2D_matrix(NPI + 2, NPJ + 2);
	mueff  = double_2D_matrix(NPI + 2, NPJ + 2);
	Gamma  = double_2D_matrix(NPI + 2, NPJ + 2);
	Cp     = double_2D_matrix(NPI + 2, NPJ + 2);
	k      = double_2D_matrix(NPI + 2, NPJ + 2);
	eps    = double_2D_matrix(NPI + 2, NPJ + 2);
    delta  = double_2D_matrix(NPI + 2, NPJ + 2);
	E      = double_2D_matrix(NPI + 2, NPJ + 2);
	E2     = double_2D_matrix(NPI + 2, NPJ + 2);
	yplus  = double_2D_matrix(NPI + 2, NPJ + 2);
	yplus1 = double_2D_matrix(NPI + 2, NPJ + 2);
	yplus2 = double_2D_matrix(NPI + 2, NPJ + 2);
	uplus  = double_2D_matrix(NPI + 2, NPJ + 2);
	tw     = double_2D_matrix(NPI + 2, NPJ + 2);
	
	xplus  = double_2D_matrix(NPI + 2, NPJ + 2);
	xplus1 = double_2D_matrix(NPI + 2, NPJ + 2);
	xplus2 = double_2D_matrix(NPI + 2, NPJ + 2);
	vplus  = double_2D_matrix(NPI + 2, NPJ + 2);
	twx    = double_2D_matrix(NPI + 2, NPJ + 2);

	u_old  = double_2D_matrix(NPI + 2, NPJ + 2);
	v_old  = double_2D_matrix(NPI + 2, NPJ + 2);
	pc_old = double_2D_matrix(NPI + 2, NPJ + 2);
	T_old  = double_2D_matrix(NPI + 2, NPJ + 2);
	k_old  = double_2D_matrix(NPI + 2, NPJ + 2);
	eps_old= double_2D_matrix(NPI + 2, NPJ + 2);

	dudx   = double_2D_matrix(NPI + 2, NPJ + 2);
	dudy   = double_2D_matrix(NPI + 2, NPJ + 2);
	dvdx   = double_2D_matrix(NPI + 2, NPJ + 2);
	dvdy   = double_2D_matrix(NPI + 2, NPJ + 2);

	aP     = double_2D_matrix(NPI + 2, NPJ + 2);
	aE     = double_2D_matrix(NPI + 2, NPJ + 2);
	aW     = double_2D_matrix(NPI + 2, NPJ + 2);
	aN     = double_2D_matrix(NPI + 2, NPJ + 2);
	aS     = double_2D_matrix(NPI + 2, NPJ + 2);
	b      = double_2D_matrix(NPI + 2, NPJ + 2);

	SP     = double_2D_matrix(NPI + 2, NPJ + 2);
	Su     = double_2D_matrix(NPI + 2, NPJ + 2);

	F_u    = double_2D_matrix(NPI + 2, NPJ + 2);
	F_v    = double_2D_matrix(NPI + 2, NPJ + 2);

	d_u    = double_2D_matrix(NPI + 2, NPJ + 2);
	d_v    = double_2D_matrix(NPI + 2, NPJ + 2);

} /* memalloc */

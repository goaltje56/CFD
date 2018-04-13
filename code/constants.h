#define NPI        20       /* number of grid cells in x-direction [-] */
#define NPJ        40        /* number of grid cells in y-direction [-] */
#define XMAX       10.0      /* width of the domain [m] */ 
#define YMAX       0.2       /* height of the domain [m] */
#define Rbluff	   0.01      /* radius of the bluff body in the domain [m] */
#define PI         3.1415927 /* value of pi [-] */
#define MAX_ITER   100       /* maximum number of outer iterations [-] */
#define U_ITER     10         /* number of Newton iterations for u equation [-] */
#define V_ITER     10         /* number of Newton iterations for v equation [-] */
#define PC_ITER    30        /* number of Newton iterations for pc equation [-] */
#define T_ITER     1         /* number of Newton iterations for T equation [-] */
#define f_ITER     1         /* number of Newton iterations for f equation [-] */
#define EPS_ITER   50         /* number of Newton iterations for Eps equation [-] */
#define K_ITER     50         /* number of Newton iterations for K equation [-] */
#define SMAXneeded 1E-8      /* maximum accepted error in mass balance [kg/s] */
#define SAVGneeded 1E-9      /* maximum accepted average error in mass balance [kg/s] */
#define LARGE      1E30      /* arbitrary very large value [-] */
#define SMALL      1E-30     /* arbitrary very small value [-] */
#define P_ATM      101000.   /* athmospheric pressure [Pa] */
#define U_IN       1       /* in flow velocity [m/s] */

#define Cmu        0.09
#define sigmak       1.
#define sigmaeps     1.3
#define C1eps      1.44
#define C2eps      1.92
#define kappa      0.4187
#define ERough     9.793
#define Ti         0.01

//#define A			4			/*this are the points with x=10 y=20 */
//#define B			6
//#define C			8
//#define D			12


#define A			6			/*this are the initial points with x=20 y=40 */
#define B			10
#define C			9
#define D			15

#define A2			3			/*this are the initial points with x=20 y=40 */
#define B2			7
#define C2			2
#define D2			8


//#define A			16			/*this are the points with x=40 y=80 */
//#define B			24
//#define C			30
//#define D			50

//#define A			32			/*this are the points with x=80 y=160 */
//#define B			48
//#define C			60
//#define D			100

//#define A			64			/*this are the points with x=160 y=320 */
//#define B			96
//#define C			120
//#define D			200

//#define A			128			/*this are the points with x=320 y=640 */
//#define B			192			/* although it gives very accurate results */
//#define C			240			/* it is not recommended to use so many points */
//#define D			400			/* as the simulation time is very long */

#define sigma_t		0.9		/*turbulent Prandtl number */

#define AA			14	
#define BB			18
#define	CC			25	
#define DD			31	

#define AA2			11	
#define BB2			15
#define	CC2			32	
#define DD2			38

/* node-function_tsp.c
   Julian F. Miller (c) 2012

   defines functions set for CGP
   for TSP problems
*/

#include "cgp_warehouse.h"
#include <math.h>

/* all functions output numbers between 0 and 1.0
*/
//double  node_type(double in[MAX_NUM_GENES_PER_NODE],
//                  int function_gene)
//{
//   double result = 0.0, temp;
//
//   switch(function_gene)
//   {
//		case 0:  /*   sqrt  */
//			result = sqrt(in[0]);
//			break;
//		case 1:  /*  square  */
//			result = in[0]*in[0];
//			break;
//		case 2:	/* cube */
//			result = in[0]*in[0]*in[0];
//			break;
//		case 3:  /*  scaled exp  */
//			result = (exp(in[0])-1.0)/(exp(1.0)-1.0);
//			break;
//		case 4:  /*   abs(sin(a+b))  */
//			result = fabs(sin(in[0]+in[1]));
//			break;
//		case 5:  /*   abs(cos(a+b))  */
//			result = fabs(cos(in[0]+in[1]));
//			break;
//		case 6:  /*   abs(tanh(a+b)) */
//			result = fabs(tanh(in[0]+in[1]));
//			break;
//		case 7:  /*   hypot  */
//			result = sqrt(in[0]*in[0]+in[1]*in[1])/sqrt(2.0);
//			break;
//		case 8:  /*   addition  */
//			result = (in[0] + in[1])/2.0;
//			break;
//		case 9:	 /* symmetric subtraction */
//			if (in[0] > in[1])
//				result = (in[0] - in[1])/2.0;
//			else
//				result = (in[1] - in[0])/2.0;
//			break;
//		case 10: /* multiplication */
//			result = in[0]*in[1];
//			break;
//		case 11: /* squared multiplication */
//			temp = in[0]*in[1];
//			result = temp*temp;
//			break;
//		case 12: /* bounded division */
//			if (fabs(in[1]) < 1e-10)
//				result = 1.0;
//			else if (fabs(in[1]) > fabs(in[0]))
//				result = in[0]/in[1];
//			else
//				result = in[1]/in[0];
//   }
//
//   return result;
//}

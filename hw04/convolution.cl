#define INF 99999.0
#define EPS 1e-6

__kernel void df(__global float * a, __global float* b, int w, int h, int dist)
{
   int row = get_global_id(0);
   int col = get_global_id(1);

   if (row >= h || col >= w)
      return;

   if (a[row * w + col] - INF > EPS)
      return;

   int res = INF;
   int start_row = 0;
   int end_row = h;
   if (row > dist) start_row = row - dist;
   if (row + dist < h) end_row = row + dist;

   int start_col = 0;
   int end_col = w;
   if (col > dist) start_col = col - dist;
   if (col + dist < w) end_col = col + dist;

   for (int i = start_row; i < end_row; i++)
  {
       for (int j = start_col; j < end_col; j++)
       {
           if (!(i == row && j == col)){
               if (a[i * w + j] - 0.0 < EPS) {
                   res = min((float)res, pow((float)(row - i), 2.0) + pow((float)(col - j), 2.0));
               }
           }
       }
  }

   b[row * w + col] = sqrt((float)res);
}
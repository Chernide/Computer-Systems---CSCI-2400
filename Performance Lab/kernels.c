/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/*
 * Please fill in the following student struct:
 */
const team_t team = {
    "dech1905@colorado.edu", //Replace this with your email address.
    "jusa1470@colorado.edu"                   //Replace this with your partner's email address. Leave blank if working alone.
};

/***************
 * FLIP KERNEL
 ***************/

/******************************************************
 * Your different versions of the flip kernel go here
 ******************************************************/
 
 char first_try[] = "First try";
 void first(int dim, pixel *src, pixel *dst){
    int i, j;
    for (i = 0; i < dim; i++){
        for (j = 0; j < dim; j++){
            dst[RIDX_F(i, j, dim)] = src[RIDX(i, j, dim)];
        }
    }
 }

char second_try[] = "Second try: Unrolling & Blocking";
void second(int dim, pixel *src, pixel *dst) 
{
    int i, j, sin, din, ii, jj;
    for(ii=0;ii<dim;ii+=16){
        for(jj=0;jj<dim;jj+=16){
            for(i=ii;i<ii+16;i++){
                for(j=jj;j<jj+16;j+=4){
                    sin = i*dim+j;
                    din = (dim-1-i)*dim+(dim-1-j);
                    dst[din] = src[sin];
                    sin +=1;
                    din -=1;
                    dst[din] = src[sin];
                    sin +=1;
                    din -=1;
                    dst[din] = src[sin];
                    sin +=1;
                    din -=1;
                    dst[din] = src[sin];
                }
           }
        }
    } 
}
/* 
 * naive_flip - The naive baseline version of flip 
 */
char naive_flip_descr[] = "naive_flip: Naive baseline implementation";
void naive_flip(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    for (i = 0; i < dim; i++){
        for (j = 0; j < dim; j++){
            dst[RIDX_F(i, j, dim)].red = src[RIDX(i, j, dim)].red;
            dst[RIDX_F(i, j, dim)].green = src[RIDX(i, j, dim)].green;
            dst[RIDX_F(i, j, dim)].blue = src[RIDX(i, j, dim)].blue;
        }
    }
}

/* 
 * flip - Your current working version of flip
 * IMPORTANT: This is the version you will be graded on
 */
char flip_descr[] = "flip: Current working version- unrolling";
void flip(int dim, pixel *src, pixel *dst) 
{    
    int i, j, sin, din;
    for(i=0;i<dim;i++){
        for(j=0;j<dim;j+=2){
            sin = i*dim+j;
            din = (dim-1-i)*dim+(dim-1-j);
            dst[din] = src[sin];
            sin +=1;
            din -=1;
            dst[din] = src[sin];
        }
    }
  
}

/*********************************************************************
 * register_flip_functions - Register all of your different versions
 *     of the flip kernel with the driver by calling the
 *     add_flip_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_flip_functions() 
{
    add_flip_function(&flip, flip_descr);  
    add_flip_function(&first, first_try);
    add_flip_function(&second, second_try);
    add_flip_function(&naive_flip, naive_flip_descr);   
    /* ... Register additional test functions here */
}


/***************
 * CONVOLVE KERNEL
 **************/
 
/***************************************************************
 * Various typedefs and helper functions for the convolve function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute a pixel value */
typedef struct {
    float red;
    float green;
    float blue;
    float weight;
} pixel_sum;

/******************************************************
 * Your different versions of the convolve kernel go here
 ******************************************************/

/*
 * naive_convolve - The naive baseline version of convolve 
 */
char naive_convolve_descr[] = "naive_convolve: Naive baseline implementation";
void naive_convolve(int dim, pixel *src, pixel *dst) 
{
    int i, j, ii, jj, curI, curJ;
    pixel_sum ps;
    
    for (j = 0; j < dim; j++){
        for (i = 0; i < dim; i++){
            ps.red    = 0.0;
            ps.green  = 0.0;
            ps.blue   = 0.0;
            ps.weight = 0.0;
            for (jj = -2; jj <= 2; jj++){
                for (ii = -2; ii <= 2; ii++){
                    curJ = j+jj;
                    if(curJ<0 || curJ>=dim){
                        continue;
                    }
                    curI = i+ii;
                    if(curI<0 || curI>=dim){
                        continue;
                    }
                    ps.red   += src[RIDX(curI, curJ, dim)].red *   kernel[ii+2][jj+2];
                    ps.green += src[RIDX(curI, curJ, dim)].green * kernel[ii+2][jj+2];
                    ps.blue  += src[RIDX(curI, curJ, dim)].blue *  kernel[ii+2][jj+2];
                    ps.weight += kernel[ii+2][jj+2];
                }
            }
            dst[RIDX(i,j,dim)].red   = (unsigned short)(ps.red/ps.weight);
            dst[RIDX(i,j,dim)].green = (unsigned short)(ps.green/ps.weight);
            dst[RIDX(i,j,dim)].blue  = (unsigned short)(ps.blue/ps.weight);
        }
    }
}
void convolve_test_one(int dim, pixel *src, pixel *dst) 
{
    int i, j, ii, jj, curI, curJ, red, green, blue, weight;
    
    for (j = 0; j < dim; j++){
        for (i = 0; i < dim; i++){
            red = 0;
            green = 0;
            blue = 0;
            weight = 0;
            for (jj = -2; jj <= 2; jj++){
                for (ii = -2; ii <= 2; ii++){
                    int k = kernel[ii+2][jj+2];
                    curJ = j+jj;
                    if(curJ<0 || curJ>=dim){
                        continue;
                    }
                    curI = i+ii;
                    if(curI<0 || curI>=dim){
                        continue;
                    }
                    int x = curI * dim + curJ;
                    red   += src[x].red * k;
                    green += src[x].green * k;
                    blue  += src[x].blue *  k;
                    weight += k;
                }
            }
            int y = i * dim + j;
            dst[y].red   = (unsigned short)(red/weight);
            dst[y].green = (unsigned short)(green/weight);
            dst[y].blue  = (unsigned short)(blue/weight);
        }
    }
}

/*
 * convolve - Your current working version of convolve. 
 * IMPORTANT: This is the version you will be graded on
 */
char convolve_descr[] = "convolve: Current working version";
void convolve(int dim, pixel *src, pixel *dst) 
{
    naive_convolve(dim, src, dst);
}

char convolve_test_one_descr[] = "first opitmization attempt";
void convolve_2(int dim, pixel *src, pixel *dst){
    convolve_test_one(dim, src, dst);
}

/********************************************************************* 
 * register_convolve_functions - Register all of your different versions
 *     of the convolve kernel with the driver by calling the
 *     add_convolve_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_convolve_functions() {
    add_convolve_function(&convolve, convolve_descr);
    add_convolve_function(&convolve_test_one, convolve_test_one_descr);
    //add_convolve_function(&naive_convolve, naive_convolve_descr);
    /* ... Register additional test functions here */
}


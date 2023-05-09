/*
	Name - Jonathan Mathurin
	Panther ID - 6169303

	I affirm that I wrote this program myself without any help from any other
	people or sources from the internet.

	------------------------------------------------

    The Bmptool program is designed to manipulate 24-bit Bmp images. The bmptool contains
    a wide range of functions, enabling users to flip horizontally or vertically, rotate, and even
    enlarge a bmp image file.

    It combines these functions with getopt, allowing the user to mix and match combinations of the aforementioned
    functions. In addition, by inputting the output/input file name, the user can easily save the images as they
    please.

    Lastly, the user is able to chain these commands together with pipes.

	Usage: bmptool [-f | -r degrees |-s scale| -v ] [-o output_file] [input_file]\n"
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bmplib.h"
#include <string.h>
/*
 * This method enlarges a 24-bit, uncompressed .bmp file
 * that has been read in using readFile()
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the original number of rows
 * cols     - the original number of columns
 *
 * scale    - the multiplier applied to EACH OF the rows and columns, e.g.
 *           if scale=2, then 2* rows and 2*cols
 *
 * new      - the new array containing the PIXELs, allocated within
 * newrows  - the new number of rows (scale*rows)
 * newcols  - the new number of cols (scale*cols)
 */

int enlarge(PIXEL* original, int rows, int cols, int scale, PIXEL** new, int* newrows, int* newcols){


    int row, col;
    /* We have to first make sure that we update newrows and newcols to be equal
     * to the original row/col, but multiplied by the scale!
     *
     * Obviously, we then create the new image using the expanded dimensions! * */
    *newrows = rows * scale;
    *newcols = cols * scale;
    *new = (PIXEL*)malloc((*newcols)*(*newrows)*sizeof(PIXEL));

    /* This is a nested-for loop that works to create the expanded image.
     * In order to function properly, the for loop first grabs a pixel in the original
     * image file. From there, it goes to the new image, and prints the original pixel
     * scale * scale number of times.
     *
     * It does sequentially, first starting with the bottom column of the equivalent location,
     * then moving up a row as needed.
     *
     * The for loop is intelligent enough to know how to calculate the proper location in the new image.
     * Meaning that if the scale is 2, then the new image copies in a 2 x 2 pixel radius, then moves
     * by 2 columns/rows to continue copying. */
    for (row=0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            //Copy the original pixel.
            PIXEL *o = original + row*cols + col;

            //This for loop is what handles the enlargement.
            for(int nrow = 0; nrow < scale; nrow++){
                for(int ncol = 0; ncol < scale; ncol++){

                    /* Copies the original pixel in a scale x scale radius.
                     * In addition, for each new col/row in the original, we in the new image
                     * by a multiplication of scale. */
                    PIXEL *n = (*new) + (nrow+(scale*row))*(*newcols)+ ((col*scale)+ncol);
                    *n = *o;
                }
            }
        }
    }
    return 0;
}


/*
 * This method rotates a 24-bit, uncompressed .bmp file that has been read
 * in using readFile(). The rotation is expressed in degrees and can be
 * positive, negative, or 0 -- but it must be a multiple of 90 degrees
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 * rotation - a positive or negative rotation,
 *
 * new      - the new array containing the PIXELs, allocated within
 *
 */

int rotate(PIXEL* original, int rows, int cols, int rotation, PIXEL** new){

    /* Creating *tempPixel as a temporary structure so that we can continuously
     * rotate the image. TempPixel is necessary because for degrees greater than
     * 90, we have to continously spin the image. So Temp just stores the image from
     * new and repeats the process as many as times as necessary.
     * */

    PIXEL *tempPixel = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));
    /*
     * num_of_rotate - This variable is what we use to store the number of rotations needed.
     * */
    int row, col, num_of_rotate;

    //Divide by 90 to determine the number of turns we need.
    num_of_rotate = rotation / 90;

    /*
     * We know that every 4 turns will be 360 degrees, meaning no movement.
     * As such, we can reduce the number of turns needed by getting the remainder
     * of rotate % 4.
     *
     * Just in case our users want to use an insanely big number,
     * this reduces processing speed.
     * */
    num_of_rotate = num_of_rotate % 4;

    /*This for loop, we use simply to copy the original image
     * over to our temporary image.
     * */
    for (row = 0; row < rows; row++)
        for (col = 0; col < cols; col++) {
            PIXEL *o = original + row * cols + col;
            PIXEL *n = tempPixel + row * cols + col;
            *n = *o;
        }

    //Obviously if rows are nonexistent, cancel.
    if ((rows <= 0) || (cols <= 0)) return -1;
    *new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

    /* If the rotation is less than 0, then we know the rotation must be counter-clockwise. */
    if(num_of_rotate < 0) {

        //Set the rotation to a positive number so that our for loop works.
        num_of_rotate = (num_of_rotate) * -1;

        /* The code below is a series of nested for-loops.
         * They work by first rotating the temporary image, and copying it to
         * the new image. Once done, the second nested for-loop copies the new image
         * over to the temporary image -- this is done, simply so that when the loop reiterates (if needed),
         * we can continue to work off of the rotated version of the new image. */
        for (int i = 1; i <= num_of_rotate; i++) {

            //This loop does the rotation counter-clockwise.
            for (row = 0; row < rows; row++)
                for (col = 0; col < cols; col++) {
                    PIXEL *o = tempPixel + row * cols + col;
                    PIXEL *n = (*new) + (col * cols) + (cols - 1 - row);
                    *n = *o;
                }
            //This for loop is what makes sure the tempPixel is current.
            for (row = 0; row < rows; row++)
                for (col = 0; col < cols; col++) {
                    PIXEL *o = (*new) + row * cols + col;
                    PIXEL *n = tempPixel + row * cols + col;
                    *n = *o;
                }
        }
    }

        /* If the rotation is positive, then we know we have to do the rotation clockwise.
         * These sets of for-loops are virtually the same as the ones above, but it instead
         * turns 90 degrees in the opposite direction. */
    else if(num_of_rotate > 0){
        for (int i = 1; i <= num_of_rotate; i++) {
            for (row = 0; row < rows; row++)
                for (col = 0; col < cols; col++) {
                    PIXEL *o = tempPixel + row * cols + col;
                    PIXEL *n = (*new) + (rows*(cols-1-col)) + (row);//(cols - 1 - row);
                    *n = *o;
                }
            for (row = 0; row < rows; row++)
                for (col = 0; col < cols; col++) {
                    PIXEL *o = (*new) + row * cols + col;
                    PIXEL *n = tempPixel + row * cols + col;
                    *n = *o;
                }
        }
    }
    free(tempPixel);
    return 0;
}

/*
 * This method Vertically flips a 24-bit, uncompressed bmp file
 * that has been read in using readFile().
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 *
 * new      - the new array containing the PIXELs, allocated within
 */

int verticalflip (PIXEL *original, PIXEL **new, int rows, int cols) {
    int row, col;

    if ((rows <= 0) || (cols <= 0)) return -1;

    //Sets the new image.
    *new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

    /*
     *   int row, col;

  if ((rows <= 0) || (cols <= 0)) {
	 return -1;
}

  *new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

  for (row=0; row < rows; row++) {
    for (col=0; col < cols; col++) {
      PIXEL* o = original + row*cols + col;
      PIXEL* n = (*new) +(rows - 1 - row)*cols + col;
      *n = *o;
    }
  }

     * */
    /* The for loop iterates over each pixel in the original image.
     * For every pixel, the for loop finds the correspondent location in the new image
     * that would be the spot for a vertical flip. */
    for (row=0; row < rows; row++)
        for (col=0; col < cols; col++) {

            //Grabs the first pixel in the original (bottom left)
            PIXEL* o = original + row*cols + col;

            //Finds the appropriate vertical location, starting from top to the bottom.
            PIXEL* n = (*new) +(rows - 1 - row)*cols + col;
            PIXEL* n = (*new) + (rows-1-row)*cols + col;

            //Copies the original pixel to the new pixel
            *n = *o;
        }
    return 0;
}

/*
 * This method horizontally flips a 24-bit, uncompressed bmp file
 * that has been read in using readFile().
 *
 * THIS IS GIVEN TO YOU SOLELY TO LOOK AT AS AN EXAMPLE
 * TRY TO UNDERSTAND HOW IT WORKS
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 *
 * new      - the new array containing the PIXELs, allocated within
 */
int flip (PIXEL *original, PIXEL **new, int rows, int cols) {
    int row, col;

    if ((rows <= 0) || (cols <= 0)) return -1;

    *new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

    /* This for loop basically just grabs a pixel in the original,
     * then it places it at the opposite of the new image.
     *
     * This is done repeatedly until the image is flipped horizontally.*/
    for (row=0; row < rows; row++)
        for (col=0; col < cols; col++) {
            PIXEL* o = original + row*cols + col;

            /* Basically, the new spot that the pixel is being copied to will always be
             * in the inverse location. */
            PIXEL* n = (*new) + row*cols + (cols-1-col);
            *n = *o;
        }
    return 0;
}

/* The copy function is utilized in order to copy one image into the next.
 * This is useful for ensuring that in the getopt operations below, the
 * image being modified is always current. */

int copy (PIXEL *original, PIXEL **copy, int rows, int cols) {
    int row, col;

    if ((rows <= 0) || (cols <= 0)) return -1;

    *copy = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

    /* A simple for loop that grabs a pixel in the original image,
     * then it copies it in the exact spot of the image that the copy
     * is being produced in.*/
    for (row=0; row < rows; row++)
        for (col=0; col < cols; col++) {
            PIXEL* o = original + row*cols + col;
            PIXEL* n = (*copy) + row*cols + col;
            *n = *o;
        }
    return 0;
}

int main(int argc, char **argv){

    //These are all the variables we'll need for our image manipulation.
    int r, c, n_row, n_col;
    PIXEL *b, *nb;

    /* These are just the typical getopt variables. */
    extern char *optarg;
    extern int optind;
    int fflag=0, rflag=0, sflag = 0, vflag=0, oflag = 0;

    /* degrees  - Stores the value from the rflag.
     * scale    - Stores the value from the sflag. */
    int degrees, scale;
    int d, err = 0;
    /* These values are all used to parse integers, or in case of oname
     * it is used to keep track of output file name. */
    char *rname, *sname, *oname;

    //Used for the name of the input file.
    char *input_file;

    static char usage[] = "usage: bmptool [-f | -r degrees |-s scale| -v ] [-o output_file] [input_file]\n";

    /* Getopt while loop to parse the command line. */
    while ((d = getopt(argc, argv, "fr:s:vo:")) != -1)
        switch (d) {
            case 'f':
                fflag = 1;
                break;
            case 'r':
                rflag = 1;
                rname = optarg;
                /* A basic sanity check to see if atoi returns a number. If it doesn't return a number
                 * run the error code! */
                if(atoi(rname) == 0) {
                    printf("%s", usage);
                    printf("ERROR - the r flag can only take back an integer!\n");
                    printf("Please resubmit the command following the usage case!\n");
                    exit(1);
                }
                degrees = atoi(rname);

                /* Remember, degrees MUST be divisible by 90. */
                if(degrees % 90 != 0){
                    printf("%s", usage);
                    printf("ERROR - the r flag can only be multiples of 90!\n");
                    printf("Please resubmit the command following the usage case!\n");
                    exit(1);
                }
                break;
            case 's':
                sflag = 1;
                sname = optarg;
                /*Sanity check to ensure that this is an integer.*/
                if(atoi(sname) == 0){
                    printf("%s", usage);
                    printf("ERROR - the s flag can only take back an integer!\n");
                    printf("Please resubmit the command following the usage case!\n");
                    exit(1);
                }
                scale = atoi(sname);
                /* Remember, the scale MUST be between 2 and 3. */
                if(scale < 2 || scale > 3){
                    printf("%s", usage);
                    printf("ERROR - Scale must be either 2 or 3!\n");
                    printf("Please resubmit the command following the usage case!\n");
                    exit(1);
                }
                break;
            case 'v':
                vflag = 1;
                break;
            case 'o':
                oflag = 1;
                oname = optarg;
                break;
            case '?':
                err = 1;
                break;
        }
    if (err) {
        fprintf(stderr, usage, argv[0]);
        exit(1);
    }

    /*If the input is not null. . */
    if(argv[optind] != NULL){
        input_file = argv[optind];
        /* Read the name of the input. If you don't locate
         * .bmp, then it ain't a valid input. Toss an error!*/
        if(!(strstr(input_file, ".bmp"))){
            printf("ERROR - Only bmp files are accepted!\n\n");
            printf("%s", usage);
            exit(1);
        }

        //Otherwise, read the file.
        readFile(input_file, &r, &c, &b);
    }

    else{
        /* If there isn't an input, then the file is NULL.
         * This allows us to read in stdin. */
        readFile(NULL, &r, &c, &b);
    }

    /* Check to see if the sflag is active.
     *  If it is, run the enlarge code! */
    if(sflag){
        enlarge(b, r, c, scale, &nb, &n_row, &n_col);

        //Make sure to keep row and cols updated.
        r = n_row;
        c = n_col;

        /* Copy the new image over to b, since we may need to continue running operations
         * on nb. As such, b is kept as a mutable copy from which nb is continuously overwritten
         * with. */
        copy(nb, &b, r, c);
    }

    /* Run the rotation. Keep a copy of the changes.*/
    if(rflag){
        rotate(b, r, c, degrees, &nb);
        copy(nb, &b, r, c);
    }

    /* Runs the vertical flip if vflag is active. */
    if(vflag){
        verticalflip(b, &nb, r, c);
        copy(nb, &b, r, c);
    }

    /*Then the horizontal flip. */
    if(fflag){
        flip (b, &nb, r, c);
        copy(nb, &b, r, c);
    }

    /*If we have an oflag, then copy it's name as the written file.*/
    if(oflag){

        //We make sure to write nb, as that is what will have the most updated change no matter what.
        writeFile(oname, r, c, nb);
    }

    else {
        //If oflag isn't active, then we write to stdout.
        writeFile(NULL, r, c, nb);
    }

    //Freeing up the memory now that we're done!
    free(b);
    free(nb);

    return 0;

}

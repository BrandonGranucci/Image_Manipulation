#include <stdio.h>
#include <stdlib.h>

int width,height,truewidth = 0;
int firstcharpos,firstwidthcharpos,firstheightcharpos;
long filesize;
unsigned char widthsize[4];
unsigned char heightsize[4];
int digitsinwidth=0;
int digitsinheight=0;

unsigned char * getFile(char *argv[]) {
    unsigned char * buffer;
    FILE * fileptr;

    /* Open the file */
    fileptr = fopen(argv[1],"rb");

    /* Acquire file length */
    fseek(fileptr, 0, SEEK_END);
    filesize = ftell(fileptr);
    rewind(fileptr);

    /* Get dynamic storage for all chars of the file */
    buffer = (unsigned char*) malloc(sizeof(char)*filesize);

    /* Copy buffer to arr */
    fread(buffer, 1, filesize, fileptr);

    /* Close file  */
    fclose(fileptr);

    return buffer;
}

/* Obtain the width, height, and firstcharpos */
void getParameters(unsigned char * buffer) {

    /* Check for signature P6 at beginning of file */
    if (buffer[0] == 80 && buffer[1] == 54){

        /* Position after P6 signature */
        int i = 2;

        /* Skip over anything thats not a actual 0-9 number
           till the first width value is reached*/
        while (buffer[i] < 48 || buffer[i] > 57) {
            i++;
        }
        firstwidthcharpos=i;
        /* Acquire the number of columns or width */
        int sizecount = 0;
        while (buffer[i] > 47 && buffer[i] < 58) {
            widthsize[sizecount] = buffer[i];
            sizecount++;
            i++;
            digitsinwidth++;
        }

        /* Convert digits to whole number */
        sscanf(widthsize, "%d", &width);
        
        /* Actual number of width is 3x because each 
        pixel has 1 Red, 1 Green, And 1 Blue Value */
        truewidth=width*3;

        /* Skip whitespace between width and height */
        while (buffer[i] < 48 || buffer[i] > 57) {
            i++;
        }
        firstheightcharpos=i;

        /* Acquire the number of rows or height */
        sizecount = 0;
        while (buffer[i] > 47 && buffer[i] < 58){
            heightsize[sizecount] = buffer[i];
            sizecount++;
            i++;
            digitsinheight++;
        }

        /* Convert digits to whole number */
        sscanf(heightsize, "%d", &height);

        /* Skip whitespace between height and 255 */
        while (buffer[i] < 48 || buffer[i] > 57) {
            i++;
        }
        /* Put in place of first char, skips 255\n First value should be 13 or carriage return*/
        i += 4;
        firstcharpos = i;
    }
    else {
        printf("Improper file signature.\n");
        exit(0);
    }
}

void negative(unsigned char * pixels) {
    int i;
    for(i = 0; i < height*truewidth; i++) {
        pixels[i] = ~(pixels[i]);
    }
}

void flipVertical(unsigned char * pixels) {
    int i, column,topval,botval,temp;
    for (column=0;column<truewidth;column++) {
        topval=column;
        botval=(truewidth*height-truewidth)+column;
        for(i=0;i<height/2;i++) {
            temp = pixels[topval];
            pixels[topval] = pixels[botval];
            pixels[botval] = temp;
            topval += truewidth;
            botval -= truewidth;
        }
    }
}

void flipHorizontal(unsigned char * pixels) {
    int i, row, leftval, rightval;
    int rightpixel[3];
    for (row=0;row<height;row++) {
        if(row!=0){
            leftval=truewidth*row-1;
        }
        else{
            leftval=0;
        }
        /* Rightmost pixel's R value from RGB */
        rightval=(truewidth*(row+1))-4;
        for(i=0;i<truewidth/6;i++) {
            rightpixel[0]=pixels[rightval];
            rightpixel[1]=pixels[rightval+1];
            rightpixel[2]=pixels[rightval+2];            
            pixels[rightval]=pixels[leftval];
            pixels[rightval+1]=pixels[leftval+1];
            pixels[rightval+2]=pixels[leftval+2];
            pixels[leftval]=rightpixel[0];
            pixels[leftval+1]=rightpixel[1];
            pixels[leftval+2]=rightpixel[2];
            leftval+=3;
            rightval-=3;
        }
    }
}

void rotateRight(unsigned char * pixels) {
    int row,rowval,i,col,changingcol;
    unsigned char * pixels_copy = malloc((height * truewidth) * sizeof(unsigned char));
    /* Create a copy of pixels */
    for (i = 0; i < height*truewidth; i++) {
        pixels_copy[i] = pixels[i];
    }
    /* Algo for Original arr: Start from Bottom Left Column and Read Up 
    and Once Top is Reached, Move to Right Neighboring Column*/

    /* Algo for New arr: New Values are placed with Simple Iteration
    through all indices, from 0 incrementing by 1 till last index*/
    col=-3;
    for(row=truewidth*(height-1);row>-1;row-=truewidth) {
        col+=3;
        changingcol=col;
        for(rowval=row;rowval<row+truewidth;rowval+=3) {
            pixels[changingcol]=pixels_copy[rowval];
            pixels[changingcol+1]=pixels_copy[rowval+1];
            pixels[changingcol+2]=pixels_copy[rowval+2];
            changingcol += height*3;
        }
    }
    free(pixels_copy);
}

void darken(unsigned char * pixels) {
    int i;
    for(i = 0; i < height*truewidth; i++) {
        pixels[i] = (pixels[i])>>1;
    }
}

void darkenReds(unsigned char * pixels) {
    int i;
    for(i = 0; i < height*truewidth; i+=3) {
        pixels[i] = (pixels[i])>>1;
    }
}

void darkenGreens(unsigned char * pixels) {
    int i;
    for(i = 1; i < height*truewidth; i+=3) {
        pixels[i] = (pixels[i])>>1;
    }
}

void darkenBlues(unsigned char * pixels) {
    int i;
    for(i = 2; i < height*truewidth; i+=3) {
        pixels[i] = (pixels[i])<<1;
    }
}

void brighten(unsigned char * pixels) {
    int i;
    for(i = 0; i < height*truewidth; i++) {
        if(pixels[i]<<1 > 255){
            pixels[i]=(unsigned char) 255;
        }
        else {
            pixels[i] = (pixels[i])<<1;
        }
    }
}

void brightenReds(unsigned char * pixels) {
    int i;
    for(i = 0; i < height*truewidth; i+=3) {
        if(pixels[i]<<1 > 255){
            pixels[i]=(unsigned char) 255;
        }
        else {
            pixels[i] = (pixels[i])<<1;
        }
    }
}

void brightenGreens(unsigned char * pixels) {
    int i;
    for(i = 1; i < height*truewidth; i+=3) {
        if(pixels[i]<<1 > 255){
            pixels[i]=(unsigned char) 255;
        }
        else {
            pixels[i] = (pixels[i])<<1;
        }
    }
}

void brightenBlues(unsigned char * pixels) {
    int i;
    for(i = 2; i < height*truewidth; i+=3) {
        if(pixels[i]<<1 > 255){
            pixels[i]=(unsigned char) 255;
        }
        else {
            pixels[i] = (pixels[i])<<1;
        }
    }
}

void monochrome(unsigned char * pixels) {
    int i,average;
    for(i = 0; i < height*truewidth; i+=3) {
        average=(pixels[i]+pixels[i+1]+pixels[i+2])/3;
        pixels[i] = (unsigned char)average;
        pixels[i+1] = (unsigned char)average;
        pixels[i+2] = (unsigned char)average;
    }
}

void undoAllChanges(unsigned char * buffer, unsigned char * pixels) {
    int i;
    for (i = 0; i < height*truewidth; i++) {
        pixels[i] = buffer[firstcharpos+i];
    }
}

void saveFile(unsigned char * buffer, unsigned char * pixels) {
    int i;
    for (i=0;i<height*truewidth; i++) {
        buffer[i+firstcharpos] = pixels[i];
    }
    FILE *write_file = fopen("output.ppm","wb");
    fwrite(buffer,filesize, 1,write_file);
    fclose(write_file);
}


void updateBufferParams(unsigned char * buffer) {
    int r,startatzero;
    /* Swap height and width in buffer */
    for(r=firstwidthcharpos,startatzero=0;startatzero<digitsinheight;r++,startatzero++){
        buffer[r]=heightsize[startatzero];
    }
    buffer[r]=(unsigned char)32;
    for(r=r+1,startatzero=0;startatzero<digitsinwidth;r++,startatzero++){
        buffer[r]=widthsize[startatzero];
    } 
    
    /* Swap saved height and width values in respective array*/
    unsigned char temp[4];
    for(r=0;r<digitsinheight;r++){
        temp[r]=heightsize[r];
    }
    for(r=0;r<digitsinwidth;r++){
        heightsize[r]=widthsize[r];
    } 
    for(r=0;r<digitsinheight;r++){
        widthsize[r]=temp[r];
    }
}

void swapHeightAndWidth(){
    int temp;
    temp=width;
    width=height;
    height=temp;
    truewidth=width*3;
    temp=digitsinheight;
    digitsinheight=digitsinwidth;
    digitsinwidth=temp;
}

void printOptions() {
    printf("Options Menu:\n");
    printf(" 1) Flip Vertically\n");
    printf(" 2) Flip Horizontally\n");
    printf(" 3) Negative (invert colors)\n");
    printf(" 4) Rotate Right (Clockwise)\n");
    printf(" 5) Rotate Left (Counter-Clockwise)\n");
    printf(" 6) Brighten\n");
    printf(" 7) Increase Color Red\n");
    printf(" 8) Increase Color Green\n");
    printf(" 9) Increase Color Blue\n");
    printf(" 10) Darken\n");
    printf(" 11) Decrease Color Red\n");
    printf(" 12) Decrease Color Green\n");
    printf(" 13) Decrease Color Blue\n");
    printf(" 14) Convert to Monochrome (Black and White)\n");
    printf(" 15) Undo All Changes\n");
    printf(" 16) Save Changes and Quit\n");
}

int main(int argc, char *argv[]) {
    int i;

    if (argc != 2) {
        printf("Improper number of command line arguments.\n");
        exit(0);
    }

    /* Get ppm files and acquire parameters */
    unsigned char * buffer = getFile(argv);
    getParameters(buffer);

    /* Create an array of the pixels */
    unsigned char * pixels = malloc((height * truewidth) * sizeof(unsigned char));
    for (i = 0; i < height*truewidth; i++) {
        pixels[i] = buffer[firstcharpos+i];
    }
    
    /* Loop thru all possible choices until exit */
    int userchoice;
    printOptions();
    while (scanf("%d", &userchoice) != -1) {
        switch(userchoice) {
            case 1:
                printf("Flipped Vertically\n");
                flipVertical(pixels);
                printOptions();
                break;
            case 2:
                printf("Flipped Horizontally\n");
                flipHorizontal(pixels);
                printOptions();
                break;
            case 3:
                printf("Making Image Negative\n");
                negative(pixels);
                printOptions();
                break;
            case 4:
                printf("Rotating Image Right\n");
                rotateRight(pixels);
                updateBufferParams(buffer);
                swapHeightAndWidth();
                printOptions();
                break;
            case 5:
                printf("Rotating Image Left\n");
                for(i=0;i<3;i++) {
                    rotateRight(pixels);
                    updateBufferParams(buffer);
                    swapHeightAndWidth();
                }
                printOptions();
                break;
            case 6:
                printf("Making Image Brighter\n");
                brighten(pixels);
                printOptions();
                break;
            case 7:
                printf("Making Image More Red\n");
                brightenReds(pixels);
                printOptions();
                break;
            case 8:
                printf("Making Image More Green\n");
                brightenGreens(pixels);
                printOptions();
                break;
            case 9:
                printf("Making Image More Blue\n");
                brightenBlues(pixels);
                printOptions();
                break;
            case 10:
                printf("Making Image Darker\n");
                darken(pixels);
                printOptions();
                break;
            case 11:
                printf("Making Image Less Red\n");
                darkenReds(pixels);
                printOptions();
                break;
            case 12:
                printf("Making Image Less Blue\n");
                darkenBlues(pixels);
                printOptions();
                break;
            case 13:
                printf("Making Image Less Green\n");
                darkenGreens(pixels);
                printOptions();
                break;
            case 14:
                printf("Making Image Monochrome\n");
                monochrome(pixels);
                printOptions();
                break;
            case 15:
                printf("Reversing All Changes\n");
                undoAllChanges(buffer,pixels);
                printOptions();
                break;
            case 16:
                printf("Saving Changes and Quiting\n");
                saveFile(buffer,pixels);
                free(buffer);
                free(pixels);
                exit(0);
            default:
                printf("Your input doesn't match, retry\n");
                printOptions();
        }
    }
    free(buffer);
    free(pixels);
    printf("Abnormal Exit, Output File Not Saved\n");
    exit(0);
}

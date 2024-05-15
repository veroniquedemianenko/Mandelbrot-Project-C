/* Travail réalisé par Benabdesadok Nayel et Demianenko Véronique */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define STRMAX 256

double map(int v, int imin, int imax, double omin, double omax);
int a;

int Py;
int Px;

struct camera {
    float coord_x ;
    float coord_y ; 
    float w ; 
    float h ;
    int width;
    int height;
} ; 

struct pixdiv {
    int iter;
    double x;
    double y;
} ;

struct render {
    float xmax;
    float ymax;
    float xmin;
    float ymin;
    int n_iter;
    float rayon;
    int W;
    int H;
    char basename[STRMAX];

    struct pixdiv *img;
    struct camera  pov;
} ;

void cam2rect(struct render *set, struct camera *pov, char *argv[]) {
    sscanf(argv[2],"%f,%f,%f,%f",&pov->coord_x,&pov->coord_y,&pov->w,&pov->h);

    set->xmax = pov->coord_x + pov->w/2 ; 
    set->ymax = pov->coord_y + pov->h/2 ; 
    set->xmin = pov->coord_x - pov->w/2 ; 
    set->ymin = pov->coord_y - pov->h/2 ; 
} 

int m2v(struct render *set, int x, int y) {
    return x+set->W*y;
}

void render_init(struct render *set, int argc, char *argv[]) {
    set->rayon = 2;
    
    if ( set->img == NULL ) {
        printf("Erreur d'allocation mémoire !\n") ;
        exit(0) ; 
    }
    if (argc>=2){
        set->n_iter = atol (argv[1]);
    }
    if (argc>=4) {
        sscanf(argv[3], "%dx%d", &set->W, &set->H);
    }
    if (argc>=5) {
        strcpy(set->basename,argv[4]);
    }
    set->img=malloc(set->W*set->H*sizeof(struct pixdiv));
}

void test_argument (int argc, char *argv[]){
    float coord_x;
    float coord_y;
    float h;
    float w;
    int height, width;

    if (atol(argv[1])==0 && argc >= 2){
        printf ("ATTENTION : le type de variable pour le premier argument est un int \n");
        exit (0);
    }
    if (argc >= 3){
        if (sscanf (argv[2],"%f,%f,%f,%f",&coord_x,&coord_y,&w,&h)!=4){
            printf("ATTENTION : il faut écrire 4 flottants séparés par une virgule sous forme de chaîne de caractère\n");
            exit(0);
        }
    }
    if (argc >= 4){
        if (sscanf (argv[3], "%dx%d", &width, &height) != 2 ){
            printf("ATTENTION: il faut pour le troisième argument deux entiers séparés par x \n");
            exit(0);
        }
    }
}

void render_image(struct render *set){
    int i=0;
    double x0= 0;
    double y0 =0;
    double xtemp;

    for (Py = 0 ; Py < set->W; Py++) {
        for (Px=0; Px < set->H; Px++) {

            x0=map(Px,0, set->W, set->xmin, set->xmax);
            y0=map(Py,0, set->H, set->ymin, set->ymax);
            set->img[0].x=x0;
            set->img[0].y=y0;
            xtemp=0;
            i=0;

            while (set->img[i].x*set->img[i].x+set->img[i].y*set->img[i].y <= (set->rayon)*(set->rayon) && i < set->n_iter ) {
                xtemp= set->img[i].x;
                set->img[i].x= set->img[i].x*set->img[i].x - set->img[i].y*set->img[i].y + x0;
                set->img[i].y = 2*set->img[i].y*xtemp + y0;
                i++;
            }
            set->img[m2v(set,Px,Py)].iter=i;
        }
        printf ("\r Numéro ligne en cours d'éxécution: %d ",Py) ;
    }
}

void image_save_bw(struct render *set){

    FILE *fout; 
    printf("%d,%d \n",set->W,set->H);

    fout = fopen("mandel.pbm", "w");
    if ( fout == NULL ) {
        printf("Impossible d'ouvrir le fichier %s\n", "mandel.pbm") ;
    } 
    else {
        printf("Écriture dans le fichier %s\n", "mandel.pbm") ;
    }

    fprintf(fout, "P1\n");
    fprintf(fout, "%d %d \n", set->W, set->H);

    for (Py = 0 ; Py < set->H; Py++) {
        for (Px=0; Px < set->W; Px++) {
            if (set->img[m2v(set,Px,Py)].iter==set->n_iter) { 
                fprintf(fout,"%d ",1); 
            }
            else {
                fprintf(fout,"%d ",0);
            }      
        }   
    }
    fclose(fout) ;
}

void image_save_altern(struct render *set){
    int i=0;

    FILE *fout; 
    printf("%d,%d \n",set->W,set->H);

    fout = fopen("mandel.pbm", "w");
    if ( fout == NULL ) {
        printf("Impossible d'ouvrir le fichier %s\n", "mandel.pbm") ;
    } 
    else {
        printf("Écriture dans le fichier %s\n", "mandel.pbm") ;
    }
    
    fprintf(fout, "P1\n");
    fprintf(fout, "%d %d \n", set->W, set->H);

    for (Py = 0 ; Py < set->W; Py++) {
        for (Px=0; Px < set->H; Px++) {
            if(set->img[m2v(set,Px,Py)].iter==set->n_iter) {
                    fprintf(fout,"%d ",1 ) ;
            }
            else {
                if(set->img[m2v(set,Px,Py)].iter%2==0) {
                    fprintf(fout,"%d ",0);
                }
                else {
                    fprintf(fout,"%d ",1);
                }
            } 
        i++;
        }   
    }
    fclose(fout) ;
}

void image_save_grey(struct render *set) {
    char tableau[STRMAX];
    int i;
    int j;
    int compt;

    FILE *fout;

    strcpy(tableau, set->basename);
    strcat(tableau, ".pgm");
    fout = fopen(tableau,"w");
    if (fout == NULL ){
        exit(0);
    }
    else {
        fprintf(fout,"P2\n");
        fprintf(fout,"%d %d\n",set->W, set->H);
        fprintf(fout,"%d\n",set->n_iter);
    
        for (i=0; i<set->H;i++) {
            for (j=0;j<set->W; j++) {
                if (set->img[m2v(set,i,j)].iter==set->n_iter) {
                    fprintf(fout,"0 ");
                }
                else {
                    fprintf(fout, "%d ",set->img[m2v(set,i,j)].iter);

                }
                compt+=1;
                if (compt==70) {
                    fprintf(fout,"\n");
                    compt=0;
                }
            }
        }
    }
    fclose(fout);
}

void image_save_grey_smoothed(struct render *set) {
    float grey;
    float x,y;
    int n_it;
    int i,j;
    int compt;
    char tableau[STRMAX];
    
    FILE *fout;

    strcpy(tableau, set->basename);
    strcat(tableau, ".pgm");
    fout = fopen(tableau,"w");

    if (fout == NULL ){
        exit(0);
    }
    else {
        fprintf(fout,"P2\n");
        fprintf(fout,"%d %d\n",set->W, set->H);
        fprintf(fout,"%d\n",set->n_iter);
    }

    for (i=0; i<set->H;i++) {
        for (j=0;j<set->W; j++) {
            if (set->img[m2v(set,j,i)].iter<=set->n_iter) {
                x=set->img[m2v(set,j,i)].x;
                y=set->img[m2v(set,j,i)].y;
                n_it=set->img[m2v(set,j,i)].iter;
                grey = 5+ n_it - log(log(x*x+y*y)/log(2))/log(2) ;
                grey=floor(512*grey/set->n_iter);

                if (grey > 255) {
                grey=255;       /* on rencontre un problème au niveau des valeurs de grey dans le mandel.pgm */
                }
                fprintf(fout, "%d ",(int)grey);
            }
            else {
                fprintf(fout, "%d ", 1);
            }
            compt+=1;
            if (compt==70) {
                fprintf(fout,"\n");
                compt=0;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    struct render set;
    struct camera pov;
    
    test_argument (argc, argv);
    cam2rect(&set,&pov, argv);
    render_init(&set,argc,argv);
    render_image(&set);
    image_save_grey_smoothed(&set);

    free(set.img);
    
    return 0;
}

double map(int v, int imin, int imax, double omin, double omax) {
    return v*(omax-omin)/(imax-imin) +omin;
}

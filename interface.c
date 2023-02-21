#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <signal.h>

#include <time.h> 
#include <locale.h>


//#include "graphics.c"
//#include "graphics.h"
//#include "gnuplot_i/src/gnuplot_i.h"
//#include "gnuplot_i/src/gnuplot_i.c"

//#include <pthread.h>     /* pthread functions and data structures */

void clear(){
    FILE *clear=popen("clear","w");
    pclose(clear);
}

static void sighandler(int signo)
{
	if (signo == SIGINT)
	{
        clear();
		printf("\nProgram quit by user - Goodbye!\n\n");
		exit(0);
	}
}

typedef struct {
    int item_num;
    int date;
    double price;
}stock_entry;

typedef struct {
    int num_items;
    stock_entry *dataset_entries;
    char *name;
}stock_dataset;

typedef struct {
    int num_sets;
    stock_dataset *datasets;
}dataset_bundle;

double gen_normal_rand_sample(){
    double ans;
    double a=((double)rand()/(double)RAND_MAX);
    double b=((double)rand()/(double)RAND_MAX);
    ans=(sqrt(-2*(log(a))))*(sin(2*M_PI*b));
    return ans;
}

void print_stock_dataset(stock_dataset dataset, char *print_type){
    int i;
    if(!strcmp(print_type,"date")){
        for(i=0;i<dataset.num_items;i++){
            printf("%d - %lf\n",dataset.dataset_entries[i].date,dataset.dataset_entries[i].price);
        }
    }
    else if(!strcmp(print_type,"item_num")){
        for(i=0;i<dataset.num_items;i++){
            printf("%d - %lf\n",dataset.dataset_entries[i].item_num,dataset.dataset_entries[i].price);
        }
    }
    else{
        printf("\nSomething went wrong!\n\n");
    }
}

stock_dataset ingest_dataset(char *path, char *ingest_format, int lines_to_skip){
    int upper=125;
    stock_dataset dataset;
    int n=0;
    dataset.dataset_entries=calloc(upper,sizeof(stock_entry));
    //Open input file
    FILE *fp = fopen(path,"r");
    char buffer[64];
    int i;
    dataset.name=malloc(strlen(path)+1);
    strcpy(dataset.name,path);
    for(i=0;i<lines_to_skip;i++){
        fgets(buffer, sizeof(buffer), fp);
    }
    while(fgets(buffer, sizeof(buffer), fp)){
        if(buffer){
            int date;
            double p;
            sscanf(buffer, ingest_format, &date, &p);
            //Enlarge struct array if needed and assign vars
            if(!(n<upper)){
                upper*=2;
                dataset.dataset_entries=realloc(dataset.dataset_entries,upper*sizeof(stock_entry));
            }  
            dataset.dataset_entries[n].date=date;
            dataset.dataset_entries[n].price=p;
            dataset.dataset_entries[n].item_num=n;
            n+=1;
        }
    }
    fclose(fp);
    dataset.dataset_entries=realloc(dataset.dataset_entries,(n)*sizeof(stock_entry));
    dataset.num_items=n;
    return dataset;
}

stock_dataset gen_stock_dataset(int n, double initial_price, double growth_mean, double growth_std_dev){
    stock_dataset dataset;
    dataset.num_items=n;
    dataset.dataset_entries=calloc(n,sizeof(stock_entry));

    double daily_std_dev=(growth_std_dev/250);
    //printf("\nDaily Std Dev: %lf\n\n",daily_std_dev);
    double daily_growth=(pow(growth_mean+1,1/250.0))-1;
    //printf("\nDaily growth target calculated to be:%lf\n\n",daily_growth);

    int i=0;
    dataset.dataset_entries[i].item_num=i;
    dataset.dataset_entries[i].price=initial_price;
    for(i=1;i<n;i++){
        //double rand_price=dataset.dataset_entries[i-1].price*(1+(gen_normal_rand_sample()*daily_std_dev)+daily_growth);
        double rand_price=dataset.dataset_entries[i-1].price*(1+gen_normal_rand_sample()*daily_std_dev+daily_growth);
        //printf("Price generates: %lf\n",rand_price);
        dataset.dataset_entries[i].item_num=i;
        dataset.dataset_entries[i].price=rand_price;
    }
    /*
    printf("\nPrinting Dataset that was made:\n");
    for(i=0;i<n;i++){
        printf("i: %d - price: %lf\n",dataset.dataset_entries[i].item_num,dataset.dataset_entries[i].price);
    }
    printf("Finished Printing!\n");
    */
    return dataset;
}

double calc_database_yearly_returns(stock_dataset dataset){
    int items=dataset.num_items;
    double init_price=dataset.dataset_entries[0].price;
    double final_price=dataset.dataset_entries[items-1].price;
    double calculated_return=(final_price-init_price)/init_price;
    calculated_return=(pow(calculated_return+1,1/(double)items))-1;
    calculated_return=(pow(1+calculated_return,250))-1;
    //printf("calculated_return: %lf\n",calculated_return);
    return calculated_return;
}

double calc_database_total_returns(stock_dataset dataset){
    int items=dataset.num_items;
    double init_price=dataset.dataset_entries[0].price;
    double final_price=dataset.dataset_entries[items-1].price;
    double calculated_return=(final_price-init_price)/init_price;
    return calculated_return;
}

double calc_database_std_dev(stock_dataset dataset){
    double calculated_std_dev;


    return calculated_std_dev;
}

void interactive_dataset_gen_and_plot(){
    //FILE *rem=popen("rm ~/Projects/stock_simulation/out/out*","w");
    FILE *rem=popen("find ./out -maxdepth 1 -name '*.dat' -delete","w");
    pclose(rem);

    //User inputs and var declarations
    int num_sets,j,days;
    double yr_growth, yr_std_dev, start_price;
    printf("\nHow many simulations shall we make: ");
    scanf("%d",&num_sets);
    printf("How many days long (1yr = 250 days): ");
    scanf("%d",&days);
    printf("Starting at price: ");
    scanf("%lf",&start_price);
    printf("What yearly growth target (in %%): ");
    scanf("%lf",&yr_growth);
    printf("What yearly std dev target (in %%): ");
    scanf("%lf",&yr_std_dev);
    printf("%d iterations %d days long starting at a price of %.2lf with %.2lf%% growth and %.2lf%% std dev!\n",num_sets,days,start_price,yr_growth,yr_std_dev);
    //
    double avg_returns=0;
    for(j=0;j<num_sets;j++){
        stock_dataset ret=gen_stock_dataset(days, start_price, yr_growth/100, yr_std_dev/100);
        //File Save
        int n=ret.num_items;
        int i;
        char str[128];
        sprintf(str, "./out/out_%d.dat",j);
        FILE *fp=fopen(str,"w");
        for(i=0;i<n;i++){
        fprintf(fp,"%d\t%lf\n", ret.dataset_entries[i].item_num, ret.dataset_entries[i].price);
        }
        fclose(fp);
        //printf("\nDone Saving to File!\n");
        //Calc Returns
        avg_returns+=calc_database_yearly_returns(ret);
        //printf("Avg returns sum: %lf\n",avg_returns);
        free(ret.dataset_entries);
    }
    avg_returns/=num_sets;
    printf("Calculated avg return was: %.2lf% \n\n",avg_returns*100);

    if(num_sets<=10000){
        printf("Graphing...\n");
        FILE *fp_gnu;
        fp_gnu = popen("gnuplot &> /dev/null","w");
        fprintf(fp_gnu, "FILES = system(\"ls -1 ./out/*.dat\")\n");
        fprintf(fp_gnu, "plot for [data in FILES] data u 1:2 with lines notitle\n");
        fflush(fp_gnu);
        sleep(1);
        printf("\nType 'q' to exit: ");
        char cli[1];
        while(strncmp(cli,"q",1)){
            scanf("%c",cli);
        }
        pclose(fp_gnu);
    }
    else{
        printf("Too many dataset iterations to graph - outputting to .dat files only!\n");
    }
}

void save_stock_dataset_to_file(stock_dataset dataset, char *filename, char *print_type){
    int i;
    FILE *fp=fopen(filename,"w");
    if(!strcmp(print_type,"date")){
        for(i=0;i<dataset.num_items;i++){
            fprintf(fp,"%d,%lf\n",dataset.dataset_entries[i].date,dataset.dataset_entries[i].price);
        }
    }
    else if(!strcmp(print_type,"item_num")){
        for(i=0;i<dataset.num_items;i++){
            fprintf(fp,"%d,%lf\n",dataset.dataset_entries[i].item_num,dataset.dataset_entries[i].price);
        }
    }
    else{
        exit(1);
    }
    fclose(fp);
}

void plot_stock_dataset(stock_dataset dataset, char *print_type){
    ////
    char *prefix="temp_";//Note: Do not change without changing corresponding rm statement!
    char *out_path=malloc(strlen(prefix)+strlen(dataset.name));
    char *mid_pt=stpcpy(out_path,prefix);
    strncpy(mid_pt,dataset.name,strlen(dataset.name));
    //printf("Name: %s\n",out_path);
    ////
    //save_stock_dataset_to_file(dataset, "temp.csv", print_type);
    save_stock_dataset_to_file(dataset, out_path, print_type);
    FILE *gnu_plot=popen("gnuplot &> /dev/null","w");
    fprintf(gnu_plot,"set datafile separator \",\"\n");
    fprintf(gnu_plot,"set xtics rotate\n");
    if(!strcmp(print_type,"date")){
        fprintf(gnu_plot,"set xdata time\n");
        fprintf(gnu_plot,"set timefmt \"%%Y%%m%%d\"\n");
        fprintf(gnu_plot,"set format x \"%%d-%%m-%%y\"\n");
    }

    ////
    char *pre="plot \"";
    char *post="\" using 1:2 with lines\n";
    char *pre_mid_post=calloc(strlen(pre)+strlen(out_path)+strlen(post)+1,sizeof(char));
    strcat(pre_mid_post,pre);
    strcat(pre_mid_post,out_path);
    strcat(pre_mid_post,post);
    //pre_mid_post[52]='!';
    //printf("strlen: %d - Final cmd: %s\n",strlen(pre_mid_post),pre_mid_post);
    ////
    //fprintf(gnu_plot,"plot \"temp.csv\" using 1:2 with lines\n");
    fprintf(gnu_plot,pre_mid_post);
    fflush(gnu_plot);
    sleep(1);
    printf("\nType 'q' to exit: ");
    char cli[1];
    while(strncmp(cli,"q",1)){
        scanf("%c",cli);
    }
    FILE *rm=popen("rm temp_*.csv","w");
    pclose(rm);
    pclose(gnu_plot);
    free(out_path);
    free(pre_mid_post);
}

int free_stock_dataset(stock_dataset dataset){
    free(dataset.dataset_entries);
    free(dataset.name);
}

int main(){
    signal(SIGINT, sighandler);
    srand(time(NULL));//Init rand() using time as seed
    setlocale(LC_NUMERIC, "");
    // to store the execution time of code
    double time_spent = 0.0;
    clock_t begin = clock();

    while (true){
        clear();
        printf("What function do you want?\n[0] - Interactive Generator\n[1] - Load Datasets from files\n[2] - \n\nEnter your choice: ");
        int user_choice = -1;
        scanf("%d",&user_choice);
        printf("\n");
        if (user_choice==-1){
            printf("Error in choice! (Got -1)\n");
            exit(1);
        }
        else{
            clear();
            printf("Got choice %d!\n",user_choice);
            if(user_choice==0){//Generate a random stock distribution
                interactive_dataset_gen_and_plot();
                }
            else if(user_choice==1){
                stock_dataset dataset;
                char *curr_set_path;
                char *curr_set_format;
                int curr_lines_to_skip;
                //char path[50];
                //scanf("What file path would you like to ingest:",path);



                //Dataset 1 - DJIA
                curr_set_path="djia_2000_2005.csv";
                curr_set_format="%d,%lf";
                curr_lines_to_skip=0;
                dataset = ingest_dataset(curr_set_path,curr_set_format,curr_lines_to_skip);
                //print_stock_dataset(dataset,"item_num");
                printf("Current dataset path: \'%s\'\n",curr_set_path);
                printf("Avg Yearly Returns: %.2lf%%\n",calc_database_yearly_returns(dataset)*100);
                printf("Total Returns: %.2lf%%\n\n",calc_database_total_returns(dataset)*100);
                plot_stock_dataset(dataset,"date");
                free_stock_dataset(dataset);

                //Dataset 2 - NVDA Stock
                curr_set_path="nvda_2010_2019.csv";
                curr_set_format="%*d,%d,%lf";
                curr_lines_to_skip=1;
                dataset = ingest_dataset(curr_set_path,curr_set_format,curr_lines_to_skip);
                //print_stock_dataset(dataset,"date");
                printf("Current dataset path: \'%s\'\n",curr_set_path);
                printf("Avg Yearly Returns: %.2lf%%\n",calc_database_yearly_returns(dataset)*100);
                printf("Total Returns: %.2lf%%\n\n",calc_database_total_returns(dataset)*100);
                plot_stock_dataset(dataset,"date");
                free_stock_dataset(dataset);



            }//
            //else if(user_choice==2){}//
            //else if(user_choice==3){}//
        }
    }

    printf("\n");
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("The elapsed time is %f seconds\n", time_spent);
    //printf("\n");
    return 0;
}
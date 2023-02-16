#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>

#include <time.h> 
#include <locale.h>


//#include "graphics.c"
//#include "graphics.h"
//#include "gnuplot_i/src/gnuplot_i.h"
//#include "gnuplot_i/src/gnuplot_i.c"

//#include <pthread.h>     /* pthread functions and data structures */


typedef struct {
    int date;
    double price;
}stock_entry;

typedef struct {
    int num_items;
    stock_entry *dataset_entries;
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

stock_dataset gen_stock_dataset(int n, double initial_price, double growth_mean, double growth_std_dev){
    stock_dataset dataset;
    dataset.num_items=n;
    dataset.dataset_entries=malloc(n*sizeof(stock_entry));

    double daily_std_dev=(growth_std_dev/250);
    //printf("\nDaily Std Dev: %lf\n\n",daily_std_dev);
    double daily_growth=(pow(growth_mean+1,1/250.0))-1;
    //printf("\nDaily growth target calculated to be:%lf\n\n",daily_growth);

    int i=0;
    dataset.dataset_entries[i].date=i;
    dataset.dataset_entries[i].price=initial_price;
    for(i=1;i<n;i++){
        //double rand_price=dataset.dataset_entries[i-1].price*(1+(gen_normal_rand_sample()*daily_std_dev)+daily_growth);
        double rand_price=dataset.dataset_entries[i-1].price*(1+gen_normal_rand_sample()*daily_std_dev+daily_growth);
        //printf("Price generates: %lf\n",rand_price);
        dataset.dataset_entries[i].date=i;
        dataset.dataset_entries[i].price=rand_price;
    }
    /*
    printf("\nPrinting Dataset that was made:\n");
    for(i=0;i<n;i++){
        printf("i: %d - price: %lf\n",dataset.dataset_entries[i].date,dataset.dataset_entries[i].price);
    }
    printf("Finished Printing!\n");
    */
    return dataset;
}

double calc_database_returns(stock_dataset dataset){
    int items=dataset.num_items;
    double init_price=dataset.dataset_entries[0].price;
    double final_price=dataset.dataset_entries[items-1].price;
    double calculated_return=(final_price-init_price)/init_price;
    calculated_return=(pow(calculated_return+1,1/(double)items))-1;
    calculated_return=(pow(1+calculated_return,250))-1;
    //printf("calculated_return: %lf\n",calculated_return);
    return calculated_return;
}

void interactive_dataset_gen_and_plot(){
    FILE *rem=popen("rm ~/Projects/stock_simulation/out/out*","w");
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
    printf("%d iterations %d days long starting at a price of %.2lf with %.2lf%% growth and %.2lf%% std dev!\n",days,start_price,num_sets,yr_growth,yr_std_dev);
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
        fprintf(fp,"%d\t%lf\n", ret.dataset_entries[i].date, ret.dataset_entries[i].price);
        }
        fclose(fp);
        //printf("\nDone Saving to File!\n");
        //Calc Returns
        avg_returns+=calc_database_returns(ret);
        //printf("Avg returns sum: %lf\n",avg_returns);
        free(ret.dataset_entries);
    }
    avg_returns/=num_sets;
    printf("Calculated avg return was: %.2lf% \n\n",avg_returns*100);

    if(num_sets<=10000){
        printf("Graphing...\n");
        FILE *fp_gnu;
        fp_gnu = popen("gnuplot","w");
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


int main(){
    srand(time(NULL));//Init rand() using time as seed
    setlocale(LC_NUMERIC, "");
    // to store the execution time of code
    double time_spent = 0.0;
    clock_t begin = clock();

    while (true){
        printf("What function do you want?\n[0] - Interactive Generator\n[1] - \n[2] - \n\nEnter your choice: ");
        int user_choice = -1;
        scanf("%d",&user_choice);
        printf("\n");
        if (user_choice==-1){
            printf("Error in choice! (Got -1)\n");
            exit(1);
        }
        else{
            printf("Got choice %d!\n",user_choice);
            if(user_choice==0){//Generate a random stock distribution
                interactive_dataset_gen_and_plot();
                }
            //else if(user_choice==1){}//
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
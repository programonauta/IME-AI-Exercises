/* OBSERVACOES:
 * - Este e' um programa exemplo de um algoritmo genetico para a otimizacao da funcao f(x) = x2 (x ao quadrado),
 *
 *     desenvolvido por Jose Luiz Ribeiro Filho.
 *
 *     - O uso do relogio como semente para o gerador de numeros aleatorios pode provocar experimentos iguais  
 *       (mesma sequencia de numeros aleatorios), se estes experimentos forem executados em um periodo inferior 
 *       ao periodo de atualizacao do relogio, que e' em geral de um segundo                                    
 *
 *       - Para se inicializar um cromossoma bin�rio de maior tamanho (por exemplo, 44 bits), uma solucao
 *       possivel consiste em gerar tres numeros aleatorios de 16 bits e transforma-los em bin�rios.
 *       Do primeiro e do segundo numero aproveitam-se os 16 bits e do terceiro apenas 12 bits.
 *
 */


/*
 *  ******************************************
 *   *           Simple Genetic Algorithm     *
 *    ******************************************
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define random(num)     (rand()%(num))
#define randomize()     srand((unsigned)time(NULL))


#define POPULATION_SIZE   10
#define CHROM_LENGTH       4
#define PCROSS           0.6
#define PMUT           0.050
#define MAX_GEN           50


/* estrutura que armazena um individuo da populacao:
 * .value:   armazena a avaliacao do individuo
 * .string:  armazena o cromossoma do individuo
 * .fitness: armazena a fitness do individuo */
struct population
{
    int             value;
    unsigned char   string[CHROM_LENGTH];
    unsigned int    fitness;
};

/* estruturas de armazenamento da populacao:
 * 'pool' armazena a populacao corrente, enquanto que 'new_pool' armazena
 * os novos individuos criados a cada geracao; ao fim de cada geracao, os novos
 * individuos sao copiados de volta para 'pool' */
struct population pool[POPULATION_SIZE];      
struct population new_pool[POPULATION_SIZE];  

/* armazena os indices dos indivuos selecionados para reproducao */
int selected[POPULATION_SIZE];

/* contador do numero de geracoes */
int generations;



/*
 *  ******************************************
 *  *                  flip                  *
 *  * Toss a biased coin                     *
 *  ******************************************
 *  */

/* 
   se um numero aleatorio [i] for menor que o numero contido em prob, ou prob 
   igual a 1, entao a funcao flip retorna verdadeiro(1), 
   caso contrario falso(0) 
   */    
int flip(double prob)                               
{
    double i;

    i=((double)rand())/RAND_MAX;     

    if ((prob == 1.0) || (i < prob))
        return (1);
    else
        return (0);
}

/*
 *  ******************************************
 *  *                 encode                 *
 *  * Code a integer into binary string      *
 *  ******************************************
 *      */
void encode(int index, int value)
{                                                               
    int i;                                                      

    /* E' recebido um indice que indica a que individuo estamos nos 
     * referenciando e o valor que sera codificado numa string binaria. 
     * Entao para codificar este valor e' necessario shiftar o valor 
     * fornecido por [i], obtendo-se entao um bit */

    for(i=0; i < CHROM_LENGTH; i++) // este bit é colocado na estrutura string
        pool[index].string[CHROM_LENGTH-1-i] = (value >> i) & 0x01;
}


/*
 *  ******************************************
 *  *                decode                  *
 *  * Decode a binary string into an integer *
 *  ******************************************
 *  */
int decode(int index)
{
    int i, value;
    value = 0;
    for (i=0; i < CHROM_LENGTH; i++) // Transforma um numero binario em decimal
        value += (pool[index].string[CHROM_LENGTH-1-i] << i);
    return value;
}


/*
 *  ******************************************
 *  *                evaluate                *
 *  * Objective function f(x)=x^2            *
 *  ******************************************
 *      */
int evaluate(int value)
{
    return value*value;             /* retorna o quadrado do numero em value */
}


/*
 *  ******************************************
 *   *         initialize_population          *
 *    *  creates and initializes a population  *
 *     ******************************************
 *      */
void initialize_population(void)
{
    int i;

    /* estabelece a semente do gerador de numeros aleatorios (necessario antes
     *        de se chamar rand() pela primeira vez) */
    randomize(); 

    /* para cada individuo i sera codificado um numero entre 0 e 2 elevado
     *        a CHROM_LENGTH */ 
    for (i=0; i < POPULATION_SIZE; i++)
        encode(i, random((int)pow(2.0, CHROM_LENGTH)));

}


/*
 *  ******************************************
 *   *              select                    *
 *    *   selects strings for reproduction     *
 *     ******************************************
 *      */
int select(double sum_fitness)
{
    int i;
    double rand_val;    /* valor randomico entre 0 e sum_fitness */
    double partial_sum; /* soma parcial do fitness dos elementos */

    /* gera o rand_val */
    rand_val = sum_fitness*(((double)rand())/RAND_MAX); 

    /* inicializa partial_sum */
    partial_sum = pool[0].fitness;

    /* encontra o indice indicado por rand_val
     *      isto e' feito atraves da soma de todos os fitness dos elementos da 
     *           populacao ate' que esta soma ultrapasse o valor gerado */
    for (i=0; partial_sum<rand_val; i++)
        partial_sum += pool[i+1].fitness;

    /* retorna o indice encontrado */
    return i;
}


/*
 *  ******************************************
 *   *               crossover                *
 *    * Swaps 2 sub-strings                    *
 *     ******************************************
 *      */
void crossover (int parent1, int parent2, int child1, int child2)
{
    int i, site;

    if (flip(PCROSS))                  /* Se um numero aleatorio gerado por flip for menor que PCROSS */
        site = random(CHROM_LENGTH);   /* determina um ponto de corte aleatorio no cromossoma, senao  */
    else                               /* marca como ponto de corte o ultimo gen do cromossoma        */
        site = CHROM_LENGTH-1;

    for (i=0; i < CHROM_LENGTH; i++)
    {
        if ((i <= site) || (site==0))  /* Caso o indice [i] seja menor que o ponto de corte, ou o ponto de corte  */
        {                              /* foi o primeiro gen, entao os respectivos genes do pai(j) serao copiados */
            /* para os respectivos genes do filho(j) (ate o ponto de corte)            */
            new_pool[child1].string[i] = pool[parent1].string[i];
            new_pool[child2].string[i] = pool[parent2].string[i];
        }
        else                           /* Senao (o indice [i] e' maior que o ponto de corte) os genes serao  */
        {                              /* copiados do outro pai - Ver definicao de crossover de um ponto */
            new_pool[child1].string[i] = pool[parent2].string[i];
            new_pool[child2].string[i] = pool[parent1].string[i];
        }
    }
}


/*
 *  ******************************************
 *   *               mutation                 *
 *    * Changes the values of string position  *
 *     ******************************************
 *      */
void mutation(void)
{
    int i, j;



    for (i=0; i < POPULATION_SIZE; i++)  /* Percorrendo todos os individuos de uma populacao e todos os genes de        */
    {                                    /* cada individuo. Se um numero aleatorio gerado por flip for menor que PMUT   */
        for (j=0; j < CHROM_LENGTH; j++) /* entao o gene [j] sera invertido, caso contrario nao.                        */
            if (flip(PMUT))              /* Neste ponto da execucao do programa os individuos ja' sofreram crossover    */
                /* e os filhos gerados estao em new_pool, entao fazemos mutacao sobre new_pool */
                /* e aproveitamos para copiar a nova populacao para a populacao corrente, ou   */
                /* seja a populacao que ira para a proxima geracao                             */
                /* Note que um bit e' representado no programa por um char                     */  
                pool[i].string[j] = !new_pool[i].string[j];
            else
                pool[i].string[j] = new_pool[i].string[j];
    }
}


/*
 *  ******************************************
 *   *                statistics              *
 *    *       Print intermediary results       *
 *     ******************************************
 *      */
void statistics(FILE *file, double improvement)
{
    int i, j;

    fprintf(file, "\nGeneration: %d\nSelected Strings\n", generations); 

    for (i=0; i< POPULATION_SIZE; i++)
        fprintf(file, " %d", selected[i]);

    fprintf(file, "\n");

    fprintf(file, "\nX\tf(x)\t New_String\tX'");

    for (i=0; i< POPULATION_SIZE; i++)
    {
        fprintf(file, "\n %d\t%u\t;", pool[i].value, pool[i].fitness);

        for (j=0; j<CHROM_LENGTH; j++)
            fprintf(file, " %d",pool[i].string[j]);

        fprintf(file, "\t%d", decode(i));
    }
    fprintf(file, "\nImprovement: %f\n", improvement); 
}


/*
 *  * Funcao principal do GA
 *   */
void main(void)
{
    int i;
    double sum_fitness, avg_fitness, old_avg_fitness;   
    FILE *file;

    generations = 1;
    avg_fitness = 1;

    /* abre o arquivo x2.txt, onde serao logadas as estatisticas do GA */
    file = fopen("x2.txt", "wt");
    if (!file)
        return;

    /* inicializa a populacao */
    initialize_population();

    do
    {
        old_avg_fitness = avg_fitness;
        sum_fitness = 0;

        /* calcula avaliacao e fitness da populacao inteira */
        for (i=0; i<POPULATION_SIZE; i++)
        {
            pool[i].value = decode(i);                   /* Decodifica o individuo [i] */    
            pool[i].fitness = evaluate(pool[i].value);   /* Avalia o individuo [i]     */ 
            sum_fitness += pool[i].fitness;             
        }
        avg_fitness = sum_fitness / POPULATION_SIZE;

        /* seleciona os individuos para reproducao */
        for (i=0; i<POPULATION_SIZE; i++)                        
            selected[i] = select(sum_fitness);

        /* realiza crossover entre os pares, sendo cada par (os pais) definido por
         *          selected[i] e selected[i+1]; os filhos gerados serao armazenados na
         *                   estrutura 'new_pool' na posicao indicada por i e i+1 */
        for (i=0; i<POPULATION_SIZE; i=i+2)
            crossover(selected[i],selected[i+1],i,i+1); 

        /* realiza mutacao sobre os individuos que acabaram de ser gerados,
         *          copiando-os entao de 'new_pool' de volta para 'pool' */
        mutation();                                                     

        /* imprime estatisticas sobre o andamento do GA */
        statistics(file, avg_fitness/old_avg_fitness);
    }
    while ((++generations < MAX_GEN) &&
            ((avg_fitness/old_avg_fitness) > 1.005) ||
            ((avg_fitness/old_avg_fitness) < 1.0)); 
    /* Obs: este tipo de controle de terminacao pode provocar termino prematuro
     *        do GA */

    /* fecha o arquivo de saida */
    fclose(file);
}




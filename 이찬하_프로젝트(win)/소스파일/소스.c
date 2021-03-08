#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
#include"mytype.h"

//#define DEBUG
//#define MAKE_SCRIPT


/*
#define MAX_SIZE 20     // 각 Level 이 갖을 수 있는 노드의 최댓값
#define MAX_CHAR 100    // 입력된 파일의 각 라인별 최대즈 허용 사이즈

typedef struct EDGE{
    double weigh;//                                                                                 현재 가중치를 저장하는 공간
    double dWeigh;//                                                                               가중치의 변화량을 저장하는 공
}EDGE;

typedef struct NODE{
    char name[100];
    int activate;
    double criticalPoint;
    EDGE edge[MAX_SIZE];
} NODE;

typedef struct GRAPH{
    int* num;
    NODE* level0;   // 상황인식 레벨
    NODE* level1;   // 감정, 합리 레벨
    NODE* level2;   // 선거인단 레벨
    NODE* level3;   // 행동 레벨
}GRAPH;
*/


//---------------------------------------------------------------------------------

void memFree(GRAPH);
void fileFree(FILE**, int);
NODE* selectLevel(GRAPH*, int);
int askMakeScript(GRAPH);
void makeScript(FILE*, GRAPH, char*);
void recordDefaultWeigh(FILE*, int, int, GRAPH);
void recordCriticalPoint(FILE*, GRAPH, char*);
void recordSimulationInput(FILE*, GRAPH, char*);
void grabAName(FILE*, GRAPH);
void setName(FILE*, GRAPH, int);
void grabAWeigh(FILE*, FILE*, GRAPH);
void grabACritical(FILE*, GRAPH);
void simulate(FILE*, GRAPH);
void readInput(FILE*, GRAPH*);
void inspectInput(GRAPH*);
void transmitSpark(GRAPH*);
void updateWeigh(GRAPH*);
void visualiseCircuit(GRAPH*);
void printSpace(int);
void beginWeighSum(GRAPH*, double*);
void inspectCircuitStatus(GRAPH*, int, int, double*);
void showGraphInfo(GRAPH);
void readGraphInfo(FILE*, int*, int*);

//---------------------------------------------------------------------------------


int main(void) {
    GRAPH graph;
    int l0Node = 0, l1Node = 0;
    int numTemp[5];
    FILE* file[5];
    FILE* basicInfoFp = NULL;
    FILE* initWeighFp = NULL;
    FILE* deltaWeighFp = NULL;
    FILE* criticalPointFp = NULL;
    FILE* simulationInputFp = NULL;

    //                          * read data from files *

    if ((basicInfoFp = fopen("graphInfo.ignite", "r")) == NULL) {
        fprintf(stderr, "\"graphInfo.ignite\"파일 없음");
        exit(1);
    }
    file[0] = basicInfoFp;

    readGraphInfo(basicInfoFp, &l0Node, &l1Node);
    numTemp[0] = l0Node;
    numTemp[1] = l1Node;
    numTemp[2] = 5;
    numTemp[3] = 1;
    graph.num = numTemp;

    graph.level0 = (NODE*)malloc(l0Node * sizeof(NODE));
    graph.level1 = (NODE*)malloc(l1Node * sizeof(NODE));
    graph.level2 = (NODE*)malloc(5 * sizeof(NODE));
    graph.level3 = (NODE*)malloc(sizeof(NODE));

    grabAName(basicInfoFp, graph);
#ifdef MAKE_SCRIPT
    askMakeScript(graph);

    memFree(graph);
    fileFree(file, 1);
    return 0;
#endif

    if ((initWeighFp = fopen("presetInfo_Init.ignite", "rt")) == NULL) {
        fprintf(stderr, "\"presetInfo_Init.ignite\"파일 없음");
        exit(1);
    }
    file[1] = initWeighFp;
    if ((deltaWeighFp = fopen("presetInfo_Delta.ignite", "r")) == NULL) {
        fprintf(stderr, "\"presetInfo_Delta.ignite\"파일 없음");
        exit(1);
    }
    file[2] = deltaWeighFp;
    if ((criticalPointFp = fopen("presetInfo_Critical.ignite", "r")) == NULL) {
        fprintf(stderr, "\"presetInfo_Critical.ignite\"파일 없음");
        exit(1);
    }
    file[3] = criticalPointFp;

    grabAWeigh(initWeighFp, deltaWeighFp, graph);//                                                                     파일에서 초기 가중치와 가중치 변화량에 대한 정보를 graph 로 옮겨주는 함수
    grabACritical(criticalPointFp, graph);  //                                                                          파일에서 노드의 흥분 역치값에 대한 정보를 graph 로 옮겨주는 함
#ifdef DEBUG
    showGraphInfo(graph);
#endif


    //                             * run simulation *


    if ((simulationInputFp = fopen("simulation_Input.ignite", "r")) == NULL) {
        fprintf(stderr, "\"presetInfo_Critical.ignite\"파일 없음");
        exit(1);
    }
    file[4] = simulationInputFp;

    simulate(simulationInputFp, graph);






    //                            * clean up the mess *

    fileFree(file, 5);
    return 0;
}


//---------------------------------------------------------------------------------



void readGraphInfo(FILE* fp, int* l0, int* l1) {
    char buffer[MAX_CHAR];
    fgets(buffer, MAX_CHAR, fp);
    *l0 = atoi(strtok(buffer, " "));
    *l1 = atoi(strtok(NULL, " "));

    if (*l0 > MAX_SIZE || *l0 < 0 || *l1 > MAX_SIZE || *l1 < 0) {
        printf("/n/n\t\t ERR: 노드의 갯수가 MAX_SIZE 보다 큽니다. graphInfo 파일을 수정하십시오");
    }

#ifdef DEBUG
    printf("l0 = %d, l1 = %d\n", *l0, *l1);
#endif

}



//-----------------------------------make script manually--------------------------------


int askMakeScript(GRAPH graph) {
    FILE* weighInitFp;
    FILE* weighDeltaFp;
    FILE* criticalPointFp;
    FILE* simulationInputFp;
    int temp1;

    printf("\n\n\t\t****WARNING**** \nentering MAKE_SCRIPT mode to write \"presetInfo_Init.ignite\" (y/n): ");
    temp1 = getc(stdin);
    if (temp1 == 'y') {

        if ((weighInitFp = fopen("presetInfo_Init.ignite", "w")) == NULL) {
            fprintf(stderr, "컴퓨터 저장공간 부족 ");
            exit(1);
        }
        makeScript(weighInitFp, graph, "WeighInitInfo");  //                                                초기 가중치 설정 과정

        fclose(weighInitFp);
    }

    int neverUseThis; while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
    printf("\n\n\t\t****WARNING**** \nentering MAKE_SCRIPT mode to write \"presetInfo_Delta.ignite\" (y/n): ");
    temp1 = getc(stdin);
    if (temp1 == 'y') {

        if ((weighDeltaFp = fopen("presetInfo_Delta.ignite", "w")) == NULL) {
            fprintf(stderr, "컴퓨터 저장공간 부족 ");
            exit(1);
        }
        makeScript(weighDeltaFp, graph, "WeighDeltaInfo");  //                                              가중치 변화량 설정 과정

        fclose(weighDeltaFp);
    }

    while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
    printf("\n\n\t\t****WARNING**** \nentering MAKE_SCRIPT mode to write \"presetInfo_Critical.ignite\" (y/n): ");
    temp1 = getc(stdin);
    if (temp1 == 'y') {

        if ((criticalPointFp = fopen("presetInfo_Critical.ignite", "w")) == NULL) {
            fprintf(stderr, "컴퓨터 저장공간 부족 ");
            exit(1);
        }
        recordCriticalPoint(criticalPointFp, graph, "CriticalPointInfo");  //                               흥분 역치 설정 과정

        fclose(criticalPointFp);
    }

    while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
    printf("\n\n\t\t****WARNING**** \nentering MAKE_SCRIPT mode to write \"simulation_Input.ignite\" (y/n): ");
    temp1 = getc(stdin);
    if (temp1 == 'y') {

        if ((simulationInputFp = fopen("simulation_Input.ignite", "w")) == NULL) {
            fprintf(stderr, "컴퓨터 저장공간 부족 ");
            exit(1);
        }
        recordSimulationInput(simulationInputFp, graph, "SimulationInput");  //                               흥분 역치 설정 과정

        fclose(simulationInputFp);
    }
}


void makeScript(FILE* fp, GRAPH graph, char* presetConcat) {
    int level[4];
    char buffer[MAX_CHAR];
    char* token;
    level[0] = graph.num[0];
    level[1] = graph.num[1];
    level[2] = graph.num[2];
    level[3] = graph.num[3];
    //                                      *프리셋 정보입력 단계*
    printf("preset 정보를 입력하시오(띄어쓰기 하지 말것): ");
    scanf("%s", buffer);
    int neverUseThis; while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
    token = strcat(buffer, presetConcat);
    fprintf(fp, "\t %s\n", token);

    for (int i = 0; i < 4 - 1; i++) {
        fprintf(fp, "\t\tLv%d -> Lv%d 엣지들의 정보\n", i, i + 1);
        printf("\n\t\tLv%d -> Lv%d 엣지들의 정보\n", i, i + 1);
        fprintf(fp, "\t");
        for (int k = 0; k < level[i + 1]; k++)
            fprintf(fp, "\tLv%d-n%d", i + 1, k);
        fprintf(fp, "\n");

        for (int j = 0; j < level[i]; j++) {
            fprintf(fp, "\tnode%d:\t", j);
            printf("\n출발노드: %s\n", selectLevel(&graph, i)[j].name);
            for (int l = 0; l < level[i + 1]; l++) {
                recordDefaultWeigh(fp, i, l, graph);
            }
            fprintf(fp, "\n");

        }

    }
}

void recordDefaultWeigh(FILE* fp, int i, int l, GRAPH graph) {
    char buffer[MAX_CHAR];
    char* token;
    double weighTemp;
    printf("도착노드 %s인 엣지의 정보 입력: ", selectLevel(&graph, i + 1)[l].name);
    scanf("%s", buffer);
    token = strtok(buffer, " \n");
    weighTemp = atof(token);
    fprintf(fp, "[%-5.2f]\t", weighTemp);

}

void recordCriticalPoint(FILE* fp, GRAPH graph, char* presetConcat) {
    double criticalTemp;
    char buffer[MAX_CHAR];
    char* token;
    int level[4];
    level[0] = graph.num[0];
    level[1] = graph.num[1];
    level[2] = graph.num[2];
    level[3] = graph.num[3];

    //                                      *프리셋 정보입력 단계*
    printf("preset 정보를 입력하시오(띄어쓰기 하지 말것): ");
    scanf("%s", buffer);
    int neverUseThis; while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
    token = strcat(buffer, presetConcat);
    fprintf(fp, "\t %s\n", token);


    for (int i = 0; i < 4; i++) {
        fprintf(fp, "\n\t\tLv%d 에 속한 노드\n", i);
        printf("\n\t\tLv%d\n", i);
        for (int j = 0; j < level[i]; j++) {
            fprintf(fp, "\tnode%d:\t", j);
            printf("\nnode%d %s 의 역치값: ", j, selectLevel(&graph, i)[j].name);
            scanf("%s", buffer);
            criticalTemp = atof(token);
            fprintf(fp, "[%-5.2f]\t", criticalTemp);

            fprintf(fp, "\n");

        }

    }

}

void recordSimulationInput(FILE* fp, GRAPH graph, char* presetConcat) {
    int proceedTemp = '1', inputTemp, counter = 0;
    char buffer[MAX_CHAR];
    char* token;



    printf("inputFile 정보를 입력하시오(띄어쓰기 하지 말것): ");
    scanf("%s", buffer);
    int neverUseThis; while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
    token = strcat(buffer, presetConcat);
    fprintf(fp, "\t %s\n", token);

    while (proceedTemp == '1') {
        printf("\t\t%d번째 simulation input", (counter + 1));
        fprintf(fp, "\n\t\t%d번째 simulation input\n", (counter + 1));
        for (int i = 0; i < graph.num[0]; i++) {
            fprintf(fp, "\tnode%d:\t", i);
            printf("\nnode%d %s 를 활성화 하려면 1을 입력하세요: ", i, selectLevel(&graph, 0)[i].name);
            inputTemp = getc(stdin);
            while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
            if (inputTemp == '1') {
                fprintf(fp, "[1]\n");
            }
            else {
                fprintf(fp, "[0]\n");
            }

        }
        counter++;
        printf("계속하려면 \"1\"을 입력하세요: ");
        proceedTemp = getchar();
    }




}



//-----------------------------------garb a value from preset--------------------------------



void grabAName(FILE* fp, GRAPH graph) {
    setName(fp, graph, 0);
    setName(fp, graph, 1);
    setName(fp, graph, 2);
    setName(fp, graph, 3);
}

void setName(FILE* fp, GRAPH graph, int lev) {
    char buffer[MAX_CHAR];
    fgets(buffer, MAX_CHAR, fp);    //                                                          가독성을 위해 입력파일에 삽입되었던 불필요한 부분 제거
    for (int i = 0; i < graph.num[lev]; i++) {
        fgets(buffer, MAX_CHAR, fp);
        strtok(buffer, ":\n");  //                                                         앞의 토큰은 필요없으므로 버림
        strcpy(selectLevel(&graph, lev)[i].name, strtok(NULL, ":\n"));

#ifdef DEBUG
        printf("level%d[%d].name = %s\n", lev, i, selectLevel(&graph, lev)[i].name);
#endif
    }
}

void grabAWeigh(FILE* initWeighFp, FILE* deltaWeighFp, GRAPH graph) {
    char buffer1[MAX_CHAR];
    char buffer2[MAX_CHAR];
    char* token;
    double weighTemp;
    NODE* levelPtr;

    fgets(buffer1, MAX_CHAR, initWeighFp);//                                                         파일 첫줄에 담긴 파일정보 생략
    fgets(buffer2, MAX_CHAR, deltaWeighFp);

    for (int i = 0; i < 3; i++) {
        levelPtr = selectLevel(&graph, i);
        fgets(buffer1, MAX_CHAR, initWeighFp);//                                                      매 새로운 level 의 첫줄과 두번째 줄엔 정보가 없음
        fgets(buffer1, MAX_CHAR, initWeighFp);
        fgets(buffer2, MAX_CHAR, deltaWeighFp);
        fgets(buffer2, MAX_CHAR, deltaWeighFp);
        for (int j = 0; j < graph.num[i]; j++) {
            fgets(buffer1, MAX_CHAR, initWeighFp);
            fgets(buffer2, MAX_CHAR, deltaWeighFp);

            strtok(buffer1, "[");//                                                                                  새로운 노드에 대한 정보를 읽을 때마다 앞서 나오는 인덱스 제거
            for (int k = 0; k < graph.num[i + 1]; k++) {
                token = strtok(NULL, "[]");
                weighTemp = atof(token);
                levelPtr[j].edge[k].weigh = weighTemp;
                strtok(NULL, "[");
            }

            strtok(buffer2, "[");
            for (int k = 0; k < graph.num[i + 1]; k++) {
                token = strtok(NULL, "[]");
                weighTemp = atof(token);
                levelPtr[j].edge[k].dWeigh = weighTemp;
                strtok(NULL, "[");
            }
        }
    }
}

void grabACritical(FILE* fp, GRAPH graph) {
    char buffer[MAX_CHAR];
    char* token;
    double criticalTemp;
    NODE* levelPtr[4];

    fgets(buffer, MAX_CHAR, fp);//                                                                                          파일 첫줄에 담긴 파일정보 생략

    for (int i = 0; i < 4; i++) {
        levelPtr[i] = selectLevel(&graph, i);
        fgets(buffer, MAX_CHAR, fp);//                                                                                      새로운 레벨을 읽을 때마다 레벨 인덱스 2줄 제거
        fgets(buffer, MAX_CHAR, fp);

        for (int j = 0; j < graph.num[i]; j++) {
            fgets(buffer, MAX_CHAR, fp);
            strtok(buffer, "[");//                                                                                 각 노드의 인덱스 제거
            token = strtok(NULL, "[]");
            criticalTemp = atof(token);
            levelPtr[i][j].criticalPoint = criticalTemp;
        }

    }

}




//-----------------------------------------util func--------------------------------------

void memFree(GRAPH graph) {
    free(graph.level0);
    free(graph.level1);
    free(graph.level2);
    free(graph.level3);
}
void fileFree(FILE** file, int num) {
    for (int i = 0; i < num; i++) {
        fclose(file[i]);
    }
}
NODE* selectLevel(GRAPH* graphPtr, int lev) {
    switch (lev) {
    case 0:
        return (*graphPtr).level0;
    case 1:
        return (*graphPtr).level1;
    case 2:
        return (*graphPtr).level2;
    default:
        return (*graphPtr).level3;
    }
}

void showGraphInfo(GRAPH graph) {
    NODE* levelPtr[4];
    printf("\n\t\t********  graphInfo  ********\n");
    for (int i = 0; i < 3; i++) {
        levelPtr[i] = selectLevel(&graph, i);
        printf("\nLv%d node info\n", i);
        for (int j = 0; j < graph.num[i]; j++) {
            printf("Lv%d.node%d\t CP: %5.2lf  ", i, j, levelPtr[i][j].criticalPoint);
            for (int k = 0; k < graph.num[i + 1]; k++) {
                printf("[%5.2lf,%5.2lf] ", levelPtr[i][j].edge[k].weigh, levelPtr[i][j].edge[k].dWeigh);
            }
            printf("\n");
        }
    }
    printf("\nLv%d node info\n", 3);
    printf("Lv%d.node%d\t CP: %5.2lf \n", 3, 0, selectLevel(&graph, 3)[0].criticalPoint);
}


//-------------------------------------visualise--------------------------------

void visualiseCircuit(GRAPH* graph) {
    int superior = 0;
    NODE* levelPtr[4];
    int heightInfo[4];
    int oddEven[4];
    int printCounter[4] = { 0,0,0,0 };

    for (int i = 0; i < 4; i++) {

        levelPtr[i] = selectLevel(graph, i);

        if ((*graph).num[i] % 2 == 0) {//                                                                                 노드의 숫자가 홀수개인지 짝수개인지 구분하기
            oddEven[i] = 0;
            heightInfo[i] = (*graph).num[i];
        }
        else {
            oddEven[i] = 1;
            heightInfo[i] = (*graph).num[i];
        }

    }

    for (int i = 0; i < 4; i++) {//                                                                                       가장 큰 (기준이 되는 정렬숫자 찾기)

        if (superior < heightInfo[i]) {
            superior = heightInfo[i];
        }

    }

    for (int counter = 0; counter < superior; counter++) {

        for (int lev = 0; lev < 4; lev++) {

            if ((superior - heightInfo[lev]) / 2 <= counter) {//

                if (printCounter[lev] >= heightInfo[lev]) {
                    printSpace(heightInfo[lev] + 3);
                    printf(" ");
                    printSpace(1);
                    continue;
                }

                if (2 * printCounter[lev] < heightInfo[lev]) {
                    if (printCounter[lev] % 2 == 0) {

                        printSpace((heightInfo[lev] / 2 - printCounter[lev]));
                        if (levelPtr[lev][printCounter[lev]].activate == 1) {
                            printf("(O)");
                        }
                        else {
                            printf("(X)");
                        }
                        printSpace(printCounter[lev] + (heightInfo[lev] / 2) + 1);

                    }
                    else {

                        printSpace((heightInfo[lev] / 2 + printCounter[lev]));
                        if (levelPtr[lev][printCounter[lev]].activate == 1) {
                            printf("  (O) ");
                        }
                        else {
                            printf("  (X) ");
                        }
                        printSpace((heightInfo[lev] / 2 - printCounter[lev]));

                    }

                }
                else {
                    if (printCounter[lev] % 2 == 0) {

                        printSpace((-heightInfo[lev] / 2 + printCounter[lev]));
                        if (levelPtr[lev][printCounter[lev]].activate == 1) {
                            printf("(O)");
                        }
                        else {
                            printf("(X)");
                        }
                        printSpace((heightInfo[lev] * 3) / 2 - printCounter[lev]);

                    }
                    else {

                        printSpace(((heightInfo[lev] * 3) / 2 - printCounter[lev]) - 1);
                        if (levelPtr[lev][printCounter[lev]].activate == 1) {
                            printf(" (O)  ");
                        }
                        else {
                            printf(" (X)  ");
                        }
                        printSpace(printCounter[lev] - heightInfo[lev] / 2);

                    }
                }
                printCounter[lev]++;
                printSpace(2);
                printf(" ");
                printSpace(1);

            }
            else {
                printSpace(heightInfo[lev] + 3);
                printf(" ");
                printSpace(1);

            }
        }
        printf("\n");


    }



}
void printSpace(int num) {
    for (int i = 0; i < num; i++) {
        printf("   ");
    }
}

//------------------------------------run simulation----------------------------

void simulate(FILE* infoFp, GRAPH graph) {
    char buffer[MAX_CHAR];
    char autoTemp = '0';
    int cycleCounter = 0;
    int furyCounter = 0;
    double prevWeighSum[4];
    fgets(buffer, MAX_CHAR, infoFp);//                                                                                        파일 가장 첫줄의 파일 정보 스킵
    fgets(buffer, MAX_CHAR, infoFp);//                                                                                        첫번째 input 전에 나오는 공백줄 제거
    beginWeighSum(&graph, prevWeighSum);

    //                          * 인풋파일 끝에 닿을 때까지 시뮬레이션 시행 *
    while (!feof(infoFp)) {//                                                                                             다음회기 진행여부 묻기

        if (cycleCounter != 0 && autoTemp != 'a') {
            printf("\n\ncontinue: y, auto: a, stop: n (y/a/n): ");
            autoTemp = getc(stdin);
            if (autoTemp != 'y' && autoTemp != 'a')
                break;
            int neverUseThis; while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
        }

        cycleCounter++;
        readInput(infoFp, &graph);

#ifdef DEBUG
        inspectInput(&graph);
#endif

        transmitSpark(&graph);
        updateWeigh(&graph);
        furyCounter += graph.level3[0].activate;

#ifdef DEBUG
        showGraphInfo(graph);
#endif

        printf("\n\t\t* %d번째 시뮬레이션 결과 출력 *\n\n", cycleCounter);
        visualiseCircuit(&graph);
        inspectCircuitStatus(&graph, cycleCounter, furyCounter, prevWeighSum);


        fgets(buffer, MAX_CHAR, infoFp);//                                                                                        매 input 회차의 끝에 있는 공백줄 제거
    }

}

void readInput(FILE* fp, GRAPH* graph) {
    char buffer[MAX_CHAR];
    char* token;
    int inputTemp;

    fgets(buffer, MAX_CHAR, fp);

    for (int i = 0; i < (*graph).num[0]; i++) {

        fgets(buffer, MAX_CHAR, fp);
        strtok(buffer, "[");
        token = strtok(NULL, "[]");
        inputTemp = atoi(token);

        if (inputTemp == 1) {
            (*graph).level0[i].activate = 1;
        }
        else (*graph).level0[i].activate = 0;

    }

}

void inspectInput(GRAPH* graph) {

    printf("\n\n\t그래프 입력값 순검! 좌로번호!\n");

    for (int i = 0; i < (*graph).num[0]; i++) {
        printf("\tNODE%d: %d\n", i, (*graph).level0[i].activate);
    }

    printf("\t이상 순검 끝!\n");
}

void transmitSpark(GRAPH* graph) {
    double sumTemp;
    NODE* levelPtr[4];
    levelPtr[0] = selectLevel(graph, 0);
    levelPtr[1] = selectLevel(graph, 1);
    levelPtr[2] = selectLevel(graph, 2);
    levelPtr[3] = selectLevel(graph, 3);

    for (int i = 0; i < 3; i++) {

        for (int j = 0; j < (*graph).num[i + 1]; j++) {

            sumTemp = 0;
            for (int k = 0; k < (*graph).num[i]; k++) {
                sumTemp += (levelPtr[i][k].activate * levelPtr[i][k].edge[j].weigh);
            }

            if (sumTemp > levelPtr[i + 1][j].criticalPoint) {
                levelPtr[i + 1][j].activate = 1;
            }
            else levelPtr[i + 1][j].activate = 0;

        }

    }

}

/*  @@가중치 업데이트 룰@@
 * 1. 한 회차에서 같이 흥분한 노드 사이의 엣지는 미리 입력된 dWeigh 만큼 가중치를 증가시킨다.
 * 2.1 한 회차에서 선행 노드가 흥분했지만, 후행 노드가 흥분하지 않았을 경우 가중치를 감소시킨다.
 * 2.2 이때 가중치를 감소시키는 수치의 합은 증가한 가중치의 합과 같게해서 전체 가중치의 합이 변하지 않도록한다.(부동소수점으로 인한 오차가 있을 수 있다.)
 * 2.2.1 *추가* 시행결과 다수의 가중치 상향 엣지와 소수의 하향 엣지가 나오는 경우 하향엣지가 과도하게 하향되는 경우를 관측.
 * 2.2.2 *추가* 간단한 보정을 통해 가중치가 급격하게 변화하는 것을 방지함, (추후에 선형대수 공부하고 좀더 정교하게 할 필요 있음)
 * 2.2.3 *추가* 절댓값으로 변경해 연산해서 음의 가중치를 갖는 엣지가 뭉쳐있을 경우 의도와 반대로 가중치가 작동하는 것을 방지
 * 2.3 노드간에 가중치의 절댓값에 퍈차를 고려해 가중치 변화량 유지는 노드단위로 끊어서 다른노드의 가중치 변동에 의한 간섭을 줄인다.
 * 3.1 선행노드가 흥분하지 않았으나 후행노드가 흥분했을 경우 흥분 전이의 방향성이 일방향임을 감안해 가중치를 변화시키지 않는다.
 * 3.2 선행노드와 후행노드가 모두 흥분하지 않았을 경우 마찬가지로 가중치를 변화시키지 않는다.
 */
void updateWeigh(GRAPH* graph) {
    double deltaSum = 0;
    double sumDelta = 0;
    double deltaMul = 0;
    int randomTemp = 0;
    NODE* levelPtr[4];
    levelPtr[0] = selectLevel(graph, 0);
    levelPtr[1] = selectLevel(graph, 1);
    levelPtr[2] = selectLevel(graph, 2);
    levelPtr[3] = selectLevel(graph, 3);

    for (int Level_ = 0; Level_ < 2; Level_++) {
        for (int Node_ = 0; Node_ < (*graph).num[Level_]; Node_++) {

            for (int NextNode_ = 0; NextNode_ < (*graph).num[Level_ + 1]; NextNode_++) {

                if ((levelPtr[Level_][Node_].activate == 1) && (levelPtr[Level_ + 1][NextNode_].activate == 1)) {

                    levelPtr[Level_][Node_].edge[NextNode_].weigh += levelPtr[Level_][Node_].edge[NextNode_].dWeigh;
                    deltaSum += fabs(levelPtr[Level_][Node_].edge[NextNode_].dWeigh);//                                       규칙 1.에 해당하는 엣지 가중치 변화량의 합
                }
                else {
                    if ((levelPtr[Level_][Node_].activate == 1) && (levelPtr[Level_ + 1][NextNode_].activate != 1)) {//     규칙 2.1에 해당하는 엣지의 가중치 변화량 합
                        sumDelta += fabs(levelPtr[Level_][Node_].edge[NextNode_].dWeigh);
                    }


                }

            }

            /*deltaMul = deltaSum / sumDelta;*/ //                                                                      이렇게 할 경우 2.2.1 문제 발생

            deltaMul = (deltaSum + (deltaSum + sumDelta) / (*graph).num[Level_ + 1] * 5) /*                                   2.2.2의 임시 방편 */
                / (sumDelta + (deltaSum + sumDelta) / (*graph).num[Level_ + 1] * 5);

            for (int NextNode_ = 0; NextNode_ < (*graph).num[Level_ + 1]; NextNode_++) {//                                  규칙 2.에 해당하는 엣지 가중치 감소 과정

                if ((levelPtr[Level_][Node_].activate == 1) && (levelPtr[Level_ + 1][NextNode_].activate != 1)) {
                    levelPtr[Level_][Node_].edge[NextNode_].weigh -= levelPtr[Level_][Node_].edge[NextNode_].dWeigh * deltaMul;
                }

            }

            deltaSum = 0;
            sumDelta = 0;
        }
    }

    /* Level 2의 가중치 연산법은 조금 다르다
     * 1. 행동할 경우 강한 피드백을 받고
     * 2. 행동하지 않고 상황을 넘어갔을 경우 약한 피드백을 받는다.
     * 3. 피드백은 rand 함수로 random 값을 받아 산출한다.
     */
     //printf("적용된 random 값 상태 보고 ");
    srand((unsigned)clock());
    for (int Level_ = 2; Level_ < 3; Level_++) {
        for (int Node_ = 0; Node_ < (*graph).num[Level_]; Node_++) {
            for (int NextNode_ = 0; NextNode_ < (*graph).num[Level_ + 1]; NextNode_++) {

                if (levelPtr[Level_ + 1][NextNode_].activate == 1) {
                    randomTemp = (rand() % 21) - 10;
                    //printf("%d번 노드: %d  ",Node_, randomTemp);
                    levelPtr[Level_][Node_].edge[NextNode_].weigh += levelPtr[Level_][Node_].edge[NextNode_].dWeigh * randomTemp;
                }
                else {
                    randomTemp = (rand() % 11) - 5;
                    //printf("%d번 노드: %d  ",Node_, randomTemp);
                    levelPtr[Level_][Node_].edge[NextNode_].weigh += levelPtr[Level_][Node_].edge[NextNode_].dWeigh * randomTemp;
                }


            }

        }
    }
}

void beginWeighSum(GRAPH* graph, double* weighSum) {
    NODE* levelPtr[4];
    levelPtr[0] = selectLevel(graph, 0);
    levelPtr[1] = selectLevel(graph, 1);
    levelPtr[2] = selectLevel(graph, 2);
    levelPtr[3] = selectLevel(graph, 3);
    for (int level_ = 0; level_ < 3; level_++) {
        for (int node_ = 0; node_ < (*graph).num[level_]; node_++) {
            for (int nextNode_ = 0; nextNode_ < (*graph).num[level_ + 1]; nextNode_++) {
                weighSum[level_] += levelPtr[level_][node_].edge[nextNode_].weigh;
            }
        }
        weighSum[3] += weighSum[level_];
    }
}
void inspectCircuitStatus(GRAPH* graph, int cycleNum, int furyNum, double* prevWeighSum) {
    int nodeNum = 0;
    double weighSum[4] = { 0 };
    int activateNum = 0;
    NODE* levelPtr[4];
    levelPtr[0] = selectLevel(graph, 0);
    levelPtr[1] = selectLevel(graph, 1);
    levelPtr[2] = selectLevel(graph, 2);
    levelPtr[3] = selectLevel(graph, 3);

    for (int level_ = 0; level_ < 4; level_++) {
        nodeNum += (*graph).num[level_];
    }

    for (int level_ = 0; level_ < 3; level_++) {
        for (int node_ = 0; node_ < (*graph).num[level_]; node_++) {
            for (int nextNode_ = 0; nextNode_ < (*graph).num[level_ + 1]; nextNode_++) {
                weighSum[level_] += levelPtr[level_][node_].edge[nextNode_].weigh;
            }
            activateNum += levelPtr[level_][node_].activate;
        }
        weighSum[3] += weighSum[level_];
    }

    printf("\n\n총 시행 횟수: %d\t\t현재까지 최종 노드 흥분 횟수: %d\t\t흥분 백분율: %6.2lf\n",
        cycleNum, furyNum, ((double)100 * furyNum) / cycleNum);

    printf("총 노드 갯수: %d\t\t이번 시행 흥분한 노드 갯수: %d\t\t흥분 백분율: %6.2lf\n",
        nodeNum, activateNum, ((double)100 * activateNum) / nodeNum);

    for (int level_ = 0; level_ < 3; level_++) {
        printf("출발점 Lv%d 인 엣지의 가중치 총합: %6.2lf\t\t직전값: %6.2lf\t\t변화 백분율: %6.2lf\n", level_, weighSum[level_],
            prevWeighSum[level_], ((weighSum[level_] - prevWeighSum[level_]) * 100) / prevWeighSum[level_]);
        prevWeighSum[level_] = weighSum[level_];
    }
    printf("전체 엣지 가중치 총합: %6.2lf\t\t직전값: %6.2lf\t\t변화 백분율: %6.2lf\n", weighSum[3],
        prevWeighSum[3], ((weighSum[3] - prevWeighSum[3]) * 100) / prevWeighSum[3]);
    prevWeighSum[3] = weighSum[3];



}

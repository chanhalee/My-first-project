#pragma once
//
// Created by 이찬하 on 2021/01/13.
//

#ifndef VIP_MYTYPE_H
#define VIP_MYTYPE_H

#define MAX_SIZE 20     // 각 Level 이 가질 수 있는 노드의 최댓값
#define MAX_CHAR 300    // 입력된 파일의 각 라인별 최대즈 허용 사이즈

typedef struct EDGE {
    double weigh;//     현재 가중치를 저장하는 공간
    double dWeigh;//    가중치의 변화량을 저장하는 공간
}EDGE;

typedef struct NODE {
    char name[100]; // 노드의 이름을 저장
    int activate;   // 노드의 흥분 여부를 저장
    double criticalPoint;   // 노드의 흥분 역치를 저장
    EDGE edge[MAX_SIZE];    // 노드에 속한 엣지의 개수를 저장
} NODE;

typedef struct GRAPH {
    int* num;
    NODE* level0;   // 상황인식 레벨
    NODE* level1;   // 감정, 합리 레벨
    NODE* level2;   // 선거인단 레벨
    NODE* level3;   // 행동 레벨
}GRAPH;
#endif //VIP_MYTYPE_H

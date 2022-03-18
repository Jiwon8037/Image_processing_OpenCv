//영상처리(4~6) 20170880 강지원
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef unsigned char uchar;

uchar** uc_alloc(int size_x, int size_y) {
    uchar** m;
    int i;
    if ((m = (uchar**)calloc(size_y, sizeof(uchar*))) == NULL) {
        printf("uc_alloc error 1\n");
        exit(0);
    }
    for (i = 0; i < size_y; i++)
        if ((m[i] = (uchar*)calloc(size_x, sizeof(uchar))) == NULL) {
            printf("uc_alloc error 2\n");
            exit(0);
        }
    return m;
}
void uc_free(int size_x, int size_y, uchar** ucmatrix) {
    int i;
    for (i = 0; i < size_y; i++) {
        free(ucmatrix[i]);
    }
    free(ucmatrix);
}
void read_ucmatrix(int size_x, int size_y, uchar** ucmatrix, const char* filename) {
    int i;
    FILE* f;//무조건 포인터선언, 하드디스크의 파일시작주소를 가리킴
    if ((f = fopen(filename, "rb")) == NULL) {
        printf("%s File  open error\n", filename);
        exit(0);
    }
    for (i = 0; i < size_y; i++)
        if (fread(ucmatrix[i], sizeof(uchar), size_x, f) != size_x) {
            printf("Data read error\n");
            exit(0);
        }
    fclose(f);
}
void write_ucmatrix(int size_x, int size_y, uchar** ucmatrix, const char* filename) {
    int i;
    FILE* f;
    if ((f = fopen(filename, "wb")) == NULL) {
        printf("%s File open error\n", filename);
        exit(0);
    }
    for (i = 0; i < size_y; i++)
        if (fwrite(ucmatrix[i], sizeof(uchar), size_x, f) != size_x) {
            printf("Data write error\n");
            exit(0);
        }
    fclose(f);
}
int img_arr[256] = { 0 };//img 그레이레벨 개수 저장 배열
double img_pdf[256] = { 0 };//pdf값 배열
double img_cdf[256] = { 0 };//cdf값 배열

int out_arr[256] = { 0 };//out 그레이레벨 개수 저장 배열
double out_pdf[256] = { 0 };
double out_cdf[256] = { 0 };

void get_histo(uchar** img, uchar** graph, int* arr, double* pdf, double* cdf, int Col, int Row) {//히스토그램, pdf cdf 구하기
    int i, j;
    double maxg = -1;
    double num;

    for (i = 0; i < Row; i++) {//img 그레이레벨 개수
        for (j = 0; j < Col; j++) {
            arr[img[i][j]]++;
        }
    }
    for (i = 0; i < 256; i++) {//pdf계산
        pdf[i] = (double)arr[i] / (Col * Row);
    }
    cdf[0] = pdf[0];//처음값 같게
    for (i = 1; i < 256; i++) {//cdf계산
        cdf[i] = pdf[i] + cdf[i - 1];
    }
}
void histo_equaliz(uchar** img, uchar** out, uchar** graph, int Col, int Row) {//평활화
    int i, j;
    double maxg = -1;
    double num;

    for (i = 0; i < Row; i++) {//평활화 결과이미지 생성
        for (j = 0; j < Col; j++) {
            out[i][j] = img_cdf[img[i][j]] * 255;
        }
    }

    for (i = 0; i < Row; i++) {//out 그레이레벨 개수 세기
        for (j = 0; j < Col; j++) {
            out_arr[out[i][j]]++;
        }
    }

    for (i = 0; i < 256; i++) {//pdf계산
        out_pdf[i] = (double)out_arr[i] / (Col * Row);
    }
    out_cdf[0] = out_pdf[0];//처음값 같게
    for (i = 1; i < 256; i++) {//cdf계산
        out_cdf[i] = out_pdf[i] + out_cdf[i - 1];
    }
}
//영상처리(4~6) 20170880 강지원
int main(int argc, char* argv[]) {//argc=아규먼트 개수 체크, 
    /*
    argv[1] 읽을 이미지파일 이름
    argv[2] Col
    argv[3] Row
    argv[4] out
    */

    int Row, Col;
    uchar** img;
    uchar** out;
    uchar** graph;

    if (argc != 5) {
        printf("%s Inputing, Col, Row, result!\n", argv[0]);
        exit(-1);
    }

    Col = atoi(argv[2]);//입력받은 문자를 정수로 넣음
    Row = atoi(argv[3]);

    img = uc_alloc(Col, Row);//메모리 할당
    out = uc_alloc(Col, Row);
    graph = uc_alloc(256, 256);

    read_ucmatrix(Col, Row, img, argv[1]);//이미지 읽기

    get_histo(img, graph, img_arr, img_pdf, img_cdf, Col, Row);//히스토그램, cdf pdf 계산
    histo_equaliz(img, out, graph, Col, Row);//평활화

    write_ucmatrix(Col,Row,out,argv[4]);

    uc_free(Col, Row, img);//메모리해제
    uc_free(Col, Row, out);
    uc_free(256, 256, graph);

    return 0;
}
//영상처리(4~6) 20170880 강지원
// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴
// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.


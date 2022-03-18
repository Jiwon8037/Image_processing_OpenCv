#include <stdio.h>
#include <math.h>
#include <assert.h>
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
void Circle(uchar** img1, uchar** img2, uchar** out, int Col, int Row, double diameter, int x, int y) {//원 모양 처리
    int i, j;
    double tmp, xSqure, ySqure;

    for (i = 0; i < Row; i++) {
        for (j = 0; j < Col; j++) {
            ySqure = (abs(y - i)) * (abs(y - i));//입력받은 y좌표 - i 의 제곰
            xSqure = (abs(x - j)) * (abs(x - j));//입력받은 x좌표 - j 의 제곰

            tmp = sqrt(ySqure + xSqure);//루트(ySqure + xSqure) -> 피타고라스

            if (tmp < diameter)out[i][j] = img2[i][j];//원부분은 처리한이미지
            else out[i][j] = img1[i][j];//나머지는 원본 이미지
        }
    }
}
void Mosaic(uchar** img, uchar** out, int Col, int Row, int Block) {//모자이크
    int i, j, tmp, count, x, y;
    for (i = 0; i < Row; i += Block) {
        for (j = 0; j < Col; j += Block) {
            tmp = 0;
            count = 0;
            for (y = 0; y < Block; y++) {
                for (x = 0; x < Block; x++) {
                    tmp += img[i + y][j + x];
                    count++;
                }
            }//평균계산

            tmp /= count;
            for (y = 0; y < Block; y++) {
                for (x = 0; x < Block; x++) {
                    out[i + y][j + x] = tmp;
                }
            }
        }
    }
}
//영상처리(4~6) 20170880 강지원
int main(int argc, char* argv[]) {//argc=아규먼트 개수 체크, 
    /*
    argv[1] 읽을 이미지파일 이름
    argv[2] 결과 이미지 이름
    argv[3] Col
    argv[4] Row
    argv[5] Block size
    argv[6] col_pos
    argv[7] row_pos
    argv[8] diameter
    */

    int Row, Col;
    uchar** img, ** out;
    uchar** resultimg;//처리한 이미지
    double diameter;//지름
    int col_pos, row_pos;//처리할 영역 x,y좌표
    int num;//모자이크 블록크기 

    if (argc != 9) {
        printf("%s Inputimg, outimg, Col, Row, Block_size, col_pos, row_pos, diameter !\n", argv[0]);
        exit(-1);
    }

    Col = atoi(argv[3]);//입력받은 문자를 정수로 넣음
    Row = atoi(argv[4]);
    num = atoi(argv[5]);
    col_pos = atoi(argv[6]);
    row_pos = atoi(argv[7]);
    diameter = atof(argv[8]);//실수라서 atof

    img = uc_alloc(Col, Row);//메모리 할당
    out = uc_alloc(Col, Row);
    resultimg = uc_alloc(Col, Row);

    read_ucmatrix(Col, Row, img, argv[1]);//이미지 읽기
    Mosaic(img, resultimg, Col, Row, num);//모자이크처리
    Circle(img, resultimg, out, Col, Row, diameter, col_pos, row_pos);//원 이미지
    write_ucmatrix(Col, Row, out, argv[2]);//아웃픗 파일 생성

    uc_free(Col, Row, img);//할당한 메모리해제
    uc_free(Col, Row, out);
    uc_free(Col, Row, resultimg);
    return 0;
}
// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴
// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.

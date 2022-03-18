//����ó��(4~6) 20170880 ������
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
    FILE* f;//������ �����ͼ���, �ϵ��ũ�� ���Ͻ����ּҸ� ����Ŵ
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
int img_arr[256] = { 0 };//img �׷��̷��� ���� ���� �迭
double img_pdf[256] = { 0 };//pdf�� �迭
double img_cdf[256] = { 0 };//cdf�� �迭

int out_arr[256] = { 0 };//out �׷��̷��� ���� ���� �迭
double out_pdf[256] = { 0 };
double out_cdf[256] = { 0 };

void get_histo(uchar** img, uchar** graph, int* arr, double* pdf, double* cdf, int Col, int Row) {//������׷�, pdf cdf ���ϱ�
    int i, j;
    double maxg = -1;
    double num;

    for (i = 0; i < Row; i++) {//img �׷��̷��� ����
        for (j = 0; j < Col; j++) {
            arr[img[i][j]]++;
        }
    }
    for (i = 0; i < 256; i++) {//pdf���
        pdf[i] = (double)arr[i] / (Col * Row);
    }
    cdf[0] = pdf[0];//ó���� ����
    for (i = 1; i < 256; i++) {//cdf���
        cdf[i] = pdf[i] + cdf[i - 1];
    }
}
void histo_equaliz(uchar** img, uchar** out, uchar** graph, int Col, int Row) {//��Ȱȭ
    int i, j;
    double maxg = -1;
    double num;

    for (i = 0; i < Row; i++) {//��Ȱȭ ����̹��� ����
        for (j = 0; j < Col; j++) {
            out[i][j] = img_cdf[img[i][j]] * 255;
        }
    }

    for (i = 0; i < Row; i++) {//out �׷��̷��� ���� ����
        for (j = 0; j < Col; j++) {
            out_arr[out[i][j]]++;
        }
    }

    for (i = 0; i < 256; i++) {//pdf���
        out_pdf[i] = (double)out_arr[i] / (Col * Row);
    }
    out_cdf[0] = out_pdf[0];//ó���� ����
    for (i = 1; i < 256; i++) {//cdf���
        out_cdf[i] = out_pdf[i] + out_cdf[i - 1];
    }
}
//����ó��(4~6) 20170880 ������
int main(int argc, char* argv[]) {//argc=�ƱԸ�Ʈ ���� üũ, 
    /*
    argv[1] ���� �̹������� �̸�
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

    Col = atoi(argv[2]);//�Է¹��� ���ڸ� ������ ����
    Row = atoi(argv[3]);

    img = uc_alloc(Col, Row);//�޸� �Ҵ�
    out = uc_alloc(Col, Row);
    graph = uc_alloc(256, 256);

    read_ucmatrix(Col, Row, img, argv[1]);//�̹��� �б�

    get_histo(img, graph, img_arr, img_pdf, img_cdf, Col, Row);//������׷�, cdf pdf ���
    histo_equaliz(img, out, graph, Col, Row);//��Ȱȭ

    write_ucmatrix(Col,Row,out,argv[4]);

    uc_free(Col, Row, img);//�޸�����
    uc_free(Col, Row, out);
    uc_free(256, 256, graph);

    return 0;
}
//����ó��(4~6) 20170880 ������
// ���α׷� ����: <Ctrl+F5> �Ǵ� [�����] > [��������� �ʰ� ����] �޴�
// ���α׷� �����: <F5> Ű �Ǵ� [�����] > [����� ����] �޴�
// ������ ���� ��: 
//   1. [�ַ�� Ž����] â�� ����Ͽ� ������ �߰�/�����մϴ�.
//   2. [�� Ž����] â�� ����Ͽ� �ҽ� ��� �����մϴ�.
//   3. [���] â�� ����Ͽ� ���� ��� �� ��Ÿ �޽����� Ȯ���մϴ�.
//   4. [���� ���] â�� ����Ͽ� ������ ���ϴ�.
//   5. [������Ʈ] > [�� �׸� �߰�]�� �̵��Ͽ� �� �ڵ� ������ ����ų�, [������Ʈ] > [���� �׸� �߰�]�� �̵��Ͽ� ���� �ڵ� ������ ������Ʈ�� �߰��մϴ�.
//   6. ���߿� �� ������Ʈ�� �ٽ� ������ [����] > [����] > [������Ʈ]�� �̵��ϰ� .sln ������ �����մϴ�.


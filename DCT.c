#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.141592654
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
double** d_alloc(int size_x, int size_y) {
	double** m;
	int i;
	if ((m = (double**)calloc(size_y, sizeof(double*))) == NULL) {
		printf("d_alloc error 1\n");
		exit(0);
	}
	for (i = 0; i < size_y; i++)
		if ((m[i] = (double*)calloc(size_x, sizeof(double))) == NULL) {
			printf("d_alloc error 2\n");
			exit(0);
		}
	return m;
}
void d_free(int size_x, int size_y, double** ucmatrix) {
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
int main(int argc, char* argv[]){

	int Row, Col;
	uchar** img, ** dct, **out;
	double** trans;

	if (argc != 4) {
		printf("%s Inputing, Col, Row!\n", argv[0]);
		exit(-1);
	}
	Col = atoi(argv[2]);//입력받은 문자를 정수로 넣음
	Row = atoi(argv[3]);

	img = uc_alloc(Col, Row);
	out = uc_alloc(Col, Row);
	dct = uc_alloc(Col, Row);
	trans = d_alloc(Col, Row);

	read_ucmatrix(Col, Row, img, argv[1]);//이미지 읽기

	int i, j, u, v, x, y;
	double cu, cv, result, MSE, SUM, PSNR;
	
	for (i = 0; i < Row/8; i++){//Forward DCT
		for (j = 0; j < Col/8; j++){
			for (u = 0; u < 8; u++){
				for (v = 0; v < 8; v++){
					result = 0.0;
					for (x = 0; x < 8; x++){
						for (y = 0; y < 8; y++){
							if (u == 0)
								cu = 1 / sqrt(2);
							else
								cu = 0.0;

							if (v == 0)
								cv = 1 / sqrt(2);
							else
								cv = 0.0;
							result += cos((((2 * (double)x + 1) * (double)u * PI) / 16)) * cos((((2 * (double)y + 1) * (double)v * PI) / 16)) * (double)img[(i * 8) + x][(j * 8) + y];
						}
					}
					trans[(i * 8) + u][(j * 8) + v] = ((cu * cv) / 4) * result;
				}
			}
		}
	}

	for (i = 0; i < Row; i++){// DCT 출력
		for (j = 0; j < Col; j++){
			dct[i][j] = (uchar)floor(trans[i][j] + 0.5);
		}
	}
	write_ucmatrix(Col,Row,dct,"dct");

	for (i = 0; i < Row / 8; i++) {
		for (j = 0; j < Col / 8; j++) {
			for (x = 0; x < 8; x++){
				for (y = 0; y < 8; y++){
					result = 0.0;
					for (u = 0; u < 8; u++){
						for (v = 0; v < 8; v++){
							if (u == 0)
								cu = 1 / sqrt(2);
							else
								cu = 0.0;
							if (v == 0)
								cv = 1 / sqrt(2);
							else
								cv = 0.0;
							result += (cu * cv) * cos((((2 * (double)x + 1) * (double)u * PI) / 16)) * cos((((2 * (double)y + 1) * (double)v * PI) / 16)) * trans[(i * 8) + u][(j * 8) + v];
						}
					}
					out[(i * 8) + x][(j * 8) + y] = (uchar)((1. / 4.) * result);
				}
			}
		}
	}

	write_ucmatrix(Col, Row, out, "invers_dct");
	
	// PSNR
	MSE = 0.;
	SUM = 0.;
	double temp;
	for (i = 0; i < Row; i++){
		for (j = 0; j < Col; j++){
			temp = (double)((int)img[i][j] - (int)out[i][j]);
			SUM += temp*temp;
		}
	}
	MSE = SUM / (double)(Col*Row);
	PSNR = 10. * log10((255. * 255.) / MSE);
	printf("MSE = %10.4lf\n", MSE);
	printf("PSNR = %10.4lf", PSNR);

	uc_free(Col, Row, out);
	uc_free(Col, Row, dct);
	uc_free(Col, Row, img);
	d_free(Col, Row, trans);
	return 0;
}
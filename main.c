#include <stdio.h>
#include <string.h>

#define QR_SIZE 21 // version 1 of QR

typedef struct {
  int size;
  int matrix[QR_SIZE][QR_SIZE];
} QRCode;

void initQrGrid(QRCode *qr);
void encodeData(int *bitPosition, char *data, int *bitArray);
void applyFinder(QRCode *qr, int row, int col);
void applyTimingPattern(QRCode *qr);
void placeData(QRCode *qr, int bitlen, int *bitStream);
void printQRCode(QRCode *qr);
void createPPMfile(QRCode *qr);
int main() {

  QRCode qr = {0};
  initQrGrid(&qr);

  int bitStream[256] = {0};
  char *data = "www.google.com";
  int bitPosition;
  encodeData(&bitPosition, data, bitStream);
  applyFinder(&qr, 0, 0);
  applyFinder(&qr, 0, QR_SIZE - 7);
  applyFinder(&qr, QR_SIZE - 7, 0);
  placeData(&qr, bitPosition, bitStream);
  printQRCode(&qr);
  createPPMfile(&qr);
  return 1;
}

void initQrGrid(QRCode *qr) {
  qr->size = QR_SIZE;

  for (size_t x = 0; x < QR_SIZE; x++)
    for (size_t y = 0; y < QR_SIZE; y++) {
      qr->matrix[x][y] = -1;
    }
}

void encodeData(int *bitPosition, char *data, int *bitArray) {
  int len = strlen(data);
  *bitPosition = 0;

  bitArray[(*bitPosition)++] = 0x4;
  bitArray[(*bitPosition)++] = len;

  for (int x = 0; x < len; x++)
    bitArray[(*bitPosition)++] = data[x];
}

void applyFinder(QRCode *qr, int row, int col) {

  for (int x = row; x < row + 7; x++) {
    for (int y = col; y < col + 7; y++) {
      // Outline for 7x7 grid
      if (x == row || x == row + 6 || y == col || y == col + 6) {
        qr->matrix[x][y] = 1;
      }
      // Inner 3x3 grid at the center
      if (x >= row + 2 && x <= row + 4 && y >= col + 2 && y <= col + 4) {
        qr->matrix[x][y] = 1;
      }
    }
  }
}

void applyTimingPattern(QRCode *qr) {

  for (int i = 8; i < qr->size - 8; i++) {
    qr->matrix[6][i] = i % 2 == 0;
    qr->matrix[i][6] = i % 2 == 0;
  }
}

void placeData(QRCode *qr, int bitlen, int *bitStream) {
  int pos = bitlen - 1;

  for (int col = qr->size - 1; col > 0; col -= 2) {
    if (col == 6)
      col--;
    for (int row = qr->size - 1; row >= 0; row--) {
      if (qr->matrix[col][row] == -1) {
        qr->matrix[col][row] = bitStream[pos--];
      }
      if (qr->matrix[col][row - 1] == -1) {
        qr->matrix[col][row - 1] = bitStream[pos--];
      }
    }
  }
}

void printQRCode(QRCode *qr) {
  for (int i = 0; i < qr->size; i++) {
    for (int j = 0; j < qr->size; j++) {
      if (qr->matrix[i][j] == 1) {
        printf("██");
      } else {
        printf("  ");
      }
    }
    printf("\n");
  }
}

void createPPMfile(QRCode *qr) {
  FILE *file = fopen("qrcode.pbm", "wb");
 fprintf(file, "P1\n%d %d\n", qr->size, qr->size);
  for (int row = 0; row < qr->size; row++) {
        for (int col = 0; col < qr->size; col++) {
            if (qr->matrix[row][col] == 1) {
                fputc('1', file); // Black
            } else {
                fputc('0', file); // White
            }
            fputc(' ', file); // Space between bits
        }
        fputc('\n', file); // Newline after each row
    }

  fclose(file);
}


#include "openNextFile.h"

void openNextFile(File &dataFile) {

  char filename[10]; // = "00000.txt";
  filename[0] = '0';
  filename[1] = '0';
  filename[2] = '0';
  filename[3] = '0';
  filename[4] = '0';
  filename[5] = '.';
  filename[6] = 't';
  filename[7] = 'x';
  filename[8] = 't';
  filename[9] = '\0';
  int fileNum = 0;
  dataFile = SD.open(filename, FILE_READ);
  while (dataFile) {
    fileNum++;
    filename[0] = 48+(fileNum/10000)%10;
    filename[1] = 48+(fileNum/1000)%10;
    filename[2] = 48+(fileNum/100)%10;
    filename[3] = 48+(fileNum/10)%10;
    filename[4] = 48+(fileNum/1)%10;
    Serial.print("Trying ");
    Serial.println(filename);
    dataFile = SD.open(filename, FILE_READ);
  }
  Serial.print("writing to file: ");
  Serial.println(filename);

  dataFile = SD.open(filename, FILE_WRITE);
}
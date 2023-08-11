#include <Gamer.h>

Gamer gamer;

void setup() {
  gamer.begin();
}

void loop() {
  int v = map(gamer.ldrValue(), 0, 1023, 99, 0);
  gamer.showScore(v);
  delay(300);
}

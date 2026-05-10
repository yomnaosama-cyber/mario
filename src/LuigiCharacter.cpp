#include "LuigiCharacter.h"
#include <QPixmap>

LuigiCharacter::LuigiCharacter() {
    QPixmap pix("assets/luigi.png");
    setPixmap(pix.scaled(80, 80));
    setZValue(10);
}

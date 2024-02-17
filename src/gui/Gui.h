#ifndef FAITHFUL_SRC_GUI_GUI_H
#define FAITHFUL_SRC_GUI_GUI_H

#include "../common/Object.h"

namespace faithful {

class IGui :
    public Transformable2D,
    public Collidable,
    public Drawable {};

}  // namespace faithful

#endif  // FAITHFUL_SRC_GUI_GUI_H

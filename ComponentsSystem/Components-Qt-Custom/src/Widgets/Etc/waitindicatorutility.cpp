#include "waitindicatorutility.hpp"

namespace QtCustom::Widgets {

double utilityPieFromDegree(const double degree)
{
    return degree * 16.0f; // See Qt QPainter::drawPie documentation
}

}

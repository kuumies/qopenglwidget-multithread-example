/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Implementation of kuu::opengl::Widget class.
 * ---------------------------------------------------------------- */

#include "opengl_widget.h"

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *
   The data of the widget.
 * ---------------------------------------------------------------- */
struct Widget::Data
{
};

/* ---------------------------------------------------------------- *
   Constructs the widget.
 * -----------------------------------------------------------------*/
Widget::Widget()
    : d(std::make_shared<Data>())
{}

} // namespace opengl
} // namespace kuu

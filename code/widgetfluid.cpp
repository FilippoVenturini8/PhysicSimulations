#include "widgetfluid.h".h"
#include "ui_widgetfluid.h"

WidgetFluid::WidgetFluid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFluid)
{
    ui->setupUi(this);

}

WidgetFluid::~WidgetFluid()
{
    delete ui;
}

#include "widgetfluid.h".h"
#include "ui_widgetfluid.h"

WidgetFluid::WidgetFluid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFluid)
{
    ui->setupUi(this);
    ui->comboBox->addItem("Double wall");
    ui->comboBox->addItem("Cube");
}

WidgetFluid::~WidgetFluid()
{
    delete ui;
}

int WidgetFluid::getComboBoxIndex(){
    return ui->comboBox->currentIndex();
}

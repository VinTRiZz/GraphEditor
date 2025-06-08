#include "grapheditorform.h"
#include "ui_grapheditorform.h"

#include "graphobject.h"

#include "logging.h"

GraphEditorForm::GraphEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphEditorForm)
{
    ui->setupUi(this);

    ui->graphScene->init();

    setupSignals();

    Graph::GraphObject tmpGraph;
    tmpGraph.setIdGenerator(ui->graphScene->getIdGenerator());

    Graph::GVertex vert;
    vert.shortName = "Vertex 1";
    vert.backgroundColor = Qt::red;
    vert.posX = 100;
    vert.posY = 100;
    auto vert1id = tmpGraph.addVertex(vert);

    vert.shortName = "Vertex 2";
    vert.borderColor = Qt::magenta;
    vert.backgroundColor = Qt::green;
    vert.posX = 100;
    vert.posY = 300;
    auto vert2id = tmpGraph.addVertex(vert);

    Graph::GConnection con;
    con.name = "My connection";
    con.idFrom = vert1id;
    con.idTo = vert2id;
    con.lineColor = Qt::darkYellow;
    con.isDirected = true;
    tmpGraph.addConnection(con);

    ui->graphScene->addObject(tmpGraph.toItem());
}

GraphEditorForm::~GraphEditorForm()
{
    delete ui;
}

void GraphEditorForm::setupSignals()
{
    connect(ui->graphProps_toggle_pushButton, &QPushButton::clicked,
            this, [this](){
        if (ui->graphProps_groupBox->isHidden()) {
            ui->graphProps_groupBox->show();
            ui->graphProps_toggle_pushButton->setText("Скрыть свойства графа");
            return;
        }

        ui->graphProps_groupBox->hide();
        ui->graphProps_toggle_pushButton->setText("Показать свойства графа");
    });
    ui->graphProps_toggle_pushButton->click(); // Чтобы UI текст заменился


}

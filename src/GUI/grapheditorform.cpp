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
    ui->graphScene->resizeScene(QSize(10000, 10000));
    ui->graphScene->scale(0.5, 0.5);

    setupSignals();

    Graph::GraphObject tmpGraph;
    tmpGraph.setIdGenerator(ui->graphScene->getIdGenerator());

    Graph::GVertex vert;
    vert.shortName = "Vertex 1";
    vert.backgroundColor = Qt::red;
    vert.posX = 100;
    vert.posY = 100;
    tmpGraph.addVertex(vert);

    vert.shortName = "Vertex 2";
    vert.borderColor = Qt::magenta;
    vert.backgroundColor = Qt::green;
    vert.posX = 100;
    vert.posY = 300;
    tmpGraph.addVertex(vert);

    vert.shortName = "Vertex 3";
    vert.backgroundColor = Qt::red;
    vert.posX = 300;
    vert.posY = 100;
    tmpGraph.addVertex(vert);

    vert.shortName = "Vertex 4";
    vert.borderColor = Qt::magenta;
    vert.backgroundColor = Qt::green;
    vert.posX = 300;
    vert.posY = 300;
    tmpGraph.addVertex(vert);

    Graph::GConnection con;
    con.name = "Connection 1";
    con.idFrom = 1;
    con.idTo = 2;
    tmpGraph.addConnection(con);

    con.name = "Connection 2";
    con.idFrom = 2;
    con.idTo = 3;
    tmpGraph.addConnection(con);

    con.name = "Connection 3";
    con.idFrom = 1;
    con.idTo = 3;
    tmpGraph.addConnection(con);

    con.name = "Connection 4";
    con.idFrom = 1;
    con.idTo = 4;
    tmpGraph.addConnection(con);

    con.name = "Connection 5";
    con.idFrom = 3;
    con.idTo = 1;
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

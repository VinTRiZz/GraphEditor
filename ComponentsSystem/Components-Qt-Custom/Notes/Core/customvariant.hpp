#include <QMetaType>

struct Token {
    QString _value;
};

Q_DECLARE_METATYPE( Token* );

QString tokenToString( Token* t ) {
    return t->_value;
}

#include <QDebug>

//int main(int argc, char* argv[]) {
//    QMetaType::registerConverter<Token*,QString>( tokenToString );
//    Token t = { QString("hello") };
//    QVariant value;
//    value.setValue( &t );
//    qDebug() << value;
//}

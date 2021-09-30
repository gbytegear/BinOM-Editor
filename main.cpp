#include <QGuiApplication>
#include <QQuickStyle>
#include <QQmlApplicationEngine>
#include <QFont>
#include <QFontDatabase>
#include <QDir>

#include <qbinom.h>

#include <QDebug>

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication app(argc, argv);

  QQuickStyle::setStyle("Material");
  app.setFont(QFont(QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(":/font/fonts/Ubuntu/Ubuntu-Bold.ttf")).at(0)));
//  qDebug() << "Font id: " << id;
//  QString family = QFontDatabase::applicationFontFamilies(id).at(0);

  qmlRegisterSingletonType<QBinOM>("BinOM", 1, 0, "BinOM", [](QQmlEngine *engine, QJSEngine *script_engine) -> QObject * {
    Q_UNUSED(engine)
    Q_UNUSED(script_engine)
    return new QBinOM();
  });

  QQmlApplicationEngine engine;
  const QUrl url(QStringLiteral("qrc:/main.qml"));
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                   &app, [url](QObject *obj, const QUrl &objUrl) {
    if (!obj && url == objUrl)
      QCoreApplication::exit(-1);
  }, Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}

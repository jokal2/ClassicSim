#include <QCoreApplication>
#include <QTimer>

#include <CommandLineControl.h>

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  qRegisterMetaType<QVector<QString> >("QVector<QString>");
  auto command_line_control = std::make_unique<CommandLineControl>();

  QObject::connect(command_line_control.get(), &CommandLineControl::finished,
                   &app, &QCoreApplication::quit);

  QTimer::singleShot(0, command_line_control.get(), &CommandLineControl::run);


  return app.exec();
}

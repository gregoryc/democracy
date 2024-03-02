// g++ -fPIC -O3 -s *.cpp $(pkg-config --cflags --libs `pkg-config --list-all |
// fgrep Qt | cut -f 1 -d ' ' | xargs`) && ./a.out

#include <QApplication>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QStandardPaths>
#include <QWebSettings>
#include <QWebView>
#include <QWindow>
#include <QtGui>
#include <QtWebKit>
int main(int argc, char **argv) {
  QApplication app(argc, argv);
  QWebView view;
  QWebSettings *d = QWebSettings::globalSettings();
  // using QWebSettings;
  d->setAttribute(QWebSettings::JavascriptEnabled, true);
  d->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
  d->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
  d->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);

  d->setAttribute(QWebSettings::AutoLoadImages, true);

  d->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
  d->setAttribute(QWebSettings::PluginsEnabled, true);
  //  app.setStyleSheet("* { color: blue; background-color: blue; border-radius:
  //  40px; opacity: 0.4; }");
  view.load(QUrl("http://ethicify.online"));
  QDockWidget window;
  window.setFeatures(QDockWidget::AllDockWidgetFeatures);
  window.setTitleBarWidget(&view);
  window.resize(600, 200);
  // window.setOpacity(0.8);
  window.show();
  window.windowHandle()->setOpacity(0.8);
  return app.exec();
}

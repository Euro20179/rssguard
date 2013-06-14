#include <QIcon>
#include <QFile>
#include <QDir>
#include <QApplication>

#include "gui/themefactory.h"
#include "core/settings.h"
#include "core/defs.h"


ThemeFactory::ThemeFactory() {
}

void ThemeFactory::setupSearchPaths() {
  QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << APP_THEME_PATH);
  qDebug("Available icon theme paths: %s.",
         qPrintable(QIcon::themeSearchPaths().join(", ")));
}

// TODO: Load currently selected "real" icon theme name instead of
// Qt default "", which stands for currently active system icon theme name.
QString ThemeFactory::getSystemIconTheme() {
#if defined(Q_OS_LINUX)
  return QString();
#else
  // It is expected that oxygen is provided as fall-back theme for
  // windows edition of RSS Guard.
  return "oxygen";
#endif
}

QString ThemeFactory::getCurrentIconTheme() {
  QString current_theme_name = Settings::getInstance().value(APP_CFG_GUI,
                                                             "icon_theme",
                                                             getSystemIconTheme()).toString();
  return current_theme_name;
}

void ThemeFactory::setCurrentIconTheme(const QString &theme_name) {
  Settings::getInstance().setValue(APP_CFG_GUI,
                                   "icon_theme",
                                   theme_name);
  loadCurrentIconTheme();
}

void ThemeFactory::loadCurrentIconTheme() {
  QString theme_name = getCurrentIconTheme();
  QStringList installed_themes = getInstalledIconThemes();

  qDebug("Installed icon themes are: %s.",
         qPrintable(installed_themes.join(", ")));

  if (!installed_themes.contains(theme_name)) {
    qDebug("Icon theme '%s' cannot be loaded because it is not installed.",
           qPrintable(theme_name));
    return;
  }
  else {
    qDebug("Loading theme '%s'.", qPrintable(theme_name));
    QIcon::setThemeName(theme_name);
  }
}

QStringList ThemeFactory::getInstalledIconThemes() {
  QStringList icon_theme_names;

  // Add system theme.
  icon_theme_names << getSystemIconTheme();

  // Iterate all directories with icon themes.
  QStringList icon_themes_paths = QIcon::themeSearchPaths();
  icon_themes_paths.removeDuplicates();

  foreach (QString icon_path, icon_themes_paths) {
    QDir icon_dir(icon_path);

    // Iterate all icon themes in this directory.
    foreach (QString icon_theme_path, icon_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot |
                                                         QDir::Readable | QDir::CaseSensitive |
                                                         QDir::NoSymLinks,
                                                         QDir::Time)) {
      // Check if index.theme file in this path exists.
      if (QFile::exists(icon_dir.path() + "/" + icon_theme_path + "/index.theme")) {
        icon_theme_names << icon_theme_path;
      }
    }
  }

  icon_theme_names.removeDuplicates();
  return icon_theme_names;
}

// zjištění názvů témat dle d:\Programovani\Materiály\Qt\qt5\qtbase\src\gui\image\qiconloader.cpp,
// řádek 172, jak se prochází ty složky
// http://doublecmd.svn.sourceforge.net/viewvc/doublecmd/trunk/src/platform/unix/uunixicontheme.pas

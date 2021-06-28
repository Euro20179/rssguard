// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/notification.h"

#include "miscellaneous/application.h"

#include <QDir>
#include <QSound>

Notification::Notification(Notification::Event event, bool balloon, const QString& sound_path)
  : m_event(event), m_balloonEnabled(balloon), m_soundPath(sound_path) {}

Notification::Event Notification::event() const {
  return m_event;
}

void Notification::setEvent(const Event& event) {
  m_event = event;
}

QString Notification::soundPath() const {
  return m_soundPath;
}

void Notification::setSoundPath(const QString& sound_path) {
  m_soundPath = sound_path;
}

void Notification::playSound(Application* app) const {
  if (!m_soundPath.isEmpty()) {
    QSound::play(QDir::toNativeSeparators(app->replaceDataUserDataFolderPlaceholder(m_soundPath)));
  }
}

QList<Notification::Event> Notification::allEvents() {
  return {
    Event::GeneralEvent,
    Event::NewUnreadArticlesFetched,
    Event::ArticlesFetchingStarted,
    Event::LoginDataRefreshed,
    Event::NewAppVersionAvailable,
  };
}

QString Notification::nameForEvent(Notification::Event event) {
  switch (event) {
    case Notification::Event::NewUnreadArticlesFetched:
      return QObject::tr("New (unread) articles fetched");

    case Notification::Event::ArticlesFetchingStarted:
      return QObject::tr("Fetching articles right now");

    case Notification::Event::LoginDataRefreshed:
      return QObject::tr("Login data refreshed");

    case Notification::Event::NewAppVersionAvailable:
      return QObject::tr("New %1 version is available").arg(APP_NAME);

    case Notification::Event::GeneralEvent:
      return QObject::tr("Miscellaneous events");

    default:
      return QObject::tr("Unknown event");
  }
}

bool Notification::balloonEnabled() const {
  return m_balloonEnabled;
}

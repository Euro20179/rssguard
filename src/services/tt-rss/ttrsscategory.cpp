// This file is part of RSS Guard.
//
// Copyright (C) 2011-2016 by Martin Rotter <rotter.martinos@gmail.com>
//
// RSS Guard is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RSS Guard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RSS Guard. If not, see <http://www.gnu.org/licenses/>.

#include "services/tt-rss/ttrsscategory.h"

#include "definitions/definitions.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "services/tt-rss/definitions.h"
#include "services/tt-rss/ttrssserviceroot.h"
#include "services/tt-rss/network/ttrssnetworkfactory.h"

#include <QVariant>


TtRssCategory::TtRssCategory(RootItem *parent) : Category(parent), m_customId(NO_PARENT_CATEGORY) {
  setIcon(qApp->icons()->fromTheme(QSL("folder-category")));
}

TtRssCategory::TtRssCategory(const QSqlRecord &record) : Category(NULL) {
  setIcon(qApp->icons()->fromTheme(QSL("folder-category")));
  setId(record.value(CAT_DB_ID_INDEX).toInt());
  setTitle(record.value(CAT_DB_TITLE_INDEX).toString());
  setCustomId(record.value(CAT_DB_CUSTOM_ID_INDEX).toInt());
}

TtRssCategory::~TtRssCategory() {
}

QString TtRssCategory::hashCode() const {
  return
      QString::number(kind()) + QL1S("-") +
      QString::number(const_cast<TtRssCategory*>(this)->getParentServiceRoot()->accountId()) + QL1S("-") +
      QString::number(customId());
}

TtRssServiceRoot *TtRssCategory::serviceRoot() {
  return qobject_cast<TtRssServiceRoot*>(getParentServiceRoot());
}

bool TtRssCategory::markAsReadUnread(RootItem::ReadStatus status) {
  QStringList ids = serviceRoot()->customIDSOfMessagesForItem(this);
  TtRssUpdateArticleResponse response = serviceRoot()->network()->updateArticles(ids, UpdateArticle::Unread,
                                                                                 status == RootItem::Unread ?
                                                                                   UpdateArticle::SetToTrue :
                                                                                   UpdateArticle::SetToFalse);

  if (serviceRoot()->network()->lastError() != QNetworkReply::NoError || response.updateStatus()  != STATUS_OK) {
    return false;
  }
  else {
    return serviceRoot()->markFeedsReadUnread(getSubTreeFeeds(), status);
  }
}

bool TtRssCategory::cleanMessages(bool clear_only_read) {
  return serviceRoot()->cleanFeeds(getSubTreeFeeds(), clear_only_read);
}

int TtRssCategory::customId() const {
  return m_customId;
}

void TtRssCategory::setCustomId(int custom_id) {
  m_customId = custom_id;
}

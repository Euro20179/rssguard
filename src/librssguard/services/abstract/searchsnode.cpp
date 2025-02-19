// For license of this file, see <project-root-folder>/LICENSE.md.

#include "services/abstract/searchsnode.h"

#include "database/databasefactory.h"
#include "database/databasequeries.h"
#include "exceptions/applicationexception.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "services/abstract/gui/formaddeditprobe.h"
#include "services/abstract/serviceroot.h"

#include "3rd-party/boolinq/boolinq.h"

SearchsNode::SearchsNode(RootItem* parent_item) : RootItem(parent_item), m_actProbeNew(nullptr) {
  setKind(RootItem::Kind::Probes);
  setId(ID_PROBES);
  setIcon(qApp->icons()->fromTheme(QSL("system-search")));
  setTitle(tr("Article probes"));
  setDescription(tr("You can see all your permanent article probes here."));
}

void SearchsNode::loadProbes(const QList<Search*>& probes) {
  for (auto* prb : probes) {
    appendChild(prb);
  }
}

QList<Message> SearchsNode::undeletedMessages() const {
  QSqlDatabase database = qApp->database()->driver()->connection(metaObject()->className());

  return {};
  // return DatabaseQueries::getUndeletedLabelledMessages(database, getParentServiceRoot()->accountId());
}

int SearchsNode::countOfUnreadMessages() const {
  auto chi = childItems();

  if (chi.isEmpty()) {
    return 0;
  }

  return boolinq::from(chi)
    .max([](RootItem* it) {
      return it->countOfUnreadMessages();
    })
    ->countOfUnreadMessages();
}

int SearchsNode::countOfAllMessages() const {
  auto chi = childItems();

  if (chi.isEmpty()) {
    return 0;
  }

  return boolinq::from(chi)
    .max([](RootItem* it) {
      return it->countOfAllMessages();
    })
    ->countOfAllMessages();
}

void SearchsNode::updateCounts(bool including_total_count) {
  // TODO: This is still rather slow because this is automatically
  // called when message is marked (un)read or starred.
  // It would be enough if only labels which are assigned to article
  // are recounted, not all.

  QSqlDatabase database = qApp->database()->driver()->threadSafeConnection(metaObject()->className());
  int account_id = getParentServiceRoot()->accountId();
  auto acc = DatabaseQueries::getMessageCountsForAllLabels(database, account_id);
  /*
  for (Label* lbl : probes()) {
    if (!acc.contains(lbl->customId())) {
      if (including_total_count) {
        lbl->setCountOfAllMessages(0);
      }

      lbl->setCountOfUnreadMessages(0);
    }
    else {
      auto ac = acc.value(lbl->customId());

      if (including_total_count) {
        lbl->setCountOfAllMessages(ac.m_total);
      }

      lbl->setCountOfUnreadMessages(ac.m_unread);
    }
  }
  */
}

Search* SearchsNode::probeById(const QString& custom_id) {
  auto chi = childItems();

  return qobject_cast<Search*>(boolinq::from(chi).firstOrDefault([custom_id](RootItem* it) {
    return it->customId() == custom_id;
  }));
}

QList<Search*> SearchsNode::probes() const {
  auto list = boolinq::from(childItems())
                .select([](RootItem* it) {
                  return static_cast<Search*>(it);
                })
                .toStdList();

  return FROM_STD_LIST(QList<Search*>, list);
}

QList<QAction*> SearchsNode::contextMenuFeedsList() {
  if (m_actProbeNew == nullptr) {
    m_actProbeNew = new QAction(qApp->icons()->fromTheme(QSL("system-search")), tr("New article probe"), this);

    connect(m_actProbeNew, &QAction::triggered, this, &SearchsNode::createProbe);
  }

  return QList<QAction*>{m_actProbeNew};
}

void SearchsNode::createProbe() {
  FormAddEditProbe frm(qApp->mainFormWidget());
  Search* new_prb = frm.execForAdd();

  if (new_prb != nullptr) {
    QSqlDatabase db = qApp->database()->driver()->connection(metaObject()->className());

    try {
      // DatabaseQueries::createLabel(db, new_prb, getParentServiceRoot()->accountId());

      getParentServiceRoot()->requestItemReassignment(new_prb, this);
    }
    catch (const ApplicationException&) {
      new_prb->deleteLater();
    }
  }
}

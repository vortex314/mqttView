#include "mqttmodel.h"

MqttModel::MqttModel(QObject *parent)
    : QAbstractTableModel(parent), timer(new QTimer(this)) {
  timer->setInterval(1000);
  connect(timer, &QTimer::timeout, this, &MqttModel::timerHit);
  timer->start();
}

void MqttModel::timerHit() {
  INFO(" timerHit ");
  QModelIndex topLeft = createIndex(1, 1);
  dataChanged(topLeft, topLeft, {Qt::DisplayRole});
}

void MqttModel::update(std::string topic, std::string message) {
  Entry newEntry = {topic, message, QTime::currentTime(), 1};
  bool found = false;
  for (uint32_t i = 0; i < table.size(); i++) {
    if (table[i].topic == topic) {
      found = true;
      QModelIndex left = createIndex(i, COL_MESSAGE);
      QModelIndex right = createIndex(i, COL_COUNT);
      newEntry.count = table[i].count + 1;
      table[i] = newEntry;
      dataChanged(left, right, {Qt::DisplayRole});
    }
  }
  if (!found) {
    beginInsertRows(QModelIndex(), table.size() - 1, table.size());
    table.push_back(newEntry);
    sort(COL_TOPIC);
    endInsertRows();
  }
}

int MqttModel::rowCount(const QModelIndex & /*parent*/) const {
  return table.size();
}

int MqttModel::columnCount(const QModelIndex & /*parent*/) const {
  return COLS;
}

void MqttModel::sort(int col) { std::sort(table.begin(), table.end()); }

QVariant MqttModel::data(const QModelIndex &index, int role) const {
  int row = index.row();
  int col = index.column();
  // generate a log message when this method gets called

  switch (role) {
    case Qt::DisplayRole: {
      if (col == COL_TOPIC) return QString(table[row].topic.c_str());
      if (col == COL_MESSAGE) return QString(table[row].message.c_str());
      if (col == COL_TIME) return table[row].time.toString("hh:mm:ss.zzz");
      if (col == COL_COUNT) return QString("%1").arg(table[row].count);
      break;
    }
    case Qt::FontRole: {
      QFont font;
      font.setPointSize(10);
      font.setBold(true);
      return font;
      break;
    }
    case Qt::BackgroundRole: {
      if (row == -1 && col == -1)  // change background only for cell(1,2)
        return QBrush(Qt::red);
      break;
    }
    case Qt::TextAlignmentRole: {
      if (row == -1 && col == -1)  // change text alignment only for cell(1,1)
        return int(Qt::AlignRight | Qt::AlignVCenter);
      break;
    }
    case Qt::CheckStateRole: {
    }
      if (row == -1 && col == -1)  // add a checkbox to cell(1,0)
        return Qt::Checked;
      break;
  }

  return QVariant();
}

QVariant MqttModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    return QString(headers[section]);
  }
  return QVariant();
}
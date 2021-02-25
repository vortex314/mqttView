
#include <Log.h>
#include <Mqtt.h>
#include <MqttPaho.h>
#include <limero.h>
#include <mqttmodel.h>
#include <qtablewidget.h>
#include <unistd.h>

#include <QApplication>
#include <QHeaderView>
#include <QTableView>
#include <QWidget>

void commandArguments(JsonObject config, int argc, char **argv);
bool loadConfig(JsonDocument &doc, const char *name);

Log logger(2048);
Thread mainThread("main");
MqttPaho mqtt(mainThread);
StaticJsonDocument<10240> jsonDoc;

int main(int argc, char **argv) {
  INFO(" starting QT ");
  Sys::init();
  commandArguments(jsonDoc.as<JsonObject>(), argc, argv);
  if (loadConfig(jsonDoc, "mqttView.json")) {
    std::string jsonString;
    serializeJsonPretty(jsonDoc, jsonString);
    INFO(" config loaded : %s ", jsonString.c_str());
  }
  JsonObject config = jsonDoc["log"];
  std::string level = config["level"] | "I";
  logger.setLogLevel(level[0]);

  INFO(" wiringMqtt started. Build : %s ", __DATE__ " " __TIME__);

  JsonObject mqttConfig = jsonDoc["mqtt"];
  mqtt.config(mqttConfig);
  mqtt.init();
  mqtt.connect();
  JsonArray subscriptions = jsonDoc["mqtt"]["subscribe"].as<JsonArray>();
  for (auto subscription : subscriptions) {
    mqtt.subscribe(subscription.as<std::string>());
  }

  MqttModel myModel;

  mqtt.incoming >>
      [&](const MqttMessage &msg) { myModel.update(msg.topic, msg.message); };

  QApplication a(argc, argv);
  QTableView tableView;
  tableView.setModel(&myModel);
  tableView.setFixedHeight(800);
  tableView.setFixedWidth(500);
  tableView.setColumnWidth(0, 200);
  tableView.setRowHeight(0, 10);
  QHeaderView *vh = tableView.verticalHeader();
  vh->setSectionResizeMode(QHeaderView::Fixed);
  vh->setDefaultSectionSize(24);
  tableView.show();

  mainThread.start();
  return a.exec();
}

/*
  load configuration file into JsonObject
*/
bool loadConfig(JsonDocument &doc, const char *name) {
  FILE *f = fopen(name, "rb");
  if (f == NULL) {
    ERROR(" cannot open config file : %s", name);
    return false;
  }
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET); /* same as rewind(f); */

  char *string = (char *)malloc(fsize + 1);
  fread(string, 1, fsize, f);
  fclose(f);

  string[fsize] = 0;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, string);
  if (error) {
    ERROR(" JSON parsing config file : %s failed.", name);
    return false;
  }
  return true;
}
/*

*/

void commandArguments(JsonObject config, int argc, char **argv) {
  int opt;

  while ((opt = getopt(argc, argv, "f:m:l:v:")) != -1) {
    switch (opt) {
      case 'm':
        config["mqtt"]["connection"] = optarg;
        break;
      case 'f':
        config["configFile"] = optarg;
        break;
      case 'v': {
        char logLevel = optarg[0];
        config["log"]["level"] = logLevel;
        break;
      }
      case 'l':
        config["log"]["file"] = optarg;
        break;
      default: /* '?' */
        fprintf(stderr,
                "Usage: %s [-v(TDIWE)] [-f configFile] [-l logFile] [-m "
                "mqtt_connection]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
  }
}
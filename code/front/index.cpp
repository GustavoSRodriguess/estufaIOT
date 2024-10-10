#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>

const char *ssid = "seuSSID";
const char *password = "suaSenha";

const char *mqtt_server = "seubroker.mqtt.com";
const int mqtt_port = 1883;
const char *mqtt_user = "seuUsuario";
const char *mqtt_password = "suaSenha";

const char *topic_temp = "estufa/temperatura";
const char *topic_hum = "estufa/umidade";
const char *topic_air = "estufa/qualidade_ar";

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// pino do sensor de qualidade do ar (substituir pelo sensor específico)
#define AIR_QUALITY_PIN A0

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
    Serial.begin(115200);

    dht.begin();

    setup_wifi();

    client.setServer(mqtt_server, mqtt_port);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();
    int qualidade_ar = analogRead(AIR_QUALITY_PIN);

    publish_data(topic_temp, String(temperatura).c_str());
    publish_data(topic_hum, String(umidade).c_str());
    publish_data(topic_air, String(qualidade_ar).c_str());

    delay(5000);
}

void setup_wifi()
{
    delay(10);
    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.println("Endereço IP: ");
    Serial.println(WiFi.localIP());
}

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Tentando conexão MQTT...");
        if (client.connect("ESP32Client", mqtt_user, mqtt_password))
        {
            Serial.println("conectado");
        }
        else
        {
            Serial.print("falhou, rc=");
            Serial.print(client.state());
            Serial.println(" tentando novamente em 5 segundos");
            delay(5000);
        }
    }
}

void publish_data(const char *topic, const char *payload)
{
    if (client.publish(topic, payload))
    {
        Serial.println("Mensagem publicada com sucesso");
    }
    else
    {
        Serial.println("Falha ao publicar mensagem");
    }
}
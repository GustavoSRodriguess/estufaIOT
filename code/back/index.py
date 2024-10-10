import paho.mqtt.client as mqtt
import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split

#instalar bibliotecas
MQTT_BROKER = "seubroker.mqtt.com"
MQTT_PORT = 1883
MQTT_TOPIC = "estufa/#"

model = RandomForestClassifier()

def train_model():
    
    data = pd.read_csv('dados_historicos_cafe.csv')
    X = data[['temperatura', 'umidade', 'qualidade_ar']]
    y = data['qualidade_cafe']
    
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2)
    
    model.fit(X_train, y_train)
    
    print(f"Acurácia do modelo: {model.score(X_test, y_test)}")

def analyze_data(temperatura, umidade, qualidade_ar):
    
    predicao = model.predict([[temperatura, umidade, qualidade_ar]])[0]
    
    if predicao == 'bom':
        return "As condições atuais são boas para o café."
    elif predicao == 'ruim':
        return "As condições atuais não são ideais. Considere ajustar os parâmetros."
    else:
        return "Condições incertas. Monitore de perto."

def on_message(client, userdata, message):
    topic = message.topic
    payload = message.payload.decode()
    
    if topic == "estufa/temperatura":
        temperatura = float(payload)
    elif topic == "estufa/umidade":
        umidade = float(payload)
    elif topic == "estufa/qualidade_ar":
        qualidade_ar = int(payload)
    
        resultado = analyze_data(temperatura, umidade, qualidade_ar)
        print(resultado)

client = mqtt.Client()
client.on_message = on_message
client.connect(MQTT_BROKER, MQTT_PORT)
client.subscribe(MQTT_TOPIC)

train_model()

print("Iniciando o loop. Pressione Ctrl+C para sair.")
client.loop_forever()